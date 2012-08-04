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

#pragma once
#ifndef NSBSHADERLIB_H
#define NSBSHADERLIB_H

#include "NSBShaderLibLibType.h"
#include <NiShaderLibrary.h>

bool NSBShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* pszDirectories[], bool bRecurseSubFolders,
    NiShaderLibrary** ppkLibrary);

#if !defined(NSBSHADERLIB_IMPORT) && !defined(NSBSHADERLIB_EXPORT)
#include "NSBShaderLibSDM.h"
static NSBShaderLibSDM NSBShaderLibSDMObject;
#endif

#endif  //#ifndef NSBSHADERLIB_H
