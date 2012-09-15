//*************************************************************************************************
// 内容:	笔刷形状,描述一个矢量形状或者是一幅栅格图,
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-28
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITSHAPE_H__
#define _SD_GAMEEDITEX_EDITSHAPE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//*************************************************************************
	// 描述一块编辑区域
	class sdEditShape : public NiRefObject
	{
	public:
		sdEditShape(){};
		virtual ~sdEditShape(){};
	};
	NiSmartPointer(sdEditShape);

	//*************************************************************************
	// 这里单位米,世界坐标
	class sdDualCircleShape : public sdEditShape
	{
	public:
		sdDualCircleShape();
		virtual ~sdDualCircleShape();

		// 获取指定位置权重
		bool	InShape(float fX, float fY);
		float	GetWeight(float fX, float fY);

		// 
		void	SetCenter(const NiPoint2& kPos) { m_kCenter = kPos;}
		void	SetCenter(float fX, float fY) { m_kCenter.x = fX; m_kCenter.y = fY;}
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