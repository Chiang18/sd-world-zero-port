#include "stdafx.h"
#include "sdVideoConfig.h"

namespace GameCore
{
//-------------------------------------------------------------------------------------------------
sdVideoConfig::sdVideoConfig()
{
	m_uiWidth = 800;
	m_uiHeight = 600;
	m_bFullScreen = false;
	m_bVSync = false;

	m_uiMipmapSkipLevel = 0;
}
//-------------------------------------------------------------------------------------------------
sdVideoConfig::~sdVideoConfig()
{
	
}
//-------------------------------------------------------------------------------------------------
}