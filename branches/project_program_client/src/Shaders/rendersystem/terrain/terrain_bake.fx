//*************************************************************************************************
// ����:	���κ決LightMap
//---------------------------------------------------------
// ����:		
// ����:		2012-08-24
// ����޸�:
//*************************************************************************************************
#include "terrain_common.h"


//---------------------------------------------------------------------------------------
// ȫ�ֱ���
//---------------------------------------------------------------------------------------
float4x4 g_vWorldToShadowSpace		: GLOBAL;

float2	g_vRecipLightMapSize		: GLOBAL;
float2	g_vRecipShadowMapSize		: GLOBAL;

float	g_fShadowCameraFarPlane		: GLOBAL;

//---------------------------------------------------------------------------------------
// �������������
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
// ������ɫ�����������
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos		: POSITION;
	float3	vWorldPos		: TEXCOORD0;
};

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(float2 vPos : POSITION0)
{
	VS_OUTPUT kOutput;

	// ��ȡѹ����XY
	//	ix * 129 + iy
	float2 iXY = floor(float2(vPos.x / 129.0, fmod(vPos.x, 129.0) + 0.01));
	
	// �任����������
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
// ������ɫ��
//---------------------------------------------------------------------------------------
float4 PS_Main(VS_OUTPUT kInput) : COLOR0
{
	// ���㵱ǰ����ͶӰ����ϵλ��
	float4 vShadowSpacePos = mul(float4(kInput.vWorldPos, 1.0), g_vWorldToShadowSpace);
	
	// ���㵱ǰ����PCF��Ӱ
	float fShadow = 1.0;//Shadow_PCF(, vShadowSpacePos.w, g_vRecipShadowMapSize, vShadowSpacePos.xy, 0.5f);
	
	return fShadow;
}

//---------------------------------------------------------------------------------------
// ��ɫ����
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