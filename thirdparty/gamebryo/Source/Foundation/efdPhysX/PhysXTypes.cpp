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

// Precompiled Header
#include "efdPhysXPCH.h"

#include "PhysXTypes.h"

const NxVec3 efdPhysX::PhysXTypes::ms_kNxVec3Zero(0.0f, 0.0f, 0.0f);
const NxMat33 efdPhysX::PhysXTypes::ms_kNxMat33Identity(
    NxVec3(1.f, 0.f, 0.f), NxVec3(0.f, 1.f, 0.f), NxVec3(0.f, 0.f, 1.f));
const NxMat34 efdPhysX::PhysXTypes::ms_kNxMat34Identity(
    efdPhysX::PhysXTypes::ms_kNxMat33Identity, efdPhysX::PhysXTypes::ms_kNxVec3Zero);

const NxPhysicsSDKDesc efdPhysX::PhysXTypes::ms_kDefaultPhysicsSDKDesc;

