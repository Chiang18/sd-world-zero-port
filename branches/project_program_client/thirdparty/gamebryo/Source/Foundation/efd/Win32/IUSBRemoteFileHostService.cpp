// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

// Precompiled Header
#include "efdPCH.h"
#include <efd/Win32/IUSBRemoteFileHostService.h>

using namespace efd;

typedef ISystemService* (*SystemServiceFactoryType)();

//-------------------------------------------------------------------------------------------------
IUSBRemoteFileHostService::IUSBRemoteFileHostService()
{
}

//-------------------------------------------------------------------------------------------------
/* virtual */
IUSBRemoteFileHostService::~IUSBRemoteFileHostService()
{
}

DynamicModule IUSBRemoteFileHostService::m_Module;
ISystemService* IUSBRemoteFileHostService::m_pSystemService = NULL;

static const char EFD_WII_REMOTE_HOST_SERVICE_LIB[] = "efdNetLibWiiUSB";

//-------------------------------------------------------------------------------------------------
ISystemService* IUSBRemoteFileHostService::Instance()
{
#if defined(EE_PLATFORM_WIN32) && defined(USEDLL)
    if (m_pSystemService)
    {
        return m_pSystemService;
    }

    utf8string libraryName = EFD_WII_REMOTE_HOST_SERVICE_LIB;

    if (!m_Module.LoadModule(libraryName))
    {
        return NULL;
    }

    SystemServiceFactoryType factory = 
        (SystemServiceFactoryType)m_Module.GetMethod("CreateUSBRemoteFileHostService");

    if (!factory)
    {
       return NULL;
    }

    m_pSystemService = factory();
    return m_pSystemService;
#else
    return NULL;
#endif
}

//-------------------------------------------------------------------------------------------------
