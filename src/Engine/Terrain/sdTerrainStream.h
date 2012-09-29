//*************************************************************************************************
// 内容:	地形数据流加载与保存
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-05
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TERRAIN_STREAM_H__
#define _SD_ENGINE_TERRAIN_STREAM_H__
#include "sdEngine.h"
#include "sdHeightMap.h"

// 用于独立的保存与加载地形数据
// (考虑改进为GB的Stream形式)
class sdTerrainStream : public NiRefObject
{
public:
	// 加载与保存高度图
	static bool SaveHeightMap(sdHeightMap* pkHeightMap, const char* szSceneFullPath);
	static sdHeightMap* LoadHeightMap(const char* szSceneFullPath);

	// 加载和保存混合贴图
	// (这里使用D3DX来处理,WZ原始方式是自己手工处理)
	static bool SaveBlendMap(NiTexture* spBlendMap, const char* szSceneFullPath);

	// 加载和保存漫反射贴图集与漫反射贴图集索引
	static bool SaveDiffuseAtlasMap(NiTexture* spDiffuseMap, const char* szSceneFullPath);
	static bool SaveDiffuseAtlasTableMap(NiTexture* spDiffuseTableMap, const char* szSceneFullPath);

	// 加载和保存混合贴图与混合贴图索引
	static bool SaveNormalAtlasMap(NiTexture* spNormalMap, const char* szSceneFullPath);
	static bool SaveNormalAtlasTableMap(NiTexture* spNormalTableMap, const char* szSceneFullPath);
};
NiSmartPointer(sdTerrainStream);
#endif