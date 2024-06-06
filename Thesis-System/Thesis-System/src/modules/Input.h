#ifndef __INPUT_H__
#define __INPUT_H__

#include "SDL.h"

#include "Module.h"

#define MAX_KEYS 300

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{
public:
	Input();
	~Input();

	void Start() override;
	void PreUpdate() override;
	void CleanUp() override;

public:
	KEY_STATE keyboard[MAX_KEYS];

private:
	const Uint8* keys = nullptr;
};

#endif /* __INPUT_H__ */