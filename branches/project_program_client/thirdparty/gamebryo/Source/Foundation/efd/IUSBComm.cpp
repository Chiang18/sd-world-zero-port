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
#include "efdPCH.h"
#include <efd/IUSBComm.h>

// The Wii uses weak linking, so if no lib is linked in before this lib these symbols will be used
#if defined(EE_PLATFORM_WII)
extern "C"
{
    void InitUSB()
    {
    }

    void RegisterNetLib()
    {
    }

    void ShutdownNetLib()
    {
    }
};
#endif //defined(EE_PLATFORM_WII)

using namespace efd;

efd::SmartPointer<IUSBComm> IUSBComm::m_spIUSBComm = NULL;
IUSBComm* IUSBComm::m_pIUSBComm = NULL;

IUSBComm::IUSBComm()
    : m_pUSBInterruptCallback(NULL)
{

}

IUSBComm::~IUSBComm()
{
    m_pIUSBComm = NULL;
}
