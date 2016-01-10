#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	void SetRange(float);
	void SetAttenuation(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	XMFLOAT3 GetPosition();
	float GetRange();
	XMFLOAT3 GetAttenuation();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	float	 m_range;
	XMFLOAT3 m_attenuation;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};