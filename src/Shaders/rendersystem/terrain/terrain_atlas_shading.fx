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
SD_POINT_CLAMP_SAMPLE(2, sdLightSampler, 			sdLightBuf, 		false);		// 屏幕局部光照缓存
SD_LINEAR_WRAP_SAMPLE(3, sdBaseNormalSampler, 		sdBaseNormalTex, 	false);		// 地形基本法线贴图
SD_POINT_CLAMP_SAMPLE(4, sdTileSampler,				sdTileTex,			false);		// 地形TileMap
SD_LINEAR_WRAP_SAMPLE(5, sdBlendSampler,			sdBlendTex,			false);		// 地形混合权重贴图
SD_POINT_CLAMP_SAMPLE(6, sdAtlasTableSampler,		sdAtlasTableTex,	false);		// 地形漫反射贴图查询表
SD_LINEAR_CLAMP_SAMPLE(7, sdDiffuseAtlasSampler,	sdDiffuseAtlasTex,	false);		// 地形漫反射贴图图集

//---------------------------------------------------------------------------------------
// 顶点着色器输入数据流
//---------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	vPos			: POSITION0;	// 屏幕矩形顶点
	float2	vUVSet0			: TEXCOORD0;	// 屏幕矩形顶点纹理坐标
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

	// 转换到投影坐标
	kOutput.vProjPos = float4(kInput.vPos, 1.0);
	
	// 纠正半像素偏移的屏幕纹理坐标
	kOutput.vUVSetScreenTex = kInput.vUVSet0 + g_vHalfPixelOffset;
	
	// 当前点对应远裁剪面上的点的世界坐标
	float4 vUVFarClipProjPos  = float4(kInput.vPos.xy, 1.0, 1.0);
	float4 vUVFarClipWorldPos = mul(vUVFarClipProjPos, g_mDepthToWorld);
	kOutput.vUVFarClipWorldPos = vUVFarClipWorldPos.xyz;
	
	// 当前点对应远裁剪面上的点的观察坐标
	// (替换掉w分量是为了避免计算误差累积?)
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
	float4 vGeoTex = tex2D(sdGeomSampler, kInput.vUVSetScreenTex);
	float fDepth = UnpackDepth(vGeoTex.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// 根据UV采样NormalMap
	float4 vBaseNormalTex 	= tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出倾斜情况,超过一定角度的不进行着色
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);

	clip(1.5 - dot(sign(vPlanarWeight), 1));
	
	// 解出世界空间法线(x,y,z)
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	// @}
	
	
	// TileMap
	// @{
	// 根据UV采样TileMap,
	float3 vIndices = tex2D(sdTileSampler, vUVSet).xyz * 255.0;
	// @} 
	
	
	// BlendMap
	// @{
	// 计算新的UV(不解,大概是为了在Tile边缘进行融合)
	//float2 tileCenterOffset = frac(vUVSet * (2048.0 / 4.0)) - 0.5;
	//vUVSet -= tileCenterOffset * (1.0 / 2048.0);
	
	// 根据UV采样BlendMap
	float4 vBlendTex = tex2D(sdBlendSampler, vUVSet);
	
	// 归一化权重
	float fTotalWeight = dot(vBlendTex.xyz, 1.0);
	vBlendTex.rgb /= fTotalWeight;
	
#ifdef _SD_EDITOR
	vBlendTex.a = max(vBlendTex.a, g_fLightMapMask);
#endif
	// @}
	
	
	// 贴图混合
	// @{
	// 采样立方体纹理
	float4 vPlanarVec = texCUBE(sdPlanarTableSampler, vWorldNormal.xzy) * 255 - 1;
	
	// 计算新的地形UV
	float2 vUVSet2 = float2(dot(vWorldPos.xy, vPlanarVec.xy), dot(vWorldPos.yz, vPlanarVec.zw));
	
	// 计算当前像素到观察点矢量
	float3 vWorldViewVector = normalize(g_vViewPos - kInput.vUVFarClipWorldPos);
	
	// 计算当前像素应取LOD(这里不解,有待进一步关注)
	float2 vLodLevel = log2(g_vFarPixelSize * fDepth / max(dot(vWorldViewVector, vWorldNormal), 0.25));
	
	// 计算图集UV
	float4 vUVSetTable;
	vUVSetTable.xyz = saturate(vIndices.bgr * g_fAtlasIdScale + g_fAtlasIdOffset);
	vUVSetTable.w	= saturate(max(vLodLevel.x, vLodLevel.y) * g_fAtlasLevelScale + g_fAtlasLevelOffset);
	
	// 贴图混合
	float4 vDiffuseGloss = SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.xw, vUVSet2) * vBlendTex.b +
							SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.yw, vUVSet2) * vBlendTex.g +
							SamplerAtlasMap(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.zw, vUVSet2) * vBlendTex.r;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuseGloss, float4(g_vDiffuseMapMask,g_fGlossMapMask));
#endif
	// @}
	
	
	// 计算最终颜色
	// @{
	// 采样LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// 计算观察方向与法线
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);
	
	// 合成光照
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBlendTex.a, vLightTex, vDiffuseLight, vSpeculatLight);
	
	// 合成最终颜色
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
	// @}
	
	//return float4(vColor, 0);
	return float4(0.3, 0.3, 0, 0);
};
//---------------------------------------------------------------------------------------
// 陡峭地区着色
float4 PS_Main_Seam(VS_OUTPUT kInput) : COLOR0
{
	// 获取地形深度
	float4 vGeoTex = tex2D(sdGeomSampler, kInput.vUVSetScreenTex);
	float fDepth = UnpackDepth(vGeoTex.xy);
	
	// 反算世界坐标
	// (根据线性深度,对相机位置和远平面对应点位置进行插值)
	float3 vWorldPos = lerp(g_vViewPos, kInput.vUVFarClipWorldPos, fDepth);
	
	// 计算当前点的地形相对UV(注意,这里没有偏移半像素)
	float2 vUVSet = vWorldPos.xy * g_vRecipTerrainSize.xy;
	
	
	// NormalMap
	// @{
	// 根据UV采样NormalMap
	float4 vBaseNormalTex 	= tex2D(sdBaseNormalSampler, vUVSet);
	
	// 解出倾斜情况,小于一定角度的不进行着色
	float3 vPlanarWeight;
	vPlanarWeight.xy 	= vBaseNormalTex.zw;
	vPlanarWeight.z 	= saturate(1.0 - vBaseNormalTex.z - vBaseNormalTex.w);

	clip(dot(sign(vPlanarWeight), 1) - 1.5);
	
	// 解出世界空间法线(x,y,z)
	float3 vWorldNormal;
	vWorldNormal.xy	= vBaseNormalTex.xy * 2.0 - 1.0;
	vWorldNormal.z 	= sqrt(dot(float3(1.0, vBaseNormalTex.xy), float3(1.0, -vBaseNormalTex.xy)));
	// @}
	
	
	// TileMap
	// @{
	// 根据UV采样TileMap,
	float3 vIndices = tex2D(sdTileSampler, vUVSet).xyz * 255.0;
	// @} 
	
	
	// BlendMap
	// @{
	// 计算新的UV(不解,大概是为了在Tile边缘进行融合)
	//float2 tileCenterOffset = frac(vUVSet * (2048.0 / 4.0)) - 0.5;
	//vUVSet -= tileCenterOffset * (1.0 / 2048.0);
	
	// 根据UV采样BlendMap
	float4 vBlendTex = tex2D(sdBlendSampler, vUVSet);
	
	// 归一化权重
	float fTotalWeight = dot(vBlendTex.xyz, 1.0);
	vBlendTex.rgb /= fTotalWeight;
	
#ifdef _SD_EDITOR
	vBlendTex.a = max(vBlendTex.a, g_fLightMapMask);
#endif
	// @}
	
	
	// 贴图混合
	// @{
	// 
	//float4 vPlanarVec = texCUBE(sdPlanarTableSampler, vWorldNormal.xzy) * 255 - 1;
	float2 vTerrainUV_XY = float2(vWorldPos.x, -vWorldPos.y);
	float2 vTerrainUV_YZ = float2(vWorldPos.y * sign(vWorldNormal.x - 0.5f), -vWorldPos.z);
	float2 vTerrainUV_XZ = float2(-vWorldPos.x * sign(vWorldNormal.y - 0.5f), -vWorldPos.z);
	
	// 计算当前像素到观察点矢量
	float3 vWorldViewVector = normalize(g_vViewPos - kInput.vUVFarClipWorldPos);
	
	// 计算当前像素应取LOD(这里不解,有待进一步关注)
	float2 vLodLevel = log2(g_vFarPixelSize * fDepth / max(dot(vWorldViewVector, vWorldNormal), 0.25));
	
	// 计算图集UV
	float4 vUVSetTable;
	vUVSetTable.xyz = saturate(vIndices.bgr * g_fAtlasIdScale + g_fAtlasIdOffset);
	vUVSetTable.w	= saturate(max(vLodLevel.x, vLodLevel.y) * g_fAtlasLevelScale + g_fAtlasLevelOffset);
	
	// 贴图混合
	float4 vDiffuseGloss = SamplerAtlasMap_Planar(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.xw, vTerrainUV_XY, vTerrainUV_YZ, vTerrainUV_XZ, vPlanarWeight) * vBlendTex.b +
						   SamplerAtlasMap_Planar(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.yw, vTerrainUV_XY, vTerrainUV_YZ, vTerrainUV_XZ, vPlanarWeight) * vBlendTex.g +
						   SamplerAtlasMap_Planar(sdDiffuseAtlasSampler, sdAtlasTableSampler, vUVSetTable.zw, vTerrainUV_XY, vTerrainUV_YZ, vTerrainUV_XZ, vPlanarWeight) * vBlendTex.r;
	
#ifdef _SD_EDITOR
	vDiffuseGloss = max(vDiffuse_Gloss, float4(g_vDiffuseMapMask,g_fGlossMapMask));
#endif
	// @}
	
	
	// 计算最终颜色
	// @{
	// 采样LightBuffer
	float4 vLightTex = tex2D(sdLightSampler, kInput.vUVSetScreenTex);
	
	// 计算观察方向与法线
	float3 vViewVector = -normalize(kInput.vUVFarClipViewPos);
	float3 vViewNormal 	= UnpackNormal(vGeoTex.zw);
	
	// 合成光照
	float3 vDiffuseLight;
	float3 vSpeculatLight;
	AccumLighting(vViewVector, vViewNormal, g_fTerrainShiness, vBlendTex.a, vLightTex, vDiffuseLight, vSpeculatLight);
	
	// 合成最终颜色
	float3 vColor = vDiffuseLight  * vDiffuseGloss.rgb * g_vTerrainDiffuseMaterial +
					vSpeculatLight * vDiffuseGloss.a   * g_vTerrainSpecularMaterial;	
	// @}
	
	return float4(vColor, 0);
}

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
technique Terrain_AtlasShading_Seam
<
	string Description = "Terrain_AtlasShading_Seam";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_3_0 VS_Main ();
		PixelShader 	= compile ps_3_0 PS_Main_Seam ();
	}
}
//---------------------------------------------------------------------------------------




















