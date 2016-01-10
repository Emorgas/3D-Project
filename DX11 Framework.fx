//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ShadowConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix LightView;
	matrix LightProj;	
	matrix LightView2;
	matrix LightProj2;
	float3 VertexLightPos;
	float pad01;
	float3 VertexLightPos2;
	float pad1;
}

cbuffer CbPerFrame
{
	float4 lightAmbientColor;
	float4 lightDiffuseColor;
	float4 lightSpecularColor;
	float4 lightDiffuseColor2;
	float4 lightSpecularColor2;
	float3 inLightPos;
	float pad0;
	float3 lightAttenuation;
	float lightRange;
	float3 inLightPos2;
	float pad02;
	float3 lightAttenuation2;
	float lightRange2;
	float3 eyePosW;
	float pad;
	float4 SpecularMaterial;
	float SpecularPower;
	float3 pad2;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float4 lightViewPosition2 : TEXCOORD3;
	float3 lightPos2 : TEXCOORD4;
};

void CalcTanBiTan(float3 norm, out float3 tan, out float3 biTan)
{

	float3 c1 = cross(norm, float3(0.0f, 0.0f, 1.0f));
	float3 c2 = cross(norm, float3(0.0f, 1.0f, 0.0f));

	if (length(c1) > length(c2))
	{
		tan = c1;
	}
	else
	{
		tan = c2;
	}
	tan = normalize(tan);

	biTan = cross(norm, tan);
	biTan = normalize(biTan);
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD0, float3 normal : NORMAL)
{
	VS_OUTPUT output;
	//Shadows//

	inPos.w = 1.0f;

	output.Pos = mul(inPos, World);
	output.worldPos = mul(inPos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.normal = mul(normal, World);
	output.normal = normalize(output.normal);
	output.TexCoord = inTexCoord;

	output.lightViewPosition = mul(inPos, World);
	output.lightViewPosition = mul(output.lightViewPosition, LightView);
	output.lightViewPosition = mul(output.lightViewPosition, LightProj);

	output.lightViewPosition2 = mul(inPos, World);
	output.lightViewPosition2 = mul(output.lightViewPosition2, LightView2);
	output.lightViewPosition2 = mul(output.lightViewPosition2, LightProj2);

	output.lightPos = VertexLightPos.xyz - output.worldPos.xyz;
	output.lightPos = normalize(output.lightPos);

	output.lightPos2 = VertexLightPos2.xyz - output.worldPos.xyz;
	output.lightPos2 = normalize(output.lightPos2);

	float3 tangent, biTangent;
	CalcTanBiTan(normal, tangent, biTangent);

	output.tangent = mul(tangent, (float3x3)World);
	output.tangent = normalize(output.tangent);
	output.biTangent = mul(biTangent, (float3x3)World);
	output.biTangent = normalize(output.biTangent);

	return output;
}

Texture2D ObjTextures[4];
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float bias;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 spec = float4(0.0f, 0.0f, 0.0f, 1.0f);
	input.normal = normalize(input.normal);
	float4 diffuse = ObjTextures[0].Sample(SampleTypeWrap, input.TexCoord);
	float4 bumpMap = ObjTextures[1].Sample(SampleTypeWrap, input.TexCoord);

	bias = 0.001f;

	bumpMap = (bumpMap * 2.0f) - 1.0f;

	float3 bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);
	bumpNormal = normalize(bumpNormal);

	float4 finalColor = diffuse * lightAmbientColor;

	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = ObjTextures[2].Sample(SampleTypeClamp, projectTexCoord).r;
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			//Create vector between light and pixel
			float3 lightToPixelVec = input.lightPos;

			//find distance between light pos and pixel pos
			float d = length(lightToPixelVec);

			//float4 finalAmbient = diffuse * lightAmbientColor;
			if (d > lightRange)
				return float4(finalColor);

			//Turn lightToPixelVec into a unit vector describing pixel direction from the light position
			lightToPixelVec /= d;

			float howMuchLight = saturate(dot(lightToPixelVec, bumpNormal));

			float3 toEye = normalize(eyePosW - input.worldPos);
			[flatten]
			if (howMuchLight > 0.0f)
			{
				float3 v = reflect(-lightToPixelVec, bumpNormal);
				float specFactor = pow(max(dot(v, toEye), 0.0f), SpecularPower);
				spec += specFactor * SpecularMaterial * lightSpecularColor;
				finalColor += howMuchLight * diffuse * lightDiffuseColor;

				//finalColor /= lightAttenuation[0] + (lightAttenuation[1] * d) + lightAttenuation[2] * (d*d);
			}
//final
		}
	}

	//Light 2
	projectTexCoord.x =  input.lightViewPosition2.x / input.lightViewPosition2.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition2.y / input.lightViewPosition2.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = ObjTextures[3].Sample(SampleTypeClamp, projectTexCoord).r;
		lightDepthValue = input.lightViewPosition2.z / input.lightViewPosition2.w;
		lightDepthValue = lightDepthValue - bias;
		if (lightDepthValue < depthValue)
		{
			//Create vector between light and pixel
			float3 lightToPixelVec = input.lightPos2;

			//find distance between light pos and pixel pos
			float d = length(lightToPixelVec);

			//float4 finalAmbient = diffuse * lightAmbientColor;
			if (d > lightRange)
				return float4(finalColor);

			//Turn lightToPixelVec into a unit vector describing pixel direction from the light position
			lightToPixelVec /= d;

			float howMuchLight = saturate(dot(lightToPixelVec, bumpNormal));

			float3 toEye = normalize(eyePosW - input.worldPos);
			[flatten]
			if (howMuchLight > 0.0f)
			{
				float3 v = reflect(-lightToPixelVec, bumpNormal);
				float specFactor = pow(max(dot(v, toEye), 0.0f), SpecularPower);
				spec += specFactor * SpecularMaterial * lightSpecularColor2;
				finalColor += howMuchLight * diffuse * lightDiffuseColor2;

				//finalColor /= lightAttenuation[0] + (lightAttenuation[1] * d) + lightAttenuation[2] * (d*d);
			}
//final
		}
	}
	finalColor = saturate(finalColor + spec);
		//finalColor = finalColor * diffuse;
	return float4(finalColor);
}

//--------------------------------------------------------------------------------------
// Shadow Vertex Shader
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//Shadow Pixel Shader
//--------------------------------------------------------------------------------------