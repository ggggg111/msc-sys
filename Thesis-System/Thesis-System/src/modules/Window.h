#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <iostream>

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Renderer;

class Window : public Module
{
public:
	Window(const std::string& name);
	~Window();

	void Start() override;
	void CleanUp() override;

public:
	SDL_Window* window;

	std::string name;

	int width;
	int height;
};

#endif /* __WINDOW_H__ */