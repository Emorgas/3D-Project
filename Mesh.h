#pragma once

#include <d3d11_1.h>
#include <directxmath.h>
#include "Structures.h"
#include "OBJLoader.h"

using namespace DirectX;
class Mesh
{
private:
	MeshData data;
	MeshData* dataP;

public:
	Mesh(ID3D11Device* device, char* modelName);
	MeshData* getData() { return dataP; } 
};