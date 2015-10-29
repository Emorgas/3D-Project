#pragma once

#include "Mesh.h"
#include <vector>

class MeshManager
{
private:
	std::vector<Mesh*> meshes;

public:
	MeshManager();
	void addMesh(ID3D11Device* device, char* modelName);
	Mesh* getMeshData(int index){ return meshes.at(index); }
};