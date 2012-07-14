//*************************************************************************************************
// 内容:	渲染路径接口
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-07-04
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PATH_H__
#define _RENDER_PATH_H__
#include <NiRefObject.h>
#include "sdTypes.h"
#include "sdRenderPass.h"

// 前置声明
class sdMap;

namespace RenderSystem
{
	// 前置声明
	class sdRenderParams;
	class sdEnvironmentParams;
	class sdPostProcessParams;

	// 渲染路径接口
	// 渲染路径是渲染策略的一种封装,它定义了如何渲染一个指定的Map到指定的RenderTarget
	//
	// RenderPath处理整个渲染,
	//	1.负责将RenderPass进行合理的组织和调用,以及创建一个有限的资源集保障
	//	2.裁剪Map得到Mesh队列,
	//	3.填充Mesh到RenderObject
	//	4.填充RenderObject到对应的RenderPass
	//
	// RenderPass处理一个特定的中间步骤,负责将RenderObject渲染到RenderTarget
	class sdRenderPath : public NiRefObject
	{
	public:
		sdRenderPath();
		virtual ~sdRenderPath();

		//*************************************************
		// 初始化与销毁
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bInitialized;};

		// 设备重建回调
		//virtual bool Recreate() = 0;

		// 渲染总接口,处理裁剪(Entity级别与Mesh级别)与渲染
		virtual void RenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget) = 0;

		//@{
		// 更新渲染参数
		virtual void UpdateRenderParams(const sdRenderParams& kRenderParams) = 0;
		virtual void UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams) = 0;
		virtual void UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams) = 0;

		//@{
		// 裁剪与渲染对象填充接口
		// (这里的主要目的是将RenderPass的功能接口封装之后暴露出来)
		//virtual void RenderTerrainMesh(NiMesh* spMesh) = 0;
		//virtual void RenderTerrainHole(NiMesh* spMesh) = 0;
		virtual void RenderStaticMesh(NiMesh* spMesh) = 0;
		//virtual void RenderSkinnedMesh(NiMesh* spMesh) = 0;
		//virtual void RenderLight(NiLight* spLight) = 0;
		//virtual void RenderDecal()
		//virtual void RenderRoad()
		//virtual void RenderProjector()
		//@}

	protected:
		bool	m_bInitialized;
	};
	NiSmartPointer(sdRenderPath);
}
#endif