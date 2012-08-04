//*************************************************************************************************
// ����:	����DX9��Gamebryo���ӳ���Ⱦ·��
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-07-05
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PATH_DX9_DEFERRED_H__
#define _RENDER_PATH_DX9_DEFERRED_H__
#include <sdRenderPath.h>
#include <sdEarlyZPass.h>
#include <sdMRTZGeometryPass.h>

namespace RenderSystem
{
	// ǰ������
	class sdRenderParams;
	class sdEnvironmentParams;
	class sdPostProcessParams;
	class sdEarlyZPass;

	// Gamebryo��DX9ʵ���ӳ���ɫ·��
	// EarlyZ
	// 
	class sdRenderPath_DX9	: public sdRenderPath
	{
	public:
		// ��������Mesh��StencilID,���ڱ궨GeometryBuffer�Ķ�������
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
		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();

		virtual void RenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget);

		virtual void UpdateRenderParams(const sdRenderParams& kRenderParams);
		virtual void UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams);
		virtual void UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams);

		virtual void RenderStaticMesh(NiMesh* spMesh);

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
		// @}

		//*******************************************************
		// ��RenderPass��ȾRenderObject��RenderTarget
		// @{
		void Draw();
		void DrawEarlyZPass();
		void DrawGeometryPasses();

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
		// }@

	
		// ��ȾĿ������Ⱦ����
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

		// ��Ⱦ����
		// @{
		sdRenderParams*			m_pkRenderParams;
		sdEnvironmentParams*	m_pkEnvironmentParams;
		sdPostProcessParams*	m_pkPostProcessParams;
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