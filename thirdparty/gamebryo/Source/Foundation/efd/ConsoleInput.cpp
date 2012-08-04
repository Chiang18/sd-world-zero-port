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

#include "efdPCH.h"

#include <efd/ConsoleInput.h>
#include <efd/Thread.h>
#include <efd/MessageService.h>
#include <efd/StreamMessage.h>
#include <efd/Metrics.h>
#include <efd/OS.h>
#include <efd/IDs.h>
#include <efd/utf8char.h>
#include <efd/StreamMessage.h>
#include <efd/ServiceManager.h>

using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO( ConsoleInput );

//-------------------------------------------------------------------------------------------------
ConsoleInput::ConsoleInput()
    : efd::ThreadFunctor()
    , ISystemService()
    , m_spMessageService(NULL)
    , m_shutdown(false)
    , m_finished(true)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 4490;
}

//-------------------------------------------------------------------------------------------------
/*virtual*/ ConsoleInput::~ConsoleInput()
{

}

//-------------------------------------------------------------------------------------------------
#if !defined(EE_PLATFORM_LINUX) && !defined(EE_PLATFORM_PS3) && !defined(EE_PLATFORM_WII)
#include <conio.h>
#endif
/*virtual*/ efd::UInt32 ConsoleInput::Execute(efd::Thread* pArg)
{
    while (!m_shutdown && m_spMessageService)
    {
        ReadInput();
    }

    m_finished = true;
    EE_UNUSED_ARG(pArg);
    return 0;
}

/*virtual*/ void ConsoleInput::ReadInput()
{
    char keyPressed[8] = {0};
#if defined(EE_PLATFORM_LINUX)
    int i;
    unsigned char ch;
    struct timeval tv;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    tv.tv_sec = tv.tv_usec = 0;
    i = select(1, &fds, NULL, NULL, &tv);
    if (i != -1)
    {
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            read(STDIN_FILENO, &ch, 1);
            keyPressed[0] = ch;
        }
    }
#elif defined(EE_PLATFORM_WIN32)
    if (_kbhit())
        keyPressed[0] = static_cast<char>(_getch());
#elif defined(EE_PLATFORM_XBOX360)
    // do nothing
#elif defined(EE_PLATFORM_PS3)
    // do nothing
#endif

    if (keyPressed[0] != 0)
    {
        StreamMessagePtr spMsg = EE_NEW MessageWrapper< StreamMessage, kMSGID_ConsoleInput >;
        EE_ASSERT(spMsg);
        if( efd::utf8_validate_char(keyPressed,1) )
        {
            efd::utf8string utf8String(keyPressed);
            *spMsg << utf8String;
            m_spMessageService->SendLocal( spMsg, kCAT_LocalMessage );
        }
    }
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ConsoleInput::OnInit()
{
    m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ConsoleInput::OnTick()
{
    ReadInput();

    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ConsoleInput::OnShutdown()
{
    m_shutdown = true;

    if (!m_finished)
    {
        return efd::AsyncResult_Pending;
    }

    m_spMessageService = NULL;

    return efd::AsyncResult_Complete;
}


