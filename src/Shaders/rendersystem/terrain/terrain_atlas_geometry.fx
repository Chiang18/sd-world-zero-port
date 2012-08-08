//*************************************************************************************************
// 内容:	地形Normal/Depth绘制到G-Buffer
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-06
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_CUBESAMPLE(0, sdPlanarTableSampler, 	sdPlanarTableTex);				//
SD_POINT_CLAMP_SAMPLE(1, sdDepthSampler, 			sdDepthBuf, 		false);		// 地形屏幕深度缓存
SD_LINEAR_WRAP_SAMPLE(2, sdBaseNormalSampler, 		sdBaseNormalTex, 	false);		// 地形基本法线
SD_POINT_CLAMP_SAMPLE(3, sdTileMapSampler,			sdTileTex,			false);		// 地形Tile
SD_LINEAR_WRAP_SAMPLE(4, sdBlendSampler,			sdBlendTex,			false);		// 地形混合权重贴图
SD_POINT_CLAMP_SAMPLE(5, sdAtlasTableSampler,		sdAtlasTableTex,	false);		// 地形法线贴图查询表
SD_POINT_CLAMP_SAMPLE(6, sdNormalAtlasSampler,		sdNormalAtlasTex,	false);		// 地形法线贴图图集

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
	float3	vUVFarClipViewPos	: TEXCOORD2;	// 当前点对应远裁剪面上的点的观察坐标
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
	float4 vUVFarClipProjPos  = float4(kInput.vPos.xy, 1.0, 1.0);
	float4 vUVFarClipWorldPos = mul(vUVFarClipProjPos, g_mDepthToWorld);
	kOutput.vUVFarClipWorldPos = vUVFarClipWorldPos.xyz;
	
	// 当前点对应远裁剪面上的点的观察坐标
	// (替换掉w分量是为了避免计算误差累积)
	kOutput.vUVFarClipViewPos  = mul(float4(vUVFarClipWorldPos.xyz, 1.0), g_mView).xyz;
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
// 从近到远,只渲染基础法线贴图
float4 PS_Main_BaseNormal(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	//  计算当前点的地形相对UV
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	// 根据UV采样NormalMap,并解出来(x,y,z)
	float4 vNormal 	= tex2D(sdBaseNormalSampler, vUVSet);
	vNormal.xy 	= vNormal.xy * 2.0 - 1.0;
	vNormal.z 	= sqrt(dot(float3(1.0, vNormal.xy), float3(1.0, -vNormal.xy)));
	
	// 变换Normal到观察空间
	float3 vViewNormal = mul(float4(vNormal.xyz, 1.0), g_mView).xyz;
	
	// 输出打包的法线和深度
	return float4(vPackedDepth, PackNormal(vViewNormal));
}

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
	
	//  计算当前点的地形相对UV
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	// 根据UV采样NormalMap,并解出来
	float4 vNormal 	= tex2D(sdBaseNormalSampler, vUVSet);
	vNormal.xy 	= vNormal.xy * 2.0 - 1.0;
	vNormal.z 	= sqrt(dot(float3(1.0, vNormal.xy), float3(1.0, -vNormal.xy)));
	
	// 变换Normal到观察空间
	float3 vViewNormal = mul(float4(vNormal.xyz, 1.0), g_mView).xyz;
	
	// 输出打包的法线和深度
	return float4(vPackedDepth, PackNormal(vViewNormal));
}

//---------------------------------------------------------------------------------------
// 渲染近处,渲染基础法线和法线贴图
float4 PS_Main_Near_BaseNormalAndNormalMap(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float2 vPackedDepth = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	float fDepth = UnpackDepth(vPackedDepth.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	//  计算当前点的地形相对UV
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	// 根据UV采样NormalMap,并从.xy解出法线
	float4 vNormal 	= tex2D(sdBaseNormalSampler, vUVSet);
	vNormal.xy 	= vNormal.xy * 2.0 - 1.0;
	vNormal.z 	= sqrt(dot(float3(1.0, vNormal.xy), float3(1.0, -vNormal.xy)));
	
	// 根据UV采样TileMap,
	float3 vIndices = tex2D(sdTileMapSampler, vUVSet).xyz * 255.0;
	
	
	
	
	
	//*************
	float3 vViewNormal = mul(float4(vNormal.xyz, 1.0), g_mView).xyz;
	//*************
	
	
	
	
	
	return float4(vPackedDepth, PackNormal(vViewNormal));
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
technique Terrain_AtlasGeometry_BaseNormal
<
	string Description = "Terrain_AtlasGeometry_BaseNormal";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_BaseNormal ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_AtlasGeometry_Far_BaseNormal
<
	string Description = "Terrain_AtlasGeometry_Far_BaseNormal";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Far_BaseNormal ();
	}
}
//---------------------------------------------------------------------------------------
technique Terrain_AtlasGeometry_Near_BaseNormalAndNormalMap
<
	string Description = "Terrain_AtlasGeometry_Near_BaseNormalAndNormalMap";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Near_BaseNormalAndNormalMap ();
	}
}
//---------------------------------------------------------------------------------------