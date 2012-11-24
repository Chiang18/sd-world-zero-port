//*************************************************************************************************
// ����:	StaticMesh��MRT3GeometryPass��ɫ������ʵ��(V3��)
//			ʹ��ѹ��MRT��Ⱦ,ʹ������RGBA8 RenderTarget
//---------------------------------------------------------
// ����:		
// ����:		2012-07-14
// ����޸�:
//*************************************************************************************************
// 1.���ﲻ��Ҫ��������ظ��ĺ�,��Ϊ���ļ��ᱻ�ظ�����
// 2.�������Flag��ͬ��̬��������,������������ظ�
#include "static_mesh_common.h"

//---------------------------------------------------------------------------------------
// ȡ��һЩ�ڲ����õ���Ԥ�����
//---------------------------------------------------------------------------------------
#undef VS_INPUT
#undef VS_OUTPUT
#undef PS_OUTPUT
#undef VS_SHADER
#undef PS_SHADER
#undef TECHNIQUE

// ���ڶ���ͨ���������
#undef DIFFUSEMAP
#undef NORMALMAP
#undef GLOSSMAP
#undef LIGHTMAP
#undef GLOWMAP
#undef DETAILNORMALMAP
#undef ALPHATEST

// ���ڶ�����ɫ����������ṹ
#undef VS_OUTPUT_VERTEX_NORMAL
#undef VS_OUTPUT_BASE_UV
#undef VS_OUTPUT_UNIQUE_UV

//---------------------------------------------------------------------------------------
// ����CHANNELS_FLAG��ֵ����ȡ��ǰ���õ�ͨ�����
//---------------------------------------------------------------------------------------
#if TEST_CHANNEL(CHANNELS_FLAG, DIFFUSEMAP_CHANNEL)
	#define DIFFUSEMAP 1
#else
	#define DIFFUSEMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, NORMALMAP_CHANNEL)
	#define NORMALMAP 1
#else
	#define NORMALMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, GLOSSMAP_CHANNEL)
	#define GLOSSMAP 1
#else
	#define GLOSSMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, LIGHTMAP_CHANNEL)
	#define LIGHTMAP 1
#else
	#define LIGHTMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, GLOWMAP_CHANNEL)
	#define GLOWMAP 1
#else
	#define GLOWMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, DETAIL_NORMAL_CHANNEL)
	#define DETAILNORMALMAP 1
#else
	#define DETAILNORMALMAP 0
#endif

#if TEST_CHANNEL(CHANNELS_FLAG, ALPHA_TEST_FLAG)
	#define ALPHATEST 1
#else
	#define ALPHATEST 0
#endif

//---------------------------------------------------------------------------------------
// ���ݵ�ǰ���õ�ͨ����������������������
//---------------------------------------------------------------------------------------
// ���û��NormalMap��Ҫ������㷨��
#if (NORMALMAP)
	#define VS_OUTPUT_VERTEX_NORMAL	0
#else
	#define VS_OUTPUT_VERTEX_NORMAL 1
#endif

// DiffuseMap/NormalMap/GlossMap������������
#if (DIFFUSEMAP || NORMALMAP || GLOSSMAP || DETAILNORMALMAP || GLOWMAP)
	#define VS_OUTPUT_BASE_UV	1
#else
	#define VS_OUTPUT_BASE_UV 0
#endif

// LightMapʹ�õ�������������
#if (LIGHTMAP)
	#define VS_OUTPUT_UNIQUE_UV	1
#else
	#define VS_OUTPUT_UNIQUE_UV 0
#endif

//---------------------------------------------------------------------------------------
// ������ɫ������������
//---------------------------------------------------------------------------------------
#define VS_INPUT HEADER_KEY(VS_Input, CHANNELS_FLAG)
struct VS_INPUT
{
	float3 	vPos			: POSITION;
	
#if VS_OUTPUT_VERTEX_NORMAL
	float3  vNormal			: NORMAL;
#else
	float3	vNormal			: NORMAL;
	float3 	vBinormal		: BINORMAL;
	float3	vTangent		: TANGENT;
#endif

#if VS_OUTPUT_BASE_UV
	float2 	vUVSet0			: TEXCOORD0;
#endif

#if VS_OUTPUT_UNIQUE_UV
	float2 	vUVSet1			: TEXCOORD1;
#endif
};

//---------------------------------------------------------------------------------------
// ������ɫ�����������
//---------------------------------------------------------------------------------------
#define VS_OUTPUT HEADER_KEY(VS_Output, CHANNELS_FLAG)
struct VS_OUTPUT
{
	float4	vProjPos		: POSITION;
	float	fDepth			: TEXCOORD0;
	
#if VS_OUTPUT_BASE_UV
	float2 	vUVSet0			: TEXCOORD1;
#endif

#if VS_OUTPUT_UNIQUE_UV
	float2 	vUVSet1			: TEXCOORD2;
#endif

#if VS_OUTPUT_VERTEX_NORMAL
	float3 	vNormal			: TEXCOORD3;
#else
	float3	vViewNormal		: TEXCOORD3;
	float3 	vViewBinormal	: TEXCOORD4;
	float3	vViewTangent	: TEXCOORD5;
#endif
};

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
#define VS_SHADER HEADER_KEY(VS_Main, CHANNELS_FLAG)
VS_OUTPUT VS_SHADER(VS_INPUT kInput)
{
	VS_OUTPUT kOutput;
	
	// ͶӰ����
	kOutput.vProjPos = mul(float4(kInput.vPos, 1.0f), g_mWorldViewProj);
	
	// �۲��������
	kOutput.fDepth = dot(float4(kInput.vPos, 1.0f), g_mWorldView._m02_m12_m22_m32);
	
	// ������������
#if VS_OUTPUT_BASE_UV
	kOutput.vUVSet0 = kInput.vUVSet0;
#endif

	// չ����������
#if VS_OUTPUT_UNIQUE_UV
	kOutput.vUVSet1 = kInput.vUVSet1 * a_vLightMapTransform.zw + a_vLightMapTransform.xy;
#endif

	// ���㷨��
#if VS_OUTPUT_VERTEX_NORMAL
	kOutput.vNormal 		= kInput.vNormal;
#else
	kOutput.vViewNormal 	= mul(float4(kInput.vNormal, 0.0f), g_mWorldView).xyz;
	kOutput.vViewBinormal 	= mul(float4(kInput.vBinormal, 0.0f), g_mWorldView).xyz;
	kOutput.vViewTangent 	= mul(float4(kInput.vTangent, 0.0f), g_mWorldView).xyz;
#endif

	return kOutput;
}

//---------------------------------------------------------------------------------------
// ������ɫ��
//---------------------------------------------------------------------------------------
#define PS_SHADER HEADER_KEY(PS_Main, CHANNELS_FLAG)
void PS_SHADER(VS_OUTPUT kInput, 				\
			   out float4 geoData	: COLOR0,	\
			   out float4 matData0	: COLOR1,	\
			   out float4 matData1	: COLOR2,	\
			   out float4 matData2	: COLOR3
			   )
{
	// ����Diffuse_Tex * Diffuse_Mat
	// ��������ͼ��Alphaͨ����AlphaTest
	// @{
#if DIFFUSEMAP
	float4 vDiffuseTex = tex2D(sdBaseSampler, kInput.vUVSet0);
	float3 vCompactedDiffuse = vDiffuseTex.rgb * g_vDiffuseMaterialColor;
	
	#ifdef ALPHATEST
		clip(vDiffuseTex.a - g_fAlphaTestRef);
	#endif
#else
	float3 vCompactedDiffuse = g_vDiffuseMaterialColor;

	// ������û�б���������ʾ
	clip(-1);
#endif	
	// @}
	
	
	// ����Glow
	// @{
#if GLOWMAP
	float4 vGlowTex = tex2D(sdGlowSampler, kInput.vUVSet0);
	matData2.rgb 	= g_vEmissiveMaterial.rgb;
	matData2.a		= vGlowTex.r;
#else
	matData2 = 0.f;
#endif
	// @}
	

	// ����Normal��Depth
	// @{
	// �۲�ռ���ȹ�һ����[0,1]
	float fDepth = kInput.fDepth / g_fCameraFarClip;
	
#if VS_OUTPUT_VERTEX_NORMAL
	float3 vNormal = kInput.vNormal;
	
	// �ϳ�DetailNormal��Normal
	#if DETAILNORMALMAP
		float3 vDetailNormal = tex2D(sdDetailNormalSampler, kInput.vUVSet0 * a_vDetailNormalUVTiling).xyz;
		vDetailNormal 		= (vDetailNormal - 0.5f) * 2.0f;
		vDetailNormal		*= a_vDetailNormalScale;
		
		// �ϳ�
		vDetailNormal.xy 	*= g_fStaticMeshNormalScale * saturate(1.0f - kInput.fDepth / a_fDetailNormalFadeDistance);
		vNormal 			+= vDetailNormal;
	#endif
	
	// ת�����۲�����ϵ����һ��
	float3 vViewNormal = normalize(mul(float4(vNormal, 1.0f), g_mWorldView)).rgb;
#else
	// ����NormalMap
	float3 vNormalTex 	= tex2D(sdNormalSampler, kInput.vUVSet0).xyz;
	vNormalTex 			= (vNormalTex - 0.5f) * 2.0f;
	vNormalTex.xy		*= g_fStaticMeshNormalScale;

	// �ϳ�DetailNormal��Normal
	#if DETAILNORMALMAP
		float3 vDetailNormal = tex2D(sdDetailNormalSampler, kInput.vUVSet0 * a_vDetailNormalUVTiling).xyz;
		vDetailNormal 		= (vDetailNormal - 0.5f) * 2.0f;
		vDetailNormal		*= a_vDetailNormalScale;
		
		// �ϳ�
		vNormalTex			+= vDetailNormal;
		vNormalTex.xy 		*= g_fStaticMeshNormalScale * saturate(1.0f - kInput.fDepth / a_fDetailNormalFadeDistance);
	#endif

	// ��һ��,��ת�����۲�����ϵ
	vNormalTex = normalize(vNormalTex);
	
	float3 vNorNormal 	= normalize(kInput.vViewNormal);
	float3 vNorBinormal = normalize(kInput.vViewBinormal);
	float3 vNorTangent 	= normalize(kInput.vViewTangent);
	float3 vViewNormal = mul(vNormalTex, float3x3(vNorTangent, vNorBinormal, vNorNormal));
#endif
	
	geoData	= float4(PackDepth(fDepth), PackNormal(vViewNormal));
	// @}
	
	
	// ���������ͼ
	//	1.LightMap��Ϣ������Alphaͨ���ĸ�7bit
	//  2.RimLight��Ϣ������Alphaͨ�������1bit
	// @{
#if LIGHTMAP
	float fLightTex = dot(tex2D(sdDarkSampler, kInput.vUVSet1), a_vLightMapChannel);
#else
	float fLightTex = a_fMainLightOcclusion;	// ����Ҳ�ܵ�LightDepth�ڵ�
#endif

	// ������Ч,edge enhancement
	int iDarkness = fLightTex * 127.0f + 0.5f;	// �任��7bit��,��[0,127],��������
	int iDarknessShiftLeft = iDarkness * 2;		// ����ƽ��1bit
	int iShouldRimLight = 0;
	matData0.a		= (iDarknessShiftLeft + iShouldRimLight) / 255.0f;	// �ϲ���任�ظ�����
	matData0.rgb 	= vCompactedDiffuse;	
	// @}
	
	
	// ����߹�(��֪��Ϊʲô��Ԥ���Ը߹������ɫ,����Ҳû���õ��߹������ɫ)
	// @{
#if GLOSSMAP
	float3 	vSpeculatTex = tex2D(sdGlossSampler, kInput.vUVSet0).rgb;
#else
	float3 	vSpeculatTex = 0.0f;	// g_vSpecularMaterialColor
#endif

	matData1.rgb	= vSpeculatTex;
	matData1.a		= g_fShiness / 255.0f;
	// @}	
}

//---------------------------------------------------------------------------------------
// ��ɫ����
//---------------------------------------------------------------------------------------
#define TECHNIQUE HEADER_KEY(MRT3Geometry, CHANNELS_FLAG)
technique TECHNIQUE
<
	string Description = MAKE_STRING(TECHNIQUE);
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_SHADER ();
		PixelShader 	= compile ps_3_0 PS_SHADER ();
	}
}
//---------------------------------------------------------------------------------------