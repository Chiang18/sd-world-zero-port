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

#include "NiWinProgramRunner.h"
#include "NiFilename.h"

//--------------------------------------------------------------------------------------------------
NiWinProgramRunner::NiWinProgramRunner()
{
    memset(&m_kProcessInformation, 0x00, sizeof(PROCESS_INFORMATION));
}
//--------------------------------------------------------------------------------------------------
NiWinProgramRunner::~NiWinProgramRunner()
{
}
//--------------------------------------------------------------------------------------------------
bool NiWinProgramRunner::Start(const char* pcTargetName, const char* pcExecutableFilename)
{
    if (pcTargetName != NULL && strcmp(pcTargetName,"[Local]") !=0 )
    {
        DisplayMsg("ERROR: Unknown target: %s", pcTargetName);
        return false;
    }

    DisplayMsg("Running %s on %s", pcExecutableFilename, pcTargetName);

    STARTUPINFO kStartupInfo;
    ZeroMemory(&kStartupInfo, sizeof(kStartupInfo));
    kStartupInfo.cb  = sizeof(kStartupInfo);
    kStartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    kStartupInfo.wShowWindow = SW_SHOWNORMAL;
    kStartupInfo.hStdOutput = m_kOutputWindow.GetWriteHandle();
    kStartupInfo.hStdError = m_kOutputWindow.GetWriteHandle();

    ZeroMemory(&m_kProcessInformation, sizeof(m_kProcessInformation));

    // Set default working directory to the location of the executable
    NiString kCurrentDirectory;
    NiString kFilename;
    SplitFilePath(pcExecutableFilename, kCurrentDirectory, kFilename);

    BOOL bCreateSuccess = CreateProcess(pcExecutableFilename, (LPSTR)(const char*)GetCommandLine(), NULL,
        NULL, FALSE, 0, NULL, kCurrentDirectory, &kStartupInfo, &m_kProcessInformation);

    if (!bCreateSuccess)
    {
        DisplayMsg("ERROR: %s. Unable to create Windows process.", 
            (const char*)NiOutputWindow::GetWindowsLastError());
    }

    return (bCreateSuccess == TRUE);
}
//--------------------------------------------------------------------------------------------------
