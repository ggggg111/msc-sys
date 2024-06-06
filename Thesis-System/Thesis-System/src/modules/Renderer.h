#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "SDL.h"
#include "imgui.h"

#include "Module.h"

class Renderer : public Module
{
public:
	Renderer();
	~Renderer();

	void Start() override;
	void PreUpdate() override;
	void PostUpdate() override;
	void CleanUp() override;

	void SetBackgroundColor(const SDL_Color& color);
	void Blit(SDL_Texture* texture, const int& x, const int& y, const SDL_Rect* section) const;

public:
	SDL_Renderer* renderer;
	SDL_Rect viewport;
	SDL_Color background_color;
};

#endif /* __RENDERER_H__ */