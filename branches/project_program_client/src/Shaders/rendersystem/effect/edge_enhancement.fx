//*************************************************************************************************
// 内容:	勾边边缘增强
//---------------------------------------------------------
// 作者:		
// 创建:		2012-11-15
// 最后修改:
//*************************************************************************************************
#include "effect_common.h"

//---------------------------------------------------------------------------------------
// 输入变量
//---------------------------------------------------------------------------------------
float	a_akEdgeParam[8]	: ATTRIBUTE;
#define	a_vBoundCenter		float3(a_akEdgeParam[0], a_akEdgeParam[1], a_akEdgeParam[2])
#define	a_fBoundRadius		a_akEdgeParam[3]
#define	a_vEdgeColor		float3(a_akEdgeParam[4], a_akEdgeParam[5], a_akEdgeParam[6])
#define	a_fDegeWidth		a_akEdgeParam[7]

float	a_akSampleWeights[13]	: ATTRIBUTE;
float2	a_akSampleOffsets[13]	: ATTRIBUTE;

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_SAMPLE(0, sdMat0Sampler,	sdMat0Buf,	false);	

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos		: POSITION;
	float4	vUVSetProjPos	: TEXCOORD0;	
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(float3 vPos : POSITION0)
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;
	
	//float3 vUpDirection 	= float3(0, 0, 1.f);
	//float3 vViewDirection 	= g_vCameraPostion - a_vBoundCenter;
	//float3 vRightDirection 	= cross(vUpDirection, vViewDirection);
	//vRightDirection = normalize(vRightDirection);
	//vUpDirection 	= normalize(cross(vViewDirection, vRightDirection));

	float3 vWorldPos = a_vBoundCenter + a_fBoundRadius * 1.2f * (vPos.x * g_vCameraRight.xyz + vPos.y * g_vCameraUp.xyz);
	float4 vProjPos = mul(float4(vWorldPos, 1.f), g_mWorldViewProj);
	kOutput.vProjPos = vProjPos;
	kOutput.vUVSetProjPos = vProjPos;
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
float4 PS_Main(VS_OUTPUT kInput) : COLOR0
{
	// 计算屏幕纹理坐标
	float2 vUVSetScreen = (kInput.vUVSetProjPos.xy / kInput.vUVSetProjPos.w) * float2(0.5f, -0.5f) 
		+ float2(0.5f, 0.5f) + g_vHalfPixelOffset;
	
	// 采样
	//
	//float fCenter 	= tex2D(sdMat0Sampler, vUVSetScreen).a;
	//float fLeft 	= tex2D(sdMat0Sampler, float2(vUVSetScreen.x - a_fDegeWidth * 2.f * g_vHalfPixelOffset.x, vUVSetScreen.y)).a;
	//float fRight 	= tex2D(sdMat0Sampler, float2(vUVSetScreen.x + a_fDegeWidth * 2.f * g_vHalfPixelOffset.x, vUVSetScreen.y)).a;
	//float fTop 		= tex2D(sdMat0Sampler, float2(vUVSetScreen.x, vUVSetScreen.y - a_fDegeWidth * 2.f * g_vHalfPixelOffset.y)).a;
	//float fBottom 	= tex2D(sdMat0Sampler, float2(vUVSetScreen.x, vUVSetScreen.y + a_fDegeWidth * 2.f * g_vHalfPixelOffset.y)).a;
	//float4 vFlag = float4(fLeft, fRight, fTop, fBottom);
	//float fValue = (dot(vFlag, 1.f) + fCenter) / 5.f;	
	//return float4(a_vEdgeColor, fValue);
	//	
	float fSample = 0.f;
	for (int i = 0; i <= 12; ++i)
	{
		fSample += a_akSampleWeights[i] * tex2D(sdMat0Sampler, vUVSetScreen + a_akSampleOffsets[i] * a_fDegeWidth).a;
	}
	
	if (fSample == 0)
		clip(-1);
		
	return float4(a_vEdgeColor, fSample * 1.6f);
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
technique Effect_EdgeEnhancement
<
	string Description = "Effect_EdgeEnhancement";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_0 VS_Main ();
		PixelShader 	= compile ps_2_0 PS_Main ();
	}
}
//---------------------------------------------------------------------------------------