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
#include "sdEditCanvas.h"
#include "sdEditBrushShape.h"

//
namespace Base
{
	namespace Math
	{
		class sdRay;
	}
}

namespace GameEditEx
{
	//****************************************************************************
	// 编辑笔刷基类
	class sdEditBrush : public NiRefObject
	{
	public:
		// 编辑笔刷
		enum eBrushType
		{
			E_BRUSH_PULL = 0,	// 地表拉伸
			E_BRUSH_SMOOTH,		// 地表平滑
			E_BRUSH_NOISE,		// 地表噪声
			E_BRUSH_AVERAGE,
			E_BRUSH_FLAT,		// 地表踏平
			E_BRUSH_DETAIL,		// 地表纹理混合
			NUM_BRUSHES,		
		};	

	public:
		sdEditBrush(){};
		virtual ~sdEditBrush() {};

		void SetEditCanvas(sdEditCanvas* pkEditCanvas) { m_pkEditCanvas = pkEditCanvas;}
		sdEditCanvas* GetEditCanvas() const {return m_pkEditCanvas;}

		void SetEditShape(sdEditShape* pkEditShape) { m_pkEditShape = pkEditShape;}
		sdEditShape* GetEditShape() const {return m_pkEditShape;}

		void SetBrushShape(sdBrushShape* pkBrushShape) { m_pkBrushShape = pkBrushShape;}
		sdBrushShape* GetBrushShape() const { return m_pkBrushShape;}

		virtual void Apply(const Base::Math::sdRay& kRay) = 0;

	protected:
		sdBrushShapePtr	m_pkBrushShape;
		sdEditShapePtr	m_pkEditShape;
		sdEditCanvasPtr m_pkEditCanvas;
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
		virtual void Apply(const Base::Math::sdRay& kRay);
	};
	NiSmartPointer(sdTerrainDeformPoolBrush);

	//****************************************************************************
	// 地表纹理编辑笔刷
}
#endif