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
	float _rotation;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	void CalculateViewProjection();

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	XMFLOAT4X4 GetViewProjection() const;

	XMVECTOR GetEye() const { return _eye; }
	XMVECTOR GetAt() const { return _at; }
	XMVECTOR GetUp() const { return _up; }


	void SetEye(XMVECTOR eye) { _eye = eye; }
	void SetEye(float x, float y, float z);
	void AddEye(float x, float y, float z);
	void SetAt(XMVECTOR at) { _at = at; }
	void SetAt(float x, float y, float z);
	void SetUp(XMVECTOR up) { _up = up; }
	void AddAt(float x, float y, float z);
	void AddZoom(float zoom);
	void ResetZoom();
	void AddRotation(float rotation) { _rotation += rotation; }
	float GetRotation() const { return _rotation; }

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
};

 