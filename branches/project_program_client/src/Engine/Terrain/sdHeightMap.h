//*************************************************************************************************
// 内容:	地表高度图
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-01
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_HEIGHT_MAP_H__
#define _TERRAIN_HEIGHT_MAP_H__
#include "sdTypes.h"

// 地表高度图,必须是正方形,大小必须是pow(2,n)个单位
//
// 坐标系:	GB默认世界坐标系,向右为X,向前为Y,向上为Z
//
// 默认高度图从(0,0,0)点开始向XY正方向延伸
//
class sdHeightMap : public NiRefObject
{
public:
	sdHeightMap(uint uiSize);
	~sdHeightMap();

	// 属性访问
	uint	GetSize() const { return m_uiSize;};
	float	GetMinHeight() const { return m_fMinHeight;}
	float	GetMaxHeight() const { return m_fMaxHeight;}

	// 高度(高度图像素坐标,原始高度)
	void	SetRawHeight(uint uiX, uint uiY, float fHeight);
	float	GetRawHeight(uint uiX, uint uiY);

	// 法线(高度图像素坐标,平均法线)
	void	GetNormal(uint uiX, uint uiY, float& fNX, float& fNY, float& fNZ);

	// 高度(高度图像素坐标,插值高度)
	//	1.按照图形渲染的方式分两块三角形,在三角形内部进行插值
	//	2.双线性插值(2x2)
	//	3.双三次插值
	float	GetHeight(float fX, float fY);
	float	GetHeight_Bilinear(float fX, float fY);
	//float	GetHeight_Bicubic(float fX, float fY);

	// 坡度(高度图像素坐标)
	//NiPoint3	GetGradients(float fX, float fY);

	// 获取LOD次级粗糙网格与LOD精细级网格在指定坐标处误差值(相差一级)
	// (为了提高效率,这里没有检查输入参数)
	//	uiX,uiY		高度图像素坐标
	//	uiSpacing	精细级网格的网格大小,应该满足power(2,n),即1/2/4/8/16/32
	float	GetMorphDelta(uint uiX, uint uiY, uint uiSpacing);

protected:
	//NiPoint3	m_kOrigin;			// 高度图起始位置与缩放(整体显示效果)
	//float		m_fScale;
	//float		m_fHorizontalScale;	// 水平缩放
	//float		m_fVerticalScale;	// 垂直缩放,基高比(显示时高度夸张比例)
	//int		m_iSpacing;			// 高度图顶点间距(Cry有此参数)
	//float		m_fBaseHeight;		// 高度图基准高度

	// 高度图数据(这是精确格式,可以考虑压缩为unsigned short)
	float	*m_pfHeight;

	// 高度图边长大小
	uint	m_uiSize;		// pow(2,n)
	uint	m_uiAllocSize;	// pow(2,n) + 1

	// 最大与最小高度,压缩数据时使用
	float	m_fMinHeight;
	float	m_fMaxHeight;

	// 高度对齐单位值
	// (有时渲染器可能需要压缩地形数据而使用定点浮点数)
	float	m_fAlignedHeight;
};
NiSmartPointer(sdHeightMap);

#endif