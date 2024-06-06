#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <list>
#include <vector>
#include <string>
#include <memory>
#include <thread>

#include "torch/torch.h"
#include "torch/script.h"
#include "opencv2/opencv.hpp"
#include "SDL.h"
#include "SDL_image.h"

#include "Module.h"
#include "gui/Setup.h"

class GuiElement;
class MainMenuBar;
class Console;
class Results;
class Visualizer;

class Editor : public Module
{
public:
	Editor();
	~Editor();

	void Start() override;
	void PreUpdate() override;
	void CleanUp() override;

	void DrawGUI();
	void LoadPoseEstimator(const std::string& path);
	void LoadRecognizer(const std::string& path);
	void LoadTransform(const std::string& path);
	void LoadVideo(const std::string& path, const cv::Size& size);
	void LoadSkeleton(const std::string& path);
	void VideoInference();
	void SkeletonInference();

	torch::jit::script::Module* GetPoseEstimator();
	torch::jit::script::Module* GetRecognizer();
	torch::jit::script::Module* GetTransform();
	torch::Tensor* GetVideo();
	c10::Dict<std::string, torch::Tensor>* GetSkeleton();
	torch::Tensor* GetOutput();
	torch::Device GetDevice() const;
	std::vector<SDL_Texture*> GetVideoTextureStream() const;
	std::vector<std::string> GetDatasetCategories() const;
	std::vector<std::pair<int, int>> GetCocoPairs() const;
	std::vector<std::pair<int, int>> GetNtuPairs() const;

	void SetDatasetCategories(const std::vector<std::string>& categories);

private:
	void AddGuiElement(GuiElement* gui_element);
	torch::Device GetModuleDevice(const torch::jit::script::Module& module) const;
	std::vector<std::string> LoadDatasetCategories(const SUPPORTED_DATASETS& current_dataset) const;
	void ShowResult();

public:
	MainMenuBar* main_menu_bar;
	Console* console;
	Setup* setup;
	Results* results;
	Visualizer* visualizer;

private:
	std::list<GuiElement*> gui_elements;

	std::unique_ptr<torch::jit::script::Module> pose_estimator;
	std::unique_ptr<torch::jit::script::Module> recognizer;
	std::unique_ptr<torch::jit::script::Module> transform;
	std::unique_ptr<torch::Tensor> video;
	std::unique_ptr<c10::Dict<std::string, torch::Tensor>> skeleton;
	std::unique_ptr<torch::Tensor> output;
	torch::Device device;

	std::vector<std::string> dataset_categories;

	std::vector<std::pair<int, int>> coco_pairs;
	std::vector<std::pair<int, int>> ntu_pairs;

	std::vector<SDL_Texture*> video_texture_stream;
	std::vector<cv::Mat> video_mat_stream;
};

#endif // __EDITOR_H__