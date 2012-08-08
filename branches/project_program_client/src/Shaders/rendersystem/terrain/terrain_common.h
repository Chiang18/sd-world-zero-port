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

#endif