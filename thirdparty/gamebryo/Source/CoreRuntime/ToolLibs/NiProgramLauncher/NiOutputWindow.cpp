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

#include "NiProgramLauncherRes.h"
#include "NiOutputWindow.h"
#include "NiProgramLauncher.h"


//--------------------------------------------------------------------------------------------------
NiOutputWindow::NiOutputWindow()
{
    m_hPipeRead = INVALID_HANDLE_VALUE;
    m_hPipeWrite = INVALID_HANDLE_VALUE;

    m_hWndDlg = NULL;
}
//--------------------------------------------------------------------------------------------------
NiOutputWindow::~NiOutputWindow()
{
    Close();
}
//--------------------------------------------------------------------------------------------------
bool NiOutputWindow::Open()
{
    if (IsOpen())
    {
        return true;
    }

    BOOL bWinApiResult = FALSE;

    SECURITY_ATTRIBUTES kSecurityAttributes;
    ZeroMemory(&kSecurityAttributes, sizeof(kSecurityAttributes));
    kSecurityAttributes.nLength = sizeof(kSecurityAttributes);
    kSecurityAttributes.bInheritHandle = TRUE;
    kSecurityAttributes.lpSecurityDescriptor = NULL;

    bWinApiResult = CreatePipe(&m_hPipeRead, &m_hPipeWrite, &kSecurityAttributes, 0);
    if (!bWinApiResult)
    {
        return false;
    }

    m_hWndDlg = CreateDialogParam(NiProgramLauncher::GetDllInstance(), 
        MAKEINTRESOURCE(IDD_OUTPUT_WINDOW), NULL, DialogProcedure, 
        reinterpret_cast<LPARAM>(this));
    if (m_hWndDlg)
    {
        return (ShowWindow(m_hWndDlg, SW_SHOW) == TRUE);
    }
    else
    {
        NiString kErrorMsg;
        kErrorMsg.Format("%s. Unable to open output window.", (const char*)GetWindowsLastError());
        MessageBox(NULL, (const char*)kErrorMsg, "Gamebryo Tool", MB_OK|MB_ICONERROR);
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
void NiOutputWindow::Close()
{
    if (m_hWndDlg)
    {
        DestroyWindow(m_hWndDlg);
        m_hWndDlg = NULL;
    }

    CloseHandle(m_hPipeRead);
    m_hPipeRead = INVALID_HANDLE_VALUE;

    CloseHandle(m_hPipeWrite);
    m_hPipeWrite = INVALID_HANDLE_VALUE;
}
//--------------------------------------------------------------------------------------------------
int NiOutputWindow::DisplayMsg(const char* pcFormat, ...)
{
    NIASSERT(pcFormat);
    if (m_hPipeWrite == INVALID_HANDLE_VALUE || !pcFormat)
    {
        return 0;
    }

    va_list kArgs;
    va_start(kArgs, pcFormat);
    int iLen = VDisplayMsg(pcFormat, kArgs);
    va_end(kArgs);

    return iLen;
}
//--------------------------------------------------------------------------------------------------
int NiOutputWindow::VDisplayMsg(const char* pcFormat, va_list kArgs)
{
    NIASSERT(pcFormat);
    if (m_hPipeWrite == INVALID_HANDLE_VALUE || !pcFormat)
    {
        return 0;
    }

    char* pcMsg = NULL;
    NiUInt32 uiMsgBufferSize = 0;

    // MS-only function to determine the number of characters in the string.
    int iMsgLen = _vscprintf(pcFormat, kArgs);
    // If the format fails, use format string directly (no substitution)
    if (iMsgLen < 0)
    {
        uiMsgBufferSize = static_cast<NiUInt32>(strlen(pcFormat) + 1);
        pcMsg = NiAlloc(char, uiMsgBufferSize);
        NiStrcpy(pcMsg, uiMsgBufferSize, pcFormat);
        iMsgLen = uiMsgBufferSize - 1;
    }
    else
    {
        uiMsgBufferSize = iMsgLen + 1;
        pcMsg = NiAlloc(char, uiMsgBufferSize);
        iMsgLen = NiVsprintf(pcMsg, uiMsgBufferSize, pcFormat, kArgs);
    }

    // Did format succeed?
    if (iMsgLen > 0)
    {
        NiUInt32 uiMsgLen = static_cast<unsigned int>(iMsgLen);

        // Win32 Edit Box control requires  '\r\n' for return characters
        for (NiUInt32 uiEnd = 0; uiEnd < uiMsgLen; ++uiEnd)
        {
            NiUInt32 uiStart = uiEnd;
            while(uiEnd < uiMsgLen  && pcMsg[uiEnd] != '\0' && pcMsg[uiEnd] != '\n')
            {
                ++uiEnd;
            }

            DWORD dwNumberOfBytesWritten = 0;    
            WriteFile(m_hPipeWrite, &pcMsg[uiStart], (uiEnd-uiStart), 
                &dwNumberOfBytesWritten, NULL);
            // Insert a new line since we've hit '\n' or the end of the string
            WriteFile(m_hPipeWrite, "\r\n", 2, &dwNumberOfBytesWritten, NULL);
        }
    }

    NiFree(pcMsg);

    Update();

    return iMsgLen;
}
//--------------------------------------------------------------------------------------------------
 bool NiOutputWindow::ReadPipe(char* pcBuf, size_t stBufSize, size_t& stBytesRead)
{
    NIASSERT(pcBuf);

    stBytesRead = 0;

    if (m_hPipeRead == INVALID_HANDLE_VALUE || !pcBuf)
    {
        return false;
    }

    DWORD dwNumBytesPeeked = 0;
    DWORD dwTotalBytesAvailable;
    DWORD dwBytesLeftThisMessage;

    BOOL bWinApiResult;
    bWinApiResult = PeekNamedPipe(m_hPipeRead, pcBuf, 1, &dwNumBytesPeeked,
        &dwTotalBytesAvailable, &dwBytesLeftThisMessage);

    if (dwNumBytesPeeked > 0)
    {
        DWORD dwNumBytesRead = 0;
        bWinApiResult = ReadFile(m_hPipeRead, pcBuf, (DWORD)(stBufSize-1),
            &dwNumBytesRead, NULL);

        if (dwNumBytesRead > 0)
        {
            pcBuf[dwNumBytesRead] = '\0';
            stBytesRead = dwNumBytesRead;
        }
    }    

    return (bWinApiResult == TRUE);
}
//--------------------------------------------------------------------------------------------------
 void NiOutputWindow::Update()
{
    HWND hEdit = GetDlgItem (m_hWndDlg, IDC_OUTPUT_EDIT_BOX);
    if (!hEdit)
    {
        return;
    }

    int iTextLength = GetWindowTextLength(hEdit);
    SetFocus(hEdit);

    char acBuf[1024];
    size_t stBytesRead = 0;
    ReadPipe(acBuf, sizeof(acBuf), stBytesRead);
    while(stBytesRead > 0)
    {
        SendMessage(hEdit, EM_SETSEL, (WPARAM)iTextLength, (LPARAM)iTextLength);  
        SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)acBuf);

        //SendDlgItemMessage(m_hWndDlg, IDC_OUTPUT_EDIT_BOX, EM_SETSEL,-1,(LPARAM)-1);
        //SendDlgItemMessage(m_hWndDlg, IDC_OUTPUT_EDIT_BOX, EM_REPLACESEL, 0, (WPARAM)acBuf);
        ReadPipe(acBuf, sizeof(acBuf), stBytesRead);
    }
}
//--------------------------------------------------------------------------------------------------
BOOL CALLBACK NiOutputWindow::DialogProcedure(HWND hDlg, UINT uMsg, WPARAM /*wParam*/, 
    LPARAM lParam)
{
    bool bResult = false;

    // GetWindowLongPtr() has a known problem on 32-bit Windows of giving a false warning.
    // Disable the warning.
    // See http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=99443
#pragma warning(push)
#pragma warning(disable : 4312)
    // Get specific instance this function is being called for
    NiOutputWindow* pkOutputWindow = NULL;
    if (uMsg == WM_INITDIALOG)
    {
        pkOutputWindow = reinterpret_cast<NiOutputWindow*>(GetWindowLongPtr(hDlg, 
            static_cast<int>(lParam)));
    }
    else
    {
        pkOutputWindow = reinterpret_cast<NiOutputWindow*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
    }
#pragma warning(pop)

    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            // Store pointer to NiOutputWindow object in the Windows HWND
            SetWindowLongPtr(hDlg, GWLP_USERDATA, static_cast<int>(lParam));            
            pkOutputWindow->ResizeTextControl(hDlg);
            InvalidateRect(hDlg, 0, true);
            bResult = true;
        }
        break;
    case WM_CLOSE:
        pkOutputWindow->Close();
        bResult = true;
        break;
    case WM_PAINT:        
        break;
    case WM_SIZE:
        pkOutputWindow->ResizeTextControl(hDlg);
        bResult = true;
        break;
    }

    return bResult;
}
//--------------------------------------------------------------------------------------------------
void NiOutputWindow::ResizeTextControl(HWND hDlg)
{
    RECT kClient; 
    GetClientRect(hDlg, &kClient);
    HWND hEditBox = GetDlgItem(hDlg, IDC_OUTPUT_EDIT_BOX);
    MoveWindow(hEditBox, 0, 0, kClient.right, kClient.bottom, TRUE); 
}
//--------------------------------------------------------------------------------------------------
NiString NiOutputWindow::GetWindowsLastError()
{
    char* pcMessage;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&pcMessage,
        0, NULL );

    NiString kErrorMsg = pcMessage;

    LocalFree(pcMessage);

    return kErrorMsg;
}
//--------------------------------------------------------------------------------------------------
bool NiOutputWindow::SetWindowTitle(const char* pcTitle)
{
    if (m_hWndDlg)
    {
        return (SetWindowText(m_hWndDlg, pcTitle) == TRUE);
    }

    return false;
}