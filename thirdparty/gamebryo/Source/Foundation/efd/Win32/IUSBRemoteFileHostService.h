// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef EE_IUSBREMOTEFILEHOSTSERVICE_H
#define EE_IUSBREMOTEFILEHOSTSERVICE_H

#include <efd/ISystemService.h>
#include <efd/DynamicModule.h>

namespace efd
{

class EE_EFD_ENTRY IUSBRemoteFileHostService : public efd::ISystemService
{
    EE_DECLARE_ABSTRACT_REFCOUNT;

public:

    /**
        Constructor
    */
    IUSBRemoteFileHostService();

    /**
        Destructor
    */
    virtual ~IUSBRemoteFileHostService();

    /* Tries to create a Remote File Host Service based on the USB interface.
       This service depends on the efdNetLibWiiUSB library which is not always 
       available so this method can return NULL

       @return ISystemService* pointer to the System Service
    */
    static ISystemService* Instance();
    
protected:
    
    static DynamicModule m_Module;
    static ISystemService* m_pSystemService;
};

} // end namespace efd

#endif // EE_IUSBREMOTEFILEHOSTSERVICE_H
