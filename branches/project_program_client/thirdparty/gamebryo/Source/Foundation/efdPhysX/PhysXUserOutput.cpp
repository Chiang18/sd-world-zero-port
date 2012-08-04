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

#include "efdPhysXPCH.h"

#include <efd/ILogger.h>
#include <efd/StringUtilities.h>
#include <efd/efdLogIDs.h>

#include "PhysXUserOutput.h"

using namespace efd;
using namespace efdPhysX;

//-------------------------------------------------------------------------------------------------
void PhysXUserOutput::reportError(NxErrorCode code, const char *message,
    const char *file, int line)
{
    const char* pErrorText = "";

    switch (code)
    {
    case NXE_NO_ERROR:
        pErrorText = "No Error";
        break;
    case NXE_INVALID_PARAMETER:
        pErrorText = "Invalid Parameter";
        break;
    case NXE_INVALID_OPERATION:
        pErrorText = "Invalid Operation";
        break;
    case NXE_OUT_OF_MEMORY:
        pErrorText = "Out of Memory";
        break;
    case NXE_INTERNAL_ERROR:
        pErrorText = "Internal Error";
        break;
    case NXE_ASSERTION:
        pErrorText = "Assertion";
        break;
    case NXE_DB_INFO:
        pErrorText = "Debug Info";
        break;
    case NXE_DB_WARNING:
        pErrorText = "Debug Warning";
        break;
    case NXE_DB_PRINT:
        pErrorText = "Debug Print";
        break;
    }

    EE_LOG_AT(efd::kPhysicsSystem, efd::ILogger::kERR1,
        ("PhysX Error: %s: %s", pErrorText, message),
        file, line);
    EE_UNUSED_ARG( message );
    EE_UNUSED_ARG( file );
    EE_UNUSED_ARG( line );
}

//-------------------------------------------------------------------------------------------------
NxAssertResponse PhysXUserOutput::reportAssertViolation (const char *message,
    const char *file, int line)
{
    EE_FAIL_MESSAGE_AT(("PhysX Assert Violation: %s", message), file, line, __FUNCTION__);
    EE_UNUSED_ARG( message );
    EE_UNUSED_ARG( file );
    EE_UNUSED_ARG( line );

    return NX_AR_CONTINUE;
}

//-------------------------------------------------------------------------------------------------
void PhysXUserOutput::print(const char *message)
{
    EE_LOG(efd::kPhysicsSystem, efd::ILogger::kLVL3, ("%s", message));
    EE_UNUSED_ARG( message );
}

//-------------------------------------------------------------------------------------------------
