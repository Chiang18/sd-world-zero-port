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
#ifndef NIMESHPARTICLESYSTEM_H
#define NIMESHPARTICLESYSTEM_H
#include "NiParticleSystem.h"
/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSMeshParticleSystem.
*/
class NIPARTICLE_ENTRY NiMeshParticleSystem : public NiParticleSystem
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    // For streaming only.
    NiMeshParticleSystem();
};

NiSmartPointer(NiMeshParticleSystem);

/// @endcond

#endif  // #ifndef NIMESHPARTICLESYSTEM_H
