//*************************************************************************************************
// ����:	��Ⱦϵͳ��GB-D3D9ʵ��
//---------------------------------------------------------
// ����:		
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_SYSTEM_DX9_H__
#define _RENDER_SYSTEM_DX9_H__
#include <sdRenderSystem.h>

namespace RenderSystem
{
	// ǰ������
	class sdRenderDevice;
	class sdRenderPath;

	// ��Ⱦϵͳ��D3D9ʵ��
	class sdRenderSystem_DX9 : public sdRenderSystem
	{
	public:
		sdRenderSystem_DX9();
		virtual ~sdRenderSystem_DX9();

		//*************************************************
		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();

		virtual inline void SetMap(sdMap* pkMap);
		virtual inline sdMap* GetMap();

		virtual inline void SetCamera(NiCamera* spCamera);
		virtual inline NiCamera* GetCamera();

		virtual void UpdateFrame(float fAccumTime, float fFrameTime);
		virtual void RenderFrame();
		virtual void DisplayFrame();

		virtual NiMesh*	GetDefaultMesh(eDefaultMesh eMesh);
		virtual NiMaterial*	GetDefaultMaterial(eDefaultMaterial eMaterial);

	protected:
		//@{
		// ����
		void DrawOpaqueUI();
		void DrawTranslucentUI();
		void DrawScene();
		//@}

		//@{
		// ����ȱʡMesh
		NiMesh*	CreateScreenQuadMesh();
		//@}

	protected:
		// ����Ⱦ�ĵ�ͼ
		sdMap*			m_pkMap;

		// �����,�Լ���زü�����
		NiCameraPtr		m_spCamera;

		// ������ȾĿ��,Ĭ����ȱʡ��ȾĿ��
		NiRenderTargetGroupPtr	m_spRenderTargetGroup;

		// ��Ⱦ�豸
		sdRenderDevice*	m_pkRenderDevice;

		// ��Ⱦ·��
		sdRenderPath*	m_pkRenderPath;


		// һЩ��Ⱦ��ȱʡ������
		//@{
		// ����ȱʡ��ȾMesh
		NiMeshPtr	m_spScreenQuadMesh;
		NiMeshPtr	m_spUnitCubeMesh;
		NiMeshPtr	m_spUnitSphereMesh;
		NiMeshPtr	m_spUnitConeMesh;
		NiMeshPtr	m_spUnitPyramidMesh;
		NiMeshPtr	m_spErrorMesh;

		// ����ȱʡ����
		NiMaterialPtr		m_spDefaultMaterials[NUM_DEFAULT_MATERIALS];

		// ����ȱʡ����
		NiSourceTexturePtr	m_kDefaultTextures[NUM_DEFAULT_TEXTURES];
		//@}
	};
	#include "sdRenderSystem_DX9.inl"
}
#endif