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

#include "NiWinProgramRunnerFactory.h"

//--------------------------------------------------------------------------------------------------
NiWinProgramRunnerFactory::~NiWinProgramRunnerFactory()
{
}
//--------------------------------------------------------------------------------------------------
NiSystemDesc::PlatformID NiWinProgramRunnerFactory::GetPlatformID()
{
    return NiSystemDesc::NI_WIN32;
}
//--------------------------------------------------------------------------------------------------
const char* NiWinProgramRunnerFactory::GetPlatformDisplayName()
{
    return "PC";
}
//--------------------------------------------------------------------------------------------------
const char* NiWinProgramRunnerFactory::GetPlatformInternalName()
{
    return "Win32";
}
//--------------------------------------------------------------------------------------------------
const char* NiWinProgramRunnerFactory::GetPlatformExecutableFileExt()
{
    return "exe";
}
//--------------------------------------------------------------------------------------------------
bool NiWinProgramRunnerFactory::GetAvailableTargetNames(NiStringArray& kTargetNames, 
    NiString* /* pkErrorInfo = NULL */)
{
    kTargetNames.Add(NI_PROGRAMRUNNER_LOCAL_TARGET_NAME);
    return true;
}
//--------------------------------------------------------------------------------------------------
 NiProgramRunner* NiWinProgramRunnerFactory::Create()
{
    return NiNew NiWinProgramRunner;
}
//--------------------------------------------------------------------------------------------------
