//*************************************************************************************************
// 内容:	场景地图
//---------------------------------------------------------
// 作者:		
// 创建:		2012-06-29
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_MAP_H__
#define _SD_ENGINE_MAP_H__
#include "sdGameObject.h"
#include "sdTerrain.h"

// 渲染系统
#include "sdRenderSystem.h"


// 场景地图
class sdMap : public sdGameObject
{
public:
	sdMap();
	~sdMap();


	// 
	bool	CreateScene();

	//*********************
	NiAVObjectPtr	m_pkRoot;
	//**********************

	// 属性访问
	// @{
	// 
	sdTerrain*	GetTerrain() { return &m_kTerrain;};

	//
	NiNode*	GetDebugNode() const { return m_spDebugNode;}

	// 渲染参数
	const RenderSystem::sdRenderParams& GetRenderParams() const { return m_kRenderParams;};
	const RenderSystem::sdEnvironmentParams& GetEnvironmentParams() const { return m_kEnvironmentParams;};
	const RenderSystem::sdPostProcessParams& GetPostProcessParams() const { return m_kPostProcessParams;};
	// @}

protected:

protected:
	

	// 地形系统
	sdTerrain	m_kTerrain;

	// 调试根节点
	NiNodePtr	m_spDebugNode;

	// 场景绘制参数(用于提供给渲染系统使用,内部不应该被分配内存和保存资源)
	RenderSystem::sdRenderParams		m_kRenderParams;
	RenderSystem::sdEnvironmentParams	m_kEnvironmentParams;
	RenderSystem::sdPostProcessParams	m_kPostProcessParams;
};
NiSmartPointer(sdMap);
#endif