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

#include "NiProgramRunner.h"
#include "NiProgramLauncher.h"

NiImplementRootRTTI(NiProgramRunner);

// ---------------------------------------------------------------------------------------------- 
NiProgramRunner::NiProgramRunner()
{
}
// ---------------------------------------------------------------------------------------------- 
NiProgramRunner::~NiProgramRunner()
{
}
// ---------------------------------------------------------------------------------------------- 
NiString& NiProgramRunner::GetCommandLine()
{
    return m_kCommandLine;
}
// ---------------------------------------------------------------------------------------------- 
void NiProgramRunner::AddFilePathMapping(const char* pcLocal, const char* pcRemote)
{
    m_kLocalFilePathSet.Add(pcLocal);
    m_kRemoteFilePathSet.Add(pcRemote);
}
// ---------------------------------------------------------------------------------------------- 
void NiProgramRunner::SetOutputWindow(bool bEnabled)
{
    if (bEnabled)
    {
        m_kOutputWindow.Open();
    }
    else
    {
        m_kOutputWindow.Close();
    }
}
// ---------------------------------------------------------------------------------------------- 
bool NiProgramRunner::GetOutputWindow()
{
    return m_kOutputWindow.IsOpen();
}
// ---------------------------------------------------------------------------------------------- 
int NiProgramRunner::DisplayMsg(const char* pcFormat, ...)
{
    NIASSERT(pcFormat);
    if (!m_kOutputWindow.IsOpen())
    {
        return -1;
    }

    va_list kArgs;
    va_start(kArgs, pcFormat);
    int iRet = m_kOutputWindow.VDisplayMsg(pcFormat, kArgs);
    va_end(kArgs);

    return iRet;
}
//--------------------------------------------------------------------------------------------------
NiString NiProgramRunner::GetEnvVariable(const char* pcName)
{
    char acValue[NI_MAX_PATH*2];
    memset(acValue, 0x00, sizeof(acValue));

    size_t stComspecLen;
    NiGetenv(&stComspecLen, acValue, sizeof(acValue), pcName);

    return NiString(acValue);
}
//--------------------------------------------------------------------------------------------------
bool NiProgramRunner::SplitFilePath(const char* pcFilePath, NiString& kPath, 
    NiString& kFilename, char cPathSeparator /* = '\\' */)
{
    kPath = "";
    kFilename = "";

    NiString kFullPath(pcFilePath);

    unsigned int uiPos = kFullPath.FindReverse(cPathSeparator);
    if (uiPos !=  NiString::INVALID_INDEX)
    {        
        kPath = kFullPath.GetSubstring(0, uiPos);
        // Exclude the trailing backslash
        kFilename = kFullPath.GetSubstring(uiPos+1, kFullPath.Length());
        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiProgramRunner::SetOutputWindowTitle(const char* pcTitle)
{
    return m_kOutputWindow.SetWindowTitle(pcTitle);
}
//--------------------------------------------------------------------------------------------------