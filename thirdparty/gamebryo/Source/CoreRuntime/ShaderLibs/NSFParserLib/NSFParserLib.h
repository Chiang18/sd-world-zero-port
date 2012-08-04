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
#ifndef NSFPARSERLIB_H
#define NSFPARSERLIB_H

#include "NSFParserLibLibType.h"

unsigned int NSFParserLib_RunShaderParser(const char* pszDirectory, 
    bool bRecurseSubFolders);

#if !defined(NSFPARSERLIB_IMPORT) && !defined(NSFPARSERLIB_EXPORT)
#include "NSFParserLibSDM.h"
static NSFParserLibSDM NSFParserLibSDMObject;
#endif

#endif  //NSFPARSERLIB_H
