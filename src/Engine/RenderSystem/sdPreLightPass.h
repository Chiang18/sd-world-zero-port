//*************************************************************************************************
// ����:	Light-Buffer����Ⱦ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-20
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _PRELIGHT_PASS_H__
#define _PRELIGHT_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	class sdPreLightPass : public sdRenderPass
	{
	public:
		sdPreLightPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdPreLightPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(uint uiStaticMeshStencilIDMask, NiRenderedTexture* spGeomTexture);
		void Destroy();

		// ��ӻ��ƶ���
		//void InsertLight(NiLight* spMesh);

	protected:
		// ģ�����
		uint	m_uiStaticMeshStencilIDMask;

		// ����
		NiMaterialPtr	m_spPointLightMaterial;
		NiMaterialPtr	m_spSpotLightMaterial;
		NiMaterialPtr	m_spRectSpotLightMaterial;

		// ��ɫ��������
		NiRenderedTexturePtr	m_spGeomTexture;

		// ��Ⱦ����
		NiPropertyList	m_kPropertyList;
	};
	NiSmartPointer(sdPreLightPass);
}
#endif