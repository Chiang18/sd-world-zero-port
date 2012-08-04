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

#include <efd/Win32/Win32File.h>
#include <efd/efdLogIDs.h>
#include <efd/ILogger.h>

namespace efd
{

//-------------------------------------------------------------------------------------------------
File* File::DefaultFileCreateFunc(const char *pcName, OpenMode eMode, 
    unsigned int uiSize, bool flushOnWrite)
{
    File* pkFile = EE_NEW Win32File(pcName, eMode, uiSize, flushOnWrite);

    if (pkFile->IsGood())
    {
        return pkFile;
    }
    else
    {
        EE_DELETE pkFile;
        return NULL;
    }
}

//-------------------------------------------------------------------------------------------------
bool File::DefaultFileAccessFunc(const char *pcName, OpenMode eMode)
{
    Win32File f(pcName, eMode, 0);
    return f.IsGood();
}

//---------------------------------------------------------------------------
bool File::DefaultCreateDirectoryFunc(const char* pcDirName)
{
    bool bCreateDir = ::CreateDirectory(pcDirName, NULL) != 0;

#ifdef EE_EFD_CONFIG_DEBUG
    if (bCreateDir == false)
    {
        DWORD dwLastError = ::GetLastError();
        
        EE_LOG(efd::kFile, ILogger::kLVL3,
            ("Create Dir Failed:\n\tDirectory: '%s'", pcDirName));

        char acString[1024];
        EE_VERIFY(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
            dwLastError, 0, acString, 1024, NULL));
        EE_LOG(efd::kFile, ILogger::kLVL3,
            ("\tErrorCode %d\tTranslation: %s", dwLastError, acString));
    }
#endif

    return bCreateDir;
}
//---------------------------------------------------------------------------
bool File::DefaultDirectoryExistsFunc(const char* pcDirName)
{
   DWORD dwAttrib = GetFileAttributes(pcDirName);
   if (dwAttrib == -1)
       return false;

   return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
//---------------------------------------------------------------------------

} // end namespace efd
