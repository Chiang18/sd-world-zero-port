//*************************************************************************************************
// ����:	������ɫ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-23
// ����޸�:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// �������������
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_CUBESAMPLE(0, sdPlanarTableSampler, 	sdPlanarTableTex);				//
SD_POINT_CLAMP_SAMPLE(1, sdGeomSampler, 			sdGeomBuf, 			false);		// ��Ļ����뷨�߻���
SD_POINT_CLAMP_SAMPLE(2, sdLightSampler, 			sdLightBuf, 		false);		// ��Ļ�ֲ����ջ���
SD_LINEAR_WRAP_SAMPLE(3, sdBaseNormalSampler, 		sdBaseNormalTex, 	false);		// ���λ���������ͼ
SD_POINT_CLAMP_SAMPLE(4, sdTileSampler,				sdTileTex,			false);		// ����TileMap
SD_LINEAR_WRAP_SAMPLE(5, sdBlendSampler,			sdBlendTex,			false);		// ���λ��Ȩ����ͼ
SD_POINT_CLAMP_SAMPLE(6, sdAtlasTableSampler,		sdAtlasTableTex,	false);		// ������������ͼ��ѯ��
SD_LINEAR_CLAMP_SAMPLE(7, sdDiffuseAtlasSampler,	sdDiffuseAtlasTex,	false);		// ������������ͼͼ��

//---------------------------------------------------------------------------------------
// ������ɫ������������
//---------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	vPos			: POSITION0;	// ��Ļ���ζ���
	float2	vUVSet0			: TEXCOORD0;	// ��Ļ���ζ�����������
};

//---------------------------------------------------------------------------------------
// ������ɫ�����������
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos			: POSITION;		// ͶӰ����
	float2	vUVSetScreenTex		: TEXCOORD0;	// ����������ƫ�Ƶ���Ļ��������
	float3	vUVNearClipWorldPos	: TEXCOORD1;	// ��ǰ���Ӧ���ü����ϵĵ����������
	float3	vUVFarClipWorldPos	: TEXCOORD2;	// ��ǰ���ӦԶ�ü����ϵĵ����������
	float3	vUVFarClipViewPos	: TEXCOORD3;	// ��ǰ���ӦԶ�ü����ϵĵ�Ĺ۲�����
};

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(VS_INPUT kInput)
{
	VS_OUTPUT kOutput;

	// ת����ͶӰ����
	kOutput.vProjPos = float4(kInput.vPos, 1.0);
	
	// ����������ƫ�Ƶ���Ļ��������
	kOutput.vUVSetScreenTex = kInput.vUVSet0 + g_vHalfPixelOffset;
	
	// ��ǰ���Ӧ���ü����ϵĵ����������
	kOutput.vUVNearClipWorldPos = mul(float4(kInput.vPos.xy, 0.0, 1.0), g_mDepthToWorld).xyz;
	
	// ��ǰ���ӦԶ�ü����ϵĵ����������
	float4 vUVFarClipProjPos  = float4(kInput.vPos.xy, 1.0, 1.0);
	float4 vUVFarClipWorldPos = mul(vUVFarClipProjPos, g_mDepthToWorld);
	kOutput.vUVFarClipWorldPos = vUVFarClipWorldPos.xyz;
	
	// ��ǰ���ӦԶ�ü����ϵĵ�Ĺ۲�����
	// (�滻��w������Ϊ�˱����������ۻ�?)
	kOutput.vUVFarClipViewPos  = mul(float4(vUVFarClipWorldPos.xyz, 1.0), g_mView).xyz;
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
float4 PS_Main_Planar(VS_OUTPUT kInput) : COLOR0
{
	// ��ȡ�������
	float4 vGeoTex = tex2D(sdGeomSampler, kInput.vUVSetScreenTex);
	float fDepth = UnpackDepth(vGeoTex.xy);
	
	// ������������
	// (�����������,�Խ�Զƽ���Ӧ��λ�ý��в�ֵ)
	float3 vWorldPos = lerp(kInput.vUVNearClipWorldPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// ���㵱ǰ��ĵ������UV(ע��,����û��ƫ�ư�����)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// ����UV����NormalMap(Sampler��Linear,û��ƫ�ư�����)
	float2 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet).xy;
	
	// �������ռ䷨��
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	// @}
	
	
	// TileMap
	// @{
	// ����UV����TileMap,
	float4 vIndices = tex2D(sdTileSampler, vUVSet) * 255.0;
	// @}
	
	
	// BlendMap
	// @{
	// �����µ�UV(����,�����Ϊ����Tile��Ե�����ں�)
	//float2 tileCenterOffset = frac(vUVSet * (2048.0 / 4.0)) - 0.5;
	//vUVSet -= tileCenterOffset * (1.0 / 2048.0);

	// ����UV����BlendMap
	float4 vBlendTex = tex2D(sdBlendSampler, vUVSet);
	
	// ��һ��Ȩ��
	float fTotalWeight = dot(vBlendTex.xyz, 1.0);
	vBlendTex.rgb /= fTotalWeight;
	
#ifdef _SD_EDITOR
	vBlendTex.a = max(vBlendTex.a, g_fLightMapMask);
#endif
	// @}
	
	
	// ��ͼ���
	// @{
	// ��������������
	float4 vPlanarVec = texCUBE(sdPlanarTableSampler, vWorldNormal.xzy) * 255 - 1;
	
	// �����µĵ���UV
	float2 vUVSet2 = float2(dot(vWorldPos.xy, vPlanarVec.xy), dot(vWorldPos.yz, vPlanarVec.zw));
	
	// ���㵱ǰ���ص��۲��ʸ��
	float3 vWorldViewVector = normalize(g_vViewPos - kInput.vUVFarClipWorldPos);
	
	// ���㵱ǰ����ӦȡLOD(���ﲻ��,�д���һ����ע)(��,�̶�����)
	float fLodLevel = log2(2.0 * length( kInput.vUVFarClipViewPos) / 768.0 / max(sqrt(dot(vWorldViewVector, vWorldNormal)), 0.25));
	
	// ����ͼ��UV
	float4 vUVSetTableU = saturate(vIndices.bgra * g_fNormalAtlasIdScale + g_fNormalAtlasIdOffset);
	float vUVSetTableV	= saturate(fLodLevel * g_fNormalAtlasLevelScale + g_fNormalAtlasLevelOffset);
	
	// ��ͼ���
	float4 vDiffuseGloss = SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, float2(vUVSetTableU.x, vUVSetTableV), vUVSet2) * vBlendTex.b +
						   SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, float2(vUVSetTableU.y, vUVSetTableV), vUVSet2) * vBlendTex.g +
						   SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, float2(vUVSetTableU.z, vUVSetTableV), vUVSet2) * vBlendTex.r;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuse_Gloss, float4(g_vDiffuseMapMask,g_fGlossMapMask));
#endif
	// @}
	
	
	// ����������ɫ
	// @{
	// ����LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// ����۲췽���뷨��
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);
	
	// �ϳɹ���
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBlendTex.a, vLightTex, vDiffuseLight, vSpeculatLight);
	
	// �ϳ�������ɫ
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
	// @}
	
	return float4(vColor, 0);
}

//---------------------------------------------------------------------------------------
// ��ɫ����
//---------------------------------------------------------------------------------------
technique Terrain_AtlasShading_Planar_Ortho
<
	string Description = "Terrain_AtlasShading_Planar_Ortho";
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