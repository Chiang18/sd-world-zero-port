#include "Stdafx.h"
#include "sdEditHelper.h"

//
#include <sdRenderSystem_DX9.h>

//
using namespace RenderSystem;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdEditHelper::sdEditHelper()
{
	sdRenderSystem* pkRenderSystem = sdRenderSystem_DX9::InstancePtr();
	NIASSERT(pkRenderSystem);

	m_pkMap = pkRenderSystem->GetMap();
}
//-------------------------------------------------------------------------------------------------
}