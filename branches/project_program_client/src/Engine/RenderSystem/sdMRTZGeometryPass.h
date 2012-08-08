//*************************************************************************************************
// ����:	Geometry-Buffer����Ⱦ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-14
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MRT_Z_GEOMETRY_PASS_H__
#define _MRT_Z_GEOMETRY_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	class sdMRTZGeometryPass : public sdRenderPass
	{
	public:
		sdMRTZGeometryPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdMRTZGeometryPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask);
		void Destroy();

		// ��ӻ��ƶ���
		void InsertStaticMesh(NiMesh* spMesh);

	protected:
		// ģ�����
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// ����
		std::vector<NiMaterialPtr>	m_kStaticGeometryMaterials;
		std::vector<NiMaterialPtr>	m_kSkinnedGeometryMaterials;

		// ��Ⱦ����
		NiPropertyList				m_kPropertyList;
	};
	NiSmartPointer(sdMRTZGeometryPass);
}
#endif