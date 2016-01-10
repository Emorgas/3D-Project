#pragma once

#include "GameObject.h"
#include <vector>

class ObjectManager
{
private:
	std::vector<GameObject*> _objects;
	ID3D11Device* _device;
public:
	ObjectManager(ID3D11Device* device);
	~ObjectManager();
	void AddObject(Mesh* mesh);
	GameObject* GetObjectByIndex(int index) { return _objects.at(index); }
	int NumOfObjects() { return _objects.size(); }
};

