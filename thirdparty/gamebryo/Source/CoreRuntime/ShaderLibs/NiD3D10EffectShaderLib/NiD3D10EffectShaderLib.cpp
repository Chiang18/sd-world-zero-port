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

// Precompiled Header
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectShaderLib.h"
#include "NiD3D10EffectShaderLibrary.h"
#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectLoader.h"

#include <NiD3D10Renderer.h>
#include <NiD3D10ShaderLibraryInterface.h>
#include <NiShaderAttributeDesc.h>
#include <NiShaderDesc.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------
#if defined(_USRDLL)
#include "NiD3D10EffectShaderSDM.h"
static NiD3D10EffectShaderSDM NiD3D10EffectShaderSDMObject;

BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NiD3D10EffectShaderLib> DLLMain CALL - ");

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
            NiStaticDataManager::RemoveLibrary("NiD3D10EffectShader");
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
NID3D10EFFECTSHADER_ENTRY bool LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* apcDirectories[], bool bRecurseSubFolders,
    NiShaderLibrary** ppkLibrary)
{
    return NiD3D10EffectShaderLib_LoadShaderLibrary(
        pkRenderer,
        iDirectoryCount,
        apcDirectories,
        bRecurseSubFolders,
        ppkLibrary);
}
//---------------------------------------------------------------------------
NID3D10EFFECTSHADER_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
bool NiD3D10EffectShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* apcDirectories[], bool bRecurseSubFolders, 
    NiShaderLibrary** ppkLibrary)
{
    if (ppkLibrary == NULL)
        return false;

    NiD3D10EffectShaderLibrary* pkLibrary = NiD3D10EffectShaderLibrary::Create(
        pkRenderer, iDirectoryCount, apcDirectories, 
        bRecurseSubFolders);

    *ppkLibrary = pkLibrary;
    return (pkLibrary != NULL);
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectShaderLib_GetD3D10EffectHLSLFlags()
{
    return NiD3D10EffectFactory::GetD3D10EffectHLSLFlags();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLib_SetD3D10EffectHLSLFlags(NiUInt32 uiHLSLFlags)
{
    NiD3D10EffectFactory::SetD3D10EffectHLSLFlags(uiHLSLFlags);
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectShaderLib_GetD3D10EffectFXFlags()
{
    return NiD3D10EffectFactory::GetD3D10EffectFXFlags();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLib_SetD3D10EffectFXFlags(NiUInt32 uiFXFlags)
{
    NiD3D10EffectFactory::SetD3D10EffectFXFlags(uiFXFlags);
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLib_AddFileExtension(const char* pcExtension, 
    bool bCompiled)
{
    NiD3D10EffectLoader* pkLoader = NiD3D10EffectLoader::GetInstance();
    NIASSERT(pkLoader);
    pkLoader->AddFileExtension(pcExtension, bCompiled);
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLib_RemoveFileExtension(const char* pcExtension)
{
    // Don't create a new loader if one doesn't exist
    NiD3D10EffectLoader* pkLoader = NiD3D10EffectLoader::GetInstance(false);
    if (pkLoader)
        pkLoader->RemoveFileExtension(pcExtension);
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderLib_EnableFXLSupport(bool bEnable)
{
    NiD3D10EffectFactory::SetFXLSupport(bEnable);
}
//---------------------------------------------------------------------------
