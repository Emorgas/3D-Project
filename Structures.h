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
	XMFLOAT3 Pos;
	XMFLOAT3 normal;
};