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
#ifndef EE_SYSTEMFRAMEWORKENUMS_H
#define EE_SYSTEMFRAMEWORKENUMS_H

#include <efd/ClassInfo.h>

namespace efd
{
/// @name Constants and definitions from SystemFrameworkEnums.h

//@{

// OnPreInit, is a synchronous state handler function implemented by the
// ISystemService components.  This function is called once, if a service
// returns a failure in the pre-init phase, the framework will be shutdown.
enum SyncResult
{
    SyncResult_Failure,
    SyncResult_Success,
};

// OnInit, OnTick, and OnShutdown are all async state handler functions implemented by the
// ISystemService components.  These functions are called repeatedly until each service
// returns a Complete result or until any service returns a failure result.
enum AsyncResult
{
    AsyncResult_Failure,
    AsyncResult_Pending,
    AsyncResult_Complete,
};


/*! System Service IDentification (SSID) number are used to identify and locate
* various system services at run-time.  By convention Gamebryo SSIDs start with
* F as in 0xFxxxxxxx
* You can use MAKEFOURCC under windows to create a number from characters and
* as long as you do not use character 240 or above for the last character it
* will be guaranteed not to collide with a Gamebryo service.
*/
typedef efd::UInt32 SSID;
const SSID INVALID_SSID = 0;


/// The invalid class ID value
const efd::ClassID INVALID_CLASSID = 0;

// This sentinel value is used to tell whether the net ID has been set yet.
const efd::UInt32 kNetID_Unassigned = 0;

/// All clients are initialized with the same fixed NetID since they start out offline.  If they 
/// connect to a ChannelManager or Gateway they will be assigned a valid net id.
const efd::UInt32 kNetID_Client = 1000;

/// An invalid netid used by categories to indicate a broadcast channel
const efd::UInt32 kNetID_Any = 999;

/// An invalid netid used by ISystemService categories to indicate a broadcast channel
const efd::UInt32 kNetID_ISystemService = 998;

//@}

} // end namespace efd

#endif // EE_SYSTEMFRAMEWORKENUMS_H
