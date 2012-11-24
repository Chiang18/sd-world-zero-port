//*************************************************************************************************
// 内容:	decal着色器具体实现
//---------------------------------------------------------
// 作者:		
// 创建:		2012-11-09
// 最后修改:
//*************************************************************************************************
#include "DecalCommon.h"

//---------------------------------------------------------------------------------------
// 取消一些内部会用到的预定义宏
//---------------------------------------------------------------------------------------
//#undef VS_INPUT
//#undef VS_OUTPUT

//#undef VS_SHADER
#undef PS_SHADER

#undef TECHNIQUE

#undef DIFFUSEMAP
#undef NORMALMAP
#undef GLOSSMAP
#undef LIGHTMAP
//#undef AOMAP
#undef GLOWMAP

//---------------------------------------------------------------------------------------
// 根据CHANNELS_FLAG的值来提取当前启用的通道情况
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

//#if TEST_CHANNEL(CHANNELS_FLAG, AOMAP_CHANNEL)
//	#define AOMAP 1
//#else
//	#define AOMAP 0
//#endif

#if TEST_CHANNEL(CHANNELS_FLAG, GLOWMAP_CHANNEL)
	#define GLOWMAP 1
#else
	#define GLOWMAP 0
#endif

//---------------------------------------------------------------------------------------
// 根据当前启用的通道情况来决定具体的组合情况
//---------------------------------------------------------------------------------------
#if (DIFFUSEMAP || GLOWMAP) && !GLOSSMAP && !LIGHTMAP
	#define PS_SHADER	HEADER_KEY(PS_Main, CHANNELS_FLAG)
	#define TECHNIQUE	HEADER_KEY(Decal_Projector, CHANNELS_FLAG)
#endif

#if defined (PS_SHADER) && defined(TECHNIQUE)

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
float4 PS_SHADER(VS_OUTPUT kInput) : COLOR0
{
	// 深度
	float4 vGeomData 	= tex2D(sdGeomSampler, kInput.vUVSetProjPos.xy/kInput.vUVSetProjPos.w);
	float  fDepth		= UnpackDepth(vGeomData.xy);

	// 观察坐标
	float3 vViewPos = kInput.vUVSetFarClipViewPos.xyz / kInput.vUVSetFarClipViewPos.w * fDepth;
	
	// 从eye坐标系转回到decal立方体坐标系
	float4 vDecalPos = mul(float4(vViewPos, 1.f), g_mViewToProjectorUV);
	
	// 计算像素对应的decal纹理坐标
	float4 vUVSet = vDecalPos / vDecalPos.w;
	
	// 裁剪掉UV不在[0,1]范围的像素
	clip(vUVSet.xyz);
	clip(1.f - vUVSet.xyz);
	
#if LIGHTMAP
	float4	vLightColor = tex2D(sdDarkSampler, vUVSet.xy);
	clip(vLightColor.a - g_fAlphaTestRef);
#endif

	// 计算Decal纹理坐标
	float2 vUVSetBase = vUVSet.xy * g_vProjectorUVScaleOffset.xy + g_vProjectorUVScaleOffset.zw;
	
#if GLOWMAP
	float4 vDiffuseColor = tex2D(sdGlowSampler, vUVSetBase) * g_vDiffuseMaterial;
#elif DIFFUSEMAP
	float4 vDiffuseColor = tex2D(sdBaseSampler, vUVSetBase) * g_vDiffuseMaterial;
#endif

#if LIGHTMAP
	vDiffuseColor *= vLightColor;
#endif

	clip(vDiffuseColor.a - g_fAlphaTestRef);

#if GLOWMAP
	return vDiffuseColor;
#endif

#if DIFFUSEMAP
	#if NORMALMAP
		float3 vLocalNormal = (tex2D(sdNormalSampler, vUVSetBase).xyz - 0.5f) * float3(2.f, 2.f, -2.f);
		float3 vViewNormal = normalize(mul(vLocalNormal, g_mWorldView));
	#else
		float3 vViewNormal = UnpackNormal(vGeomData.zw);
	#endif

	float3 vViewVec = -normalize(vViewPos);
	
	float3 vDiffuseLight;
	float3 vSpecularLight;
	float4 vLocalLight = tex2D(sdLightSampler, kInput.vUVSetProjPos.xy);
	AccumLighting(vViewVec, vViewNormal, g_vSpecularMaterial.a, 1, 1, vLocalLight, vDiffuseLight, vSpecularLight);
	
	float4 vColor;
	vColor.rgb = vDiffuseColor.rgb * vDiffuseLight + vSpecularLight * g_vSpecularMaterial.rgb;
	vColor.a = vDiffuseColor.a;
	
	return vColor;
#endif
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
technique TECHNIQUE
<
	string Description = MAKE_STRING(TECHNIQUE);
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_Main ();
		PixelShader 	= compile ps_2_a PS_SHADER ();
	}
}
//---------------------------------------------------------------------------------------
#endif