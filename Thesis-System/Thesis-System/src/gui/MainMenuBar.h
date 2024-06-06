#ifndef __MAIN_MENU_BAR_H__
#define __MAIN_MENU_BAR_H__

#include "GuiElement.h"

class MainMenuBar : public GuiElement
{
public:
	MainMenuBar();
	~MainMenuBar();

	void Update() override;
};

#endif // __MAIN_MENU_BAR_H__