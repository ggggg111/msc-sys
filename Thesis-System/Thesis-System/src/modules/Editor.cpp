#include <thread>

#include "imgui.h"
#include "c10/cuda/CUDACachingAllocator.h"

#include "utils/Log.h"
#include "utils/Loading.h"
#include "utils/Globals.h"
#include "Application.h"
#include "modules/Input.h"
#include "modules/Renderer.h"
#include "modules/TorchLoader.h"
#include "Editor.h"
#include "gui/GuiElement.h"
#include "gui/MainMenuBar.h"
#include "gui/Console.h"
#include "gui/Results.h"
#include "gui/Visualizer.h"

Editor::Editor() : device("cuda")
{
	this->AddGuiElement(this->main_menu_bar = new MainMenuBar());
	this->AddGuiElement(this->console = new Console());
	this->AddGuiElement(this->setup = new Setup());
	this->AddGuiElement(this->results = new Results());
	this->AddGuiElement(this->visualizer = new Visualizer());
}

Editor::~Editor()
{
	for(auto item = this->gui_elements.rbegin(); item != this->gui_elements.rend(); ++item)
	{
		delete* item;
	}
}

void Editor::Start()
{
	bool is_cuda_available = App->torch_loader->IsCudaAvailable();

	this->device = is_cuda_available ? torch::kCUDA : torch::kCPU;

	if(is_cuda_available)
		CONSOLE_LOG("CUDA is available! Device is set to CUDA");
	else
		CONSOLE_LOG("CUDA is not available! Device is set to CPU");

	this->coco_pairs = TorchLoader::LoadCocoPairs();
	this->ntu_pairs = TorchLoader::LoadNtuPairs();

	for(auto item = this->gui_elements.begin(); item != this->gui_elements.end(); ++item)
	{
		(*item)->Start();
	}
}

void Editor::PreUpdate()
{
	if(App->input->keyboard[SDL_SCANCODE_ESCAPE] == KEY_DOWN || App->input->keyboard[SDL_SCANCODE_ESCAPE] == KEY_REPEAT)
	{
		App->Exit();
	}
}

void Editor::CleanUp()
{
	for(auto item = this->gui_elements.rbegin(); item != this->gui_elements.rend(); ++item)
	{
		(*item)->CleanUp();
	}

	RELEASE_TEXTURE_VECTOR(this->video_texture_stream);
	RELEASE_MATRIX_VECTOR(this->video_mat_stream);
}

void Editor::DrawGUI()
{
	for(auto item = this->gui_elements.begin(); item != this->gui_elements.end(); ++item)
	{
		(*item)->Update();
	}
}

void Editor::LoadPoseEstimator(const std::string& path)
{
	std::thread load_pose_estimator_thread([this, path]()
	{
		this->setup->UpdatePoseEstimatorName(path);
		this->pose_estimator = App->torch_loader->LoadModule(path, this->device);
	});

	load_pose_estimator_thread.detach();
}

void Editor::LoadRecognizer(const std::string& path)
{
	std::thread load_recognizer_thread([this, path]()
	{
		this->setup->UpdateRecognizerName(path);
		this->recognizer = App->torch_loader->LoadModule(path, this->device);
	});

	load_recognizer_thread.detach();
}

void Editor::LoadTransform(const std::string& path)
{
	std::thread load_transform_thread([this, path]()
	{
		this->setup->UpdateTransformName(path);
		this->transform = App->torch_loader->LoadModule(path, c10::kCPU);
	});

	load_transform_thread.detach();
}

void Editor::LoadVideo(const std::string& path, const cv::Size& size)
{
	std::thread load_video_thread([this, path, size]()
	{
		cv::Size original_size;

		this->video = App->torch_loader->LoadVideo(path, size, original_size, this->video_texture_stream, this->video_mat_stream, App->renderer->renderer);

		this->setup->UpdateVideoInfo(path, *this->video, original_size);
		this->visualizer->SetSkeletonRatio((float)original_size.height / (float)original_size.width);
	});

	load_video_thread.detach();
}

void Editor::LoadSkeleton(const std::string& path)
{
	std::thread load_input_thread([this, path]()
	{
		auto skeleton_dict = App->torch_loader->LoadPickleTensor(path).toGenericDict();

		c10::Dict<std::string, torch::Tensor> ret;

		for(auto k = skeleton_dict.begin(); k != skeleton_dict.end(); ++k)
		{
			if(k->value().isTensor())
			{
				std::string key = k->key().toStringRef();
				torch::Tensor value = k->value().toTensor();

				if(value.dtype() == torch::kF16)
					value = value.to(torch::kF32);
				if(value.dtype() == torch::kI64)
					value = value.to(torch::kI32);

				ret.insert(key, value);
			}
		}

		this->skeleton = std::make_unique<c10::Dict<std::string, torch::Tensor>>(ret);

		this->setup->UpdateSkeletonInfo(skeleton_dict);
	});

	load_input_thread.detach();
}

void Editor::AddGuiElement(GuiElement* gui_element)
{
	this->gui_elements.push_back(gui_element);
}

torch::Device Editor::GetModuleDevice(const torch::jit::script::Module& module) const
{
	for(const auto& parameter : module.parameters())
	{
		return parameter.device();
	}
}

torch::Device Editor::GetDevice() const
{
	return this->device;
}

torch::Tensor* Editor::GetVideo()
{
	return this->video ? this->video.get() : nullptr;
}

torch::Tensor* Editor::GetOutput()
{
	return this->output ? this->output.get() : nullptr;
}

std::vector<SDL_Texture*> Editor::GetVideoTextureStream() const
{
	return this->video_texture_stream;
}

c10::Dict<std::string, torch::Tensor>* Editor::GetSkeleton()
{
	return this->skeleton ? this->skeleton.get() : nullptr;
}

torch::jit::script::Module* Editor::GetPoseEstimator()
{
	return this->pose_estimator ? this->pose_estimator.get() : nullptr;
}

torch::jit::script::Module* Editor::GetRecognizer()
{
	return this->recognizer ? this->recognizer.get() : nullptr;
}

torch::jit::script::Module* Editor::GetTransform()
{
	return this->transform ? this->transform.get() : nullptr;
}

std::vector<std::string> Editor::GetDatasetCategories() const
{
	return this->dataset_categories;
}

std::vector<std::pair<int, int>> Editor::GetCocoPairs() const
{
	return this->coco_pairs;
}

std::vector<std::pair<int, int>> Editor::GetNtuPairs() const
{
	return this->ntu_pairs;
}

void Editor::SetDatasetCategories(const std::vector<std::string>& categories)
{
	this->dataset_categories = categories;
}

void Editor::VideoInference()
{
	std::thread video_inference_thread([this]()
	{
		c10::cuda::CUDACachingAllocator::emptyCache();

		CONSOLE_LOG("Performing video inference...");

		torch::Tensor video_input = this->GetVideo()->to(this->device);

		int original_w = this->setup->GetVideoOriginalWidth();
		int original_h = this->setup->GetVideoOriginalHeight();

		int target_w = this->setup->GetVideoTargetWidth();
		int target_h = this->setup->GetVideoTargetHeight();

		Sleep(4000);

		CONSOLE_LOG("Clearing textures...");

		//RELEASE_TEXTURE_VECTOR(this->video_texture_stream);
		this->video_texture_stream.clear();

		int num_frames = video_input.size(0);

		torch::Tensor t_kps = torch::zeros({ this->setup->GetNumberOfPersons(), num_frames , 17, 3 });

		auto pairs = this->coco_pairs;

		for(int i = 0; i < num_frames; ++i)
		{
			std::vector<torch::jit::IValue> video_inputs;
			torch::Tensor in_frame = video_input[i].unsqueeze(0);
			video_inputs.push_back(in_frame);

			std::unique_ptr<torch::Tensor> pose_estimator_output;

			try
			{
				pose_estimator_output = std::make_unique<torch::Tensor>(this->GetPoseEstimator()->forward(video_inputs).toTensor());
			}
			catch(const std::runtime_error& e)
			{
				CONSOLE_LOG("Runtime error occurred: %s", e.what());
			}

			if(pose_estimator_output)
			{
				*pose_estimator_output = pose_estimator_output->squeeze().transpose(0, 1).detach();

				int num_detections = pose_estimator_output->size(0);

				std::vector<cv::Rect> bboxes;
				std::vector<float> scores;
				std::vector<torch::Tensor> keypoints;
				std::vector<int> indices;

				torch::Tensor bbox_tensor = pose_estimator_output->slice(1, 0, 4);
				torch::Tensor scores_tensor = pose_estimator_output->slice(1, 4, 5);
				torch::Tensor keypoints_tensor = pose_estimator_output->slice(1, 5, 57).reshape({ num_detections, 17, 3 });

				bbox_tensor.select(1, 0).sub_(bbox_tensor.select(1, 2).mul(0.5f));
				bbox_tensor.select(1, 1).sub_(bbox_tensor.select(1, 3).mul(0.5f));
				bbox_tensor.select(1, 0).div_(target_w).mul_(original_w);
				bbox_tensor.select(1, 1).div_(target_h).mul_(original_h);
				bbox_tensor.select(1, 2).div_(target_w).mul_(original_w);
				bbox_tensor.select(1, 3).div_(target_h).mul_(original_h);
				keypoints_tensor.select(2, 0).div_(target_w).mul_(original_w);
				keypoints_tensor.select(2, 1).div_(target_h).mul_(original_h);

				keypoints = keypoints_tensor.chunk(num_detections, 0);

				for(int d = 0; d < num_detections; ++d)
				{
					bboxes.push_back(cv::Rect(bbox_tensor[d][0].item<int>(), bbox_tensor[d][1].item<int>(), bbox_tensor[d][2].item<int>(), bbox_tensor[d][3].item<int>()));
					scores.push_back(scores_tensor[d].item<float>());
				}

				cv::dnn::NMSBoxes(bboxes, scores, this->setup->GetDetectionScoreThreshold(), this->setup->GetDetectionNMSThreshold(), indices);

				std::vector<cv::Rect> filtered_bboxes(indices.size());
				std::vector<torch::Tensor> filtered_keypoints(indices.size());

				std::transform(indices.begin(), indices.end(), filtered_bboxes.begin(),
					[&bboxes](const int& index) { return bboxes[index]; });
				std::transform(indices.begin(), indices.end(), filtered_keypoints.begin(),
					[&keypoints](const int& index) { return keypoints[index]; });

				cv::Mat mat = this->video_mat_stream[i];

				for(const auto& bbox : filtered_bboxes)
				{
					cv::rectangle(mat, bbox, cv::Scalar(255, 255, 0), 4);
				}

				int k_index = 0;
				for(auto keypoint : filtered_keypoints)
				{
					keypoint = keypoint.squeeze();

					t_kps.index({ k_index, i }) = keypoint;

					for(int k = 0; k < keypoint.size(0); k++)
					{
						int x = keypoint[k][0].item<int>();
						int y = keypoint[k][1].item<int>();

						cv::circle(mat, cv::Point(x, y), 5, cv::Scalar(255, 0, 0), -1);
					}

					for(const auto& pair : pairs)
					{
						int pair_a = pair.first;
						int pair_b = pair.second;

						int x1 = keypoint[pair_a][0].item<int>();
						int y1 = keypoint[pair_a][1].item<int>();
						int x2 = keypoint[pair_b][0].item<int>();
						int y2 = keypoint[pair_b][1].item<int>();

						cv::line(mat, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 2);
					}

					k_index++;
				}

				SDL_Surface* frame_surface = SDL_CreateRGBSurfaceFrom(mat.data, mat.cols, mat.rows, 24, mat.step, 0x0000FF, 0x00FF00, 0xFF0000, 0);

				SDL_SetHint("SDL_TextureAccess", "SDL_TEXTUREACCESS_STREAMING");
				this->video_texture_stream.push_back(SDL_CreateTextureFromSurface(App->renderer->renderer, frame_surface));

				SDL_FreeSurface(frame_surface);
			}
		}

		RELEASE_MATRIX_VECTOR(this->video_mat_stream);

		int num_channels = t_kps.size(3);

		for(int i = t_kps.size(0) - 1; i >= 0; --i)
		{
			if(torch::all(t_kps[i].eq(0)).item<bool>())
			{
				t_kps = torch::cat({ t_kps.slice(0, 0, i), t_kps.slice(0, i + 1) });
			}
		}

		torch::Tensor t_scores = t_kps.index({ "...", 2 });
		t_kps = t_kps.index({ "...", torch::indexing::Slice(0, num_channels - 1) });

		c10::Dict<std::string, torch::Tensor> sk;
		sk.insert("keypoint", t_kps.clone());
		sk.insert("img_shape", torch::tensor({ original_h, original_w }, { torch::kInt32 }));
		sk.insert("original_shape", torch::tensor({ original_h, original_w }, { torch::kInt32 }));
		sk.insert("total_frames", torch::tensor(num_frames, { torch::kInt32 }));
		sk.insert("keypoint_score", t_scores.clone());

		this->skeleton = std::make_unique<c10::Dict<std::string, torch::Tensor>>();
		for(auto k = sk.begin(); k != sk.end(); ++k)
		{
			this->skeleton->insert(k->key(), k->value().clone());
		}

		std::vector<torch::jit::IValue> keypoint_transform_inputs;
		keypoint_transform_inputs.push_back(sk);

		auto t = *this->GetTransform();
		torch::Tensor transformed_keypoints;

		try
		{
			transformed_keypoints = t.forward(keypoint_transform_inputs).toGenericDict().at("keypoint").toTensor();
		}
		catch(const std::runtime_error& e)
		{
			std::cout << "Runtime error occurred: " << e.what() << std::endl;
		}

		std::vector<torch::jit::IValue> keypoint_inputs;
		keypoint_inputs.push_back(transformed_keypoints.to(this->device));

		try
		{
			this->output = std::make_unique<torch::Tensor>(this->GetRecognizer()->forward(keypoint_inputs).toTensor().squeeze().softmax(0).detach().cpu());

			this->ShowResult();
		}
		catch(const std::runtime_error& e)
		{
			CONSOLE_LOG("Runtime error occurred: %s", e.what());
		}
	});

	video_inference_thread.detach();
}

void Editor::SkeletonInference()
{
	std::thread skeleton_inference_thread([this]()
	{
		c10::cuda::CUDACachingAllocator::emptyCache();

		CONSOLE_LOG("Performing skeleton inference...");

		c10::Dict<std::string, torch::Tensor> sk;
		for(auto k = this->GetSkeleton()->begin(); k != this->GetSkeleton()->end(); ++k)
		{
			sk.insert(k->key(), k->value().clone());
		}

		std::vector<torch::jit::IValue> keypoint_transform_inputs;
		keypoint_transform_inputs.push_back(sk);

		auto t = *this->GetTransform();
		torch::Tensor transformed_keypoints;

		try
		{
			transformed_keypoints = t.forward(keypoint_transform_inputs).toGenericDict().at("keypoint").toTensor();
		}
		catch(const std::runtime_error& e)
		{
			std::cout << "Runtime error occurred: " << e.what() << std::endl;
		}

		std::vector<torch::jit::IValue> keypoint_inputs;
		keypoint_inputs.push_back(transformed_keypoints.to(this->device));

		try
		{
			this->output = std::make_unique<torch::Tensor>(this->GetRecognizer()->forward(keypoint_inputs).toTensor().squeeze().softmax(0).detach().cpu());

			this->ShowResult();
		}
		catch(const std::runtime_error& e)
		{
			CONSOLE_LOG("Runtime error occurred: %s", e.what());
		}
	});

	skeleton_inference_thread.detach();
}

std::vector<std::string> Editor::LoadDatasetCategories(const SUPPORTED_DATASETS& current_dataset) const
{
	std::string path = "";

	switch(current_dataset)
	{
		case NTURGBD60:
			path = "label_maps/ntu60.txt";
			break;

		case NTURGBD120:
			path = "label_maps/ntu120.txt";
			break;

		case UCF101:
			path = "label_maps/ucf101.txt";
			break;

		case HMDB51:
			path = "label_maps/hmdb51.txt";
			break;

		case DIVING48:
			path = "label_maps/diving48.txt";
			break;

		case FINEGYM99:
			path = "label_maps/gym.txt";
			break;

		default:
			break;
	}

	return LoadLabelMap(path);
}

void Editor::ShowResult()
{
	this->dataset_categories = this->LoadDatasetCategories(this->setup->GetCurrentDataset());

	int num_probabilities = (int)this->GetOutput()->size(0);
	int num_dataset_categories = (int)this->dataset_categories.size();

	if(num_probabilities == num_dataset_categories)
	{
		int prediction_index = this->GetOutput()->argmax(0).item().toInt();

		CONSOLE_LOG("Inference result: %s", this->dataset_categories[prediction_index].c_str());
	}
	else
	{
		CONSOLE_LOG("Size of output probabilites is %d, while size of the dataset categories is %d. Please choose the corresponding dataset, so that the sizes are both the same, and inference again", num_probabilities, num_dataset_categories);
	}
}