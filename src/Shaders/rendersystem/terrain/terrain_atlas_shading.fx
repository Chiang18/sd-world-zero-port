//*************************************************************************************************
// 内容:	地形着色
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-08
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------
SD_POINT_CLAMP_CUBESAMPLE(0, sdPlanarTableSampler, 	sdPlanarTableTex);				//
SD_POINT_CLAMP_SAMPLE(1, sdGeomSampler, 			sdGeomBuf, 			false);		// 屏幕深度与法线缓存
SD_POINT_CLAMP_SAMPLE(2, sdLightSampler, 			sdLightBuf, 		false);		// 局部光照缓存
SD_LINEAR_WRAP_SAMPLE(3, sdBaseNormalSampler, 		sdBaseNormalTex, 	false);		// 地形基本法线贴图
SD_POINT_CLAMP_SAMPLE(4, sdTileMapSampler,			sdTileTex,			false);		// 地形Tile
SD_LINEAR_WRAP_SAMPLE(5, sdBlendSampler,			sdBlendTex,			false);		// 地形混合权重贴图
SD_POINT_CLAMP_SAMPLE(6, sdAtlasTableSampler,		sdAtlasTableTex,	false);		// 地形漫反射贴图查询表
SD_POINT_CLAMP_SAMPLE(7, sdDiffuseAtlasSampler,		sdDiffuseAtlasTex,	false);		// 地形漫反射贴图图集

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
// 平坦地区着色
float4 PS_Main_Planar(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	//float2 vPackedDepthTex = tex2D(sdDepthSampler, kInput.vUVSetScreenTex).xy;
	//float fDepth = UnpackDepth(vPackedDepthTex.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	//float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV
	//float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;

	// 根据UV采样NormalMap,
	//float4 vNormalTex 	= tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出世界空间法线(x,y,z), 转换到观察坐标系
	//float3 vWorldNormal = vNormalTex.xy 
	//vWorldNormal.xy	= vWorldNormal.xy * 2.0 - 1.0;
	//vWorldNormal.z 	= sqrt(dot(float3(1.0, vWorldNormal.xy), float3(1.0, -vWorldNormal.xy)));
	
	//
	
	
	
	return float4(1,0,0,0);
};
//---------------------------------------------------------------------------------------
// 陡峭地区着色



//---------------------------------------------------------------------------------------
// 着色技术
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




















