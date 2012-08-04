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
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net
#include "NiProgramLauncherPCH.h"

#include "NiProgramLauncher.h"
#include "NiProgramRunnerFactory.h"
#include "NiWinProgramRunnerFactory.h"
#include "NiVersion.h"
#include "NiFileFinder.h"

#if _MSC_VER == 1400 //VC8.0
#define COMPILER_SUFFIX "VC80"
#elif _MSC_VER == 1500 //VC9.0
#define COMPILER_SUFFIX "VC90"
#else
    #error Unsupported version of Visual Studio
#endif

#if defined(NIDEBUG)
#define NI_BUILD_SUFFIX "D"
#elif defined (NIRELEASE)
#define NI_BUILD_SUFFIX "R"
#else
#define NI_BUILD_SUFFIX "S"
#endif

NiProgramLauncher* NiProgramLauncher::ms_pkInstance = NULL;
HINSTANCE NiProgramLauncher::ms_hDllInstance = NULL;

//--------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hDllInstance, DWORD, LPVOID)
{
    NiProgramLauncher::SetDllInstance(hDllInstance);

    return TRUE;
}
//--------------------------------------------------------------------------------------------------
NiProgramLauncher* NiProgramLauncher::GetInstance()
{
    NIASSERT(ms_pkInstance);
    return ms_pkInstance;
}
//--------------------------------------------------------------------------------------------------
void NiProgramLauncher::SetDllInstance(HINSTANCE hDllInstance)
{
    ms_hDllInstance = hDllInstance;
}
//--------------------------------------------------------------------------------------------------
HINSTANCE NiProgramLauncher::GetDllInstance()
{
    return ms_hDllInstance;
}
//--------------------------------------------------------------------------------------------------
NiProgramLauncher::NiProgramLauncher()
{
}
//--------------------------------------------------------------------------------------------------
NiProgramLauncher::~NiProgramLauncher()
{
    m_kPlatformFactoryArray.RemoveAll();

    for (unsigned int ui = 0; ui < m_kDLLHandles.GetSize(); ui++)
    {
        FreeLibrary(m_kDLLHandles.GetAt(ui));
    }
}
//--------------------------------------------------------------------------------------------------
bool NiProgramLauncher::Initialize(NiString* pkErrorInfo /* = NULL */, 
    const char* pcPluginDirectory /* = NULL */)
{
    NIASSERT(NiProgramLauncher::ms_pkInstance == NULL);
    NiProgramLauncher::ms_pkInstance = NiNew NiProgramLauncher();

    return ms_pkInstance->CreateFactories(pkErrorInfo, pcPluginDirectory);
}
//--------------------------------------------------------------------------------------------------
void NiProgramLauncher::Shutdown()
{
    NiDelete NiProgramLauncher::ms_pkInstance;
    NiProgramLauncher::ms_pkInstance = NULL;
}
// ---------------------------------------------------------------------------------------------- 
void NiProgramLauncher::GetAvailablePlatformNames(NiStringArray& kPlatformNames) const
{
    kPlatformNames.RemoveAll();

    unsigned int uiNumOfPlatforms = m_kPlatformFactoryArray.GetSize();
    kPlatformNames.SetSize(uiNumOfPlatforms);

    for(unsigned int uiIndex = 0; uiIndex < uiNumOfPlatforms; ++uiIndex)
    {
        NiProgramRunnerFactory* pkPlatformFactory = m_kPlatformFactoryArray.GetAt(uiIndex);
        NIASSERT(pkPlatformFactory);

        kPlatformNames.Add(pkPlatformFactory->GetPlatformDisplayName());
    }
}
// ---------------------------------------------------------------------------------------------- 
bool NiProgramLauncher::GetAvailableTargetNames(const char* pcPlatformName, 
    NiStringArray& kTargetNames, NiString* pkErrorInfo /* = NULL */) const
{
    NIASSERT(pcPlatformName);

    kTargetNames.RemoveAll();

    NiProgramRunnerFactory* pkPlatformFactory = GetProcessFactory(pcPlatformName);
    if (!pkPlatformFactory)
    {
        NIASSERT(!"Attempting to get targets for unavailable platform");
        if (pkErrorInfo)
        {
            *pkErrorInfo = "Attempting to get targets for unavailable platform";
        }
        return false;
    }

    return pkPlatformFactory->GetAvailableTargetNames(kTargetNames, pkErrorInfo);
}
// ---------------------------------------------------------------------------------------------- 
NiProgramRunner* NiProgramLauncher::Create(const char* pcPlatformName)
{
    NiProgramRunnerFactory* pkPlatformFactory = GetProcessFactory(pcPlatformName);
    if (pkPlatformFactory)
    {
        return pkPlatformFactory->Create();
    }

    return NULL;
}
//--------------------------------------------------------------------------------------------------
NiProgramRunnerFactory* NiProgramLauncher::GetProcessFactory(const char* pcPlatformName) const
{
    unsigned int uiNumOfPlatforms = m_kPlatformFactoryArray.GetSize();
    for(unsigned int uiIndex = 0; uiIndex < uiNumOfPlatforms; ++uiIndex)
    {
        NiProgramRunnerFactory* pkPlatformFactory = m_kPlatformFactoryArray.GetAt(uiIndex);
        NIASSERT(pkPlatformFactory);

        if (strcmp(pkPlatformFactory->GetPlatformDisplayName(), pcPlatformName) == 0)
        {
            return pkPlatformFactory;
        }
    }

    return NULL;
}
//--------------------------------------------------------------------------------------------------
bool NiProgramLauncher::CreateFactories(NiString* pkErrorInfo /* = NULL */, 
    const char* pcPluginDirectory /* = NULL */)
{
    // Add factories that are local to this DLL (the Windows program launcher factory)
    NiWinProgramRunnerFactory* pkPCProcessFactory = NiNew NiWinProgramRunnerFactory();
    m_kPlatformFactoryArray.Add(pkPCProcessFactory);

    
    char acPluginDirectory[NI_MAX_PATH];
    // Has the caller provided a directory to search for program launcher plug-ins?
    if (pcPluginDirectory)
    {
        NiStrcpy(acPluginDirectory, NI_MAX_PATH, pcPluginDirectory);
    }
    else
    {
        NiSprintf(acPluginDirectory, NI_MAX_PATH, "%s\\sdk\\Win32\\DLL", 
            NiProgramRunner::GetEnvVariable("EMERGENT_PATH"));
    }

    // Construct plug-in filename pattern based on the current version. The
    // wildcard represents the platform (e.g. Wii, PS3, etc)
    char acSearchPath[NI_MAX_PATH];
    NiSprintf(acSearchPath, NI_MAX_PATH, "%s\\Ni*ProgramLauncher%d%d%s%s.dll",
        acPluginDirectory, 
        GAMEBRYO_MAJOR_VERSION, GAMEBRYO_MINOR_VERSION, COMPILER_SUFFIX, NI_BUILD_SUFFIX);

    // Search for other DLLs that contain dev kit launcher factories and add them to 
    // m_kPlatformFactoryArray
    return AddPluginDirectory(acSearchPath, false, pkErrorInfo);
}
//--------------------------------------------------------------------------------------------------
bool NiProgramLauncher::AddPluginDirectory(const char* pcPath, bool bRecurse, 
    NiString* pkErrorInfo /* = NULL */)
{
    NiFileFinder kFinder(pcPath, bRecurse, ".dll");

    if (!kFinder.HasMoreFiles())
    {
        if (pkErrorInfo)
        {
            pkErrorInfo->Format("Unable to find any console launcher plug-ins in the following "
                "directory: [%s]", pcPath);
        }
        return false;
    }

    bool bInvalidLoadingAdded = false;
    while(kFinder.HasMoreFiles())
    {
        NiFoundFile* pkFile = kFinder.GetNextFile();
        if (pkFile)
        {
            bool bAdded = AddLibrary(pkFile->m_strPath);
            if (!bAdded)
            {
                if (!bInvalidLoadingAdded)
                {
                    if (pkErrorInfo)
                    {
                        *pkErrorInfo += "Unable to load the following plug-ins: ";
                    }
                    bInvalidLoadingAdded = true;
                }
                if (pkErrorInfo)
                {
                    *pkErrorInfo += pkFile->m_strPath;
                    *pkErrorInfo += " ";
                }
            }
        }
    }

    return !bInvalidLoadingAdded;
}
//--------------------------------------------------------------------------------------------------
bool NiProgramLauncher::AddLibrary(NiString strPath)
{
    NiModuleRef hPlugin = LoadLibrary(strPath);
    if (!hPlugin)
    {
        return false;
    }

    // If we're attempting to load the NiProgramLauncher DLL, simply return true since we've
    // already added local factories in NiProgramLauncher::CreateFactories()
    if (ms_hDllInstance == hPlugin)
    {
        return true;
    }

    NiCreateProcessFactoryFunction pfnCreateProcessFactory = (NiCreateProcessFactoryFunction)
        GetProcAddress(hPlugin, "NiCreateProcessFactory");
    if (pfnCreateProcessFactory)
    {
        NiProgramRunnerFactory* pkPCProcessFactory = pfnCreateProcessFactory();
        if (pkPCProcessFactory)
        {
            m_kDLLHandles.Add(hPlugin);
            m_kPlatformFactoryArray.Add(pkPCProcessFactory);
            return true;
        }
    }

    FreeLibrary(hPlugin);

    return false;
}
//--------------------------------------------------------------------------------------------------

