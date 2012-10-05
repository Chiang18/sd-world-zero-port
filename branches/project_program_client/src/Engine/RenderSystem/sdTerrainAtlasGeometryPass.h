//*************************************************************************************************
// ����:	Terrain��Geometry-Buffer����Ⱦ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-06
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_ATLAS_GEOMETRY_PASS_H__
#define _TERRAIN_ATLAS_GEOMETRY_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// ǰ������
	class sdTerrainParams;

	// ������Ⱦ�����λ���
	class sdTerrainAtlasGeometryPass : public sdRenderPass
	{
	public:
		sdTerrainAtlasGeometryPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdTerrainAtlasGeometryPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(
			uint uiStencilRef, 
			uint uiStencilMask,
			uint uiStencilFarTerrainBit,
			NiRenderedTexture* spDepthTexture);
		void Destroy();

		// ���õ�����Ⱦ��Ϣ(���ڸ����ڲ���һЩ��ͼ��Ϣ)
		void SetTerrainParams(const sdTerrainParams& kTerrainParams);

	protected:
		// ģ�����
		uint	m_uiStencilRef;
		uint	m_uiStencilMask;
		uint	m_uiStencilFarTerrainBit;

		// ����
		// @{
		// ֻ��Ⱦ�������߲���
		NiMaterialPtr	m_spBaseNormalOnlyMaterial;

		// ͸��ͶӰ�������:
		//	1.������Ⱦ�������ߺͷ�����ͼ�Ĳ���
		//	2.Զ����Ⱦ�������ߵĲ���
		NiMaterialPtr	m_spFarBaseNormalMaterial;
		NiMaterialPtr	m_spNearNormalMapMaterial;

		// ����ͶӰ�������:
		//	1.������Ⱦ�������ߺͷ�����ͼ�Ĳ���
		//	2.Զ����Ⱦ�������ߵĲ���
		NiMaterialPtr	m_spOrthoFarBaseNormalMaterial;
		NiMaterialPtr	m_spOrthoNearNormalMapMaterial;
		// @}

		// �����������
		NiRenderedTexturePtr	m_spDepthTexture;

		// ��������ͼ,����ȷ���ر���ͼͶӰ����
		NiSourceCubeMapPtr		m_spUVTableCubeMap;

		// ������Ⱦ����
		NiPropertyList			m_kPropertyList;
		NiTexturingPropertyPtr	m_spTexturingProp;
	};
	NiSmartPointer(sdTerrainAtlasGeometryPass);
}
#endif