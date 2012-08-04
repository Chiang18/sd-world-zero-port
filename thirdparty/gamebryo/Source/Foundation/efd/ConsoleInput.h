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
#ifndef CONSOLEINPUT_H
#define CONSOLEINPUT_H

#include <efd/ThreadFunctor.h>
#include <efd/MessageService.h>

namespace efd
{

class EE_EFD_ENTRY ConsoleInput : public efd::ThreadFunctor, public ISystemService
{
    EE_DECLARE_CLASS1(ConsoleInput, kCLASSID_ConsoleInput, ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT;
public:

    ConsoleInput();
    virtual ~ConsoleInput();

    // Overridden virtual functions inherit base documentation and thus
    // are not documented here.
    virtual efd::UInt32 Execute(efd::Thread* pArg);
    virtual void ReadInput();

    virtual AsyncResult OnInit();
    virtual AsyncResult OnTick();
    virtual AsyncResult OnShutdown();

    const char* GetDisplayName() const
    {
        return "ConsoleInput";
    }

protected:
    efd::SmartPointer<MessageService> m_spMessageService;
    bool m_shutdown;
    bool m_finished;
};


/*! \typedef ConsoleInputPtr
 * A smart pointer (reference counting, self deleting pointer) for the
 * ConsoleInput class
 */
typedef efd::SmartPointer<ConsoleInput> ConsoleInputPtr;

} // end namespace efd

#endif // CONSOLEINPUT_H
