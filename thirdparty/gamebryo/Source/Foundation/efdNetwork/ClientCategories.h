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

#pragma once
#ifndef EE_CLIENTCATEGORIES_H
#define EE_CLIENTCATEGORIES_H

#include "NetCategory.h"
#include <efd/efdBaseIDs.h>

namespace efd
{

/*!
    Clients communicate to the gateway using a set of constant categories.  The gateway
    knows how to map these to a specific server category that is then used on the back-end.
    For the most part the client should only directly communicate to a very small number
    of categories.
 */

/// The client talks directly to the gateway for communication regarding connection, login,
/// logout, and disconnection.
static const efd::Category kCAT_MyGateway =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_MyGateway);

/// All other client commands typically go to the client's controller.  The controller
/// contains game specific logic that can understand client commands, validate them as
/// valid game commands, and either process those commands to pass them on to other systems
/// that process those commands.
static const efd::Category kCAT_MyController =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_MyController);

/// Talk to the ServerLogin service.
static const efd::Category kCAT_MyServer =
    Category( UniversalID::ECU_Any, kNetID_Any, kBASEID_MyServer);


} // end namespace efd


#endif // EE_CLIENTCATEGORIES_H
