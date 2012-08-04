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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NSFParserLibPCH.h"

#include "NSFParserLibSDM.h"
#include "NSFLoader.h"

//---------------------------------------------------------------------------
#if defined(WIN32)
NiImplementSDMConstructor(NSFParserLib, "NiMain NiMesh NiFloodgate \
    NiDX9Renderer NiD3D10Renderer");
#elif defined(_PS3)
NiImplementSDMConstructor(NSFParserLib, "NiMain NiMesh NiFloodgate \
    NiPS3Renderer");
#elif defined(_XENON)
NiImplementSDMConstructor(NSFParserLib, "NiMain NiMesh NiFloodgate \
    NiXenonRenderer");
#endif

//---------------------------------------------------------------------------
void NSFParserLibSDM::Init()
{
    NiImplementSDMInitCheck();
    NiShaderParser::AddParserCallback("NSFParserLib",
        &NSFLoader::Create);
}
//---------------------------------------------------------------------------
void NSFParserLibSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiShaderParser::RemoveParserCallback(&NSFLoader::Create);
    NSFLoader::Destroy();
}
//---------------------------------------------------------------------------
