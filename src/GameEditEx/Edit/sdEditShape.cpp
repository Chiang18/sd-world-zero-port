#include "Stdafx.h"
#include "sdEditShape.h"

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdDualCircleShape::sdDualCircleShape()
: m_fInnerRadius(0.0f)
, m_fOuterRadius(5.0f)
, m_fHardness(1.0f)
{
	
}
//-------------------------------------------------------------------------------------------------
sdDualCircleShape::~sdDualCircleShape()
{

}
//-------------------------------------------------------------------------------------------------
void sdDualCircleShape::SetInnerRadius(float fInnerRadius)
{ 
	m_fInnerRadius = fInnerRadius;
	if (m_fOuterRadius < fInnerRadius)
		m_fOuterRadius = fInnerRadius;
}

void sdDualCircleShape::SetOuterRadius(float fOuterRadius)
{ 
	m_fOuterRadius = fOuterRadius;
	if (m_fInnerRadius > fOuterRadius)
		m_fInnerRadius = fOuterRadius;
}

void sdDualCircleShape::SetHardness(float fHardness)
{
	if (fHardness < 0.0f)	fHardness = 0.0f;
	if (fHardness > 1.0f)	fHardness = 1.0f;
	m_fHardness = fHardness;
}
//-------------------------------------------------------------------------------------------------
float sdDualCircleShape::GetWeight(float fX, float fY)
{
	if (m_fInnerRadius > m_fOuterRadius)
		return 0.0f;

	float fDistance = sqrt((fX - m_kCenter.x) * (fX - m_kCenter.x) + (fY - m_kCenter.y) *  (fY - m_kCenter.y));
	if (fDistance <= m_fInnerRadius)
	{
		return m_fHardness;
	}
	else if (fDistance < m_fOuterRadius)
	{
		return m_fHardness * (1.0f - (fDistance - m_fInnerRadius)/ (m_fOuterRadius - m_fInnerRadius));
	}
	else
	{
		return 0.0f;
	}
}
//-------------------------------------------------------------------------------------------------
}