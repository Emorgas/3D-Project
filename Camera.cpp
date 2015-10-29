#include "Camera.h"

Camera::Camera(XMVECTOR eye, XMVECTOR at, XMVECTOR up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _eye(eye),
	_at(at),
	_up(up),
	_windowWidth(windowWidth),
	_windowHeight(windowHeight),
	_nearDepth(nearDepth),
	_farDepth(farDepth)
{
	_zoom = XM_PIDIV2;
	XMStoreFloat4x4(&_view, XMMatrixIdentity());
	XMStoreFloat4x4(&_projection, XMMatrixIdentity());
	XMStoreFloat4x4(&_rotation, XMMatrixIdentity());
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

	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(_zoom, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMMATRIX Camera::GetViewProjection() const
{
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX proj = XMLoadFloat4x4(&_projection);
	XMMATRIX viewProj;
	viewProj = XMMatrixMultiply(view, proj);
	return viewProj;
}

XMMATRIX Camera::GetProjection()
{
	XMMATRIX proj;
	proj = XMLoadFloat4x4(&_projection);
	return proj;
}
XMMATRIX Camera::GetView()
{
	XMMATRIX view;
	view = XMLoadFloat4x4(&_view);
	return view;
}

void Camera::SetEye(float x, float y, float z)
{
	XMVECTOR eye = XMVectorSet(x, y, z, 0.0f);
	_eye = eye;
}

void Camera::AddEye(float x, float y, float z)
{
	XMVECTOR eye = XMVectorSet(x, y, z, 0.0f);
	_eye += eye;
}

void Camera::SetAt(float x, float y, float z)
{
	XMVECTOR at = XMVectorSet(x, y, z, 0.0f);
	_at = at;
}

void Camera::AddAt(float x, float y, float z)
{
	XMVECTOR at = XMVectorSet(x, y, z, 0.0f);
	_at += at;
}

void Camera::AddZoom(float zoom)
{
	float temp = XMConvertToDegrees(_zoom);
	temp += zoom;
	_zoom = XMConvertToRadians(temp);
	if (_zoom > _ZOOMMAX)
	{
		_zoom = _ZOOMMAX;
	}
	else if (_zoom < _ZOOMMIN)
	{
		_zoom = _ZOOMMIN;
	}
}

void Camera::ResetZoom()
{
	_zoom = _ZOOMDEFAULT;
}

void Camera::Update()
{
	CalculateViewProjection();

	//Create Rotation Matrix
	XMStoreFloat4x4(&_rotation, XMMatrixRotationRollPitchYaw(_camPitch, _camYaw, 0));

	//Create new Forward and Up vectors
	XMMATRIX rot = XMLoadFloat4x4(&_rotation);
	_at = XMVector3TransformCoord(_defaultForward, rot);
	_up = XMVector3TransformCoord(_defaultUp, rot);

	//Create new forward and strafe vectors
	_camForward = XMVector3Normalize(_at);
	_camRight = XMVector3Cross(_up, _at);
	_camRight = XMVector3Normalize(_camRight);

	_eye += _camRight * _moveLeftRight;
	_eye += _camForward * _moveBackForward;
	_eye += _up * _moveUpDown;

	_moveLeftRight = 0.0f;
	_moveBackForward = 0.0f;
	_moveUpDown = 0.0f;

	//Take into account eye position
	_at = _eye + _at;
	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_eye, _at, _up));
}