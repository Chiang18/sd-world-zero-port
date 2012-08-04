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

#include "efdPCH.h"

#include <efd/FoundationSDM.h>
#include <efd/MemManager.h>
#include <efd/GlobalStringTable.h>
#include <efd/SystemDesc.h>
#include <efd/Point3.h>
#include <efd/TimeType.h>
#include <efd/StackUtils.h>
#include <efd/AssertDialog.h>

#if defined(EE_PLATFORM_WII)
    #include <efd/Wii/WiiLockedCache.h>
#endif

#if defined(EE_PLATFORM_WII) || defined(EE_PLATFORM_WIN32)
#include <efd/IUSBComm.h>
#endif

#if defined(EE_PLATFORM_PS3)
    #include <efd/PS3/SpursManager_PS3.h>
#endif

//CODEBLOCK(1) - DO NOT DELETE THIS LINE

using namespace efd;

bool FoundationSDM::ms_initialized = false;

//-------------------------------------------------------------------------------------------------
void FoundationSDM::Init()
{
    EE_IMPLEMENT_SDM_INIT_CHECK();

//CODEBLOCK(2) - DO NOT DELETE THIS LINE

#if defined(EE_PLATFORM_WII)
    OSInit();
    OSInitFastCast();

    WiiLockedCache::_SDMInit();
#endif

    MemManager::_SDMInit();

    SystemDesc::_SDMInit();

    Point3::_SDMInit();
    SetInitialTimeInSec();

#if defined(EE_PLATFORM_PS3)
    SpursManager::_SDMInit();
#endif

    // Register the default assert dialog handler.
    AssertHandler handler = efd::AssertHelper::SetAssertHandler(efd::DisplayAssertDialog);
    // If there was already a handler installed, reinstall it because that is what the application
    // really wants.
    if (handler != NULL)
    {
        efd::AssertHelper::SetAssertHandler(handler);
    }
}
//-------------------------------------------------------------------------------------------------
void FoundationSDM::Shutdown()
{
    EE_IMPLEMENT_SDM_SHUTDOWN_CHECK();

#if defined(EE_PLATFORM_PS3)
    SpursManager::_SDMShutdown();
#endif

    Point3::_SDMShutdown();

    SystemDesc::_SDMShutdown();

    MemManager::_SDMShutdown();

#if defined(EE_PLATFORM_WII)
    WiiLockedCache::_SDMShutdown();
#endif

#if defined(EE_PLATFORM_WII) || defined(EE_PLATFORM_WIN32)
    IUSBComm::_SDMShutdown();
#endif

    StackUtils::TurnOffStackTracing();
}
//-------------------------------------------------------------------------------------------------
void FoundationSDM::PerThreadInit()
{
    MemManager::_SDMPerThreadInit();
}
//-------------------------------------------------------------------------------------------------
void FoundationSDM::PerThreadShutdown()
{
    MemManager::_SDMPerThreadShutdown();
}
//-------------------------------------------------------------------------------------------------
