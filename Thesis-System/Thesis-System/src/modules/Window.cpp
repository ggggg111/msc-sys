#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "utils/Log.h"
#include "Window.h"

Window::Window(const std::string& name)
	: Module(), window(nullptr), name(name), width(-1), height(-1)
{

}

Window::~Window()
{

}

void Window::Start()
{
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		CONSOLE_LOG("SDL can't be initialized. SDL_Init error: %s", SDL_GetError());
	}

	int image_format_flags = IMG_INIT_JPG | IMG_INIT_PNG;

	if((IMG_Init(image_format_flags) & image_format_flags) != image_format_flags)
	{
		CONSOLE_LOG("SDL_image can't be initialized. IMG_Init error: %s", IMG_GetError());
	}

	SDL_DisplayMode display_mode;
	if(SDL_GetDesktopDisplayMode(0, &display_mode) != 0)
	{
		CONSOLE_LOG("Can't get desktop display mode. SDL_GetDesktopDisplayMode error: %s", SDL_GetError());
	}

	this->width = display_mode.w;
	this->height = display_mode.h;

	CONSOLE_LOG("Window resolution and refresh rate: %dx%d@%dHz", this->width, this->height, display_mode.refresh_rate);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN);

	this->window = SDL_CreateWindow(this->name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, window_flags);
	if(this->window == nullptr)
	{
		CONSOLE_LOG("Window can't be created. SDL_CreateWindow error: %s", SDL_GetError());
	}
}

void Window::CleanUp()
{
	SDL_DestroyWindow(this->window);
	IMG_Quit();
	SDL_Quit();
}