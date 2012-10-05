//*************************************************************************************************
// 内容:	地表法线图
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-08
// 最后修改:	2012-10-02
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_NORMAL_MAP_H__
#define _TERRAIN_NORMAL_MAP_H__
#include "sdTypes.h"
#include "sdDynamicTexture.h"

// 前置声明
class sdHeightMap;

// 地表法线图,与HeightMap密度相同
class sdNormalMap : public NiRefObject
{
public:
	sdNormalMap(uint uiSize, sdHeightMap* pkHeightMap);
	sdNormalMap(NiTexture* spNormalTexture);
	~sdNormalMap();

	// 更新,分别是整体更新/更新指定区域(单位1)
	void	Update();
	void	Update(uint uiX, uint uiY, uint uiW, uint uiH);

	// 
	NiTexture*	GetGBTexture() const;

protected:
	// 高度图(用于编辑状态)
	sdHeightMap*	m_pkHeightMap;

	// NormalMap的纹理
	RenderSystem::sdDynamicTexturePtr	m_pkDynamicNormalTexture;	// 用于编辑状态,以提供动态更新
	NiTexturePtr	m_spNormalTexture;	// 用于游戏状态

	// 坡度阈值
	NiPoint3	m_kPlanarThreshold;
};
NiSmartPointer(sdNormalMap);
#endif