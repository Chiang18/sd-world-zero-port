//*************************************************************************************************
// ����:	Terrain��ɫ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-09
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_ATLAS_SHADING_PASS_H__
#define _TERRAIN_ATLAS_SHADING_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	// ǰ������
	class sdTerrainParams;

	// ������Ⱦ�����λ���
	class sdTerrainAtlasShadingPass : public sdRenderPass
	{
	public:
		sdTerrainAtlasShadingPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdTerrainAtlasShadingPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(
			uint uiStencilRef, 
			uint uiStencilMask,
			NiRenderedTexture* spLightTexture, 
			NiRenderedTexture* spGeomTexture);
		void Destroy();

		// ���õ�����Ⱦ��Ϣ(���ڸ����ڲ���һЩ��ͼ��Ϣ)
		void SetTerrainParams(const sdTerrainParams& kTerrainParams);

	protected:
		// ģ�����
		uint	m_uiStencilRef;
		uint	m_uiStencilMask;
	
		// ����
		NiMaterialPtr	m_spPlanarShadingMaterial;
		NiMaterialPtr	m_spSeamShadingMaterial;

		// ��ɫ��������
		NiRenderedTexturePtr	m_spLightTexture;
		NiRenderedTexturePtr	m_spGeomTexture;

		// ������Ⱦ����
		NiPropertyList			m_kPropertyList;
		NiTexturingPropertyPtr	m_spTexturingProp;
	};
	NiSmartPointer(sdTerrainAtlasShadingPass);
}
#endif