#ifndef __RESULTS_H__
#define __RESULTS_H__

#include <vector>
#include <string>

#include "GuiElement.h"

class Results : public GuiElement
{
public:
	Results();
	~Results();

	void Update() override;
};

#endif // __RESULTS_H__