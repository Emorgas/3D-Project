#include "MeshManager.h"

MeshManager::MeshManager()
{
}

void MeshManager::addMesh(ID3D11Device* device, char* modelName)
{
	meshes.push_back(new Mesh(device, modelName));
}