//*************************************************************************************************
// 内容:	地形着色器的公共部分
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-04
// 最后修改:
//*************************************************************************************************
#ifndef _TERRAIN_COMMON_H__
#define _TERRAIN_COMMON_H__

//
#include "../common.h"

// 地形Tile几何信息
float4	a_vTerrainQuadParam 	: ATTRIBUTE;
#define	a_vQuadOrigin			float2(a_vTerrainQuadParam[0], a_vTerrainQuadParam[1])
#define	a_fQuadScale			a_vTerrainQuadParam[2]
#define	a_fQuadMorphFactor		a_vTerrainQuadParam[3]

// 地形整体大小的倒数(用于计算地形点UV坐标)
float2	g_vRecipTerrainSize		: GLOBAL;

// 地形近与远平面的分界面
float	g_fTerrainFarStart		: GLOBAL;


// 地形材质信息


// 地形贴图

//*****************************************************************************
// 图集采样(用于Diffuse图集和Normal图集)
//*****************************************************************************
float4 SamplerAtlasMap(sampler kAtlasMap, sampler kAtlasTable, float2 vTableUV, float2 vTerrainUV)
{
	// 采样图集查找表
	float4 vTileInfo = tex2D(kAtlasTable, vTableUV);
	
	// 计算图集采样坐标
	float2 vAtlasTavleUV = frac(vTerrainUV * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	
	// 返回贴图采样值
	return tex2D(kAtlasMap, vAtlasTavleUV);
}
//-----------------------------------------------------------------------------
float4 SamplerAtlasMap_Planar(sampler kAtlasMap, sampler kAtlasTable, float2 vTableUV,
	float2 vTerrainUV_XY, float2 vTerrainUV_YZ,  float2 vTerrainUV_XZ, float3 vWeight)
{
	// 采样图集查找表
	float4 vTileInfo = tex2D(kAtlasTable, vTableUV);
	
	// 计算图集采样坐标
	float2 vAtlasTavleUV_XY = frac(vTerrainUV_XY * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	float2 vAtlasTavleUV_YZ = frac(vTerrainUV_YZ * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	float2 vAtlasTavleUV_XZ = frac(vTerrainUV_YZ * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	
	// 返回贴图采样值
	return tex2D(kAtlasMap, vAtlasTavleUV_XY) * vWeight.z + 
		   tex2D(kAtlasMap, vAtlasTavleUV_YZ) * vWeight.x +
		   tex2D(kAtlasMap, vAtlasTavleUV_XZ) * vWeight.y;
}
//*****************************************************************************
#endif