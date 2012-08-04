/******************************************************************************
 *
 * EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with Emergent Game Technologies and may not
 * be copied or disclosed except in accordance with the terms of that
 * agreement.
 *
 *      Copyright (c) 1996-2009 Emergent Game Technologies.
 *      All Rights Reserved.
 *
 * Emergent Game Technologies, Calabasas, CA 91302
 * http://www.emergent.net
 *
\******************************************************************************/

#pragma once
#ifndef EE_Win32PlatformServiceService_h
#define EE_Win32PlatformServiceService_h

#include <efd/OS.h>

#include <efd/ISystemService.h>
#include <efd/ServiceManager.h>


namespace efd
{
    class EE_EFD_ENTRY Win32PlatformService : public efd::ISystemService
    {
        EE_DECLARE_CLASS1(Win32PlatformService, efd::kCLASSID_Win32PlatformService, ISystemService);
        EE_DECLARE_CONCRETE_REFCOUNT;

    public:

        Win32PlatformService(
            HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPTSTR lpCmdLine,
            efd::SInt32 nCmdShow = SW_SHOWNORMAL);

        virtual ~Win32PlatformService();

        /// @name Configuration options
        /// These functions are only useful to call prior to PreInit.  These values are used
        /// during the default RegisterClass/InitInstance functions.
        //@{

        /// Sets the initial window title from a string.
        void SetWindowTitle(const efd::utf8string& strTitle);
        /// Sets the initial window title from a resource string.
        void SetWindowTitle(efd::UInt32 titleResourceID);

        /// Sets the window class to use when creating a window.
        void SetWindowClass(const efd::utf8string& strClass);
        /// Sets the window class to use when creating a window.
        void SetWindowClass(efd::UInt32 classResourceID);

        /// Sets the large (and optionally small) icons for the created window.
        void SetWindowIcon(efd::UInt32 largeIconResourceID, efd::UInt32 smallIconResourceID = 0);

        /// Sets the initial window width.
        void SetWindowWidth(efd::UInt32 width);
        /// Sets the initial window height.
        void SetWindowHeight(efd::UInt32 height);

        /// Sets the WndProc function to filter messages.
        void SetWndProc(WNDPROC pFunc);
        //@}

        /// @name Window accessor functions
        //@{
        /// Return the HINSTANCE for this window.
        efd::InstanceRef GetInstanceRef() const;

        /// Returns the HWND for this window.
        efd::WindowRef GetWindowRef() const;
        //@}

    protected:
        /// Creates the window.
        virtual BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

        /// Registers the window class to be created in InitInstance.
        virtual ATOM RegisterClass(HINSTANCE hInstance);

        /// Sets up window instance.
        virtual efd::SyncResult OnPreInit();

        /// Default message pump.  Quits on quit message.
        virtual efd::AsyncResult OnTick();
        
        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual const char* GetDisplayName() const;

        /// Default WndProc function.  Quits on escape button.
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        efd::InstanceRef m_hInstance;
        efd::InstanceRef m_hPrevInstance;
        LPTSTR m_lpCmdLine;
        efd::SInt32 m_nCmdShow;
        efd::WindowRef m_hWnd;

        efd::utf8string m_strWindowTitle;
        efd::utf8string m_strWindowClass;

        efd::UInt32 m_idWindowMenu;
        efd::UInt32 m_idLargeIcon;
        efd::UInt32 m_idSmallIcon;

        efd::UInt32 m_windowWidth;
        efd::UInt32 m_windowHeight;
        WNDPROC m_pWndProc;
    };

    typedef efd::SmartPointer<Win32PlatformService> Win32PlatformServicePtr;

} // end namespace efd

#endif // EE_Win32PlatformServiceService_h
