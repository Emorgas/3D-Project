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

struct Light
{
	float3 dir;
	float3 pos;
	float range;
	float3 att;
	float4 ambient;
	float4 diffuse;
};

cbuffer CbPerFrame
{
	Light light;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(inPos, World);
	output.worldPos = mul(inPos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.normal = mul(normal, World);
	output.TexCoord = inTexCoord;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	input.normal = normalize(input.normal);
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);

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

	float howMuchLight = dot(lightToPixelVec, input.normal);

	if (howMuchLight > 0.0f)
	{
		finalColor += howMuchLight * diffuse * light.diffuse;

		finalColor /= light.att[0] + (light.att[1] * d) + light.att[2] * (d*d);
	}
	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);
}
