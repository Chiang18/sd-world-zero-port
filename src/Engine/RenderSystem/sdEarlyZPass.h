//*************************************************************************************************
// ����:	EarlyZ����Ⱦ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _EARLYZ_PASS_H__
#define _EARLYZ_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// ����д��Depth-Stencil Buffer,���Ի�ȡ˫��д���ٶ�
	class sdEarlyZPass : public sdRenderPass
	{
	public:
		sdEarlyZPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdEarlyZPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask);
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
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// ����
		NiMaterialPtr	m_spSolidMeshMaterial;
	};
	NiSmartPointer(sdEarlyZPass);

}
#endif