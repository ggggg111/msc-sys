#ifndef __SETUP_H__
#define __SETUP_H__

#include <string>
#include <vector>
#include <memory>

#include "torch/torch.h"

#include "GuiElement.h"

struct VideoInfo
{
	std::string name;
	int num_frames;
	int original_w;
	int original_h;
	int target_w = 640;
	int target_h = 640;
};

struct SkeletonInfo
{
	std::string frame_dir;
	int label;
	int image_width = -1;
	int image_height = -1;
	int original_width = -1;
	int original_height = -1;
	int total_frames;
	std::string dataset_name;
	std::vector<std::string> categories;
};

enum SUPPORTED_DATASETS
{
	NTURGBD60,
	NTURGBD120,
	UCF101,
	HMDB51,
	DIVING48,
	FINEGYM99
};

class Setup : public GuiElement
{
public:
	Setup();
	~Setup();

	void Update() override;

	void UpdatePoseEstimatorName(const std::string& path);
	void UpdateRecognizerName(const std::string& path);
	void UpdateTransformName(const std::string& path);
	void UpdateVideoInfo(const std::string& path, const torch::Tensor& sample, const cv::Size& original_size);
	void UpdateSkeletonInfo(const c10::Dict<c10::IValue, c10::IValue>& sample);

	int GetVideoNumFrames() const;
	int GetVideoOriginalWidth() const;
	int GetVideoOriginalHeight() const;
	int GetVideoTargetWidth() const;
	int GetVideoTargetHeight() const;
	float GetDetectionScoreThreshold() const;
	float GetDetectionNMSThreshold() const;
	int GetNumberOfPersons() const;
	SUPPORTED_DATASETS GetCurrentDataset() const;
	SkeletonInfo* GetSkeletonInfo();

private:
	void AssignCurrentDatasetFromSkeleton(const std::string& dataset_name);

private:
	std::string pose_estimator_name;
	std::string recognizer_name;
	std::string transform_name;
	std::string current_device;

	VideoInfo video_info;
	std::unique_ptr<SkeletonInfo> skeleton_info;

	float detection_score_threshold = 0.7f;
	float detection_nms_threshold = 0.6f;
	int num_persons = 2;

	SUPPORTED_DATASETS current_dataset = NTURGBD60;
};

#endif // __SETUP_H__