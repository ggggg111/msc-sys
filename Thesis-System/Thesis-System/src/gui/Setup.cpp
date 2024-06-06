#include <iostream>
#include <filesystem>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "utils/Log.h"
#include "modules/TorchLoader.h"
#include "modules/Editor.h"
#include "modules/UserInterface.h"
#include "Application.h"
#include "Setup.h"
#include "gui/Visualizer.h"

Setup::Setup() : GuiElement()
{

}

Setup::~Setup()
{

}

void Setup::Update()
{
	if(ImGui::Begin("Setup"))
	{
		ImGui::Text("Device: %s", App->editor->GetDevice().str().c_str());

		ImGui::Separator();

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();

		if(ImGui::BeginTabBar("TabBarMode"))
		{
			if(ImGui::BeginTabItem("Video"))
			{
				if(ImGui::Button("Load Pose Estimator", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Pose Estimator", "Choose File", ".pt,.pth", "./checkpoints/pose_estimators/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Pose Estimator"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading pose estimator from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadPoseEstimator(file_path_name);
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading pose estimator file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::TextUnformatted("Pose Estimator:");
				ImGui::TextWrapped(this->pose_estimator_name.c_str());

				ImGui::Separator();

				if(ImGui::Button("Load Recognizer", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Recognizer", "Choose File", ".pt,.pth", "./checkpoints/recognizers/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Recognizer"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading recognizer from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadRecognizer(file_path_name);
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading recognizer file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::TextUnformatted("Recognizer:");
				ImGui::TextWrapped(this->recognizer_name.c_str());

				ImGui::Separator();

				if(ImGui::Button("Load Video", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Video", "Choose File", ".avi,.mp4", "./videos/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Video"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading input video from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadVideo(file_path_name, { this->video_info.target_w, this->video_info.target_h });
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading video file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::Text("Video settings:");
				ImGui::SameLine();
				UserInterface::HelpMarker("Please note the default YOLO models expect a width and height of 640 pixels");

				ImGui::SetNextItemWidth(100.0f);
				ImGui::DragInt("Target weight", &this->video_info.target_w, 1.0f, 100, 2000);
				ImGui::SetNextItemWidth(100.0f);
				ImGui::DragInt("Target height", &this->video_info.target_h, 1.0f, 100, 2000);

				ImGui::Separator();

				ImGui::Text("Video:");

				if(App->editor->GetVideo())
				{
					ImGui::TextWrapped("%s", this->video_info.name.c_str());
					ImGui::TextWrapped("Num frames: %d", this->video_info.num_frames);
					ImGui::TextWrapped("Original width: %d", this->video_info.original_w);
					ImGui::TextWrapped("Original height: %d", this->video_info.original_h);
					ImGui::TextWrapped("Target width: %d", this->video_info.target_w);
					ImGui::TextWrapped("Target height: %d", this->video_info.target_h);
				}

				ImGui::Separator();

				if(ImGui::Button("Load Transform", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Transform", "Choose File", ".pt,.pth", "./transforms/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Transform"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading transform from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadTransform(file_path_name);
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading transform file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::TextUnformatted("Transform:");
				ImGui::TextWrapped(this->transform_name.c_str());

				ImGui::Separator();

				if(ImGui::Button("Inference", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					if(App->editor->GetPoseEstimator() && App->editor->GetRecognizer() && App->editor->GetVideo())
					{
						App->editor->VideoInference();
					}
					else
					{
						CONSOLE_LOG("Please load a pose estimator, a recognizer, and an input video first");
					}
				}

				ImGui::Separator();

				const char* datasets[] = { "NTURGB+D60", "NTURGB+D120", "UCF101", "HMDB51", "Diving48", "FineGym99" };
				ImGui::Combo("Dataset", (int*)&this->current_dataset, datasets, IM_ARRAYSIZE(datasets));
				ImGui::SameLine();
				UserInterface::HelpMarker("Select a dataset to obtain the inference category label from the input sample. Make sure it's the same as the used in the recognizer");

				ImGui::SetNextItemWidth(100.0f);
				ImGui::SliderFloat("Detection score threshold", &this->detection_score_threshold, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				UserInterface::HelpMarker("The detection threshold is used to detect human persons given the detected object probability distribution");
				ImGui::SetNextItemWidth(100.0f);
				ImGui::SliderFloat("Detection NMS threshold", &this->detection_nms_threshold, 0.0f, 1.0f, "%.2f");
				ImGui::SameLine();
				UserInterface::HelpMarker("The non-maximum-suppression threshold is used to select one bounding box from many overlapping bounding boxes");

				ImGui::SetNextItemWidth(100.0f);
				ImGui::SliderInt("Number of persons", &this->num_persons, 1, 10);
				ImGui::SameLine();
				UserInterface::HelpMarker("The top most accurate number of persons detected in the video");

				ImGui::EndTabItem();
			}

			if(ImGui::BeginTabItem("Skeleton"))
			{
				if(ImGui::Button("Load Recognizer", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Recognizer", "Choose File", ".pt,.pth", "./checkpoints/recognizers/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Recognizer"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading recognizer from path %s...", file_path_name.c_str());

						App->editor->LoadRecognizer(file_path_name);
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::TextUnformatted("Recognizer:");
				ImGui::TextWrapped(this->recognizer_name.c_str());

				ImGui::Separator();

				if(ImGui::Button("Load Skeleton", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Skeleton", "Choose File", ".pkl", "./skeletons/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Skeleton"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading input skeleton from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadSkeleton(file_path_name);
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading skeleton file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::Text("Skeleton:");

				if(App->editor->GetSkeleton() && this->GetSkeletonInfo())
				{
					ImGui::TextWrapped("%s", this->skeleton_info->frame_dir.c_str());
					ImGui::TextWrapped("Label: %s", this->skeleton_info->categories[this->skeleton_info->label].c_str());

					if(this->skeleton_info->image_width != -1)
						ImGui::TextWrapped("Image width: %d", this->skeleton_info->image_width);
					if(this->skeleton_info->image_height != -1)
						ImGui::TextWrapped("Image height: %d", this->skeleton_info->image_height);

					if(this->skeleton_info->original_width != -1)
						ImGui::TextWrapped("Original width: %d", this->skeleton_info->original_width);
					if(this->skeleton_info->original_height != -1)
						ImGui::TextWrapped("Original height: %d", this->skeleton_info->original_height);

					ImGui::TextWrapped("Total frames: %d", this->skeleton_info->total_frames);
					ImGui::TextWrapped("Dataset: %s", this->skeleton_info->dataset_name.c_str());
					ImGui::TextWrapped("Num categories: %d", (int)this->skeleton_info->categories.size());
				}

				ImGui::Separator();

				if(ImGui::Button("Load Transform", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					ImGuiFileDialog::Instance()->OpenDialog("Load Transform", "Choose File", ".pt,.pth", "./transforms/",
						1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize({ 1400, 600 }, ImGuiCond_Once);

				if(ImGuiFileDialog::Instance()->Display("Load Transform"))
				{
					if(ImGuiFileDialog::Instance()->IsOk())
					{
						std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

						CONSOLE_LOG("Loading transform from path %s...", file_path_name.c_str());

						try
						{
							App->editor->LoadTransform(file_path_name);
						}
						catch(const c10::Error& e)
						{
							CONSOLE_LOG("Error loading transform file: %s", e.what());
						}
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::Separator();

				ImGui::TextUnformatted("Transform:");
				ImGui::TextWrapped(this->transform_name.c_str());

				ImGui::Separator();

				if(ImGui::Button("Inference", ImVec2(-FLT_MIN, 2 * ImGui::GetTextLineHeightWithSpacing())))
				{
					if(App->editor->GetRecognizer() && App->editor->GetSkeleton())
					{
						App->editor->SkeletonInference();
					}
					else
					{
						CONSOLE_LOG("Please load a recognizer and an input skeleton first");
					}
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

void Setup::UpdatePoseEstimatorName(const std::string& path)
{
	this->pose_estimator_name = std::filesystem::path(path).filename().string();
}

void Setup::UpdateRecognizerName(const std::string& path)
{
	this->recognizer_name = std::filesystem::path(path).filename().string();
}

void Setup::UpdateTransformName(const std::string& path)
{
	this->transform_name = std::filesystem::path(path).filename().string();
}

void Setup::UpdateVideoInfo(const std::string& path, const torch::Tensor& sample, const cv::Size& original_size)
{
	auto sample_sizes = sample.sizes();

	this->video_info.name = std::filesystem::path(path).filename().string();
	this->video_info.num_frames = sample_sizes[0];
	this->video_info.original_w = original_size.width;
	this->video_info.original_h = original_size.height;
}

void Setup::UpdateSkeletonInfo(const c10::Dict<c10::IValue, c10::IValue>& sample)
{
	this->skeleton_info = std::make_unique<SkeletonInfo>();

	if(sample.contains("img_shape"))
	{
		this->skeleton_info->image_width = sample.at("img_shape").toTensor()[1].item<int>();
		this->skeleton_info->image_height = sample.at("img_shape").toTensor()[0].item<int>();

		App->editor->visualizer->SetSkeletonRatio((float)this->skeleton_info->image_height / (float)this->skeleton_info->image_width);
	}
	else
	{
		App->editor->visualizer->SetSkeletonRatio(1.0f);
	}

	if(sample.contains("original_shape"))
	{
		this->skeleton_info->original_width = sample.at("original_shape").toTensor()[1].item<int>();
		this->skeleton_info->original_height = sample.at("original_shape").toTensor()[0].item<int>();
	}

	std::string dataset_name = sample.at("dataset_name").toStringRef();

	this->skeleton_info->frame_dir = sample.at("frame_dir").toStringRef();
	this->skeleton_info->label = (int)sample.at("label").toTensor().item<int>();
	this->skeleton_info->total_frames = (int)sample.at("total_frames").toTensor().item<int>();
	this->skeleton_info->dataset_name = dataset_name;

	auto categories_tuple = sample.at("categories").toTuple()->elements();
	this->skeleton_info->categories.clear();
	for(const auto& element : categories_tuple)
	{
		if(element.isString())
		{
			this->skeleton_info->categories.push_back(element.toStringRef());
		}
	}

	this->AssignCurrentDatasetFromSkeleton(dataset_name);
	App->editor->SetDatasetCategories(this->skeleton_info->categories);
}

void Setup::AssignCurrentDatasetFromSkeleton(const std::string& dataset_name)
{
	if(dataset_name.compare("ntu60") == 0)
		this->current_dataset = SUPPORTED_DATASETS::NTURGBD60;
	else if(dataset_name.compare("ntu120") == 0)
		this->current_dataset = SUPPORTED_DATASETS::NTURGBD120;
	else if(dataset_name.compare("ucf101") == 0)
		this->current_dataset = SUPPORTED_DATASETS::UCF101;
	else if(dataset_name.compare("hmdb51") == 0)
		this->current_dataset = SUPPORTED_DATASETS::HMDB51;
	else if(dataset_name.compare("diving48") == 0)
		this->current_dataset = SUPPORTED_DATASETS::DIVING48;
	else if(dataset_name.compare("gym") == 0)
		this->current_dataset = SUPPORTED_DATASETS::FINEGYM99;
	else
		throw std::runtime_error("Dataset not supported");
}

int Setup::GetVideoNumFrames() const
{
	return this->video_info.num_frames;
}

int Setup::GetVideoOriginalWidth() const
{
	return this->video_info.original_w;
}

int Setup::GetVideoOriginalHeight() const
{
	return this->video_info.original_h;
}

int Setup::GetVideoTargetWidth() const
{
	return this->video_info.target_w;
}

int Setup::GetVideoTargetHeight() const
{
	return this->video_info.target_h;
}

float Setup::GetDetectionScoreThreshold() const
{
	return this->detection_score_threshold;
}

float Setup::GetDetectionNMSThreshold() const
{
	return this->detection_nms_threshold;
}

int Setup::GetNumberOfPersons() const
{
	return this->num_persons;
}

SUPPORTED_DATASETS Setup::GetCurrentDataset() const
{
	return this->current_dataset;
}

SkeletonInfo* Setup::GetSkeletonInfo()
{
	return this->skeleton_info ? this->skeleton_info.get() : nullptr;
}