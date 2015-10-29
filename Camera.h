#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

class Camera
{
private:
	XMVECTOR _defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR _defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR _camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR _camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	float _moveLeftRight = 0.0f;
	float _moveBackForward = 0.0f;
	float _moveUpDown = 0.0f;

	float _camYaw = 0.0f;
	float _camPitch = 0.0f;

	XMVECTOR _eye;
	XMVECTOR _at;
	XMVECTOR _up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;
	FLOAT _zoom;
	const FLOAT _ZOOMMAX = 2.75f;
	const FLOAT _ZOOMMIN = 0.39f;
	const FLOAT _ZOOMDEFAULT = XM_PIDIV2;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
	XMFLOAT4X4 _rotation;

public:
	Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	void CalculateViewProjection();

	XMMATRIX GetView();
	XMMATRIX GetProjection();

	XMMATRIX GetViewProjection() const;

	XMVECTOR GetEye() const { return _eye; }
	XMVECTOR GetAt() const { return _at; }
	XMVECTOR GetUp() const { return _up; }


	void AdjustYawAndPitch(float yaw, float pitch) { _camYaw += yaw; _camPitch += pitch; }
	void AddMoveForward(float move) { _moveBackForward += move; }
	void AddMoveRight(float move) { _moveLeftRight += move; }
	void AddMoveUp(float move) { _moveUpDown += move; }
	void SetEye(XMVECTOR eye) { _eye = eye; }
	void SetEye(float x, float y, float z);
	void AddEye(float x, float y, float z);
	void SetAt(XMVECTOR at) { _at = at; }
	void SetAt(float x, float y, float z);
	void SetUp(XMVECTOR up) { _up = up; }
	void AddAt(float x, float y, float z);
	void AddZoom(float zoom);
	void ResetZoom();
	void Update();

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
};

