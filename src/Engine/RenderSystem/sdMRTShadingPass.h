//*************************************************************************************************
// ����:	Geometry-Buffer����Ⱦ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-14
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MRT_SHADING_PASS_H__
#define _MRT_SHADING_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	class sdMRTShadingPass : public sdRenderPass
	{
	public:
		sdMRTShadingPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdMRTShadingPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(uint uiStaticMeshStencilID, 
			uint uiStaticMeshStencilIDMask,
			NiRenderedTexture* spLightTexture, 
			NiRenderedTexture* spGeomTexture,
			NiRenderedTexture* spAlbedoTexture,	
			NiRenderedTexture* spGlossTexture);
		void Destroy();

	protected:
		// ģ�����
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// ����
		NiMaterialPtr	m_spStaticShadingMaterial;
		NiMaterialPtr	m_spSkinnedShadingMaterial;

		// ��ɫ��������
		NiRenderedTexturePtr	m_spLightTexture;
		NiRenderedTexturePtr	m_spGeomTexture;
		NiRenderedTexturePtr	m_spAlbedoTexture;
		NiRenderedTexturePtr	m_spGlossTexture;

		// ��Ⱦ����
		NiPropertyList	m_kPropertyList;
	};
	NiSmartPointer(sdMRTShadingPass);
}
#endif