//*************************************************************************************************
// 内容:	地形Normal/Depth绘制到G-Buffer
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-23
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
float4 g_vRecipUVRepeats[3]	: GLOBAL;

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_SAMPLE(0, sdDepthSampler,		sdDepthBuf, 		false);		// 地形屏幕深度缓存
SD_LINEAR_WRAP_SAMPLE(1, sdBaseNormalSampler, 	sdBaseNormalTex, 	false);		// 地形基本法线贴图
SD_LINEAR_WRAP_SAMPLE(2, sdBlendSampler,		sdBlendTex,			false);		// 地形混合权重贴图

SD_SAMPLE_EX( 3, sdNormalSampler00, sdNormalTex00, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 4, sdNormalSampler01, sdNormalTex01, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 5, sdNormalSampler02, sdNormalTex02, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 6, sdNormalSampler03, sdNormalTex03, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 7, sdNormalSampler04, sdNormalTex04, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 8, sdNormalSampler05, sdNormalTex05, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX( 9, sdNormalSampler06, sdNormalTex06, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX(10, sdNormalSampler07, sdNormalTex07, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX(11, sdNormalSampler08, sdNormalTex08, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX(12, sdNormalSampler09, sdNormalTex09, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX(13, sdNormalSampler10, sdNormalTex10, WRAP, LINEAR, ANISOTROPIC, LINEAR)
SD_SAMPLE_EX(14, sdNormalSampler11, sdNormalTex11, WRAP, LINEAR, ANISOTROPIC, LINEAR)

//---------------------------------------------------------------------------------------
// 顶点着色器输入数据流
//---------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	vPos			: POSITION0;	// 屏幕矩形顶点
	float2	vUVSet0			: TEXCOORD0;	// 屏幕矩形纹理坐标
};

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos			: POSITION;		// 投影坐标
	float2	vUVSetScreenTex		: TEXCOORD0;	// 纠正半像素偏移的屏幕纹理坐标
	float3	vUVFarClipWorldPos	: TEXCOORD1;	// 当前点对应远裁剪面上的点的世界坐标
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(VS_INPUT kInput)
{
	VS_OUTPUT kOutput;

	// 投影坐标
	kOutput.vProjPos = float4(kInput.vPos, 1.0);
	
	// 纠正半像素偏移的屏幕纹理坐标
	kOutput.vUVSetScreenTex = kInput.vUVSet0 + g_vHalfPixelOffset;
	
	// 当前点对应远裁剪面上的点的世界坐标
	kOutput.vUVFarClipWorldPos = mul(float4(kInput.vPos.xy, 1.0, 1.0), g_mDepthToWorld).xyz;
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
// 渲染远处,只渲染基础法线贴图
float4 PS_Main_Far_BaseNormal(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 裁剪掉指定近平面以内的像素
	clip(length(vWorldPos - g_vViewPos) - g_fTerrainFarStart);
	
	
	// BaseNormalMap
	// @{
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素,因为BaseNormalMap是Linear采样的)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	// 根据UV采样NormalMap
	float4 vBaseNormalTex 	= tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出世界空间法线
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	
	// 变换Normal到观察空间
	float3 vViewNormal = mul(float4(vWorldNormal.xyz, 0.0), g_mView).xyz;
	// @}
	
	
	// 输出打包的法线和深度
	return float4(vPackedDepth, PackNormal(vViewNormal));
}

//---------------------------------------------------------------------------------------
// 渲染近处,渲染基础法线和法线贴图, XY平面
float4 PS_Main_Near_BaseNormalAndNormalMap_XY(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// 根据UV采样NormalMap(Sampler是Linear,没有偏移半像素)
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出倾斜情况
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.z - 0.001);
	
	// 解出世界空间法线
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	
	// 计算当前点的切线空间
	float3 vWorldBinormal 	= cross(float3(1, 0, 0), vWorldNormal);
	float3 vWorldTangent 	= cross(vWorldNormal, vWorldBinormal);
	// @}
	
	
	// BlendMap
	// @{
	// 计算UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// 采样BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	
	//
	float fTotalWeight = dot(float3(dot(vBlendWeight00, 1.0), dot(vBlendWeight01, 1.0), dot(vBlendWeight02, 1.0)), 1.0);
	// @}
	
	
	// NormalMap
	// @{
	// 计算UV
	float2 vUVSet2 = float2(vWorldPos.x, -vWorldPos.y);
	
	// 采样NormalMap
	float3 vNormal = tex2D(sdNormalSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vNormal += tex2D(sdNormalSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vNormal += tex2D(sdNormalSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vNormal += tex2D(sdNormalSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vNormal += tex2D(sdNormalSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vNormal += tex2D(sdNormalSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vNormal += tex2D(sdNormalSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vNormal += tex2D(sdNormalSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vNormal += tex2D(sdNormalSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vNormal += tex2D(sdNormalSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vNormal += tex2D(sdNormalSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vNormal += tex2D(sdNormalSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
	vNormal /= fTotalWeight;
	
	vNormal = vNormal * 2.0 - 1.0;
	
	vNormal.xy *= g_fTerrainNormalScale;
	
	float fNormalSmooth = saturate(5.0 -5.0 * length(vWorldPos - g_vViewPos) / g_fTerrainFarStart);
	vNormal.xy *= fNormalSmooth;
	vNormal = normalize(vNormal);
	
	vNormal = vNormal.z * vWorldNormal.xyz + vNormal.y * vWorldBinormal + vNormal.x * vWorldTangent;
	
		// 转换到观察坐标系
	vNormal = mul(float4(vNormal, 0), g_mView);
	// @}
	
	return float4(vPackedDepth, PackNormal(vNormal) * vPlanarWeight.z);
}

//---------------------------------------------------------------------------------------
// 渲染近处,渲染基础法线和法线贴图, YZ平面
float4 PS_Main_Near_BaseNormalAndNormalMap_YZ(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// 根据UV采样NormalMap(Sampler是Linear,没有偏移半像素)
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出倾斜情况
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.x - 0.001);
	
	// 解出世界空间法线
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	
	// 计算当前点的切线空间
	float3 vWorldBinormal 	= cross(float3(0, sign(vWorldNormal.x), 0), vWorldNormal);
	float3 vWorldTangent 	= cross(vWorldNormal, vWorldBinormal);
	// @}
	
	
	// BlendMap
	// @{
	// 计算UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// 采样BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	// @}
	
	
	// NormalMap
	// @{
	// 计算UV
	float2 vUVSet2 = float2(vWorldPos.y * sign(vWorldNormal.x), -vWorldPos.z);
	
	// 采样NormalMap
	float3 vNormal = tex2D(sdNormalSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vNormal += tex2D(sdNormalSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vNormal += tex2D(sdNormalSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vNormal += tex2D(sdNormalSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vNormal += tex2D(sdNormalSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vNormal += tex2D(sdNormalSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vNormal += tex2D(sdNormalSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vNormal += tex2D(sdNormalSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vNormal += tex2D(sdNormalSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vNormal += tex2D(sdNormalSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vNormal += tex2D(sdNormalSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vNormal += tex2D(sdNormalSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
	vNormal = vNormal * 2.0 - 1.0;
	
	vNormal.xy *= g_fTerrainNormalScale;
	
	float fNormalSmooth = saturate(5.0 -5.0 * length(vWorldPos - g_vViewPos) / g_fTerrainFarStart);
	vNormal.xy *= fNormalSmooth;
	vNormal = normalize(vNormal);
	
	vNormal = vNormal.z * vWorldNormal.xyz + vNormal.y * vWorldBinormal + vNormal.x * vWorldTangent;
	
		// 转换到观察坐标系
	vNormal = mul(float4(vNormal, 0), g_mView);
	// @}
	
	return float4(vPackedDepth * step(vPlanarWeight.z, 0.001), PackNormal(vNormal) * vPlanarWeight.x);
}

//---------------------------------------------------------------------------------------
// 渲染近处,渲染基础法线和法线贴图, XZ平面
float4 PS_Main_Near_BaseNormalAndNormalMap_XZ(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// 根据UV采样NormalMap(Sampler是Linear,没有偏移半像素)
	float4 vBaseNormalTex = tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出倾斜情况
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);	
	
	clip(vPlanarWeight.y - 0.001);
	
	// 解出世界空间法线
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	
	// 计算当前点的切线空间
	float3 vWorldBinormal 	= cross(float3(-sign(vWorldNormal.x), 0, 0), vWorldNormal);
	float3 vWorldTangent 	= cross(vWorldNormal, vWorldBinormal);
	// @}
	
	
	// BlendMap
	// @{
	// 计算UV
	float2 vUVSet1 = vUVSet * 0.5;
	
	// 采样BlendMap
	float4 vBlendWeight00 = tex2D(sdBlendSampler, vUVSet1);
	float4 vBlendWeight01 = tex2D(sdBlendSampler, vUVSet1 + float2(0.5, 0.0));
	float4 vBlendWeight02 = tex2D(sdBlendSampler, vUVSet1 + float2(0.0, 0.5));
	// @}
	
	
	// NormalMap
	// @{
	// 计算UV
	float2 vUVSet2 = float2(-vWorldPos.x * sign(vWorldNormal.y), -vWorldPos.z);
	
	// 采样NormalMap
	float3 vNormal = tex2D(sdNormalSampler00, vUVSet2 * g_vRecipUVRepeats[0].x) * vBlendWeight00.a;
	vNormal += tex2D(sdNormalSampler01, vUVSet2 * g_vRecipUVRepeats[0].y) * vBlendWeight00.r;
	vNormal += tex2D(sdNormalSampler02, vUVSet2 * g_vRecipUVRepeats[0].z) * vBlendWeight00.g;
	vNormal += tex2D(sdNormalSampler03, vUVSet2 * g_vRecipUVRepeats[0].w) * vBlendWeight00.b;
	vNormal += tex2D(sdNormalSampler04, vUVSet2 * g_vRecipUVRepeats[1].x) * vBlendWeight01.a;
	vNormal += tex2D(sdNormalSampler05, vUVSet2 * g_vRecipUVRepeats[1].y) * vBlendWeight01.r;
	vNormal += tex2D(sdNormalSampler06, vUVSet2 * g_vRecipUVRepeats[1].z) * vBlendWeight01.g;
	vNormal += tex2D(sdNormalSampler07, vUVSet2 * g_vRecipUVRepeats[1].w) * vBlendWeight01.b;
	vNormal += tex2D(sdNormalSampler08, vUVSet2 * g_vRecipUVRepeats[2].x) * vBlendWeight02.a;
	vNormal += tex2D(sdNormalSampler09, vUVSet2 * g_vRecipUVRepeats[2].y) * vBlendWeight02.r;
	vNormal += tex2D(sdNormalSampler10, vUVSet2 * g_vRecipUVRepeats[2].z) * vBlendWeight02.g;
	vNormal += tex2D(sdNormalSampler11, vUVSet2 * g_vRecipUVRepeats[2].w) * vBlendWeight02.b;
	
	vNormal = vNormal * 2.0 - 1.0;
	
	vNormal.xy *= g_fTerrainNormalScale;
	
	float fNormalSmooth = saturate(5.0 -5.0 * length(vWorldPos - g_vViewPos) / g_fTerrainFarStart);
	vNormal.xy *= fNormalSmooth;
	vNormal = normalize(vNormal);
	
	vNormal = vNormal.z * vWorldNormal.xyz + vNormal.y * vWorldBinormal + vNormal.x * vWorldTangent;
	
	// 转换到观察坐标系
	vNormal = mul(float4(vNormal, 0), g_mView);
	// @}
	
	float2 vOutputDepth = step(vPlanarWeight.xz, 0.001);
	return float4(vPackedDepth * vOutputDepth.x * vOutputDepth.y, PackNormal(vNormal) * vPlanarWeight.y);
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
technique Terrain_MultiPassGeometry_Far_BaseNormal
<
	string Description = "Terrain_MultiPass_Geometry_Far_BaseNormal";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_Main ();
		PixelShader 	= compile ps_2_a PS_Main_Far_BaseNormal ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_XY
<
	string Description = "Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_XY";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_Main ();
		PixelShader 	= compile ps_2_a PS_Main_Near_BaseNormalAndNormalMap_XY ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_YZ
<
	string Description = "Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_YZ";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_Main ();
		PixelShader 	= compile ps_2_a PS_Main_Near_BaseNormalAndNormalMap_YZ ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_XZ
<
	string Description = "Terrain_MultiPassGeometry_Near_BaseNormalAndNormalMap_XZ";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_a VS_Main ();
		PixelShader 	= compile ps_2_a PS_Main_Near_BaseNormalAndNormalMap_XZ ();
	}
}
//---------------------------------------------------------------------------------------