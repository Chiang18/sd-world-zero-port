//*************************************************************************************************
// ����:	������ɫ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-23
// ����޸�:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
float4 g_vRecipUVRepeats[3]	: GLOBAL;

//---------------------------------------------------------------------------------------
// �������������
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_SAMPLE(0, sdGeomSampler,			sdGeomBuf, 			false);		// ��Ļ����뷨�߻���
SD_POINT_CLAMP_SAMPLE(1, sdLightSampler,		sdLightBuf, 		false);		// ��Ļ�ֲ����ջ���

SD_LINEAR_WRAP_SAMPLE(2, sdBaseDiffuseSampler, 	sdBaseDiffuseTex, 	false);		// ���λ�����������ͼ
SD_LINEAR_WRAP_SAMPLE(3, sdBaseNormalSampler, 	sdBaseNormalTex, 	false);		// ���λ���������ͼ
SD_LINEAR_WRAP_SAMPLE(4, sdBlendSampler,		sdBlendTex,			false);		// ���λ��Ȩ����ͼ

SD_LINEAR_WRAP_SAMPLE( 5, sdDiffuseSampler00, sdDiffuseTex00, false)
SD_LINEAR_WRAP_SAMPLE( 6, sdDiffuseSampler01, sdDiffuseTex01, false)
SD_LINEAR_WRAP_SAMPLE( 7, sdDiffuseSampler02, sdDiffuseTex02, false)
SD_LINEAR_WRAP_SAMPLE( 8, sdDiffuseSampler03, sdDiffuseTex03, false)
SD_LINEAR_WRAP_SAMPLE( 9, sdDiffuseSampler04, sdDiffuseTex04, false)
SD_LINEAR_WRAP_SAMPLE(10, sdDiffuseSampler05, sdDiffuseTex05, false)
SD_LINEAR_WRAP_SAMPLE(11, sdDiffuseSampler06, sdDiffuseTex06, false)
SD_LINEAR_WRAP_SAMPLE(12, sdDiffuseSampler07, sdDiffuseTex07, false)
SD_LINEAR_WRAP_SAMPLE(13, sdDiffuseSampler08, sdDiffuseTex08, false)
SD_LINEAR_WRAP_SAMPLE(14, sdDiffuseSampler09, sdDiffuseTex09, false)
SD_LINEAR_WRAP_SAMPLE(15, sdDiffuseSampler10, sdDiffuseTex10, false)
SD_LINEAR_WRAP_SAMPLE(16, sdDiffuseSampler11, sdDiffuseTex11, false)

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
// 
//---------------------------------------------------------------------------------------
// ƽ����ɫXY
float4 PS_Main_Planar_XY(VS_OUTPUT kInput) : COLOR0
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
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// �����б���
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.z - 0.001);
	// @}
	
	
	// BlendMap
	// @{
	// ����UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// ����BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	
	//
	float fTotalWeight = dot(float3(dot(vBlendWeight00, 1.0), dot(vBlendWeight01, 1.0), dot(vBlendWeight02, 1.0)), 1.0);
	
	// ����BlendMap
	float4 vBaseDiffuseDarkColor = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.5));

#ifdef _SD_EDITOR
	vBaseDiffuseDarkColor =  max(vBaseDiffuseDarkColor, float4(g_vVertexColorMask, g_fLightMapMask));
#endif
	// @}
	
	
	// NormalMap
	// @{
	// ����UV
	float2 vUVSet2 = float2(vWorldPos.x, -vWorldPos.y);
	
	// ����NormalMap
	float4 vDiffuseGloss = tex2D(sdDiffuseSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
	vDiffuseGloss /= fTotalWeight;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuseGloss, float4(g_vDiffuseMapMask, g_fGlossMapMask));
#endif
	
	vDiffuseGloss *= float4(vBaseDiffuseDarkColor.rgb, 1.0);
	// @}
	
	
	// ����������ɫ
	// @{
	// ����LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// ����۲췽���뷨��
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);
	
#if 0
	// �ϳɹ���
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBaseDiffuseDarkColor.a, vLightTex, vDiffuseLight, vSpeculatLight);
#else
	// ����
	float3 vLightVector = -g_vMainLightDir;
	float3 vReflectVector = reflect(-vLightVector, vViewNormal);
	float3 vNormalLightVector = saturate(dot(vViewNormal, vLightVector));
	float fViewReflectVector = saturate(dot(vViewVector, vReflectVector));
	
	// �������ع���
	float4 vLocalLight = vLightTex * g_fLocalLightRange;
	
	// �ϳɹ���
	float3 vDiffuseLight = lerp(g_vMainLightAmbientColor, g_vMainLightColor, vBaseDiffuseDarkColor.a * vNormalLightVector);
	float3 vSpeculatLight = vDiffuseLight * pow(fViewReflectVector, g_fTerrainShiness);
	
	vDiffuseLight 	+= vLocalLight.rgb * g_fLocalLightRange;
	vSpeculatLight	+= vLocalLight.rgb / (dot(vLocalLight.rgb, g_vLuminScale) + 0.001) * vLocalLight.a;
	
	//vDiffuseLight += lerp(g_vTerraimAmbientColor, g_vSkyAmbientColor, (vViewNormal.z + 1.0) * 0.5);
	vDiffuseLight += g_vSkyAmbientColor;
#endif	
	
	// �ϳ�������ɫ
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
					
	vColor *= vPlanarWeight.z;
	// @}
	
	return float4(vColor, 0);
}

//---------------------------------------------------------------------------------------
// ƽ����ɫYZ
float4 PS_Main_Planar_YZ(VS_OUTPUT kInput) : COLOR0
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
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// �����б���
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.x - 0.001);
	// @}
	
	
	// BlendMap
	// @{
	// ����UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// ����BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	
	//
	float fTotalWeight = dot(float3(dot(vBlendWeight00, 1.0), dot(vBlendWeight01, 1.0), dot(vBlendWeight02, 1.0)), 1.0);
	
	// ����BlendMap
	float4 vBaseDiffuseDarkColor = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.5));

#ifdef _SD_EDITOR
	vBaseDiffuseDarkColor =  max(vBaseDiffuseDarkColor, float4(g_vVertexColorMask, g_fLightMapMask));
#endif
	// @}
	
	
	// NormalMap
	// @{
	// ����UV
	float2 vUVSet2 = float2(vWorldPos.y * sign(vBaseNormalTex.x - 0.5), -vWorldPos.z);
	
	// ����NormalMap
	float4 vDiffuseGloss = tex2D(sdDiffuseSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuseGloss, float4(g_vDiffuseMapMask, g_fGlossMapMask));
#endif
	
	vDiffuseGloss *= float4(vBaseDiffuseDarkColor.rgb, 1.0);
	// @}
	
	
	// ����������ɫ
	// @{
	// ����LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// ����۲췽���뷨��
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);

#if 0
	// �ϳɹ���
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBaseDiffuseDarkColor.a, vLightTex, vDiffuseLight, vSpeculatLight);
#else
	// ����
	float3 vLightVector = -g_vMainLightDir;
	float3 vReflectVector = reflect(-vLightVector, vViewNormal);
	float3 vNormalLightVector = saturate(dot(vViewNormal, vLightVector));
	float fViewReflectVector = saturate(dot(vViewVector, vReflectVector));
	
	// �������ع���
	float4 vLocalLight = vLightTex * g_fLocalLightRange;
	
	// �ϳɹ���
	float3 vDiffuseLight = lerp(g_vMainLightAmbientColor, g_vMainLightColor, vBaseDiffuseDarkColor.a * vNormalLightVector);
	float3 vSpeculatLight = vDiffuseLight * pow(fViewReflectVector, g_fTerrainShiness);
	
	vDiffuseLight 	+= vLocalLight.rgb * g_fLocalLightRange;
	vSpeculatLight	+= vLocalLight.rgb / (dot(vLocalLight.rgb, g_vLuminScale) + 0.001) * vLocalLight.a;
	
	//vDiffuseLight += lerp(g_vTerraimAmbientColor, g_vSkyAmbientColor, (vViewNormal.z + 1.0) * 0.5);
	vDiffuseLight += g_vSkyAmbientColor;
#endif	

	// �ϳ�������ɫ
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
					
	vColor *= vPlanarWeight.x;
	// @}
	
	return float4(vColor, 0);
}

//---------------------------------------------------------------------------------------
// ƽ����ɫXZ
float4 PS_Main_Planar_XZ(VS_OUTPUT kInput) : COLOR0
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
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// �����б���
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.y - 0.001);
	// @}
	
	
	// BlendMap
	// @{
	// ����UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// ����BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	
	//
	float fTotalWeight = dot(float3(dot(vBlendWeight00, 1.0), dot(vBlendWeight01, 1.0), dot(vBlendWeight02, 1.0)), 1.0);
	
	// ����BlendMap
	float4 vBaseDiffuseDarkColor = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.5));

#ifdef _SD_EDITOR
	vBaseDiffuseDarkColor =  max(vBaseDiffuseDarkColor, float4(g_vVertexColorMask, g_fLightMapMask));
#endif
	// @}
	
	
	// NormalMap
	// @{
	// ����UV
	float2 vUVSet2 = float2(-vWorldPos.x * sign(vBaseNormalTex.y - 0.5), -vWorldPos.z);
	
	// ����NormalMap
	float4 vDiffuseGloss = tex2D(sdDiffuseSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vDiffuseGloss += tex2D(sdDiffuseSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vDiffuseGloss += tex2D(sdDiffuseSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vDiffuseGloss += tex2D(sdDiffuseSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vDiffuseGloss += tex2D(sdDiffuseSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
	vDiffuseGloss /= fTotalWeight;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuseGloss, float4(g_vDiffuseMapMask, g_fGlossMapMask));
#endif
	
	vDiffuseGloss *= float4(vBaseDiffuseDarkColor.rgb, 1.0);
	// @}
	
	
	// ����������ɫ
	// @{
	// ����LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// ����۲췽���뷨��
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);
	
#if 0
	// �ϳɹ���
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBaseDiffuseDarkColor.a, vLightTex, vDiffuseLight, vSpeculatLight);
#else
	// ����
	float3 vLightVector = -g_vMainLightDir;
	float3 vReflectVector = reflect(-vLightVector, vViewNormal);
	float3 vNormalLightVector = saturate(dot(vViewNormal, vLightVector));
	float fViewReflectVector = saturate(dot(vViewVector, vReflectVector));
	
	// �������ع���
	float4 vLocalLight = vLightTex * g_fLocalLightRange;
	
	// �ϳɹ���
	float3 vDiffuseLight = lerp(g_vMainLightAmbientColor, g_vMainLightColor, vBaseDiffuseDarkColor.a * vNormalLightVector);
	float3 vSpeculatLight = vDiffuseLight * pow(fViewReflectVector, g_fTerrainShiness);
	
	vDiffuseLight 	+= vLocalLight.rgb * g_fLocalLightRange;
	vSpeculatLight	+= vLocalLight.rgb / (dot(vLocalLight.rgb, g_vLuminScale) + 0.001) * vLocalLight.a;
	
	//vDiffuseLight += lerp(g_vTerraimAmbientColor, g_vSkyAmbientColor, (vViewNormal.z + 1.0) * 0.5);
	vDiffuseLight += g_vSkyAmbientColor;
#endif
	
	// �ϳ�������ɫ
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
					
	vColor *= vPlanarWeight.y;
	// @}
	
	return float4(vColor, 0);
}

//---------------------------------------------------------------------------------------
// ��ɫ����
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
technique Terrain_AtlasShading_Planar_XY_Ortho
<
	string Description = "Terrain_AtlasShading_Planar_XY_Ortho";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Planar_XY ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_AtlasShading_Planar_YZ_Ortho
<
	string Description = "Terrain_AtlasShading_Planar_YZ_Ortho";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Planar_YZ ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_AtlasShading_Planar_XZ_Ortho
<
	string Description = "Terrain_AtlasShading_Planar_XZ_Ortho";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Planar_XZ ();
	}
}
//---------------------------------------------------------------------------------------