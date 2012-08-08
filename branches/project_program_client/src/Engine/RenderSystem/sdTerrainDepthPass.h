//*************************************************************************************************
// ����:	Terrain���Ԥ��Ⱦ��ZBuffer
//---------------------------------------------------------
// ����:		
// ����:		2012-08-04
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_DEPTH_PASS_H__
#define _TERRAIN_DEPTH_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// 1.��Ⱦ������ȵ�һ������
	// 2.��Ⱦ���ε�ģ�建��
	class sdTerrainDepthPass : public sdRenderPass
	{
	public:
		sdTerrainDepthPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdTerrainDepthPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(uint uiStencilRef, uint uiStencilWriteMask);
		void Destroy();

		// ��ӻ��ƶ���
		void InsertStaticMesh(NiMesh* spMesh);

	protected:
		//@{
		// �麯���̳�
		virtual RenderObjectComparator GetComparator();

		// ���Ƶ�����Ƚ���
		static bool	Comparator(sdRenderObject* lhs, sdRenderObject* rhs);
		//@}

	protected:
		// ģ�����
		uint	m_uiStencilRef;
		uint	m_uiStencilWriteMask;

		// ����
		NiMaterialPtr	m_spMaterial;
	};
	NiSmartPointer(sdTerrainDepthPass);
}
#endif