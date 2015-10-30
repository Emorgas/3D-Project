//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;

}

struct PointLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 pos;
	float range;
	
	float3 att;
	float pad;

};

cbuffer CbPerFrame
{
	PointLight light;
	float3 eyePosW;
	float pad;
	float4 SpecularMaterial;
	float SpecularPower;
	float3 pad2;
};

Texture2D ObjTexture[2];
SamplerState ObjSamplerState;

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 biTangent : BITANGENT)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(inPos, World);
	output.worldPos = mul(inPos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.normal = mul(normal, World);
	output.TexCoord = inTexCoord;
	output.tangent = mul(tangent, (float3x3)World);
	output.tangent = normalize(output.tangent);
	output.biTangent = mul(biTangent, (float3x3)World);
	output.biTangent = normalize(output.biTangent);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	input.normal = normalize(input.normal);
	float4 diffuse = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
	float4 bumpMap = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
	
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	float3 bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);
	bumpNormal = normalize(bumpNormal);

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	//Create vector between light and pixel
	float3 lightToPixelVec = light.pos - input.worldPos;

	//find distance between light pos and pixel pos
	float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * light.ambient;
	if (d > light.range)
		return float4(finalAmbient, diffuse.a);

	//Turn lightToPixelVec into a unit vector describing pixel direction from the light position
	lightToPixelVec /= d;

	float howMuchLight = dot(lightToPixelVec, bumpNormal);

	float3 toEye = normalize(eyePosW - input.worldPos);
	float3 spec;
	[flatten]
	if (howMuchLight > 0.0f)
	{
		float3 v = reflect(-lightToPixelVec, bumpNormal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), SpecularPower);
		spec = specFactor * SpecularMaterial * light.specular;
		finalColor += howMuchLight * diffuse * light.diffuse;

		finalColor /= light.att[0] + (light.att[1] * d) + light.att[2] * (d*d);
	}

	finalColor = saturate(finalColor + finalAmbient + spec);

	return float4(finalColor, diffuse.a);
}
