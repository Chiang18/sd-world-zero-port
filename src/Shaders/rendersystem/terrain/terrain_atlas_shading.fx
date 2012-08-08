//*************************************************************************************************
// ����:	������ɫ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-08
// ����޸�:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// �������������
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_CUBESAMPLE(0, sdPlanarTableSampler, 	sdPlanarTableTex);				//
SD_POINT_CLAMP_SAMPLE(1, sdGeomSampler, 			sdGeomBuf, 			false);		// ��Ļ����뷨�߻���
SD_POINT_CLAMP_SAMPLE(2, sdLightSampler, 			sdLightBuf, 		false);		// �ֲ����ջ���
SD_LINEAR_WRAP_SAMPLE(3, sdBaseNormalSampler, 		sdBaseNormalTex, 	false);		// ���λ���������ͼ
SD_POINT_CLAMP_SAMPLE(4, sdTileMapSampler,			sdTileTex,			false);		// ����Tile
SD_LINEAR_WRAP_SAMPLE(5, sdBlendSampler,			sdBlendTex,			false);		// ���λ��Ȩ����ͼ
SD_POINT_CLAMP_SAMPLE(6, sdAtlasTableSampler,		sdAtlasTableTex,	false);		// ������������ͼ��ѯ��
SD_POINT_CLAMP_SAMPLE(7, sdDiffuseAtlasSampler,		sdDiffuseAtlasTex,	false);		// ������������ͼͼ��


//---------------------------------------------------------------------------------------
// ������ɫ�����������
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos			: POSITION;		// ͶӰ����
	float2	vUVSetScreenTex		: TEXCOORD0;	// ����������ƫ�Ƶ���Ļ��������
	float3	vUVFarClipWorldPos	: TEXCOORD1;	// ��ǰ���ӦԶ�ü����ϵĵ����������
	float3	vUVFarClipViewPos	: TEXCOORD2;	// ��ǰ���ӦԶ�ü����ϵĵ�Ĺ۲�����
};


//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(VS_INPUT kInput)
{
	VS_OUTPUT kOutput;

	// ͶӰ����
	kOutput.vProjPos = float4(kInput.vPos, 1.0);
	
	// ����������ƫ�Ƶ���Ļ��������
	kOutput.vUVSetScreenTex = kInput.vUVSet0 + g_vHalfPixelOffset;
	
	// ��ǰ���ӦԶ�ü����ϵĵ����������
	float4 vUVFarClipProjPos  = float4(kInput.vPos.xy, 1.0, 1.0);
	float4 vUVFarClipWorldPos = mul(vUVFarClipProjPos, g_mDepthToWorld);
	kOutput.vUVFarClipWorldPos = vUVFarClipWorldPos.xyz;
	
	// ��ǰ���ӦԶ�ü����ϵĵ�Ĺ۲�����
	// (�滻��w������Ϊ�˱����������ۻ�)
	kOutput.vUVFarClipViewPos  = mul(float4(vUVFarClipWorldPos.xyz, 1.0), g_mView).xyz;
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
// 






















