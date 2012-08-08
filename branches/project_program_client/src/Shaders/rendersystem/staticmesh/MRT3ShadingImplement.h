//*************************************************************************************************
// 内容:	StaticMesh的MRTZShadingPass着色器具体实现
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-19
// 最后修改:
//*************************************************************************************************
// 1.这里不需要定义避免重复的宏,因为本文件会被重复包含
// 2.这里根据Flag不同动态生成名字,避免出现命名重复
#include "static_mesh_common.h"

//---------------------------------------------------------------------------------------
// 取消一些内部会用到的预定义宏
//---------------------------------------------------------------------------------------
// 用于定义着色器输入输出数据结构名称
#undef VS_INPUT
#undef VS_OUTPUT

// 用于定义着色器名称
#undef VS_SHADER
#undef PS_SHADER

// 用于定义
#undef TECHNIQUE

// 用于定义通道启用情况
#undef RIMLIGHT

//---------------------------------------------------------------------------------------
// 根据CHANNELS_FLAG的值来提取当前启用的通道情况
//---------------------------------------------------------------------------------------
#if TEST_CHANNEL(CHANNELS_FLAG, RIMLIGHT_CHANNEL)
	#define RIMLIGHT 1
#else
	#define RIMLIGHT 0
#endif

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
#define VS_OUTPUT HEADER_KEY(VS_Output, CHANNELS_FLAG)
struct VS_OUTPUT
{
	float4	vPos			: POSITION;
	float2 	vProjPos		: TEXCOORD0;
	float3	vViewPos		: TEXCOORD1;
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
#define VS_SHADER HEADER_KEY(VS_Main, CHANNELS_FLAG)
VS_OUTPUT VS_SHADER(float3 vPos : POSITION, float2 vUVSet0 : TEXCOORD0)
{
	VS_OUTPUT kOutput;
	
	kOutput.vPos 		= float4(vPos, 1.0f);
	kOutput.vProjPos 	= vUVSet0 + g_vHalfPixelOffset;
	kOutput.vViewPos	= float3(vPos.xy + g_vCameraFrustum.xy, g_vCameraFrustum.z);
	
	return kOutput;
};

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
#define PS_SHADER HEADER_KEY(PS_Main, CHANNELS_FLAG)
float4 PS_SHADER(VS_OUTPUT kInput) : COLOR0
{
	// 深度与法线纹理
	// @{
	float4 vGeomData 	= tex2D(sdGeomBufferSampler, kInput.vProjPos);
	float3 vViewNormal 	= UnpackNormal(vGeomData.zw);
	float  fDepth		= UnpackDepth(vGeomData.xy);
	// @}
	
	
	// 光照贴图纹理
	// @{
	float4 vLocalLight	= tex2D(sdLightBufferSampler, kInput.vProjPos) * g_fLocalLightRadio;
	// }@
	
	
	// 材质纹理
	// @{
	float4 vMatData0	= tex2D(sdMatBuffer0Sampler, kInput.vProjPos);
	float4 vMatData1	= tex2D(sdMatBuffer1Sampler, kInput.vProjPos);
	
	// 解压Diffuse
	float3 vDiffuseTex	= vMatData0.rgb;
	
	// 解压LightMap和Edge标记
	float fLMAndEdge 	= vMatData0.a * 255.0f;
	float fLightMap		= 0.0f;
	float fRimLightMask	= modf(fLMAndEdge / 2.0f, fLightMap);
	fRimLightMask		= fRimLightMask * 2.0f;
	fLightMap			/= 127.0f;
	
	// 解压Specular
	float3 vSpecularTex	= vMatData1.rgb;
	
	// 解压Shiness
	float fShiness		= vMatData1.a * 255.0f;
	// @}
	
	
	// 光照计算
	// @{
	float3	vDiffuseLight;
	float3	vSpecularLight;
	float3	vViewVector = -normalize(kInput.vViewPos);
	AccumLighting(vViewVector, vViewNormal, fShiness, fLightMap, vLocalLight, vDiffuseLight, vSpecularLight);
	// @}
	
	
	// RimLight计算
	// @{
#ifdef	RIMLIGHT
	// 完全看不懂
	//float rimlightattenuation = 1.0f;
	//float eta = 0.99f;
	//float power = 2.5f;
	//float factor = ((1.0f - eta) * (1.0f - eta)) /  ((1.0f + eta) * (1.0f + eta));
	//float fresnel = factor + (1.0f - factor) * pow((1.0f - sacturate(dot(vViewVector, vViewNormal))), power);
	//float anglecos = (dot(vViewVector, g_vMainLightDir) + 1.0f)/ 2.0f;
	//float rimdiffuse = clamp(anglecos, 0.35f, 0.7f);
	//float brightness = dot(g_vMainLightColor, float3(0.2125f, 0.7154f, 0.0721f));
	//float3 rimcolor = g_vMainLightColor / brightness;
	//float3 vRimLight = fRimLightMask * fresnel * clamp(brightness * 1.8f, 0.2f, 0.35f) * rimcolor * rimdiffuse * rimlightattenuation;

	float3 vRimLight = 0.0f;
#else
	float3 vRimLight = 0.0f;
#endif	
	// @}
	

	// 合成光照
	float3 vColor = vDiffuseTex * vDiffuseLight + vSpecularTex * vSpecularLight + vRimLight;

	return float4(vColor, 0);
};

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
#define TECHNIQUE HEADER_KEY(MRT3Shading, CHANNELS_FLAG)
technique TECHNIQUE
<
	string Description = MAKE_STRING(TECHNIQUE);
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_SHADER ();
		PixelShader 	= compile ps_2_a PS_SHADER ();
	}
};
//---------------------------------------------------------------------------------------