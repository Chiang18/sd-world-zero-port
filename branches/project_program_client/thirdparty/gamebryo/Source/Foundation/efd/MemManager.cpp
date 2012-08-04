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
#include "efdPCH.h"

#include <efd/MemManager.h>
#include <efd/StandardAllocator.h>
#include <efd/MemTracker.h>
#include <efd/StaticDataManager.h>
#include <efd/Logger.h>

#include <efd/DefaultInitializeMemoryManager.h>
#include <efd/efdLibType.h>

//-------------------------------------------------------------------------------------------------

// Note: This must be _DEBUG, not EE_EFD_CONFIG_DEBUG, as _DEBUG is what gates macros in crtdbg.h
#if defined(_DEBUG) && defined(EE_PLATFORM_WIN32)

#include <crtdbg.h>
#define EE_USING_CRTDBG 1

// This hook is used to temporarily suppress the leak report until we have finished shutting
// down the memory manager.  For more information, see the comments in SDMShutdown or ~MemManager.
static int __cdecl EE_CrtDebugReportHook(int reportType, char* /*pMsg*/, int* /*pnRet*/)
{
    // Bury warnings (the channel used for leak reports), but not errors.

    // If this hook returns TRUE, it is considered handled.
    // FALSE indicates that the caller should print
    return (reportType == _CRT_WARN) ? TRUE : FALSE;
}

static bool EE_InstalledReportHook = false;

#else

#define EE_USING_CRTDBG 0

#endif

//-------------------------------------------------------------------------------------------------

using namespace efd;

// Make use of the default memory allocator implementation if efd is being built
// as a DLL.  
#if defined(EE_EFD_EXPORT)
EE_USE_DEFAULT_ALLOCATOR;
#endif

namespace efd
{

MemManager* efd::MemManager::ms_pkMemManager = 0;

//-------------------------------------------------------------------------------------------------
MemManager::MemManager()
    : m_pkAllocator(NULL)
    , m_bMemLogHandlerCreated(false)
{
    m_pkAllocator = CreateGlobalMemoryAllocator();
    EE_MEMASSERT(m_pkAllocator);
    m_pkAllocator->Initialize();
}

//-------------------------------------------------------------------------------------------------
MemManager::~MemManager()
{
#if EE_USING_CRTDBG
    if (EE_InstalledReportHook)
    {
        // Remove the CRT debug report hook that was installed at the end of EE_SHUTDOWN
        // (during SDM shutdown) which was burying any leak reports.
        _CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, EE_CrtDebugReportHook);
    }
#endif

    efd::LoggerSingleton::DestroyInstance();
    m_pkAllocator->Shutdown();

    EE_EXTERNAL_DELETE(m_pkAllocator);
    m_pkAllocator = NULL;

#if EE_USING_CRTDBG
    if (EE_InstalledReportHook)
    {
        // Trigger a leak report to list any *real* leaks.
        _CrtDumpMemoryLeaks();
    }
#endif
}

//-------------------------------------------------------------------------------------------------
void MemManager::CreateMemoryLogHandler()
{
    if (ms_pkMemManager->m_pkAllocator && !ms_pkMemManager->m_bMemLogHandlerCreated)
    {
        ms_pkMemManager->m_bMemLogHandlerCreated = true;
        ms_pkMemManager->m_pkAllocator->CreateMemoryLogHandler();
    }
}

//-------------------------------------------------------------------------------------------------
MemManager& MemManager::Get()
{
    static MemManager ms_kManager;
    ms_pkMemManager = &ms_kManager;
    ms_pkMemManager->CreateMemoryLogHandler();

    return ms_kManager;
}

//-------------------------------------------------------------------------------------------------
bool MemManager::VerifyAddress(const void* pvMemory)
{
    EE_MEMASSERT(ms_pkMemManager->m_pkAllocator);
    return ms_pkMemManager->m_pkAllocator->VerifyAddress(pvMemory);
}

//---------------------------------------------------------------------------

void MemManager::_SDMInit()
{
}

//---------------------------------------------------------------------------
void MemManager::_SDMShutdown()
{
#if EE_USING_CRTDBG
    // Because of the way that the pre-main allocator is instantiated, it may end up in the
    // 'atexit' chain after the object used by MSVC to trigger a CRT debug report.  This
    // causes the allocator and associated pools such as the small object allocator to be falsely
    // reported as leaks.  To prevent this, a report hook is installed at the end of EE_SHUTDOWN
    // (during SDM shutdown) which buries any leak reports.  It is removed and a new report
    // generation forced in MemManager::~MemManager, to catch any *real* leaks.
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, EE_CrtDebugReportHook);
    
    EE_InstalledReportHook = true;
#endif
}

//-------------------------------------------------------------------------------------------------
void MemManager::_SDMPerThreadInit()
{
    MemManager::GetAllocator()->PerThreadInit();
}

//-------------------------------------------------------------------------------------------------
void MemManager::_SDMPerThreadShutdown()
{
    MemManager::GetAllocator()->PerThreadShutdown();
}

//-------------------------------------------------------------------------------------------------
} // end namespace efd

// Implement the static TLS inside StandardAllocator if it has one
EE_IMPLEMENT_SMALL_OBJECT_ALLOCATOR(efd::StandardAllocator);

//-------------------------------------------------------------------------------------------------
