#pragma once

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

struct SimpleVertex
{
	SimpleVertex() {}
	SimpleVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : Pos(x, y, z), TexCoord(u, v), normal(nx, ny, nz) {}
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
	XMFLOAT3 normal;
};

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad1;
	XMFLOAT3 pos;
	float range;
	XMFLOAT3 att;
	float pad2;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};