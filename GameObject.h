#pragma once

#include <directxmath.h>
#include <DirectXCollision.h>
#include <d3d11_1.h>
#include <vector>

using namespace DirectX;

class Mesh;

class GameObject
{
private:
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _scale;
	XMFLOAT4X4 _rotation;
	XMFLOAT4X4 _translation;
	bool _wireFrame = false;

	ID3D11Device* _pd3dDevice;

	 Mesh* _mesh;
	 bool _draw = true;

	 XMFLOAT3 _pos;

	std::vector<ID3D11ShaderResourceView*> _textures;

public:
	BoundingBox _bounds;

	GameObject(ID3D11Device* device, Mesh* mesh);
	~GameObject();

	XMFLOAT4X4 GetWorld() const { return _world; };
	Mesh* GetMesh() { return _mesh; }

	void UpdateWorld();
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void ToggleWireFrame() { _wireFrame = !_wireFrame; }
	XMFLOAT3 GetPos(){ return _pos; }
	bool GetWireFrame() { return _wireFrame; }
	void SetObjectDrawStatus(bool status);
	bool ObjectToBeDrawn() { return _draw; }
	void AddTexture(ID3D11ShaderResourceView* texture);
	std::vector<ID3D11ShaderResourceView*> GetTextureArray() { return _textures; }

	void Update(float elapsedTime);
	void SetBuffers(ID3D11DeviceContext* context);
	void Draw(ID3D11DeviceContext * pImmediateContext);
};

