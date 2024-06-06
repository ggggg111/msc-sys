#include "utils/Log.h"
#include "utils/Globals.h"
#include "TorchLoader.h"

TorchLoader::TorchLoader()
{

}

TorchLoader::~TorchLoader()
{

}

void TorchLoader::Start()
{
	torch::manual_seed(0);
	torch::cuda::manual_seed_all(0);
}

void TorchLoader::CleanUp()
{

}

bool TorchLoader::IsCudaAvailable() const
{
	return torch::cuda::is_available();
}

std::unique_ptr<torch::jit::script::Module> TorchLoader::LoadModule(const std::string& path, const torch::Device& device) const
{
	try
	{
		std::unique_ptr<torch::jit::script::Module> module = std::make_unique<torch::jit::script::Module>(torch::jit::load(path, device));

		module->eval();

		CONSOLE_LOG("Loaded model from path %s", path.c_str());

		return module;
	}
	catch(const c10::Error& e)
	{
		CONSOLE_LOG("Cant't load model from path %s, error %s", path.c_str(), e.msg().c_str());
	}
	catch(const std::exception& e)
	{
		CONSOLE_LOG("Cant't load model from path %s, exception %s", path.c_str(), e.what());
	}
}

[[deprecated]]
std::vector<torch::jit::IValue> TorchLoader::GetSampleInputs(const torch::Device& device)
{
	std::vector<torch::jit::IValue> inputs;
	inputs.push_back(torch::ones({ 1, 3, 100, 17, 2 }, { device }).to(torch::kFloat32));

	return inputs;
}

std::unique_ptr<torch::Tensor> TorchLoader::LoadVideo(const std::string& path, const cv::Size& size, cv::Size& original_size, std::vector<SDL_Texture*>& video_texture_stream, std::vector<cv::Mat>& video_mat_stream, SDL_Renderer* renderer) const
{
	RELEASE_TEXTURE_VECTOR(video_texture_stream);
	RELEASE_MATRIX_VECTOR(video_mat_stream);

	cv::VideoCapture cap(path);

	if(!cap.isOpened())
	{
		CONSOLE_LOG("Can't open input video %s", path.c_str());
	}

	original_size = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));

	torch::Tensor video_tensor;
	cv::Mat frame;

	while(cap.read(frame))
	{
		video_mat_stream.push_back(frame);

		cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(frame.data, frame.cols, frame.rows, 24, frame.step, 0x0000FF, 0x00FF00, 0xFF0000, 0);

		SDL_SetHint("SDL_TextureAccess", "SDL_TEXTUREACCESS_STREAMING");
		video_texture_stream.push_back(SDL_CreateTextureFromSurface(renderer, surface));

		SDL_FreeSurface(surface);

		cv::resize(frame, frame, size);

		torch::Tensor frame_tensor = torch::from_blob(frame.data, { 1, frame.rows, frame.cols, 3 }, torch::kByte)
			.permute({ 0, 3, 1, 2 }).to(torch::kFloat).div(255.0f);

		if(!video_tensor.defined())
		{
			video_tensor = frame_tensor;
		}
		else
		{
			video_tensor = torch::cat({ video_tensor, frame_tensor }, 0);
		}

		frame.release();
	}

	cap.release();

	CONSOLE_LOG("Loaded input video from path %s with width %d and height %d", path.c_str(), size.width, size.height);

	return std::make_unique<torch::Tensor>(video_tensor);
}

std::vector<char> TorchLoader::GetBytes(const std::string& path) const
{
	std::ifstream input(path, std::ios::binary);

	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));

	input.close();

	return bytes;
}

[[deprecated]]
void TorchLoader::LimitNumPersons(torch::Tensor& keypoints, const int& num_persons, const int& max_persons)
{
	if(num_persons > max_persons)
	{
		keypoints = keypoints.index_select(0, torch::arange(max_persons, torch::kInt));
	}
}

[[deprecated]]
void TorchLoader::AddZeroPadding(torch::Tensor& keypoints, const int& num_persons, const int& max_persons)
{
	if(num_persons < max_persons)
	{
		auto keypoints_shape = keypoints.sizes().vec();
		keypoints_shape[0] = max_persons - num_persons;

		keypoints = torch::cat({ keypoints, torch::zeros(keypoints_shape) }, 0);
	}
}

[[deprecated]]
void TorchLoader::NormalizeKeypoints(torch::Tensor& keypoints, const float& width, const float& height)
{
	keypoints.index({ "...", 0 }) = (keypoints.index({ "...", 0 }) - ((float)width * 0.5f)) / ((float)width * 0.5f);
	keypoints.index({ "...", 1 }) = (keypoints.index({ "...", 1 }) - ((float)height * 0.5f)) / ((float)height * 0.5f);
}

[[deprecated]]
void TorchLoader::UniformSample(torch::Tensor& keypoints, const int& clip_len, const int& total_frames)
{
	float split_size = (float)(total_frames - 1) / (float)clip_len;

	std::vector<torch::Tensor> indices;
	indices.reserve(clip_len);

	for(int count = 0; count < clip_len; ++count)
	{
		int start = (int)(std::round(split_size * count));
		int end = (int)(std::round(split_size * (count + 1)));

		indices.push_back(torch::randint(start, end + 1, { 1 }));
	}

	torch::Tensor indices_tensor = torch::stack(indices, -1).squeeze().to(torch::kInt);
	std::cout << keypoints.sizes() << std::endl;
	std::cout << indices_tensor.sizes() << std::endl;
	keypoints = keypoints.index_select(1, indices_tensor);
}

c10::IValue TorchLoader::LoadPickleTensor(const std::string& path) const
{
	try
	{
		c10::IValue skeleton = torch::pickle_load(this->GetBytes(path));

		CONSOLE_LOG("Loaded input skeleton from path %s", path.c_str());

		return skeleton;
	}
	catch(const c10::Error& e)
	{
		CONSOLE_LOG("Cant't load input skeleton from path %s, error %s", path.c_str(), e.msg().c_str());
	}
	catch(const std::exception& e)
	{
		CONSOLE_LOG("Cant't load input skeleton from path %s, exception %s", path.c_str(), e.what());
	}
}

[[deprecated]]
void TorchLoader::TransformSkeletonsFromVideo(torch::Tensor& skeleton, const int& width, const int& height, const int& clip_len)
{
	TorchLoader::NormalizeKeypoints(skeleton, (float)width, (float)height);
	TorchLoader::UniformSample(skeleton, clip_len, skeleton.size(1));

	skeleton = skeleton.unsqueeze(0).contiguous().cpu();
}

[[deprecated]]
void TorchLoader::TransformSkeletonsFromSkeleton(torch::Tensor& skeleton, const SkeletonInfo& skeleton_info, const int& clip_len, const int& max_persons)
{
	skeleton = skeleton.to(torch::kFloat);

	TorchLoader::NormalizeKeypoints(skeleton, (float)skeleton_info.original_width, (float)skeleton_info.original_height);

	int num_persons = skeleton.size(0);

	TorchLoader::LimitNumPersons(skeleton, num_persons, max_persons);
	TorchLoader::AddZeroPadding(skeleton, num_persons, max_persons);

	TorchLoader::UniformSample(skeleton, clip_len, skeleton_info.total_frames);

	skeleton = skeleton.unsqueeze(0).contiguous().cpu();
}

std::vector<std::pair<int, int>> TorchLoader::LoadCocoPairs()
{
	return {
		{0, 1},
		{0, 2},
		{1, 3},
		{2, 4},
		{5, 6},
		{5, 7},
		{5, 11},
		{7, 9},
		{6, 8},
		{6, 12},
		{8, 10},
		{11, 12},
		{11, 13},
		{13, 15},
		{12, 14},
		{14, 16},
	};
}

std::vector<std::pair<int, int>> TorchLoader::LoadNtuPairs()
{
	return {
		{0, 1},
		{1, 20},
		{2, 20},
		{3, 2},
		{4, 20},
		{5, 4},
		{6, 5},
		{7, 6},
		{8, 20},
		{9, 8},
		{10, 9},
		{11, 10},
		{12, 0},
		{13, 12},
		{14, 13},
		{15, 14},
		{16, 0},
		{17, 16},
		{18, 17},
		{19, 18},
		{21, 22},
		{20, 20},
		{22, 7},
		{23, 24},
		{24, 11},
	};
}