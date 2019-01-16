#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "Point.h"
#include "GL/glew.h"
#include <vector>

class GameObject;
class ComponentCamera;

enum class Directions {
	UP,
	DOWN,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool            Init();
	update_status   PreUpdate();
	void Zoom();
	void PickGameObject();
	update_status   Update();
	bool            CleanUp();

	void Focus();

	ComponentCamera* sceneCamera = nullptr;
	ComponentCamera* selectedCamera = nullptr;

	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;

private:

	void Move(Directions dir);
	void MouseUpdate();
	void Orbit();
	void SetPlaneDistances(float nearDist, float farDist);

	std::vector<GameObject*> objectsPossiblePick;
};

#endif /* __ModuleCamera_h__ */
