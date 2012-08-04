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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NSFParserLibPCH.h"

#include "NSFParserLib.h"
#include "NSFLoader.h"
#include "NSFParserLibLibType.h"

//---------------------------------------------------------------------------
#if defined(_USRDLL)

#include "NSFParserLibSDM.h"
static NSFParserLibSDM NSFParserLibSDMObject;

BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NSFParserLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            NiOutputDebugString("PROCESS ATTACH!\n");
            NiStaticDataManager::ProcessAccumulatedLibraries();
        }
        break;
    case DLL_THREAD_ATTACH:
        {
            NiOutputDebugString("THREAD ATTACH!\n");
        }
        break;
    case DLL_PROCESS_DETACH:
        {
            //  Shutdown anything needed here
            NiOutputDebugString("PROCESS DETACH!\n");
            NiStaticDataManager::RemoveLibrary("NSFParserLib");
        }
        break;
    case DLL_THREAD_DETACH:
        {
            NiOutputDebugString("THREAD DETACH!\n");
        }
        break;
    }

    return (TRUE);
}
//---------------------------------------------------------------------------
NSFPARSERLIB_ENTRY unsigned int RunShaderParser(const char* pszDirectory,
    bool bRecurseSubFolders)
{
    return NSFParserLib_RunShaderParser(pszDirectory, bRecurseSubFolders);
}
//---------------------------------------------------------------------------
NSFPARSERLIB_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
//---------------------------------------------------------------------------
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
unsigned int NSFParserLib_RunShaderParser(const char* pszDirectory, 
    bool bRecurseSubFolders)
{
    // Create a loader...
    NiShaderParser* pkLoader = NSFLoader::Create();
    NIASSERT(pkLoader);

    unsigned int uiCount;
    pkLoader->ParseAllFiles(pszDirectory, bRecurseSubFolders, uiCount);
    return uiCount;
}
//---------------------------------------------------------------------------
