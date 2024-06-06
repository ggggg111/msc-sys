#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#ifndef IM_CLAMP
#define IM_CLAMP(V, MN, MX) ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
#endif

#include <string>

#include "Module.h"

class UserInterface : public Module
{
public:
	UserInterface();
	~UserInterface();

	void Start() override;
	void CleanUp() override;

	void Draw();
	static void HelpMarker(const std::string& description);

private:
	void SetTheme();
};

#endif /* __USER_INTERFACE_H__ */