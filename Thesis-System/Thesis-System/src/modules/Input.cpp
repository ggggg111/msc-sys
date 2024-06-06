#include <iostream>

#include "imgui_impl_sdl2.h"

#include "utils/Log.h"
#include "Application.h"
#include "Input.h"
#include "Editor.h"
#include "Renderer.h"
#include "Window.h"

Input::Input() : Module()
{
	for(int i = 0; i < MAX_KEYS; ++i)
		this->keyboard[i] = KEY_IDLE;
}

Input::~Input()
{

}

void Input::Start()
{
	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		CONSOLE_LOG("SDL events can't be initialized. SDL_InitSubSystem error: %s\n", SDL_GetError());
	}
}

void Input::PreUpdate()
{
	SDL_PumpEvents();

	this->keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(this->keys[i] == 1)
		{
			if(this->keyboard[i] == KEY_IDLE)
				this->keyboard[i] = KEY_DOWN;
			else
				this->keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(this->keyboard[i] == KEY_REPEAT || this->keyboard[i] == KEY_DOWN)
				this->keyboard[i] = KEY_UP;
			else
				this->keyboard[i] = KEY_IDLE;
		}
	}

	SDL_Event e;
	while(SDL_PollEvent(&e) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_QUIT:
			{
				App->Exit();

				break;
			}

			case SDL_DROPFILE:
			{
				std::string dropped_file_path = e.drop.file;

				CONSOLE_LOG("User dropped file %s", dropped_file_path.c_str());

				break;
			}

			default:
				break;
		}
	}
}

void Input::CleanUp()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
}