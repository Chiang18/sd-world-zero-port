//-------------------------------------------------------------------------------------------------
inline sdLight::ELightType sdLight::GetType() const
{
	return m_eLightType;
}
//-------------------------------------------------------------------------------------------------
inline NiLight* sdLight::GetGBLight()
{
	return m_spLight;
}
//-------------------------------------------------------------------------------------------------
inline float sdLight::GetDimmer() const
{
	return m_spLight->GetDimmer();
}
//-------------------------------------------------------------------------------------------------
inline const NiColor& sdLight::GetAmbientColor() const
{
	return m_spLight->GetAmbientColor();
}
//-------------------------------------------------------------------------------------------------
inline const NiColor& sdLight::GetDiffuseColor() const
{
	return m_spLight->GetDiffuseColor();
}
//-------------------------------------------------------------------------------------------------
inline const NiColor& sdLight::GetSpecularColor() const
{
	return m_spLight->GetSpecularColor();
}
//-------------------------------------------------------------------------------------------------
inline void sdLight::SetDimmer(float fDimmer)
{
	m_spLight->SetDimmer(fDimmer);
}
//-------------------------------------------------------------------------------------------------
inline void sdLight::SetAmbientColor(const NiColor& kColor)
{
	m_spLight->SetAmbientColor(kColor);
}
//-------------------------------------------------------------------------------------------------
inline void sdLight::SetDiffuseColor(const NiColor& kColor)
{
	m_spLight->SetDiffuseColor(kColor);
}
//-------------------------------------------------------------------------------------------------
inline void sdLight::SetSpecularColor(const NiColor& kColor)
{
	m_spLight->SetSpecularColor(kColor);
}
//-------------------------------------------------------------------------------------------------