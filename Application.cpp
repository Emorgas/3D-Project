#include "Application.h"
#include <stdio.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pDepthVertexShader = nullptr;
	_pDepthPixelShader = nullptr;
	_pDepthVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	_pCbPerFrameBuffer = nullptr;
	_camManager = nullptr;
	_input = nullptr;
	_renderTexture = nullptr;
	_light1 = nullptr;
	_pShadowConstantBuffer = nullptr;
	_light2 = nullptr;
	_renderTexture2 = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	_renderTexture = new RenderTextureClass;
	_renderTexture->Initialize(_pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_FAR, SCREEN_NEAR);

	_renderTexture2 = new RenderTextureClass;
	_renderTexture2->Initialize(_pd3dDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_FAR, SCREEN_NEAR);

	//Create Managers
	_camManager = new CameraManager();
	_textureManager = new TextureManager(_pd3dDevice);
	_meshManager = new MeshManager();
	_objectManager = new ObjectManager(_pd3dDevice);

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	LoadSceneFromFile("scene1.txt");

	_camManager->Init();

	_input = new Input();
	_input->Initialise(_hInst, _hWnd);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_sampleStateWrap);

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.BorderColor[0] = 0;
	sampDesc.BorderColor[1] = 0;
	sampDesc.BorderColor[2] = 0;
	sampDesc.BorderColor[3] = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_sampleStateClamp);

	_light1 = new Light;
	_light1->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	_light1->SetSpecularColor(0.7f, 0.7f, 0.7f, 1.0f);
	_light1->SetLookAt(0.0f, 0.0f, 0.0f);
	_light1->SetPosition(5.0f, 8.0f, -5.0f);
	_light1->SetRange(100.0f);
	_light1->SetAttenuation(0.0f, 0.2f, 0.0f);

	_light1->GenerateProjectionMatrix(SCREEN_FAR, SCREEN_NEAR);

	_light2 = new Light;
	_light2->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_light2->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	_light2->SetSpecularColor(0.7f, 0.7f, 0.7f, 1.0f);
	_light2->SetLookAt(0.0f, 0.0f, 0.0f);
	_light2->SetPosition(-5.0f, 8.0f, -5.0f);
	_light2->SetRange(100.0f);
	_light2->SetAttenuation(0.0f, 0.2f, 0.0f);

	_light2->GenerateProjectionMatrix(SCREEN_FAR, SCREEN_NEAR);
	return S_OK;
}

HRESULT Application::LoadSceneFromFile(std::string filename)
{
	int meshID = -1, colorTexID = -1, NrmTexID = -1, objectCount = 0;
	std::ifstream inFile;
	inFile.open(filename);

	if (!inFile.good())
		return S_FALSE;
	else
	{
		std::string input;

		while (!inFile.eof())
		{
			inFile >> input;

			if (input.compare("m") == 0) //mesh
			{
				std::string strname = "";
				inFile >> strname;
				std::vector<char> writable(strname.begin(), strname.end());
				writable.push_back('\0');
				_meshManager->addMesh(_pd3dDevice, &writable[0]);
				meshID++;
			}
			else if (input.compare("t") == 0) //Texture
			{
				std::string strname = "";
				inFile >> strname;
				std::vector<char> writable(strname.begin(), strname.end());
				writable.push_back('\0');
				size_t nameSize = strlen(&writable[0]) + 1;
				const size_t newSize = 100;
				size_t convertedChars = 0;
				wchar_t wcstring[newSize];
				mbstowcs_s(&convertedChars, wcstring, nameSize, &writable[0], _TRUNCATE);
				_textureManager->AddTexture(wcstring);
				if (colorTexID == -1)
					colorTexID = 0;
				else
					colorTexID += 2;
			}
			else if (input.compare("tn") == 0) //Texure Normal Map
			{
				std::string strname = "";
				inFile >> strname;
				std::vector<char> writable(strname.begin(), strname.end());
				writable.push_back('\0');
				size_t nameSize = strlen(&writable[0]) + 1;
				const size_t newSize = 100;
				size_t convertedChars = 0;
				wchar_t wcstring[newSize];
				mbstowcs_s(&convertedChars, wcstring, nameSize, &writable[0], _TRUNCATE);
				_textureManager->AddTexture(wcstring);
				NrmTexID = colorTexID + 1;

			}
			else if (input.compare("i") == 0) //instance of current object
			{
				XMFLOAT3 pos;
				inFile >> pos.x;
				inFile >> pos.y;
				inFile >> pos.z;
				_objectManager->AddObject(_meshManager->getMeshData(meshID));
				_objectManager->GetObjectByIndex(objectCount)->AddTexture(_textureManager->GetTextureByIndex(colorTexID));
				_objectManager->GetObjectByIndex(objectCount)->AddTexture(_textureManager->GetTextureByIndex(NrmTexID));
				_objectManager->GetObjectByIndex(objectCount)->SetTranslation(pos.x, pos.y, pos.z);
				objectCount++;
			}
			else if (input.compare("c") == 0) //Camera
			{
				XMFLOAT4 pos(0.0f, 0.0f, 0.0f, 0.0f);
				XMFLOAT4 at(0.0f, 0.0f, 0.0f, 0.0f);

				inFile >> pos.x;
				inFile >> pos.y;
				inFile >> pos.z;
				inFile >> pos.w;

				inFile >> at.x;
				inFile >> at.y;
				inFile >> at.z;
				inFile >> at.w;

				XMVECTOR eye = XMVectorSet(pos.x, pos.y, pos.z, pos.w);
				XMVECTOR lookAt = XMVectorSet(at.x, at.y, at.z, at.w);
				XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

				_camManager->AddCamera(new Camera(eye, lookAt, up, (FLOAT)_WindowWidth, (FLOAT)_WindowHeight, 0.001f, SCREEN_FAR));
			}
		}
	}
	inFile.close();
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = _compiler->CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

	if (FAILED(hr))
	{
		printf("Failed compiling vertex shader %08X\n", hr);
		MessageBox(nullptr,
			L"The FX file cannot be compiled. Error!", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = _compiler->CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	//SHADOW DEPTH SHADER///////////////////////////////////////////////////////////
	// Compile the vertex shader
	ID3DBlob* pVSBlob2 = nullptr;
	hr = _compiler->CompileShaderFromFile(L"Depth.fx", "VS", "vs_4_0", &pVSBlob2);

	if (FAILED(hr))
	{
		printf("Failed compiling vertex shader %08X\n", hr);
		MessageBox(nullptr,
			L"The FX file cannot be compiled. Error!", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob2->GetBufferPointer(), pVSBlob2->GetBufferSize(), nullptr, &_pDepthVertexShader);

	if (FAILED(hr))
	{
		pVSBlob2->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob2 = nullptr;
	hr = _compiler->CompileShaderFromFile(L"Depth.fx", "PS", "ps_4_0", &pPSBlob2);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob2->GetBufferPointer(), pPSBlob2->GetBufferSize(), nullptr, &_pDepthPixelShader);
	pPSBlob2->Release();

	if (FAILED(hr))
		return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout2[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements2 = ARRAYSIZE(layout2);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout2, numElements2, pVSBlob2->GetBufferPointer(),
		pVSBlob2->GetBufferSize(), &_pDepthVertexLayout);
	pVSBlob2->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pDepthVertexLayout);
	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 1280, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;
	//Create Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Setup the viewport
	_viewport;
	_viewport.Width = (FLOAT)_WindowWidth;
	_viewport.Height = (FLOAT)_WindowHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &_viewport);

	InitShadersAndInputLayout();

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CbPerFrame);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, NULL, &_pCbPerFrameBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadowConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, NULL, &_pShadowConstantBuffer);

	//Create Wireframe State
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	//Create solid State
	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidDesc.FillMode = D3D11_FILL_SOLID;
	solidDesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&solidDesc, &_solid);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Application::Cleanup()
{
	if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_pConstantBuffer) _pConstantBuffer->Release();
	if (_pVertexBuffer) _pVertexBuffer->Release();
	if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pVertexLayout) _pVertexLayout->Release();
	if (_pVertexShader) _pVertexShader->Release();
	if (_pPixelShader) _pPixelShader->Release();
	if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pSwapChain) _pSwapChain->Release();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solid) _solid->Release();
	if (_pCbPerFrameBuffer) _pCbPerFrameBuffer->Release();
	if (_renderTexture) _renderTexture->Shutdown();
	if (_renderTexture2) _renderTexture2->Shutdown();
}

void Application::Update()
{
	_camManager->GetActiveCamera()->Update();
	// Update our time
	static float t = 0.0f;

	_input->Update();
	HandleInput();
	XMFLOAT3 pos = _light1->GetPosition();
	pos.x -= LightMoveSpeed;
	if (_light1->GetPosition().x < -10.0f)
	{
		pos.x = 10.0f;
	}
	_light1->SetPosition(pos.x, pos.y, pos.z);

	pos = _light2->GetPosition();
	pos.x += LightMoveSpeed;
	if (_light2->GetPosition().x > 10.0f)
	{
		pos.x = -10.0f;
	}
	_light2->SetPosition(pos.x, pos.y, pos.z);



	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}
	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		_objectManager->GetObjectByIndex(i)->UpdateWorld();
	}
}

void Application::HandleInput()
{
	//Escape
	if (_input->IsEscapePressed())
	{
		PostQuitMessage(0);
	}

	//Camera Look Movement
	if (_input->IsWPressed())
	{
		_camManager->GetActiveCamera()->AddMoveForward(CameraMoveSpeed);
	}
	if (_input->IsAPressed())
	{
		_camManager->GetActiveCamera()->AddMoveRight(-CameraMoveSpeed);
	}
	if (_input->IsSPressed())
	{
		_camManager->GetActiveCamera()->AddMoveForward(-CameraMoveSpeed);
	}
	if (_input->IsDPressed())
	{
		_camManager->GetActiveCamera()->AddMoveRight(CameraMoveSpeed);
	}
	if (_input->IsQPressed())
	{
		_camManager->GetActiveCamera()->AddMoveUp(CameraMoveSpeed);
	}
	if (_input->IsZPressed())
	{
		_camManager->GetActiveCamera()->AddMoveUp(-CameraMoveSpeed);
	}
	if (_input->HasMouseMoved())
	{
		_camManager->GetActiveCamera()->AdjustYawAndPitch((_input->GetMouseMove().x * 0.001f), (_input->GetMouseMove().y * 0.001f));
	}

	if (_input->IsLMBPressed())
	{
		if (_beginCheck == false)
		{
			_beginCheck = true;
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(_hWnd, &p);
			TestIntersection(p.x, p.y);
		}
	}

	if (_input->IsLMBPressed() == false)
	{
		_beginCheck = false;
	}
}

void Application::TestIntersection(int mouseX, int mouseY)
{
	float pointX, pointY;
	XMMATRIX proj, view, inverseView, world, toLocal, inverseWorld;
	XMVECTOR rayOrigin, rayDir;
	XMFLOAT3 dir, origin;
	bool intersect, result;

	//Turn mouse coords into -1 to +1
	pointX = ((2.0f * (float)mouseX) / (float)_WindowWidth) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)_WindowHeight) - 1.0f)*-1.0f;

	//adjust to account for aspect ratio using the projection matrix
	XMFLOAT4X4 tempProj;
	XMStoreFloat4x4(&tempProj, _camManager->GetActiveCamera()->GetProjection());
	pointX = pointX / tempProj._11;
	pointY = pointY / tempProj._22;

	//inverse the view matrix
	view = _camManager->GetActiveCamera()->GetView();
	inverseView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMFLOAT4X4 inverseViewFloat;
	XMStoreFloat4x4(&inverseViewFloat, inverseView);
	//calculate ray dir by multiplying position by inverse view matrix
	dir.x = (pointX * inverseViewFloat._11) + (pointY * inverseViewFloat._21) + inverseViewFloat._31;
	dir.y = (pointX * inverseViewFloat._12) + (pointY * inverseViewFloat._22) + inverseViewFloat._32;
	dir.z = (pointX * inverseViewFloat._13) + (pointY * inverseViewFloat._23) + inverseViewFloat._33;

	rayDir = XMLoadFloat3(&dir);
	//origin of ray is the camera;
	rayOrigin = _camManager->GetActiveCamera()->GetEye();

	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		world = XMLoadFloat4x4(&_objectManager->GetObjectByIndex(i)->GetWorld());
		inverseWorld = (XMMatrixInverse(&XMMatrixDeterminant(world), world));


		//Make dir unit length
		rayDir = XMVector3Normalize(rayDir);

		int pickedObj = -1;
		bool intersect;
		float closest = 200.0f;
		float f = 100.0f;
		intersect = _objectManager->GetObjectByIndex(i)->_bounds.Intersects(rayOrigin, rayDir, f);
		if (intersect)
		{
			_objectManager->GetObjectByIndex(i)->ToggleWireFrame();
			break;
		}

	}

}



void Application::Draw()
{
	XMMATRIX world = XMLoadFloat4x4(&_world);
	//Render the scene depth to a texture
	RenderSceneDepth(world);
	RenderSceneDepth2(world);

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	XMMATRIX view = _camManager->GetActiveCamera()->GetView();
	XMMATRIX projection = _camManager->GetActiveCamera()->GetProjection();

	_light1->GenerateViewMatrix();
	XMMATRIX lightViewMatrix;
	_light1->GetViewMatrix(lightViewMatrix);
	XMMATRIX lightProjectionMatrix;
	_light1->GetProjectionMatrix(lightProjectionMatrix);

	_light2->GenerateViewMatrix();
	XMMATRIX lightViewMatrix2;
	_light2->GetViewMatrix(lightViewMatrix2);
	XMMATRIX lightProjectionMatrix2;
	_light2->GetProjectionMatrix(lightProjectionMatrix2);

	//This buffer only changes once per frame
	XMFLOAT3 tempEyePos;
	XMStoreFloat3(&tempEyePos, _camManager->GetActiveCamera()->GetEye());
	CbPerFrame cbpf;
	cbpf.eyePosW = tempEyePos;
	cbpf.lightAmbientColor = _light1->GetAmbientColor();
	cbpf.lightDiffuseColor = _light1->GetDiffuseColor();
	cbpf.lightSpecularColor = _light1->GetSpecularColor();
	cbpf.lightDiffuseColor2 = _light2->GetDiffuseColor();
	cbpf.lightSpecularColor2 = _light2->GetSpecularColor();
	cbpf.lightPos = _light1->GetPosition();
	cbpf.lightPos2 = _light2->GetPosition();
	cbpf.lightAttenuation = _light1->GetAttenuation();
	cbpf.lightAttenuation2 = _light2->GetAttenuation();
	cbpf.lightRange = _light1->GetRange();
	cbpf.lightRange2 = _light2->GetRange();
	cbpf.SpecularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cbpf.SpecularPower = 10.0f;

	ComputeFrustumCulling(_camManager->GetActiveCamera()->CreateViewFrustum());
	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		if (_objectManager->GetObjectByIndex(i)->ObjectToBeDrawn() == true)
		{
			if (_objectManager->GetObjectByIndex(i)->GetWireFrame() == true)
				_pImmediateContext->RSSetState(_wireFrame);
			else
				_pImmediateContext->RSSetState(_solid);
			world = XMLoadFloat4x4(&_objectManager->GetObjectByIndex(i)->GetWorld());
			XMMatrixIdentity() *= world;
			_objectManager->GetObjectByIndex(i)->SetBuffers(_pImmediateContext);//Setup the buffers
			//Setup everything else
			ShadowConstantBuffer scb;
			scb.mWorld = XMMatrixTranspose(world);
			scb.mView = XMMatrixTranspose(view);
			scb.mProjection = XMMatrixTranspose(projection);
			scb.mLightView = XMMatrixTranspose(lightViewMatrix);
			scb.mLightProj = XMMatrixTranspose(lightProjectionMatrix);
			scb.mLightProj2 = XMMatrixTranspose(lightProjectionMatrix2);
			scb.mLightView2 = XMMatrixTranspose(lightViewMatrix2);
			scb.mVertexLightPos = XMFLOAT3(_light1->GetPosition().x, _light1->GetPosition().y, _light1->GetPosition().z);
			scb.mVertexLightPos2 = XMFLOAT3(_light2->GetPosition().x, _light2->GetPosition().y, _light2->GetPosition().z);
			_pImmediateContext->UpdateSubresource(_pShadowConstantBuffer, 0, nullptr, &scb, 0, 0);
			_pImmediateContext->VSSetConstantBuffers(0, 1, &_pShadowConstantBuffer);

			//Textures
			std::vector<ID3D11ShaderResourceView*> textures = _objectManager->GetObjectByIndex(i)->GetTextureArray();
			ID3D11ShaderResourceView* tex[5];
			for (int i = 0; i < textures.size(); i++)
			{
				tex[i] = textures.at(i);
			}
			tex[textures.size()] = _renderTexture->GetShaderResourceView();
			tex[textures.size() + 1] = _renderTexture2->GetShaderResourceView();
			_pImmediateContext->PSSetShaderResources(0, textures.size() + 2, tex);
			_pImmediateContext->UpdateSubresource(_pCbPerFrameBuffer, 0, nullptr, &cbpf, 0, 0);
			_pImmediateContext->PSSetConstantBuffers(0, 1, &_pCbPerFrameBuffer);
			_pImmediateContext->IASetInputLayout(_pVertexLayout);
			_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
			_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
			_pImmediateContext->PSSetSamplers(0, 1, &_sampleStateClamp);
			_pImmediateContext->PSSetSamplers(1, 1, &_sampleStateWrap);
			_objectManager->GetObjectByIndex(i)->Draw(_pImmediateContext);
		}
	}
	_pSwapChain->Present(0, 0);
}

void Application::RenderSceneDepth(XMMATRIX& world)
{
	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
	_renderTexture->SetRenderTarget(_pImmediateContext);
	_renderTexture->ClearRenderTarget(_pImmediateContext, 0.0f, 0.0f, 0.0f, 1.0f);

	_light1->GenerateViewMatrix();
	_light1->GetViewMatrix(lightViewMatrix);
	_light1->GetProjectionMatrix(lightProjectionMatrix);

	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		world = XMLoadFloat4x4(&_objectManager->GetObjectByIndex(i)->GetWorld());
		XMMatrixIdentity() *= world;
		//OPEN GRAPHICS CLASS WORK FROM HERE
		_objectManager->GetObjectByIndex(i)->SetBuffers(_pImmediateContext);//Setup the buffers
		//Setup everything else
		ConstantBuffer cb;
		cb.mWorld = XMMatrixTranspose(world);
		cb.mView = XMMatrixTranspose(lightViewMatrix);
		cb.mProjection = XMMatrixTranspose(lightProjectionMatrix);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->IASetInputLayout(_pDepthVertexLayout);
		_pImmediateContext->VSSetShader(_pDepthVertexShader, nullptr, 0);
		_pImmediateContext->PSSetShader(_pDepthPixelShader, nullptr, 0);
		//Everything else
		_objectManager->GetObjectByIndex(i)->Draw(_pImmediateContext);//draw the objects
	}
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);
	_pImmediateContext->RSSetViewports(1, &_viewport);
}

void Application::RenderSceneDepth2(XMMATRIX& world)
{
	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
	_renderTexture2->SetRenderTarget(_pImmediateContext);
	_renderTexture2->ClearRenderTarget(_pImmediateContext, 0.0f, 0.0f, 0.0f, 1.0f);

	_light2->GenerateViewMatrix();
	_light2->GetViewMatrix(lightViewMatrix);
	_light2->GetProjectionMatrix(lightProjectionMatrix);

	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		world = XMLoadFloat4x4(&_objectManager->GetObjectByIndex(i)->GetWorld());
		XMMatrixIdentity() *= world;
		_objectManager->GetObjectByIndex(i)->SetBuffers(_pImmediateContext);

		ConstantBuffer cb;
		cb.mWorld = XMMatrixTranspose(world);
		cb.mView = XMMatrixTranspose(lightViewMatrix);
		cb.mProjection = XMMatrixTranspose(lightProjectionMatrix);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
		_pImmediateContext->IASetInputLayout(_pDepthVertexLayout);
		_pImmediateContext->VSSetShader(_pDepthVertexShader, nullptr, 0);
		_pImmediateContext->PSSetShader(_pDepthPixelShader, nullptr, 0);

		_objectManager->GetObjectByIndex(i)->Draw(_pImmediateContext);
	}
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);
	_pImmediateContext->RSSetViewports(1, &_viewport);
}

void Application::ComputeFrustumCulling(std::vector<XMFLOAT4> &frustumPlanes)
{
	ContainmentType c;
	std::vector<XMVECTOR> planes;
	for (int i = 0; i < 6; i++)
	{
		planes.emplace_back(XMLoadFloat4(&frustumPlanes[i]));
	}

	for (int i = 0; i < _objectManager->NumOfObjects(); i++)
	{
		_objectManager->GetObjectByIndex(i)->SetObjectDrawStatus(true);
		for (int planeID = 0; planeID < 6; ++planeID)
		{
			XMVECTOR planeNormal = XMVectorSet(frustumPlanes[planeID].x, frustumPlanes[planeID].y, frustumPlanes[planeID].z, 0.0f);
			float planeConstant = frustumPlanes[planeID].w;

			XMFLOAT3 axisVert;

			//x axis
			if (frustumPlanes[planeID].x < 0.0f)
				axisVert.x = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[0].x + _objectManager->GetObjectByIndex(i)->GetPos().x;
			else
				axisVert.x = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[1].x + _objectManager->GetObjectByIndex(i)->GetPos().x;
			//y axis
			if (frustumPlanes[planeID].y < 0.0f)
				axisVert.y = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[0].y + _objectManager->GetObjectByIndex(i)->GetPos().y;
			else
				axisVert.y = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[1].y + _objectManager->GetObjectByIndex(i)->GetPos().y;
			//z axis
			if (frustumPlanes[planeID].z < 0.0f)
				axisVert.z = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[0].z + _objectManager->GetObjectByIndex(i)->GetPos().z;
			else
				axisVert.z = _objectManager->GetObjectByIndex(i)->GetMesh()->getData()->AABB[1].z + _objectManager->GetObjectByIndex(i)->GetPos().z;

			if (XMVectorGetX(XMVector3Dot(planeNormal, XMLoadFloat3(&axisVert))) + planeConstant < 0.0f)
			{
				_objectManager->GetObjectByIndex(i)->SetObjectDrawStatus(false);
				break;
			}

		}
	}
}