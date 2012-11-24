//*************************************************************************************************
// 内容:	贴花着色器的公共部分
//---------------------------------------------------------
// 作者:		
// 创建:		2012-11-08
// 最后修改:	
//*************************************************************************************************
#ifndef _DecalCommon_H__
#define _DecalCommon_H__

//
#include "../common.h"

//---------------------------------------------------------------------------------------
// 输入变量
//---------------------------------------------------------------------------------------
// 观察坐标系到贴花立方体坐标系变换矩阵
float4x4	g_mViewToProjectorUV		: GLOBAL;


float4		g_vProjectorUVScaleOffset	: GLOBAL;
float3		g_vProjectorDirection		: GLOBAL;

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_SAMPLE(0, sdGeomSampler,		sdGeomBuf,	false);		// 屏幕深度与法线缓存
SD_POINT_CLAMP_SAMPLE(1, sdLightSampler,	sdLightBuf,	false);		// 光照缓存


//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 	vProjPos				: POSITION0;
	float4	vUVSetProjPos			: TEXCOORD0;
	float4	vUVSetFarClipViewPos	: TEXCOORD1;
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(float3 vPos : POSITION0)
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;
	
	float4 vProjPos = mul(float4(vPos, 1.f), g_mWorldViewProj);
	kOutput.vProjPos = vProjPos;
	
	float2 vTProjPos = vProjPos.xy / vProjPos.w;
	kOutput.vUVSetFarClipViewPos = mul(float4(vTProjPos, 1, 1), g_mDepthToView) * vProjPos.w;

	float2 vVProjPos = GetTexCoordForPixel(vTProjPos);
	kOutput.vUVSetProjPos  = float4(vVProjPos, 0, 1) * vProjPos.w;
	
	return kOutput;
}
//---------------------------------------------------------------------------------------
#endif