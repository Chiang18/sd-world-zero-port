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
#include "NSBShaderLibPCH.h"

#include "NSBLoader.h"
#include "NSBShaderLibrary.h"

#ifdef WIN32
#include "NSBD3D10ShaderLibrary.h"
#endif

#include <NiShaderLibraryDesc.h>
#include <NiFilename.h>

//---------------------------------------------------------------------------
NSBLoader* NSBLoader::ms_pkLoader = 0;

//---------------------------------------------------------------------------
NSBLoader::NSBLoader()
{
    m_kBinaryList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBLoader::~NSBLoader()
{
    m_kBinaryListIter = m_kBinaryList.GetHeadPos();
    while (m_kBinaryListIter)
    {
        char* pszName = m_kBinaryList.GetNext(m_kBinaryListIter);
        if (pszName)
            NiFree(pszName);
    }
    m_kBinaryList.RemoveAll();
}
//---------------------------------------------------------------------------
NSBLoader* NSBLoader::Create()
{
    if (!ms_pkLoader)
        ms_pkLoader = NiNew NSBLoader();

    return ms_pkLoader;
}
//---------------------------------------------------------------------------
void NSBLoader::Destroy()
{
    if (ms_pkLoader)
        NiDelete ms_pkLoader;

    ms_pkLoader = 0;
}
//---------------------------------------------------------------------------
bool NSBLoader::LoadAllNSBFiles(NSBShaderLibrary* pkLibrary, 
    const char* pszDirectory, bool bRecurseSubDirs)
{
    FindAllNSBFiles(pszDirectory, bRecurseSubDirs);
    if (!LoadAllNSBFiles(pkLibrary))
        return false;

    return true;
}
//---------------------------------------------------------------------------
#ifdef WIN32
bool NSBLoader::LoadAllNSBD3D10Files(NSBD3D10ShaderLibrary* pkLibrary, 
    const char* pszDirectory, bool bRecurseSubDirs)
{
    FindAllNSBFiles(pszDirectory, bRecurseSubDirs);
    if (!LoadAllNSBD3D10Files(pkLibrary))
        return false;

    return true;
}
#endif
//---------------------------------------------------------------------------
unsigned int NSBLoader::GetBinaryFileCount()
{
    return m_kBinaryList.GetSize();
}
//---------------------------------------------------------------------------
const char* NSBLoader::GetFirstBinaryFile()
{
    m_kBinaryListIter = m_kBinaryList.GetHeadPos();
    
    if (m_kBinaryListIter)
        return m_kBinaryList.GetNext(m_kBinaryListIter);
    return 0;
}
//---------------------------------------------------------------------------
const char* NSBLoader::GetNextBinaryFile()
{
    if (m_kBinaryListIter)
        return m_kBinaryList.GetNext(m_kBinaryListIter);
    return 0;
}
//---------------------------------------------------------------------------
void NSBLoader::FindAllNSBFiles(const char* pszDirectory, 
    bool bRecurseDirectories)
{
    LoadAllNSBFilesInDirectory(pszDirectory, ".NSB", bRecurseDirectories,
        &m_kBinaryList);
}
//---------------------------------------------------------------------------
bool NSBLoader::ProcessNSBFile(const char* pszFilename, const char* pszExt,
    NiTPointerList<char*>* pkFileList)
{
    if (!pszFilename || (strcmp(pszFilename, "") == 0))
        return false;

    NiFilename kFilename(pszFilename);
    if (NiStricmp(kFilename.GetExt(), pszExt) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_0, 
            "        Found %s File %s\n", pszExt, pszFilename);

        // Add it to the list
        size_t stLen = strlen(pszFilename) + 1;
        char* pszNew = NiAlloc(char, stLen);
        NIASSERT(pszNew);

        NiStrcpy(pszNew, stLen, pszFilename);

        pkFileList->AddTail(pszNew);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NSBLoader::LoadAllNSBFiles(NSBShaderLibrary* pkLibrary)
{
    bool bResult = false;

    const char* pcFile = GetFirstBinaryFile();
    while (pcFile)
    {
        bResult |= LoadNSBFile(pkLibrary, pcFile);
        pcFile = GetNextBinaryFile();
    }

    return bResult;
}
//---------------------------------------------------------------------------
bool NSBLoader::LoadNSBFile(NSBShaderLibrary* pkLibrary, const char* pcFile)
{
    NIASSERT(pkLibrary);

    // Parse the name and see if it's in the list.
    NiFilename kFilename(pcFile);

    if (pkLibrary->GetNSBShader(kFilename.GetFilename()))
    {
        NILOG(NIMESSAGE_GENERAL_0, "%s (%s) already loaded?\n", 
            kFilename.GetFilename(), pcFile);
    }
    else
    {
        NILOG(NIMESSAGE_GENERAL_0, "Attempting to load %s\n", pcFile);

        NSBShader* pkNSBShader = NiNew NSBShader();
        if (pkNSBShader)
        {
            if (pkNSBShader->Load(pcFile))
            {
                NILOG(NIMESSAGE_GENERAL_0, "Loaded %s\n", pcFile);
                pkLibrary->InsertNSBShaderIntoList(pkNSBShader);
                return true;
            }
            else
            {
                NILOG(NIMESSAGE_GENERAL_0, "Failed to load %s\n",
                    pcFile);
                NiDelete pkNSBShader;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
#ifdef WIN32
//---------------------------------------------------------------------------
bool NSBLoader::LoadAllNSBD3D10Files(NSBD3D10ShaderLibrary* pkLibrary)
{
    bool bResult = false;

    const char* pcFile = GetFirstBinaryFile();
    while (pcFile)
    {
        bResult |= LoadNSBD3D10File(pkLibrary, pcFile);
        pcFile = GetNextBinaryFile();
    }
    return bResult;
}
//---------------------------------------------------------------------------
bool NSBLoader::LoadNSBD3D10File(NSBD3D10ShaderLibrary* pkLibrary,
    const char* pcFile)
{
    NIASSERT(pkLibrary);

    // Parse the name and see if it's in the list.
    NiFilename kFilename(pcFile);

    if (pkLibrary->GetNSBD3D10Shader(kFilename.GetFilename()))
    {
        NILOG(NIMESSAGE_GENERAL_0, "%s (%s) already loaded?\n", 
            kFilename.GetFilename(), pcFile);
        return false;
    }
    else
    {
        NILOG(NIMESSAGE_GENERAL_0, "Attempting to load %s\n", pcFile);

        NSBD3D10Shader* pkNSBD3D10Shader = NiNew NSBD3D10Shader();
        if (pkNSBD3D10Shader)
        {
            if (pkNSBD3D10Shader->Load(pcFile))
            {
                NILOG(NIMESSAGE_GENERAL_0, "Loaded %s\n", pcFile);
                pkLibrary->InsertNSBD3D10ShaderIntoList(pkNSBD3D10Shader);
                return true;
            }
            else
            {
                NILOG(NIMESSAGE_GENERAL_0, "Failed to load %s\n",
                    pcFile);
                NiDelete pkNSBD3D10Shader;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
#endif // #ifdef WIN32
//---------------------------------------------------------------------------
