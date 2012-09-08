//*************************************************************************************************
// ����:	����DX9��Gamebryo���ӳ���Ⱦ·��
//---------------------------------------------------------
// ����:		
// ����:		2012-07-05
// ����޸�:
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
	// Gamebryo��DX9ʵ���ӳ���ɫ·��
	// 
	// 
	class sdRenderPath_DX9	: public sdRenderPath
	{
	public:
		// ��������Mesh��StencilID,���ڱ궨Geometry-Buffer�Ķ�������
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

		// һЩ��������λ(����ͨStencil���ʹ��)
		enum ePixelStencilBit
		{
			E_PSB_RECEIVE_DECAL		= 0x3,	// ���ڱ�ǿ����������Bitλ��
			E_PSB_DISTANCE_TERRAIN	= 0x4,	// ���Ա��Զ�����ε�Bitλ��
		};


	public:
		sdRenderPath_DX9();
		virtual ~sdRenderPath_DX9();

		//*************************************************
		// �麯���̳�
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
		// ������Ⱦ��Դ��
		void CreateAllRenderedTexture();
		void ReleaseAllRenderTexture();

		void BeginRenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);
		void EndRenderScene();

		// ��NiNode���ռ�NiMesh����(Ŀǰ��֧����ͨMesh)
		void CollectMeshes(NiNode* spNode, std::vector<NiMesh*>& kMeshVec);

		//*******************************************************
		// ��Դ׼�������
		// @{
		void Update(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);
		void UpdateRenderTargets(NiRenderTargetGroup* spRenderTarget);
		//void UpdateCameras();
		//void UpdateReflectionCamera();
		// @}

		//*******************************************************
		// ��Map��ȾRenderObject��RenderPass(�ü���ַ�)
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
		// ��RenderPass��ȾRenderObject��RenderTarget
		// @{
		void Draw();

		void DrawEarlyZPass();
		void DrawTerrainDepthPass();
		void DrawGeometryPasses();
		void DrawPreLightPass();
		void DrawShadingPasses();

		// �����ʺϱ�RenderPath��RenderDevice��ʼ����Ⱦ״̬
		// (RenderState�л���,����ֻ�����ò��ܸ�λ)
		void PrepareRenderStates();

		// ������ɫ������
		void PrepareShaderConstants();
		// @}

	protected:
		// ��Ⱦ���������
		sdRenderObjectAlloc*	m_pkRenderObjectAlloc;

		// ��ʱ��������ָ��
		sdMap*					m_pkCurMap;
		NiCamera*				m_spCurCam;
		NiRenderTargetGroup*	m_spCurFinalRenderTarget;

		// @{
		// ��Ⱦͨ��
		sdEarlyZPassPtr			m_pkEarlyZPass;
		sdMRTZGeometryPassPtr	m_pkMRTZGeometryPass;
		sdMRTShadingPassPtr		m_pkMRTShadingPass;

		sdTerrainDepthPassPtr			m_pkTerrainDepthPass;
		sdTerrainAtlasGeometryPassPtr	m_pkTerrainAtlasGeometryPass;
		sdTerrainAtlasShadingPassPtr	m_pkTerrainAtlasShadingPass;

		sdDebugGizmoPassPtr		m_pkDebugGizmoPass;
		// }@

	
		// ��ȾĿ������Ⱦ����
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


		// ��Ⱦ����
		// @{
		sdRenderParams			m_kRenderParams;
		sdEnvironmentParams		m_kEnvironmentParams;
		sdPostProcessParams		m_kPostProcessParams;
		sdTerrainParams			m_kTerrainParams;
		// }@


		// ��Ⱦ�趨
		// @{
		bool	m_bEnableRendering;
		bool	m_bUseEarlyZ;
		// }@
	};
	NiSmartPointer(sdRenderPath_DX9);
}
#endif