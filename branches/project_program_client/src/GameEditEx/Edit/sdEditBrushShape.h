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
	// ���Ʊ�ˢ����
	class sdBrushShape : public NiNode
	{
	public:
		virtual ~sdBrushShape(){};

		// ���ݵ�ǰλ��,����ʰȡ���θ߶�,������ʾ
		virtual void UpdateShape(){};
	};
	NiSmartPointer(sdBrushShape);

	//****************************************************************************
	// ˫Բ����ˢ
	class sdDualCircleMesh : public sdBrushShape
	{
	public:
		sdDualCircleMesh();
		~sdDualCircleMesh();

		// �麯���̳�
		void UpdateShape();

		// ����
		void SetInnerRadius(float fInnerRadius) { m_fInnerRadius = fInnerRadius;}
		void SetOuterRadius(float fOuterRadius) { m_fOuterRadius = fOuterRadius;}
		float GetInnerRadius() const { return m_fInnerRadius;}
		float GetOuterRadius() const { return m_fOuterRadius;}

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