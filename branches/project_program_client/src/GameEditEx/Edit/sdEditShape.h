//*************************************************************************************************
// ����:	���α�ˢ��״,����һ��ʸ����״������һ��դ��ͼ,
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITSHAPE_H__
#define _SD_GAMEEDITEX_EDITSHAPE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//*************************************************************************
	class sdEditShape : public NiRefObject
	{
	public:
		sdEditShape(){};
		virtual ~sdEditShape(){};
	};
	NiSmartPointer(sdEditShape);

	//*************************************************************************
	class sdDualCircleShape : public sdEditShape
	{
	public:
		sdDualCircleShape();
		virtual ~sdDualCircleShape();

		// ��ȡָ��λ��Ȩ��
		float	GetWeight(float fX, float fY);

		// 
		void	SetCenter(const NiPoint2& kPos) { m_kCenter = kPos;}
		void	SetInnerRadius(float fInnerRadius);
		void	SetOuterRadius(float fOuterRadius);
		void	SetHardness(float fHardness);

		const NiPoint2& GetCenter() const { return m_kCenter;}
		float GetInnerRadius() const { return m_fInnerRadius;}
		float GetOuterRadius() const { return m_fOuterRadius;}
		float GetHardness() const { return m_fHardness;}

	protected:
		NiPoint2	m_kCenter;
		float	m_fInnerRadius;
		float	m_fOuterRadius;
		float	m_fHardness;
	};
	NiSmartPointer(sdDualCircleShape);
}
#endif