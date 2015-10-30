#pragma once

 #include <directxmath.h>
#include <d3d11_1.h>

#include "Structures.h"

using namespace DirectX;

class GameObject
{
private:
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _scale;
	XMFLOAT4X4 _rotation;
	XMFLOAT4X4 _translation;

	ID3D11Device* _pd3dDevice;

	//Temp Solution
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	SimpleVertex* vertices[];

public:

	GameObject();
	~GameObject();

	XMFLOAT4X4 GetWorld() const { return _world; };

	void UpdateWorld();

	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void CalculateModelVectors();
	void CalculateTangentBitangent(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	void Initialise(ID3D11Device* device);
	void Update(float elapsedTime);
	void Draw(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pImmediateContext);
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
};

