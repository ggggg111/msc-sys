#ifndef __MODULE_H__
#define __MODULE_H__

class Module
{
public:
	Module() {}
	virtual ~Module() {}

	virtual void Start() {}
	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}
	virtual void CleanUp() {}
};

#endif /* __MODULE_H__ */