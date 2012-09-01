//*************************************************************************************************
// ����:	���α�ˢͼ��(������ʾ)
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITBRUSHSHAPE_H__
#define _SD_GAMEEDITEX_EDITBRUSHSHAPE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//****************************************************************************
	// ˫Բ����ˢ
	class sdDualCircleMesh : public NiNode
	{
	public:
		sdDualCircleMesh();
		~sdDualCircleMesh();

		// ���ݵ�ǰλ��,����ʰȡ���θ߶�,������ʾ
		void UpdateShape();

	protected:
		void CreateInnerCircle();
		void CreateOutterCirlce();
		void UpdateInnerCircle();
		void UpdateOutterCirlce();	

	protected:
		float	m_fInnerRadius;
		float	m_fOuterRadius;
		NiMeshPtr	m_spInnerCircle;
		NiMeshPtr	m_spOuterCircle;

		// Բˢ��Բ�ܲ�������
		const static uint ms_uiCirclePointCount = 50;
	};
	NiSmartPointer(sdDualCircleMesh);
}
#endif