#include "Stdafx.h"
#include "sdHeightMap.h"

//-------------------------------------------------------------------------------------------------
sdHeightMap::sdHeightMap(uint uiSize)
: m_pfHeight(NULL)
, m_uiSize(uiSize)
, m_uiAllocSize(uiSize + 1)
, m_fMinHeight(0.0f)
, m_fMaxHeight(0.0f)
, m_fAlignedHeight(0.05f)
{
	// 检查尺寸
	if (!efd::IsPowerOf2(uiSize))
		return;

	// 创建高度图数据
	m_pfHeight = new float[m_uiAllocSize * m_uiAllocSize];
	NIASSERT(m_pfHeight);
	memset(m_pfHeight, 0, m_uiAllocSize * m_uiAllocSize * sizeof(float));
}
//-------------------------------------------------------------------------------------------------
sdHeightMap::~sdHeightMap()
{
	if (m_pfHeight)
	{
		delete m_pfHeight;
		m_pfHeight = NULL;

		m_uiSize = 0;
		m_uiAllocSize = 0;
	}
}
//-------------------------------------------------------------------------------------------------
float sdHeightMap::GetRawHeight(uint uiX, uint uiY)
{
	// 溢出则返回对应边缘部分的值
	if (uiX > m_uiSize)	uiX = m_uiSize;
	if (uiY > m_uiSize)	uiY = m_uiSize;

	return m_pfHeight[uiY * m_uiAllocSize + uiX];
}
//-------------------------------------------------------------------------------------------------
void sdHeightMap::SetRawHeight(uint uiX, uint uiY, float fHeight)
{
	// 溢出则不设置
	if (uiX > m_uiSize)	return;
	if (uiY > m_uiSize)	return;

	// 检查并更新高度范围
	const static float fMaxHeightRange = 1000.0f;
	if (fHeight < m_fMinHeight)
	{
		if (m_fMaxHeight - fHeight < fMaxHeightRange)	
			m_fMinHeight = fHeight;
		else 
			return;
	}
	else if (fHeight > m_fMaxHeight)
	{
		if (fHeight - m_fMinHeight < fMaxHeightRange)	
			m_fMaxHeight = fHeight;
		else 
			return;
	}

	// 高度对齐
	if (m_fAlignedHeight > 0.0f)
		fHeight = floor(fHeight / m_fAlignedHeight + 0.5f) * m_fAlignedHeight;

	// 高度保存
	m_pfHeight[uiY * m_uiAllocSize + uiX] = fHeight;
}
//-------------------------------------------------------------------------------------------------
void sdHeightMap::GetNormal(uint uiX, uint uiY, float& fNX, float& fNY, float& fNZ)
{
	//	v6----v7---v8
	//	| \   | \  |
	//	|  \  |  \ |
	//	| 	\ |   \|
	//	v4----v----v5
	//	| \	  | \  |
	//	|  \  |  \ |
	//	|   \ |   \|
	//	v1---v2----v3
	//
	// 如上图,不过目前仅仅采用了4邻接点的法线的平均值

	uint uiXMinus = ((int)uiX-1) < 0 ? uiX : uiX-1;	// 防止uint下溢
	uint uiYMinus = ((int)uiY-1) < 0 ? uiY : uiY-1;
	NiPoint3 kV2( 0, -1,	GetRawHeight(uiX, uiYMinus));
	NiPoint3 kV5( 1,  0,	GetRawHeight(uiX+1, uiY));
	NiPoint3 kV7( 0,  1,	GetRawHeight(uiX, uiY+1));
	NiPoint3 kV4(-1,  0,	GetRawHeight(uiXMinus, uiY));

	kV2.Unitize();
	kV5.Unitize();
	kV7.Unitize();
	kV4.Unitize();
	NiPoint3 kV = kV2.Cross(kV5) + kV5.Cross(kV7) + kV7.Cross(kV4) + kV4.Cross(kV2);
	kV.Unitize();

	fNX = kV.x;
	fNY = kV.y;
	fNZ = kV.z;
}
//-------------------------------------------------------------------------------------------------
float sdHeightMap::GetMorphDelta(uint uiX, uint uiY, uint uiSpacing)
{
	// LOD采样示意:
	//	1.tt点为下一级粗糙LOD的采样点
	//	2.tf/ft/ff点为下一级粗糙LOD对精细LOD产生误差的点
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft

	// 判定是否位于非变形点上
	bool bXSample = !(uiX & 0x1);
	bool bYSample = !(uiY & 0x1);

	// 获取粗级LOD的可能高度
	float fLODHeight = 0.0f;
	if (bXSample)
	{
		if (bYSample)
		{
			// 位于tt点
			// 此时两级LOD采样同一个点,没有误差
			return 0.0f;
		}
		else
		{
			// 位于tf点(采样上下两个tt点,这里不会产生下溢)
			fLODHeight += GetRawHeight(uiX, uiY - uiSpacing);
			fLODHeight += GetRawHeight(uiX, uiY + uiSpacing);
			fLODHeight *= 0.5f;
		}
	}
	else
	{
		if (bYSample)
		{
			// 位于ft点(采样左右两个tt点,这里不会产生下溢)
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY);
			fLODHeight *= 0.5f;
		}
		else
		{
			// 位于ff点(采样四角四个tt点,这里不会产生下溢)
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY - uiSpacing);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY + uiSpacing);
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY + uiSpacing);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY - uiSpacing);
			fLODHeight *= 0.25f;
		}
	}

	// 计算偏差
	return fLODHeight - GetRawHeight(uiX, uiY);
}
//-------------------------------------------------------------------------------------------------