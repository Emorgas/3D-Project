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
	SimpleVertex(XMFLOAT3 pos, XMFLOAT2 texC, XMFLOAT3 norm) : Pos(pos), TexCoord(texC), normal(norm){}
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(PointLight));
	}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3 pos;
	float range;
	
	XMFLOAT3 att;
	float pad;
	
};

//struct Material
//{
//	Material(XMFLOAT4 amb, XMFLOAT4 diff, XMFLOAT4 spec, XMFLOAT4 ref) : Ambient(amb), Diffuse(diff), Specular(spec), Reflect(ref) {}
//	XMFLOAT4 Ambient;
//	XMFLOAT4 Diffuse;
//	XMFLOAT4 Specular;
//	XMFLOAT4 Reflect;
//};