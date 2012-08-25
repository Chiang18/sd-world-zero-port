//*************************************************************************************************
// 内容:	地形烘焙LightMap
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-24
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"


//---------------------------------------------------------------------------------------
// 全局变量
//---------------------------------------------------------------------------------------
float4x4 g_vWorldToShadowSpace		: GLOBAL;

float2	g_vRecipLightMapSize		: GLOBAL;
float2	g_vRecipShadowMapSize		: GLOBAL;

float	g_fShadowCameraFarPlane		: GLOBAL;

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
// ShadowMap
texture sdShadowBuf
<
	string NTM = "shader";
	int NTMIndex = 3;
>;

sampler sdShadowSampler = sampler_state
{
	Texture = (sdShadowBuf);
	AddressU = CLAMP;
	AddressV = CLAMP;	
	AddressW = CLAMP;	
	MipFilter = NONE;
	MinFilter = POINT;
	MagFilter = POINT;
};

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos		: POSITION;
	float3	vWorldPos		: TEXCOORD0;
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(float2 vPos : POSITION0)
{
	VS_OUTPUT kOutput;

	// 提取压缩的XY
	//	ix * 129 + iy
	float2 iXY = floor(float2(vPos.x / 129.0, fmod(vPos.x, 129.0) + 0.01));
	
	// 变换到世界坐标
	// 	floor(fH * 20.0f + 0.5f)
	float4 vWorldPosition = float4(iXY * a_fQuadScale + a_vQuadOrigin, vPos.y * 0.05, 1.0);

	//
	float2 vUVSet = vWorldPosition.xy * g_vRecipTerrainSize;
	vUVSet = vUVSet * float2(2.0, -2.0) + float2(-1.0. 1.0);
	vUVSet = vUVSet + g_vRecipLightMapSize * float2(-1.0. 1.0);
	
	kOutput.vProjPos = float4(vUVSet, 0.0, 1.0);
	kOutput.vWorldPos = vWorldPosition.xyz;
	return kOutput;
}
//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
float4 PS_Main(VS_OUTPUT kInput) : COLOR0
{
	// 计算当前像素投影坐标系位置
	float4 vShadowSpacePos = mul(float4(kInput.vWorldPos, 1.0), g_vWorldToShadowSpace);
	
	// 计算当前像素PCF阴影
	float fShadow = 1.0;//Shadow_PCF(, vShadowSpacePos.w, g_vRecipShadowMapSize, vShadowSpacePos.xy, 0.5f);
	
	return fShadow;
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
technique Terrain_Bake
<
	string Description = "Terrain_Bake";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main ();
	}
}
//---------------------------------------------------------------------------------------