#pragma once

#include "Camera.h"
#include <vector>

class CameraManager
{
private:
	std::vector<Camera*> _cameras;
	Camera* _activeCam;
	int _curCamera;

public:
	CameraManager();
	void AddCamera(Camera* cam);
	void SwitchCam(bool up);
	void Init();
	Camera* GetActiveCamera() { return _activeCam; }
	Camera* GetCamera(int index) { return _cameras.at(index); }
};