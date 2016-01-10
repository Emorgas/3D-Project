#include "GameObject.h"
#include "Mesh.h"
GameObject::GameObject(ID3D11Device* device, Mesh* mesh)
{
	_pd3dDevice = device;
	_mesh = mesh;
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&_rotation, XMMatrixIdentity());
	XMStoreFloat4x4(&_translation, XMMatrixIdentity());
	XMVECTOR min, max;
	min = XMLoadFloat3(&_mesh->getData()->AABB.at(0));
	max = XMLoadFloat3(&_mesh->getData()->AABB.at(1));

	BoundingBox::CreateFromPoints(_bounds, min, max);
}
GameObject::~GameObject()
{
}

void GameObject::AddTexture(ID3D11ShaderResourceView* texture)
{
	_textures.emplace_back(texture);
}

void GameObject::SetScale(float x, float y, float z)
{
	XMStoreFloat4x4(&_scale, XMMatrixScaling(x, y, z));
}

void GameObject::SetRotation(float x, float y, float z)
{
	_pos = { x, y, z };
	XMStoreFloat4x4(&_rotation, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z));
}

void GameObject::SetTranslation(float x, float y, float z)
{
	_pos = { x, y, z };
	XMStoreFloat4x4(&_translation, XMMatrixTranslation(x, y, z));
	XMMATRIX translate = XMLoadFloat4x4(&_translation);
	_bounds.Center = _pos;
}

void GameObject::SetObjectDrawStatus(bool status)
{
	_draw = status;
}

void GameObject::UpdateWorld()
{
	XMMATRIX scale = XMLoadFloat4x4(&_scale);
	XMMATRIX rotate = XMLoadFloat4x4(&_rotation);
	XMMATRIX translate = XMLoadFloat4x4(&_translation);
	XMMATRIX world = scale * rotate * translate;
	XMStoreFloat4x4(&_world, scale * rotate * translate);
}

void GameObject::Update(float elapsedTime)
{
	// TODO: Add GameObject logic 
}

void GameObject::SetBuffers(ID3D11DeviceContext* context)
{
	context->IASetVertexBuffers(0, 1, &_mesh->getData()->VertexBuffer, &_mesh->getData()->VBStride, &_mesh->getData()->VBOffset);
	context->IASetIndexBuffer(_mesh->getData()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

void GameObject::Draw(ID3D11DeviceContext * pImmediateContext)
{
	pImmediateContext->DrawIndexed(_mesh->getData()->IndexCount, 0, 0);
}
