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
	static bool SaveHeightMap(sdHeightMap* pkHeightMap, const std::string& szSceneFullPath);
	static sdHeightMap* LoadHeightMap(const std::string& szSceneFullPath);

};
NiSmartPointer(sdTerrainStream);
#endif