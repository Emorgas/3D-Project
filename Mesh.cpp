#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, char* modelName)
{
	data = OBJLoader::Load(modelName, device);
	dataP = &data;
}