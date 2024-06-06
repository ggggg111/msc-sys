#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <iostream>
#include <list>
#include <queue>

class Module;
class Window;
class Renderer;
class UserInterface;
class Input;
class Editor;
class TorchLoader;

class Application
{
public:
	Application(const std::string& name);
	~Application();

	void Run();

	void Start();
	void Update();
	void CleanUp();

	void RequestBrowser(const std::string& url) const;

	bool IsRunning() const;
	void Exit();

private:
	void AddModule(Module* module);

public:
	Window* window;
	Renderer* renderer;
	UserInterface* user_interface;
	Input* input;
	Editor* editor;
	TorchLoader* torch_loader;

private:
	bool running;
	std::string name;
	std::list<Module*> modules;
};

extern Application* App;

#endif /* __APPLICATION_H__ */