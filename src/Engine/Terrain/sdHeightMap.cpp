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
	// ���ߴ�
	if (!efd::IsPowerOf2(uiSize))
		return;

	// �����߶�ͼ����
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
	// ����򷵻ض�Ӧ��Ե���ֵ�ֵ
	if (uiX > m_uiSize)	uiX = m_uiSize;
	if (uiY > m_uiSize)	uiY = m_uiSize;

	return m_pfHeight[uiY * m_uiAllocSize + uiX];
}
//-------------------------------------------------------------------------------------------------
void sdHeightMap::SetRawHeight(uint uiX, uint uiY, float fHeight)
{
	// ���������
	if (uiX > m_uiSize)	return;
	if (uiY > m_uiSize)	return;

	// ��鲢���¸߶ȷ�Χ
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

	// �߶ȶ���
	if (m_fAlignedHeight > 0.0f)
		fHeight = floor(fHeight / m_fAlignedHeight + 0.5f) * m_fAlignedHeight;

	// �߶ȱ���
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
	// ����ͼ,����Ŀǰ����������4�ڽӵ�ķ��ߵ�ƽ��ֵ

	uint uiXMinus = ((int)uiX-1) < 0 ? uiX : uiX-1;	// ��ֹuint����
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
	// LOD����ʾ��:
	//	1.tt��Ϊ��һ���ֲ�LOD�Ĳ�����
	//	2.tf/ft/ff��Ϊ��һ���ֲ�LOD�Ծ�ϸLOD�������ĵ�
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft
	// tf ff tf tf tt tf
	// tt ft tt ft tt ft

	// �ж��Ƿ�λ�ڷǱ��ε���
	bool bXSample = !(uiX & 0x1);
	bool bYSample = !(uiY & 0x1);

	// ��ȡ�ּ�LOD�Ŀ��ܸ߶�
	float fLODHeight = 0.0f;
	if (bXSample)
	{
		if (bYSample)
		{
			// λ��tt��
			// ��ʱ����LOD����ͬһ����,û�����
			return 0.0f;
		}
		else
		{
			// λ��tf��(������������tt��,���ﲻ���������)
			fLODHeight += GetRawHeight(uiX, uiY - uiSpacing);
			fLODHeight += GetRawHeight(uiX, uiY + uiSpacing);
			fLODHeight *= 0.5f;
		}
	}
	else
	{
		if (bYSample)
		{
			// λ��ft��(������������tt��,���ﲻ���������)
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY);
			fLODHeight *= 0.5f;
		}
		else
		{
			// λ��ff��(�����Ľ��ĸ�tt��,���ﲻ���������)
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY - uiSpacing);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY + uiSpacing);
			fLODHeight += GetRawHeight(uiX - uiSpacing, uiY + uiSpacing);
			fLODHeight += GetRawHeight(uiX + uiSpacing, uiY - uiSpacing);
			fLODHeight *= 0.25f;
		}
	}

	// ����ƫ��
	return fLODHeight - GetRawHeight(uiX, uiY);
}
//-------------------------------------------------------------------------------------------------