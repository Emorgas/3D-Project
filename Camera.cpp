#include "Camera.h"

Camera::Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _eye(eye), _at(at), _up(up), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth)
{
}

Camera::~Camera()
{
}

void Camera::CalculateViewProjection()
{
	// Initialize the view matrix
	XMVECTOR Eye = _eye;
	XMVECTOR At = _at;
	XMVECTOR Up = _up;

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMFLOAT4X4 Camera::GetViewProjection() const
{
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}

void Camera::SetEye(float x, float y, float z)
{
	XMVECTOR eye = XMVectorSet(x, y, z, 0.0f);
	_eye = eye;
}

void Camera::SetAt(float x, float y, float z)
{
	XMVECTOR at = XMVectorSet(x, y, z, 0.0f);
	_at = at;
}