//*************************************************************************************************
// 内容:	基于DX9的Gamebryo的延迟渲染路径
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-07-05
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PATH_DX9_DEFERRED_H__
#define _RENDER_PATH_DX9_DEFERRED_H__
#include <sdRenderPath.h>
#include <sdEarlyZPass.h>
#include <sdMRTZGeometryPass.h>

namespace RenderSystem
{
	// 前置声明
	class sdRenderParams;
	class sdEnvironmentParams;
	class sdPostProcessParams;
	class sdEarlyZPass;

	// Gamebryo的DX9实现延迟着色路径
	// EarlyZ
	// 
	class sdRenderPath_DX9	: public sdRenderPath
	{
	public:
		// 各种类型Mesh的StencilID,用于标定GeometryBuffer的对象类型
		enum eMeshStencilID
		{
			E_SID_SKY			= 0,
			E_SID_TERRAIN		= 1,
			E_SID_STATIC_MESH	= 2,
			E_SID_SKINNED_MESH	= 3,
			E_SID_DECAL			= 4,
			E_SID_LEAF			= 5,
			E_SID_GRASS			= 6,
			E_SID_WATER			= 7,
			E_SID_MASK			= 0x07,
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

		virtual void RenderStaticMesh(NiMesh* spMesh);

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
		// @}

		//*******************************************************
		// 从RenderPass渲染RenderObject到RenderTarget
		// @{
		void Draw();
		void DrawEarlyZPass();
		void DrawGeometryPasses();

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
		// }@

	
		// 渲染目标与渲染纹理
		// @{
		// G-Buffer
		//   |----depth88, normal88
		//	 |----diffuse_tex*diffuse_color888, lightmap8
		//	 |----specular_tex*specular_color888, shiness8
		NiRenderTargetGroupPtr	m_spGeometryAlbedoMRT;
		NiRenderedTexturePtr	m_spGeometryTexture;
		NiRenderedTexturePtr	m_spAlbedoTexture;
		NiRenderedTexturePtr	m_spGlossTexture;
		// }@

		// 渲染参数
		// @{
		sdRenderParams*			m_pkRenderParams;
		sdEnvironmentParams*	m_pkEnvironmentParams;
		sdPostProcessParams*	m_pkPostProcessParams;
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