//*************************************************************************************************
// 内容:	地形笔刷, wz原设计为:
//				Brush(画刷)			包装器
//				Shape(形状)			用于提供一个矩形区域内掩模和权重
//				Canvas(画布)		用于根据有效掩模位置的权重操作对象
//				BrushShape(形状)	刷子显示对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-28
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITBRUSH_H__
#define _SD_GAMEEDITEX_EDITBRUSH_H__
#include "sdGameEditEx.h"
#include "sdEditShape.h"

namespace GameEditEx
{
	//****************************************************************************
	// 编辑笔刷基类
	class sdEditBrush : public NiRefObject
	{
	public:
		sdEditBrush(){};
		virtual ~sdEditBrush() {};

		void SetEditShape(sdEditShape* pkEditShape) { m_pkEditShape = pkEditShape;}
		void SetBrushShape(NiAVObject* spAVObject) { m_spBrushShape = spAVObject;}
		virtual void Apply() = 0;

	protected:
		NiAVObjectPtr	m_spBrushShape;
		sdEditShapePtr	m_pkEditShape;
		
	};
	NiSmartPointer(sdEditBrush);

	//****************************************************************************
	// 地表高度拉伸笔刷
	class sdTerrainDeformPoolBrush : public sdEditBrush
	{
	public:
		sdTerrainDeformPoolBrush();
		virtual ~sdTerrainDeformPoolBrush();

		// 虚函数继承
		virtual void Apply();

	protected:
	
	};
	NiSmartPointer(sdTerrainDeformPoolBrush);

	//****************************************************************************
	// 地表纹理编辑笔刷
}
#endif