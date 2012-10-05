//-------------------------------------------------------------------------------------------------
inline uint sdEditTerrainHelper::GetDeformBrushType()
{ 
	return m_pkTerrainDeformMode->GetBrushType();
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetActiveDeformBrush(uint uiBrush)
{ 
	m_pkTerrainDeformMode->SetActiveDeformBrush((sdEditBrush::EBrushType)uiBrush);
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetDeformBrushVisible(bool bVisible)
{
	m_pkTerrainDeformMode->SetShowBrush(bVisible);
}
//-------------------------------------------------------------------------------------------------
inline float sdEditTerrainHelper::GetDeformBrushOutterRadius()
{ 
	return m_pkTerrainDeformMode->GetOutterRadius();
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetDeformBrushOutterRadius(float fRadius)
{
	m_pkTerrainDeformMode->SetOutterRadius(fRadius);
}
//-------------------------------------------------------------------------------------------------
inline float sdEditTerrainHelper::GetDeformBrushInnerRadius()
{ 
	return m_pkTerrainDeformMode->GetInnerRadius();
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetDeformBrushInnerRadius(float fRadius)
{ 
	m_pkTerrainDeformMode->SetInnerRadius(fRadius);
}
//-------------------------------------------------------------------------------------------------
inline float sdEditTerrainHelper::GetDeformBrushHardness()
{ 
	return m_pkTerrainDeformMode->GetHardness();
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetDeformBrushHardness(float fHardness)
{ 
	m_pkTerrainDeformMode->SetHardness(fHardness);
}
//-------------------------------------------------------------------------------------------------
inline float sdEditTerrainHelper::GetDeformBrushHeight()
{ 
	return m_pkTerrainDeformMode->GetHeight();
}
//-------------------------------------------------------------------------------------------------
inline void sdEditTerrainHelper::SetDeformBrushHeight(float fHeight)
{
	m_pkTerrainDeformMode->SetHeight(fHeight);
}
//-------------------------------------------------------------------------------------------------