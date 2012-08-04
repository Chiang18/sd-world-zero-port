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

#include "NiTerrainPCH.h"
#include "NiTerrainSDM.h"

#include "NiTerrainSector.h"
#include "NiSurface.h"
#include "NiTerrain.h"
#include "NiTerrainShadowVisitor.h"
#include "NiTerrainCullingProcess.h"
#include "NiTerrainShadowClickValidator.h"
#include "NiTerrainCullingProcess.h"

NiImplementSDMConstructor(NiTerrain, "NiMesh NiFloodgate NiMain");

#ifdef NITERRAIN_EXPORT
NiImplementDllMain(NiTerrain);
#endif

//---------------------------------------------------------------------------
void NiTerrainSDM::Init()
{
    NiImplementSDMInitCheck();

    // Register to GameBryo factory system:

    NiSurfacePalette::CreateInstance();
    NiMetaDataStore::CreateInstance();

    NiTerrain::_SDMInit();
    NiSurface::_SDMInit();

    // Register the NiTerrainShadowVisitor as the NiShadowVisitor to be used
    // by the shaodowing system. The NiTerrainShadowVisitor ensures that terrain LOD
    // handle correctly when rendering terrain to a shadow map.
    NiShadowManager::SetShadowVisitorFactory(
        NiTerrainShadowVisitor::CreateTerrainShadowVisitor);

    // Register the NiTerrainShadowClickValidator as the NiShadowClickValidator to be 
    // used by the shadowing system. The NiTerrainShadowClickValidator ensures that 
    // the terrain LOD is recomputed when validating a shadow render click.
    NiShadowManager::SetShadowRenderClickValidatorFactory(
        NiTerrainShadowClickValidator::CreateTerrainShadowClickValidator);

    // Register the NiTerrainShadowCullingProcess as the NiCullingProcess to be 
    // used by the shadowing system. The NiTerrainShadowCullingProcess ensures that
    // only one LOD will ever be rendered to a shadow map. This is especially important
    // for PSSM.
    NiShadowManager::SetShadowRenderCullingProcessFactory(
        NiTerrainCullingProcess::CreateTerrainCullingProcess);

}
//---------------------------------------------------------------------------
void NiTerrainSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiSurfacePalette::DestroyInstance();
    NiMetaDataStore::DestroyInstance();

    NiTerrain::_SDMShutdown();
    NiSurface::_SDMShutdown();
}
//---------------------------------------------------------------------------
