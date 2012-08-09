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
// ������ɫ������������
//---------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	vPos			: POSITION0;	// ��Ļ���ζ���
	float2	vUVSet0			: TEXCOORD0;	// ��Ļ������������
};

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
// ƽ̹������ɫ
float4 PS_Main_Planar(VS_OUTPUT kInput) : COLOR0
{
	// ��ȡ�������
	//float2 vPackedDepthTex = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	//float fDepth = UnpackDepth(vPackedDepthTex.xy);
	
	// ������������
	// (�����������,�����λ�ú�Զƽ���Ӧ��λ�ý��в�ֵ)
	//float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// ���㵱ǰ��ĵ������UV
	//float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;

	// ����UV����NormalMap,
	//float4 vNormalTex 	= tex2D(sdBaseNormalSampler, vUVSet);
	
	// �������ռ䷨��(x,y,z), ת�����۲�����ϵ
	//float3 vWorldNormal = vNormalTex.xy 
	//vWorldNormal.xy	= vWorldNormal.xy * 2.0 - 1.0;
	//vWorldNormal.z 	= sqrt(dot(float3(1.0, vWorldNormal.xy), float3(1.0, -vWorldNormal.xy)));
	
	//
	
	
	
	return float4(1,0,0,0);
};
//---------------------------------------------------------------------------------------
// ���͵�����ɫ



//---------------------------------------------------------------------------------------
// ��ɫ����
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
technique Terrain_AtlasShading_Planar
<
	string Description = "Terrain_AtlasShading_Planar";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Planar ();
	}
}
//---------------------------------------------------------------------------------------




















