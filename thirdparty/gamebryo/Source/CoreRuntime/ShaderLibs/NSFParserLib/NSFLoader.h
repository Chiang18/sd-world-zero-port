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

#pragma once
#ifndef NSFLOADER_H
#define NSFLOADER_H

#include "NSFParserLibLibType.h"
#include "NiShaderParser.h"
#include "NiTList.h"
#include "NiTPointerList.h"

class NiShaderLibraryDesc;
NiSmartPointer(NSFLoader);

class NSFPARSERLIB_ENTRY NSFLoader : public NiShaderParser
{
protected:
    NSFLoader();

public:
    ~NSFLoader();

    virtual bool ParseFile(const char* pcFile, unsigned int& uiCount,
        NiTObjectArray<NiFixedString>* pkFileNames = NULL);

    virtual void ParseAllFiles(const char* pszDirectory,
        bool bRecurseDirectories, unsigned int& uiCount,
        NiTObjectArray<NiFixedString>* pkFileNames = NULL);

    static NiShaderParser* Create();
    static void Destroy();

    virtual unsigned int GetNumSupportedMimeTypes() const;
    virtual const char* GetSupportedMimeType(unsigned int uiIdx) const;
    virtual const char* GetOutputMimeType() const;

    unsigned int GetTextFileCount();
    const char* GetFirstTextFile();
    const char* GetNextTextFile();

protected:
    void FindAllNSFTextFiles(const char* pcDirectory, 
        bool bRecurseDirectories);

    unsigned int LoadAllNSFFilesInDirectory(const char* pcDirectory, 
        const char* pcExt, bool bRecurseDirectories,
        NiTPointerList<char*>* pkFileList);
    bool ProcessNSFFile(const char* pcFilename, const char* pcExt,
        NiTPointerList<char*>* pkFileList);
    
    unsigned int LoadAllNSFTextFiles(
        NiTObjectArray<NiFixedString>* pkFileNames = NULL);

    static NSFLoaderPtr ms_spLoader;

    NiTListIterator m_kNSFTextIter;
    NiTPointerList<char*> m_kNSFTextList;
};

#endif  //NSFLOADER_H
