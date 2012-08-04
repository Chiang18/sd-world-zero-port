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

#include "NSFTextFile.h"

#include <NiSystem.h>

//---------------------------------------------------------------------------
NSFTextFile::NSFTextFile() :
    m_pcFilename(0), 
    m_pcData(0), 
    m_uiSize(0), 
    m_uiPos(0)
{
}
//---------------------------------------------------------------------------
NSFTextFile::~NSFTextFile()
{
    NiFree(m_pcData);
    NiFree(m_pcFilename);
}
//---------------------------------------------------------------------------
int NSFTextFile::Load(const char* pszFilename)
{
    NiFile* pkFile = NiFile::GetFile( pszFilename, NiFile::READ_ONLY );
    if (pkFile == NULL)
        return 1;

    unsigned int uiDataLen = pkFile->GetFileSize();


    size_t stLen = strlen(pszFilename) + 1;
    m_pcFilename = NiAlloc(char, stLen);
    NIASSERT(m_pcFilename);
    NiStrcpy(m_pcFilename, stLen, pszFilename);

    m_uiSize = uiDataLen;

    m_uiPos = 0;
    
    unsigned int auiComponetSizes[1];
    auiComponetSizes[0] = m_uiSize;
    m_pcData = NiAlloc(char, m_uiSize + 1);
    unsigned int uiBytes = 0;
    uiBytes = pkFile->BinaryRead(m_pcData,
        m_uiSize,auiComponetSizes,1);

    NIASSERT(uiBytes == m_uiSize);

    NiDelete pkFile;
    
    return 0;
}
//---------------------------------------------------------------------------
