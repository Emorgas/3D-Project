#include "ObjectManager.h"


ObjectManager::ObjectManager(ID3D11Device* device)
{
	_device = device;
}


ObjectManager::~ObjectManager()
{
}

void ObjectManager::AddObject(Mesh* mesh)
{
	_objects.emplace_back(new GameObject(_device, mesh));
}
