#ifndef __VISUALIZER_H__
#define __VISUALIZER_H__

#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#include "GuiElement.h"

#define COCO_NUM_KEYPOINTS 17
#define NTURGBD_NUM_KEYPOINTS 25

enum VISUALIZATION_MODE
{
	BONES = 0,
	JOINTS,
};

class Visualizer : public GuiElement
{
public:
	Visualizer();
	~Visualizer();

	void Update() override;

	void SetSkeletonRatio(const float& ratio);

private:
	VISUALIZATION_MODE visualization_mode = BONES;
	float skeleton_visualization_ratio = 1.0f;
};

#endif // __VISUALIZER_H__