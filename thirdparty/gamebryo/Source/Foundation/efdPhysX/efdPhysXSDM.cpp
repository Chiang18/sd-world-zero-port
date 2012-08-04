// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

// Precompiled Header
#include "efdPhysXPCH.h"

#include <efd/StaticDataManager.h>

#include "efdPhysXSDM.h"
#include "PhysXSDKManager.h"

using namespace efdPhysX;

EE_IMPLEMENT_SDM_CONSTRUCTOR(efdPhysX, "");

#ifdef EE_EFDPHYSX_EXPORT
EE_IMPLEMENT_DLLMAIN(efdPhysX);
#endif

//-------------------------------------------------------------------------------------------------
void efdPhysXSDM::Init()
{
    EE_IMPLEMENT_SDM_INIT_CHECK();

    PhysXSDKManager::ms_pPhysXManager = EE_NEW PhysXSDKManager();
    EE_ASSERT(PhysXSDKManager::ms_pPhysXManager);
}
//-------------------------------------------------------------------------------------------------
void efdPhysXSDM::Shutdown()
{
    EE_IMPLEMENT_SDM_SHUTDOWN_CHECK();

    EE_DELETE PhysXSDKManager::ms_pPhysXManager;
}
//-------------------------------------------------------------------------------------------------
