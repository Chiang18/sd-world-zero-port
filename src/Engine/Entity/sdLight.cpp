#include "Stdafx.h"
#include "sdLight.h"

//-------------------------------------------------------------------------------------------------
sdLight::sdLight(sdLight::ELightType eLightType)
: m_eLightType(eLightType)
{
	switch(eLightType)
	{
	case E_LT_AMBIENT:
		{
			m_spLight = NiNew NiAmbientLight;
			NIASSERT(m_spLight);
			break;
		}
	case E_LT_DIR:
		{
			m_spLight = NiNew NiDirectionalLight;
			NIASSERT(m_spLight);
			m_spLight->SetAmbientColor(NiColor::BLACK);
			break;
		}
	}

	// ¹Ò½ÓGBLight
	AttachChild(m_spLight);

	// ¸üÐÂ
	Update(0.0f);
}
//-------------------------------------------------------------------------------------------------
sdLight::~sdLight()
{
	m_spLight = 0;
}
//-------------------------------------------------------------------------------------------------
void sdLight::UpdateWorldBound()
{
	switch(m_eLightType)
	{
	case E_LT_POINT:
	case E_LT_SPOT:
		{
			break;
		}

	case E_LT_AMBIENT:
	case E_LT_DIR:
		{
			m_kWorldBound.SetCenter(NiPoint3::ZERO);
			m_kWorldBound.SetRadius(FLT_MAX);
			break;
		}
	}
}
//-------------------------------------------------------------------------------------------------
void sdLight::UpdateDownwardPass(NiUpdateProcess &kUpdate)
{
	__super::UpdateDownwardPass(kUpdate);
	UpdateWorldBound();
}
//-------------------------------------------------------------------------------------------------