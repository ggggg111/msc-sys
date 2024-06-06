#ifndef __GUI_ELEMENT_H__
#define __GUI_ELEMENT_H__

class GuiElement
{
public:
	GuiElement() {}
	virtual ~GuiElement() {}

	virtual void Start() {}
	virtual void Update() {}
	virtual void CleanUp() {}

public:
	bool active = true;
};

#endif /* __GUI_ELEMENT_H__ */