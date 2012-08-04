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

#include "NiTerrainComponentPCH.h"
#include "NiTerrainComponentSDM.h"

#include <NiTerrainSector.h>
#include "NiTerrainComponent.h"
#include "NiTerrainSectorComponent.h"
#include "NiWaterComponent.h"
#include <NiSurface.h>
#include <NiTerrain.h>

#include <NiFactories.h>

NiImplementSDMConstructor(NiTerrainComponent, "NiMesh NiFloodgate NiMain NiEntity NiTerrain");

#ifdef  NITERRAINCOMPONENT_EXPORT
NiImplementDllMain(NiTerrainComponent);
#endif

NiFactoryDeclarePropIntf(NiTerrainComponent);
NiFactoryDeclarePropIntf(NiTerrainSectorComponent);
NiFactoryDeclarePropIntf(NiWaterComponent);

//---------------------------------------------------------------------------
void NiTerrainComponentSDM::Init()
{
    NiImplementSDMInitCheck();

    // Register to GameBryo factory system:
    NiFactoryRegisterPropIntf(NiTerrainComponent);
    NiFactoryRegisterPropIntf(NiTerrainSectorComponent);
    NiFactoryRegisterPropIntf(NiWaterComponent);

    NiTerrainComponent::_SDMInit();
    NiTerrainSectorComponent::_SDMInit();
    NiWaterComponent::_SDMInit();   
}
//---------------------------------------------------------------------------
void NiTerrainComponentSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiTerrainComponent::_SDMShutdown();
    NiTerrainSectorComponent::_SDMShutdown();
    NiWaterComponent::_SDMShutdown();   
}
//---------------------------------------------------------------------------
