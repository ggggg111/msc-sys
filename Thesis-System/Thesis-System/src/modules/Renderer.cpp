#include <iostream>
#include <cmath>

#include "utils/Log.h"
#include "Application.h"
#include "Renderer.h"
#include "Window.h"
#include "Editor.h"
#include "UserInterface.h"

Renderer::Renderer()
	: Module(), renderer(nullptr), viewport({ 0, 0, 0, 0 }), background_color({ 0, 0, 0, 0 })
{

}

Renderer::~Renderer()
{

}

void Renderer::Start()
{
	this->renderer = SDL_CreateRenderer(App->window->window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);

	if(this->renderer == nullptr)
	{
		CONSOLE_LOG("Renderer can't be created. SDL_CreateRenderer error: %s", SDL_GetError());
	}

	SDL_RenderGetViewport(this->renderer, &this->viewport);
}

void Renderer::PreUpdate()
{
	SDL_RenderClear(this->renderer);
}

void Renderer::PostUpdate()
{
	SDL_SetRenderDrawColor(this->renderer, this->background_color.r, this->background_color.g, this->background_color.g, this->background_color.a);

	App->user_interface->Draw();

	SDL_RenderPresent(this->renderer);
}

void Renderer::CleanUp()
{
	SDL_DestroyRenderer(this->renderer);
}

void Renderer::SetBackgroundColor(const SDL_Color& color)
{
	this->background_color = color;
}

void Renderer::Blit(SDL_Texture* texture, const int& x, const int& y, const SDL_Rect* section) const
{
	SDL_Rect rect = { x, y, NULL, NULL };

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, 0.0, nullptr, SDL_FLIP_NONE) != 0)
	{
		CONSOLE_LOG("Cannot blit to screen. SDL_RenderCopyEx error: %s", SDL_GetError());
	}
}