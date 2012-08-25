//*************************************************************************************************
// ����:	������ɫ���Ĺ�������
//---------------------------------------------------------
// ����:		
// ����:		2012-08-04
// ����޸�:
//*************************************************************************************************
#ifndef _TERRAIN_COMMON_H__
#define _TERRAIN_COMMON_H__

//
#include "../common.h"

// ����Tile������Ϣ
float4	a_vTerrainQuadParam 	: ATTRIBUTE;
#define	a_vQuadOrigin			float2(a_vTerrainQuadParam[0], a_vTerrainQuadParam[1])
#define	a_fQuadScale			a_vTerrainQuadParam[2]
#define	a_fQuadMorphFactor		a_vTerrainQuadParam[3]

// ���������С�ĵ���(���ڼ�����ε�UV����)
float2	g_vRecipTerrainSize		: GLOBAL;

// ���ν���Զƽ��ķֽ���
float	g_fTerrainFarStart		: GLOBAL;

// Զ�����ش�С(���ڼ�������LOD)
float2 	g_vFarPixelSize			: GLOBAL;

// ���β�����Ϣ
float3 	g_vTerrainDiffuseMaterial	: GLOBAL;
float3 	g_vTerrainSpecularMaterial	: GLOBAL;
float	g_fTerrainShiness			: GLOBAL;

// ������ͼͼ����Ϣ(Diffuse��ͼ����Normal��ͼ���Ƕ�Ӧ��)
float4	g_vAtlasTableParam		: GLOBAL;
#define g_fAtlasIdScale			g_vAtlasTableParam.x
#define g_fAtlasIdOffset		g_vAtlasTableParam.y
#define g_fAtlasLevelScale		g_vAtlasTableParam.z
#define g_fAtlasLevelOffset		g_vAtlasTableParam.w

//*****************************************************************************
// ͼ������(����Diffuseͼ����Normalͼ��)
//*****************************************************************************
float4 SamplerAtlasMap(sampler kAtlasMap, sampler kAtlasTable, float2 vTableUV, float2 vTerrainUV)
{
	// ����ͼ�����ұ�
	float4 vTileInfo = tex2D(kAtlasTable, vTableUV);
	
	// ����ͼ����������
	float2 vAtlasTavleUV = frac(vTerrainUV * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	
	// ������ͼ����ֵ
	return tex2D(kAtlasMap, vAtlasTavleUV);
}
//-----------------------------------------------------------------------------
float4 SamplerAtlasMap_Planar(sampler kAtlasMap, sampler kAtlasTable, float2 vTableUV,
	float2 vTerrainUV_XY, float2 vTerrainUV_YZ,  float2 vTerrainUV_XZ, float3 vWeight)
{
	// ����ͼ�����ұ�
	float4 vTileInfo = tex2D(kAtlasTable, vTableUV);
	
	// ����ͼ����������
	float2 vAtlasTavleUV_XY = frac(vTerrainUV_XY * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	float2 vAtlasTavleUV_YZ = frac(vTerrainUV_YZ * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	float2 vAtlasTavleUV_XZ = frac(vTerrainUV_YZ * vTileInfo.x) * vTileInfo.y + vTileInfo.zw;
	
	// ������ͼ����ֵ
	return tex2D(kAtlasMap, vAtlasTavleUV_XY) * vWeight.z + 
		   tex2D(kAtlasMap, vAtlasTavleUV_YZ) * vWeight.x +
		   tex2D(kAtlasMap, vAtlasTavleUV_XZ) * vWeight.y;
}
//*****************************************************************************
#endif