
#include "light.h"


Light::Light()
{
}


Light::Light(const Light& other)
{
}


Light::~Light()
{
}


void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	return;
}


void Light::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;
}

void Light::SetRange(float range)
{
	m_range = range;
	return;
}

void Light::SetAttenuation(float a0, float a1, float a2)
{
	m_attenuation = XMFLOAT3(a0, a1, a2);
	return;
}


XMFLOAT4 Light::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT4 Light::GetSpecularColor()
{
	return m_specularColor;
}

XMFLOAT3 Light::GetPosition()
{
	return m_position;
}

float Light::GetRange()
{
	return m_range;
}

XMFLOAT3 Light::GetAttenuation()
{
	return m_attenuation;
}


void Light::GenerateViewMatrix()
{
	XMFLOAT3 up;
	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR look = XMLoadFloat3(&m_lookAt);

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	XMVECTOR upVec = XMLoadFloat3(&up);

	// Create the view matrix from the three vectors.
	m_viewMatrix = XMMatrixLookAtLH(pos, look, upVec);

	return;
}


void Light::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;


	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fieldOfView, screenAspect, screenNear, screenDepth);

	return;
}


void Light::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}


void Light::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}