#include "stdafx.h"
#include "sdRenderSystem.h"

namespace RenderSystem
{

//-------------------------------------------------------------------------------------------------
void sdRenderParams::Reset()
{
	diffuseAnisotropicOffset = 0;
	normalAnisotropicOffset = 0;
	diffuseLodBiasOffset = 0.0f;
	normalLodBiasOffset = 0.0f;
	normalScaleOffset = 0.0f;

	buildingChannelMask = 0xffffffff;
	actorChannelMask = 0xffffffff;
	terrainChannelMask = 0xffffffff;

	densityMode = false;
	densityMaxThreshold = 40000.0f;
	densityMinThreshold = 10000.0f;
}
//-------------------------------------------------------------------------------------------------
void sdRenderParams::EnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel, bool bEnable)
{
	NIASSERT(eObjectType >= 0 && eObjectType < NUM_GAMEOBJECT_TYPE);
	NIASSERT(eChannel >= 0 && eChannel < NUM_SHADING_CHANNELS);

	if (E_BUILDING == eObjectType)
	{
		if(bEnable)	buildingChannelMask |= (1 << eChannel);
		else		buildingChannelMask &= ~(1 << eChannel);
	}
	else if (E_ACTOR == eObjectType)
	{
		if(bEnable)	actorChannelMask |= (1 << eChannel);
		else		actorChannelMask &= ~(1 << eChannel);
	}
	else if (E_TERRAIN == eObjectType)
	{
		if(bEnable)	terrainChannelMask |= (1 << eChannel);
		else		terrainChannelMask &= ~(1 << eChannel);
	}
}
//-------------------------------------------------------------------------------------------------
bool sdRenderParams::IsEnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel)
{
	NIASSERT(eObjectType >= 0 && eObjectType < NUM_GAMEOBJECT_TYPE);
	NIASSERT(eChannel >= 0 && eChannel < NUM_SHADING_CHANNELS);

	if (E_BUILDING == eObjectType)
	{
		return 0 != (buildingChannelMask & (1 << eChannel));
	}
	else if (E_ACTOR == eObjectType)
	{
		return 0 != (actorChannelMask & (1 << eChannel));
	}
	else if (E_TERRAIN == eObjectType)
	{
		return 0 !=	(terrainChannelMask & (1 << eChannel));
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
void sdEnvironmentParams::Reset()
{
	fogColor = NiColor::BLACK;
	fogStart = 0.0f;
	fogEnd	= 1.0f;
	fogDensity = 0.001f;
	fogHeightStart = -100.0f;
	fogHeightEnd   = 500.0f;
	fogColorDensity = 1.0f;

	skyFogColor = NiColor::BLACK;
	skyFogTop = 1.0f;
	skyFogBottom = 0.0f;
	skyFogDensity = 0.0f;
}
//-------------------------------------------------------------------------------------------------
void sdPostProcessParams::Reset()
{
	dofStart = 0.0f;
	dofEnd = 0.0f;
	dofSkyDepth = 1.0f;
	dofBlurWidth = 1.0f;
}
//-------------------------------------------------------------------------------------------------
sdRenderSystem::sdRenderSystem()
	: m_bInitialized(false)
{

}
//-------------------------------------------------------------------------------------------------
sdRenderSystem::~sdRenderSystem()
{

}
//-------------------------------------------------------------------------------------------------
}