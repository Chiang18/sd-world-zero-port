//*************************************************************************************************
// ����:	������ɫ���Ĺ�������
//---------------------------------------------------------
// ����:		
// ����:		2012-11-08
// ����޸�:	
//*************************************************************************************************
#ifndef _DecalCommon_H__
#define _DecalCommon_H__

//
#include "../common.h"

//---------------------------------------------------------------------------------------
// �������
//---------------------------------------------------------------------------------------
// �۲�����ϵ����������������ϵ�任����
float4x4	g_mViewToProjectorUV		: GLOBAL;


float4		g_vProjectorUVScaleOffset	: GLOBAL;
float3		g_vProjectorDirection		: GLOBAL;

//---------------------------------------------------------------------------------------
// �������������
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_SAMPLE(0, sdGeomSampler,		sdGeomBuf,	false);		// ��Ļ����뷨�߻���
SD_POINT_CLAMP_SAMPLE(1, sdLightSampler,	sdLightBuf,	false);		// ���ջ���


//---------------------------------------------------------------------------------------
// ������ɫ�����������
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 	vProjPos				: POSITION0;
	float4	vUVSetProjPos			: TEXCOORD0;
	float4	vUVSetFarClipViewPos	: TEXCOORD1;
};

//---------------------------------------------------------------------------------------
// ������ɫ��
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