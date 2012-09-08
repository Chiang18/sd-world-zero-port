//*************************************************************************************************
// 内容:	基于DX9的Gamebryo的延迟渲染路径
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-05
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PATH_DX9_DEFERRED_H__
#define _RENDER_PATH_DX9_DEFERRED_H__
#include "sdRenderPath.h"
#include "sdRenderSystem.h"
#include "sdEarlyZPass.h"
#include "sdMRTZGeometryPass.h"
#include "sdMRTShadingPass.h"
#include "sdTerrainDepthPass.h"
#include "sdTerrainAtlasGeometryPass.h"
#include "sdTerrainAtlasShadingPass.h"
#include "sdDebugGizmoPass.h"

namespace RenderSystem
{
	// Gamebryo的DX9实现延迟着色路径
	// 
	// 
	class sdRenderPath_DX9	: public sdRenderPath
	{
	public:
		// 各种类型Mesh的StencilID,用于标定Geometry-Buffer的对象类型
		enum eMeshStencilID
		{
			E_SID_SKY			= 0,	// 0000 0000
			E_SID_TERRAIN		= 1,	// 0000 0001
			E_SID_STATIC_MESH	= 2,	// 0000 0010
			E_SID_SKINNED_MESH	= 3,	// 0000 0011
			E_SID_DECAL			= 4,	// 0000 0100
			E_SID_LEAF			= 5,	// 0000 0101
			E_SID_GRASS			= 6,	// 0000 0110
			E_SID_WATER			= 7,	// 0000 0111
			E_SID_MASK			= 0x07,	
		};

		// 一些特殊掩码位(与普通Stencil组合使用)
		enum ePixelStencilBit
		{
			E_PSB_RECEIVE_DECAL		= 0x3,	// 用于标记可贴花区域的Bit位置
			E_PSB_DISTANCE_TERRAIN	= 0x4,	// 用以标记远处地形的Bit位置
		};


	public:
		sdRenderPath_DX9();
		virtual ~sdRenderPath_DX9();

		//*************************************************
		// 虚函数继承
		virtual bool Initialize();
		virtual void Destroy();

		virtual void RenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);

		virtual void UpdateRenderParams(const sdRenderParams& kRenderParams);
		virtual void UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams);
		virtual void UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams);
		virtual void UpdateTerrainParams(const sdTerrainParams& kTerrainParams);

		virtual void RenderStaticMesh(NiMesh* spMesh);
		virtual void RenderTerrainMesh(NiMesh* spMesh);

	protected:
		// 创建渲染资源集
		void CreateAllRenderedTexture();
		void ReleaseAllRenderTexture();

		void BeginRenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);
		void EndRenderScene();

		// 从NiNode树收集NiMesh对象(目前仅支持普通Mesh)
		void CollectMeshes(NiNode* spNode, std::vector<NiMesh*>& kMeshVec);

		//*******************************************************
		// 资源准备与更新
		// @{
		void Update(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);
		void UpdateRenderTargets(NiRenderTargetGroup* spRenderTarget);
		//void UpdateCameras();
		//void UpdateReflectionCamera();
		// @}

		//*******************************************************
		// 从Map渲染RenderObject到RenderPass(裁剪与分发)
		// @{
		void Render();
		//void RenderShadow();
		//void RenderReflection();
		void RenderMainView();

		void OctreeCull();
		//void UmbraCull();

		void RenderTerrain();
		void RenderDebugGizmo();
		// @}

		//*******************************************************
		// 从RenderPass渲染RenderObject到RenderTarget
		// @{
		void Draw();

		void DrawEarlyZPass();
		void DrawTerrainDepthPass();
		void DrawGeometryPasses();
		void DrawPreLightPass();
		void DrawShadingPasses();

		// 设置适合本RenderPath的RenderDevice初始的渲染状态
		// (RenderState有缓存,所以只管设置不管复位)
		void PrepareRenderStates();

		// 设置着色器常量
		void PrepareShaderConstants();
		// @}

	protected:
		// 渲染对象分配器
		sdRenderObjectAlloc*	m_pkRenderObjectAlloc;

		// 临时场景对象指针
		sdMap*					m_pkCurMap;
		NiCamera*				m_spCurCam;
		NiRenderTargetGroup*	m_spCurFinalRenderTarget;

		// @{
		// 渲染通道
		sdEarlyZPassPtr			m_pkEarlyZPass;
		sdMRTZGeometryPassPtr	m_pkMRTZGeometryPass;
		sdMRTShadingPassPtr		m_pkMRTShadingPass;

		sdTerrainDepthPassPtr			m_pkTerrainDepthPass;
		sdTerrainAtlasGeometryPassPtr	m_pkTerrainAtlasGeometryPass;
		sdTerrainAtlasShadingPassPtr	m_pkTerrainAtlasShadingPass;

		sdDebugGizmoPassPtr		m_pkDebugGizmoPass;
		// }@

	
		// 渲染目标与渲染纹理
		// @{
		// Geometry-Buffer
		//   |----depth88, normal88
		//	 |----diffuse_tex*diffuse_color888, lightmap8
		//	 |----specular_tex*specular_color888, shiness8
		NiRenderTargetGroupPtr	m_spGeometryAlbedoMRT;
		NiRenderedTexturePtr	m_spGeometryTexture;
		NiRenderedTexturePtr	m_spAlbedoTexture;
		NiRenderedTexturePtr	m_spGlossTexture;
		
		// Light-Buffer
		NiRenderTargetGroupPtr	m_spLightTarget;
		NiRenderedTexturePtr	m_spDepthOrLightTexture;

		// Terrain-Depth
		NiRenderTargetGroupPtr	m_spTerrainDepthTarget;
		// }@


		// 渲染参数
		// @{
		sdRenderParams			m_kRenderParams;
		sdEnvironmentParams		m_kEnvironmentParams;
		sdPostProcessParams		m_kPostProcessParams;
		sdTerrainParams			m_kTerrainParams;
		// }@


		// 渲染设定
		// @{
		bool	m_bEnableRendering;
		bool	m_bUseEarlyZ;
		// }@
	};
	NiSmartPointer(sdRenderPath_DX9);
}
#endif