#include "CameraManager.h"

CameraManager::CameraManager()
{
}

void CameraManager::AddCamera(Camera* cam)
{
	_cameras.push_back(cam);
}

void CameraManager::SwitchCam(bool up)
{
	if (up)
	{
		if (_curCamera + 1 < _cameras.size())
		{
			_activeCam = _cameras.at(_curCamera + 1);
			_curCamera += 1;
		}
		else
		{
			_activeCam = _cameras.at(0);
			_curCamera = 0;
		}
	}
	else if (!up)
	{
		if (_curCamera > 0)
		{
			_activeCam = _cameras.at(_curCamera - 1);
			_curCamera -= 1;
		}
		else
		{
			_activeCam = _cameras.at(_cameras.size() - 1);
			_curCamera = _cameras.size() - 1;
		}
	}
}

void CameraManager::Init()
{
	_activeCam = _cameras.at(0);
	_curCamera = 0;
}