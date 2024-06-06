#ifndef __TORCH_LOADER_H__
#define __TORCH_LOADER_H__

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "torch/torch.h"
#include "torch/script.h"
#include "opencv2/opencv.hpp"
#include "SDL.h"
#include "SDL_image.h"

#include "Module.h"

#include "gui/Setup.h"

struct SDL_Renderer;

class TorchLoader : public Module
{
public:
	TorchLoader();
	~TorchLoader();

	void Start() override;
	void CleanUp() override;

	bool IsCudaAvailable() const;

	std::unique_ptr<torch::jit::script::Module> LoadModule(const std::string& path, const torch::Device& device) const;
	std::unique_ptr<torch::Tensor> LoadVideo(const std::string& path, const cv::Size& size, cv::Size& original_size, std::vector<SDL_Texture*>& video_texture_stream, std::vector<cv::Mat>& video_mat_stream, SDL_Renderer* renderer) const;
	c10::IValue LoadPickleTensor(const std::string& path) const;

	[[deprecated]]
	std::vector<torch::jit::IValue> GetSampleInputs(const torch::Device& device);
	[[deprecated]]
	static void TransformSkeletonsFromVideo(torch::Tensor& skeleton, const int& width, const int& height, const int& clip_len);
	[[deprecated]]
	static void TransformSkeletonsFromSkeleton(torch::Tensor& skeleton, const SkeletonInfo& skeleton_info, const int& clip_len, const int& max_persons);

public:
	static std::vector<std::pair<int, int>> LoadCocoPairs();
	static std::vector<std::pair<int, int>> LoadNtuPairs();

private:
	std::vector<char> GetBytes(const std::string& path) const;

	[[deprecated]]
	static void LimitNumPersons(torch::Tensor& keypoints, const int& num_persons, const int& max_persons);
	[[deprecated]]
	static void AddZeroPadding(torch::Tensor& keypoints, const int& num_persons, const int& max_persons);
	[[deprecated]]
	static void NormalizeKeypoints(torch::Tensor& keypoints, const float& width, const float& height);
	[[deprecated]]
	static void UniformSample(torch::Tensor& keypoints, const int& clip_len, const int& total_frames);
};

#endif /* __TORCH_LOADER_H__ */