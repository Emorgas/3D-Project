#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include <string>
#include "resource.h"
#include "CameraManager.h"
#include "Input.h"
#include "Structures.h"
#include "GameObject.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "OBJLoader.h"
#include "ObjectManager.h"
#include "ShaderCompiler.h"
#include "rendertextureclass.h"
#include "Light.h"

using namespace DirectX;

const int SHADOWMAP_WIDTH = 2048;
const int SHADOWMAP_HEIGHT = 2048;

const float SCREEN_NEAR = 1.0f;
const float SCREEN_FAR = 100.0f;
const float CameraMoveSpeed = 0.1f;	//CHANGE THIS IF CAMERA MOVES TO FAST
const float LightMoveSpeed = 0.1f;	//CHANGE THIS IF LIGHTS MOVE TOO FAST

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct ShadowConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMMATRIX mLightView;
	XMMATRIX mLightProj;
	XMMATRIX mLightView2;
	XMMATRIX mLightProj2;
	XMFLOAT3 mVertexLightPos;
	float pad0;
	XMFLOAT3 mVertexLightPos2;
	float pad1;
};

struct CbPerFrame
{
	XMFLOAT4 lightAmbientColor;
	XMFLOAT4 lightDiffuseColor;
	XMFLOAT4 lightSpecularColor;
	XMFLOAT4 lightDiffuseColor2;
	XMFLOAT4 lightSpecularColor2;
	XMFLOAT3 lightPos;
	float pad0;
	XMFLOAT3 lightAttenuation;
	float lightRange;
	XMFLOAT3 lightPos2;
	float pad02;
	XMFLOAT3 lightAttenuation2;
	float lightRange2;
	XMFLOAT3 eyePosW;
	float pad;
	XMFLOAT4 SpecularMaterial;
	float SpecularPower;
	XMFLOAT3 pad2;
};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11VertexShader*     _pDepthVertexShader;
	ID3D11PixelShader*      _pDepthPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11InputLayout*      _pDepthVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11Buffer*			_pShadowConstantBuffer;
	ID3D11Buffer*			_pCbPerFrameBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	CameraManager*			_camManager;
	Input*					_input;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*  _wireFrame;
	ID3D11RasterizerState*  _solid;
	ID3D11SamplerState*     _sampleStateClamp;
	ID3D11SamplerState*		_sampleStateWrap;
	CbPerFrame				_constBuffPerFrame;
	MeshManager*			_meshManager;
	ObjectManager*			_objectManager;
	TextureManager*			_textureManager;
	D3D11_VIEWPORT			_viewport;
	ShaderCompiler*			_compiler;

	RenderTextureClass*		_renderTexture;
	Light*					_light1;
	RenderTextureClass*		_renderTexture2;
	Light*					_light2;
	bool					_beginCheck = false;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT InitShadersAndInputLayout();
	void HandleInput();
	void TestIntersection(int, int);
	HRESULT LoadSceneFromFile(std::string filename);

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
	void RenderSceneDepth(XMMATRIX& world);
	void RenderSceneDepth2(XMMATRIX& world);
	void ComputeFrustumCulling(std::vector<XMFLOAT4> &frustumPlanes);
};

