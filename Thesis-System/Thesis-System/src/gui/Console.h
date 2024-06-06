#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "GuiElement.h"

class Console : public GuiElement
{
public:
	Console();
	~Console();

	void Update() override;
	void CleanUp() override;

private:
	bool auto_scroll;
};

#endif // __CONSOLE_H__