#include <stdio.h>

#include "SDL.h"

#include "Application.h"
#include "modules/Window.h"
#include "modules/Renderer.h"

Application* App = nullptr;

int main(int argc, char* args[])
{
	App = new Application("MSc Thesis");
	App->Run();

	delete App;

	return 0;
}