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
	class sdBrushDoubalCircle : public NiNode
	{
	public:
		sdBrushDoubalCircle();
		~sdBrushDoubalCircle();

		// ���ݵ�ǰλ��,����ʰȡ���θ߶�,������ʾ
		void Update();

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
	};
}
#endif