#include "GameObject.h"

GameObject::GameObject()
{
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
}
GameObject::~GameObject()
{
}

void GameObject::Initialise(ID3D11Device* device)
{
	_pd3dDevice = device;
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&_rotation, XMMatrixIdentity());
	XMStoreFloat4x4(&_translation, XMMatrixIdentity());

	InitVertexBuffer();
	InitIndexBuffer();
	CalculateModelVectors();
}

void GameObject::SetScale(float x, float y, float z)
{
	XMStoreFloat4x4(&_scale, XMMatrixScaling(x, y, z));
}

void GameObject::SetRotation(float x, float y, float z)
{
	XMStoreFloat4x4(&_rotation, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z));
}

void GameObject::SetTranslation(float x, float y, float z)
{
	XMStoreFloat4x4(&_translation, XMMatrixTranslation(x, y, z));
}

void GameObject::UpdateWorld()
{
	XMMATRIX scale = XMLoadFloat4x4(&_scale);
	XMMATRIX rotate = XMLoadFloat4x4(&_rotation);
	XMMATRIX translate = XMLoadFloat4x4(&_translation);

	XMStoreFloat4x4(&_world, scale * rotate * translate);
}

void GameObject::Update(float elapsedTime)
{
	// TODO: Add GameObject logic 
}

void GameObject::Draw(ID3D11Device * pd3dDevice, ID3D11DeviceContext * pImmediateContext)
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers	
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	pImmediateContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);


	pImmediateContext->DrawIndexed(36, 0, 0);
}

HRESULT GameObject::InitVertexBuffer()
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		// Front Face
		SimpleVertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		SimpleVertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		SimpleVertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		SimpleVertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

		// Back Face
		SimpleVertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,-1.0f, -1.0f, 1.0f),
		SimpleVertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
		SimpleVertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
		SimpleVertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,-1.0f,  1.0f, 1.0f),

		// Top Face
		SimpleVertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,-1.0f, 1.0f, -1.0f),
		SimpleVertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,-1.0f, 1.0f,  1.0f),
		SimpleVertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f),
		SimpleVertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

		// Bottom Face
		SimpleVertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),
		SimpleVertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		SimpleVertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f),
		SimpleVertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,-1.0f, -1.0f,  1.0f),

		// Left Face
		SimpleVertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,-1.0f, -1.0f,  1.0f),
		SimpleVertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,-1.0f,  1.0f,  1.0f),
		SimpleVertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,-1.0f,  1.0f, -1.0f),
		SimpleVertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),

		// Right Face
		SimpleVertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
		SimpleVertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f),
		SimpleVertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f),
		SimpleVertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f),
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_vertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT GameObject::InitIndexBuffer()
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * 12 * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_indexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void GameObject::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, biTangent, normal;

	faceCount = 24 / 3; //THIS IS HARD CODED FIX LATER WHEN MODELS ARE LOADED. YOU WANT TO GET A COUNT OF THE VERTICES IN THE MODEL

	index = 0;

	for (i = 0; i < faceCount; i++)
	{
		
	}
}