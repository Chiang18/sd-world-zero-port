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

// Precompiled Header
#include "NiPhysXParticlePCH.h"

#include <efdPhysX/PhysXSDKManager.h>

#include "NiPhysXPSMeshParticleSystem.h"
#include "NiPhysXParticle.h"

using namespace efdPhysX;

NiImplementRTTI(NiPhysXPSMeshParticleSystem, NiPSMeshParticleSystem);

//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem* NiPhysXPSMeshParticleSystem::Create(
    NiUInt32 uiMaxNumParticles, AlignMethod eNormalMethod,
    const NiPoint3& kNormalDirection,
    AlignMethod eUpMethod, const NiPoint3& kUpDirection,
    bool bHasLivingSpawner, bool bHasColors, bool bHasRotations, 
    bool bWorldSpace, bool bDynamicBounds, bool bAttachMeshModifiers, 
    NiUInt16 usMeshPoolSize, NiUInt16 usNumGenerations, 
    bool bMeshAutoFillPools, const float fScaleAmount, 
    const float fScaleRest, const float fScaleLimit,
    NiPhysXPSMeshParticleSystemProp* pkProp, 
    NiUInt32 uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations, 
    NiUInt32 uiActorPoolSize, bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
{
    // Create simulator.
    NiPhysXPSSimulator* pkSimulator = NiNew NiPhysXPSSimulator();

    // Add simulation steps.
    pkSimulator->AddStep(NiNew NiPhysXPSSimulatorInitialStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorGeneralStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorForcesStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorCollidersStep());
    pkSimulator->AddStep(NiNew NiPhysXPSSimulatorFinalStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorMeshAlignStep(fScaleAmount, fScaleRest, fScaleLimit));

    // Create bound updater, if requested.
    NiPSBoundUpdater* pkBoundUpdater = NULL;
    if (bDynamicBounds)
    {
        pkBoundUpdater = NiNew NiPSBoundUpdater();
    }

    // Create particle system.
    NiPhysXPSMeshParticleSystem* pkSystem = NiNew NiPhysXPSMeshParticleSystem(
        pkSimulator, uiMaxNumParticles, eNormalMethod, kNormalDirection,
        eUpMethod, kUpDirection, bHasLivingSpawner, bHasColors, bHasRotations, 
        bHasRotations, bWorldSpace, pkBoundUpdater, usMeshPoolSize, 
        usNumGenerations, bMeshAutoFillPools, pkProp, uiCompartmentID, 
        bPhysXSpace, bPhysXRotations, uiActorPoolSize, bFillActorPoolsOnLoad, 
        bKeepsMeshes);

    // Add simulator.
    pkSystem->AddModifier(pkSimulator, bAttachMeshModifiers);

    return pkSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem* NiPhysXPSMeshParticleSystem::Create(
    NiPSMeshParticleSystem* pkPSys, NiPhysXPSMeshParticleSystemProp* pkProp,
    NiUInt32 uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations, 
    NiUInt32 uiActorPoolSize, bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
{
    NiPhysXPSMeshParticleSystem* pkPhysXMeshPsys = 
        NiNew NiPhysXPSMeshParticleSystem(pkPSys, pkProp, uiCompartmentID, 
            bPhysXSpace, bPhysXRotations, uiActorPoolSize, 
            bFillActorPoolsOnLoad, bKeepsMeshes);

    return pkPhysXMeshPsys;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem::NiPhysXPSMeshParticleSystem(
    NiPSSimulator* pkSimulator, NiUInt32 uiMaxNumParticles, 
    NiPSParticleSystem::AlignMethod eNormalMethod, const NiPoint3& kNormalDirection,
    NiPSParticleSystem::AlignMethod eUpMethod, const NiPoint3& kUpDirection,
    bool bHasLivingSpawner, bool bHasColors, bool bHasRotations, bool bHasRotationAxes,
    bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater, NiUInt16 usMeshPoolSize, 
    NiUInt16 usNumGenerations, bool bMeshAutoFillPools, 
    NiPhysXPSMeshParticleSystemProp* pkProp, NiUInt32 uiCompartmentID, 
    bool bPhysXSpace, bool bPhysXRotations, NiUInt32 uiActorPoolSize, 
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes) : NiPSMeshParticleSystem(
    pkSimulator, uiMaxNumParticles, eNormalMethod, kNormalDirection, eUpMethod,
    kUpDirection, bHasLivingSpawner, bHasColors, bHasRotations, bHasRotationAxes,
    bWorldSpace, pkBoundUpdater, usMeshPoolSize, usNumGenerations, bMeshAutoFillPools),
    m_ppkActors(0), m_pkLastVelocities(0), m_pkActorPose(0), m_pkActorVelocity(0)
{
    SetOnConstruction(uiMaxNumParticles, usNumGenerations, pkProp,
        uiCompartmentID, uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes,
        bPhysXRotations, bPhysXSpace);
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem::NiPhysXPSMeshParticleSystem(
    NiPSMeshParticleSystem* pkPSys, NiPhysXPSMeshParticleSystemProp* pkProp, 
    NiUInt32 uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations, 
    NiUInt32 uiActorPoolSize, bool bFillActorPoolsOnLoad,  bool bKeepsMeshes) 
    : NiPSMeshParticleSystem(), m_ppkActors(0), m_pkLastVelocities(0),
    m_pkActorPose(0), m_pkActorVelocity(0)
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    ((NiPhysXPSMeshParticleSystem*)pkPSys)->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);
    
    // Cloning misses setting the target on the controllers attached to the
    // particle system object. Go through and set them now.
    NiTimeController* pkControllers = GetControllers();
    while (pkControllers)
    {
        pkControllers->SetTarget(this);
        pkControllers = pkControllers->GetNext();
    }
        
    NiPSSimulator* pkNewSimulator = NiNew NiPhysXPSSimulator();
    // First add the NiPhysXPSSimulatorInitialStep.
    pkNewSimulator->AddStep(NiNew NiPhysXPSSimulatorInitialStep());

    // Iterate over all the original steps and add them, but don't
    // add the NiPSSimulatorFinalStep, it will be replaced by a
    // NiPhysXPSSimulatorFinalStep.
    for (NiUInt32 ui = 0; ui < m_pkSimulator->GetStepCount(); ui++)
    {
        NiPSSimulatorStep* pkStep = m_pkSimulator->GetStepAt(ui);
        if (!NiDynamicCast(NiPSSimulatorFinalStep, pkStep))
        {
            pkNewSimulator->AddStep(pkStep);
        }
    }

    // Add the NiPhysXPSSimulatorFinalStep as the last step.
    pkNewSimulator->AddStep(NiNew NiPhysXPSSimulatorFinalStep());

    // Detach the current simulator from the particle system and 
    // attach the new simulator.
    RemoveModifier(m_pkSimulator);
    m_pkSimulator = pkNewSimulator;
    AddModifierAt(0, m_pkSimulator, true);

    // Set the stuff the clone didn't catch
    SetOnConstruction(m_uiMaxNumParticles, 
        NiPSMeshParticleSystem::GetNumGenerations(), pkProp, uiCompartmentID,
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes, bPhysXRotations,
        bPhysXSpace);
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem::NiPhysXPSMeshParticleSystem()
{
    m_uFlags = 0;

    m_ppkActors = 0;
    m_pkLastVelocities = 0;
    m_pkActorPose = 0;
    m_pkActorVelocity = 0;

    m_pkScene = 0;
    m_pkProp = 0;
    m_spDest = 0;
    m_uiCompartmentID = 0;

    m_kPhysXToPSys.MakeIdentity();
    m_fScalePToW = 1.0f;

    m_uiDefaultActorPoolSize =
        (NiUInt32)NiPSMeshParticleSystem::DEFAULT_POOL_SIZE;
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NiUInt32 uiFinalIndex = m_uiNumParticles - 1;
    NiUInt32 uiFlags = m_puiFlags[uiIndexToRemove];
    NiUInt16 usGeneration = NiPSFlagsHelpers::GetGeneration(uiFlags);

    NiPSMeshParticleSystem::RemoveParticle(uiIndexToRemove);

    NxActor* pkRemovedActor = m_ppkActors[uiIndexToRemove];
    m_ppkActors[uiIndexToRemove] = 0;
   
    // If the index is less than the maximum number of particles,
    // then swap position of the last element to uiIndexRemove.
    if (uiIndexToRemove < uiFinalIndex)
    {
        m_ppkActors[uiIndexToRemove] = m_ppkActors[uiFinalIndex];
        m_pkActorVelocity[uiIndexToRemove] = m_pkActorVelocity[uiFinalIndex];
    }

    if (pkRemovedActor)
    {
        // Add the actor to the appropriate pool. It will be deleted if the
        // pool is full
        AddActorToPool(usGeneration, pkRemovedActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::SetScene(NiPhysXScene* pkScene)
{
    if (m_pkScene)
    {
        // Clear out all particles.
        ResetParticleSystem();
        
        // Clear the actor pools
        for (NiUInt32 ui = m_kActorPools.GetSize(); ui > 0; ui--)
        {
            EmptyActorPool(ui - 1);
        }
    }

    m_pkScene = pkScene;
    
    if (!m_pkScene)
        return;

    // Prime the transforms. If they change later, it will get caught in an
    // update.
    NiTransform kInvPSysXform;
    GetWorldTransform().Invert(kInvPSysXform);
    m_kPhysXToPSys = kInvPSysXform * m_pkScene->GetSceneXform();
    m_fScalePToW = m_pkScene->GetScaleFactor();
    
    // Create the individual pools for each generation
    for(NiUInt32 ui = 0; ui < GetNumGenerations(); ui++)
    {
        ActorArray* pkPool = m_kActorPools.GetAt(ui);
        pkPool->SetSize(m_uiDefaultActorPoolSize);
    }

    if (GetBit(FLAG_FILL_POOLS_ON_LOAD) && m_uiDefaultActorPoolSize > 0)
    {
        // Fill the Pools for each generation
        NiUInt32 uiGen;
        for (uiGen = 0; uiGen < GetNumGenerations(); uiGen++)
        {
            // Make sure we have a mesh to put in that pool
            if ((uiGen < m_kActorDescriptors.GetAllocatedSize()) &&
                m_kActorDescriptors.GetAt(uiGen))
            {
                FillActorPool(uiGen, m_kActorDescriptors.GetAt(uiGen));
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::SetNumGenerations(NiUInt32 uiNumGenerations)
{
    // Removing Pools
    NiUInt32 uiOldSize = m_kActorPools.GetSize();
    if (uiNumGenerations < uiOldSize)
    {
        for (NiUInt32 ui = uiOldSize; ui > uiNumGenerations; ui--)
        {
            EmptyActorPool(ui - 1);
            NiDelete m_kActorPools.RemoveEnd();
        }
    } // Adding Pools
    else if (uiNumGenerations > uiOldSize)
    {
        m_kActorPools.SetSize(uiNumGenerations);

        // Allocate the New Pools.
        NiUInt32 ui;
        for (ui = uiOldSize; ui < uiNumGenerations; ui++)
        {
           // Create the Pool
            ActorArray* pkPool = NiNew ActorArray();
            pkPool->SetSize(m_uiDefaultActorPoolSize);

            m_kActorPools.SetAt(ui, pkPool);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::FillActorPool(NiUInt32 uiGeneration,
    NiPhysXActorDesc* pkOriginal)
{
    if (!m_pkScene || !m_pkScene->GetPhysXScene())
        return;
        
    NIASSERT(pkOriginal != NULL);

    // Make sure we have something to fill
    if (uiGeneration >= m_kActorPools.GetSize() ||
        !m_kActorPools.GetAt(uiGeneration))
        return;

    ActorArray* pkPool = m_kActorPools.GetAt(uiGeneration);
    
    // Get any compartment
    NxCompartment* pkCompartment = 0;
    if (m_uiCompartmentID)
    {
        pkCompartment = m_pkScene->GetCompartmentAt(m_uiCompartmentID);
    }

    PhysXSDKManager::GetManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for(NiUInt32 ui = 0; ui < pkPool->GetAllocatedSize(); ui++)
    {
        NxActor* pkActor = pkOriginal->CreateActor(m_pkScene->GetPhysXScene(),
            pkCompartment, 0, NiPhysXTypes::NXMAT34_ID,
            GetBit(FLAG_KEEP_ACTOR_MESHES));
        if (!pkActor)
            continue;
        NxVec3 kPosn(
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.x
            + m_kActorPoolCenter.x,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.y
            + m_kActorPoolCenter.y,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.z
            + m_kActorPoolCenter.z);
        pkActor->setGlobalPosition(kPosn);
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
        
        pkPool->SetAt(ui, pkActor);
    }
    PhysXSDKManager::GetManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
NxActor* NiPhysXPSMeshParticleSystem::RemoveActorFromPool(
    NiUInt32 uiGeneration)
{
    // Check if the generation has a pool
    if (uiGeneration >= m_kActorPools.GetSize())
    {
        return NULL;
    }

    // Check if the generation has a pool allocated
    if (!m_kActorPools.GetAt(uiGeneration))
        return NULL;

    return m_kActorPools.GetAt(uiGeneration)->RemoveEnd();
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::AddActorToPool(NiUInt32 uiGeneration, 
    NxActor* pkActor)
{
    if (!pkActor)
        return;
        
    // Check if the generation has a pool
    if (uiGeneration >= m_kActorPools.GetSize())
        return;

    // Check there is a pool to add into
    if (!m_kActorPools.GetAt(uiGeneration))
        return;

    if (m_kActorPools.GetAt(uiGeneration)->GetSize() <
        m_kActorPools.GetAt(uiGeneration)->GetAllocatedSize())
    {
        m_kActorPools.GetAt(uiGeneration)->Add(pkActor);
        PhysXSDKManager::GetManager()->WaitSDKLock();
        NIASSERT(!m_pkScene->GetInSimFetch());
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
        PhysXSDKManager::GetManager()->ReleaseSDKLock();
    }
    else
    {
        PhysXSDKManager::GetManager()->WaitSDKLock();
        NIASSERT(!m_pkScene->GetInSimFetch());
        m_pkScene->GetPhysXScene()->releaseActor(*pkActor);
        PhysXSDKManager::GetManager()->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::EmptyActorPool(NiUInt32 uiGeneration)
{
    NIASSERT(uiGeneration < m_kActorPools.GetSize());

    // It could be that the PhysX scene has already been deleted, in which
    // case the actors are all gone.
    if (m_pkScene && m_pkScene->GetPhysXScene())
    {
        ActorArray* pkActorArray = m_kActorPools.GetAt(uiGeneration);
        
        PhysXSDKManager::GetManager()->WaitSDKLock();
        for (NiUInt32 ui = 0; ui < pkActorArray->GetSize(); ui++)
        {
            NxActor* pkActor = pkActorArray->GetAt(ui);
            if (pkActor)
                m_pkScene->GetPhysXScene()->releaseActor(*pkActor);
        }
        PhysXSDKManager::GetManager()->ReleaseSDKLock();
    }
    m_kActorPools.GetAt(uiGeneration)->RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::MapActorPools(NiTMap<NxActor*, bool>& kMap)
{
    for (NiUInt32 ui = 0; ui < GetNumGenerations(); ui++)
    {
        ActorArray* pkActorArray = m_kActorPools.GetAt(ui);
        for (NiUInt32 uj = 0; uj < pkActorArray->GetSize(); uj++)
        {
            NxActor* pkActor = pkActorArray->GetAt(uj);
            if (pkActor)
                kMap.SetAt(pkActor, true);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPhysXPSMeshParticleSystem::GetPhysXState()
{
    if (!GetBit(FLAG_NEEDS_UPDATE) || !m_pkScene)
        return false;

    // Push PhysX particle state to GB.
    PhysXSDKManager::GetManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ui++)
    {
        NxActor* pkActor = m_ppkActors[ui];
        NIASSERT(pkActor);

        m_pkActorPose[ui] = pkActor->getGlobalPose();

        m_pkActorVelocity[ui] = pkActor->getLinearVelocity();
    }
    PhysXSDKManager::GetManager()->ReleaseSDKLock();
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::SetOnConstruction(NiUInt32 uiMaxNumParticles,
    NiUInt16 usNumGenerations, NiPhysXPSMeshParticleSystemProp* pkProp,
    NiUInt32 uiCompartmentID, NiUInt32 uiActorPoolSize,
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes, bool bPhysXRotations,
    bool bPhysXSpace)
{
    NIASSERT(uiMaxNumParticles > 0);

    m_kPhysXToPSys.MakeIdentity();
    m_fScalePToW = 1.0f;

    // Set the scene in which it resides to 0 for now.
    m_pkScene = 0;
    m_uiCompartmentID = uiCompartmentID;

    // Set up space for the actor descriptors
    m_kActorDescriptors.SetSize(usNumGenerations);
    
    SetBit(bFillActorPoolsOnLoad, FLAG_FILL_POOLS_ON_LOAD);
    SetBit(bKeepsMeshes, FLAG_KEEP_ACTOR_MESHES);

    // Initialize the size of the pool. This may be ignored later.
    m_uiDefaultActorPoolSize = uiActorPoolSize;
    if (m_uiDefaultActorPoolSize == (NiUInt32)DEFAULT_POOL_SIZE)
        m_uiDefaultActorPoolSize = uiMaxNumParticles;

    // Set the number of pools, one for each generation
    m_kActorPools.SetSize(usNumGenerations);
    
    // Create the individual pools for each generation, but set to zero size
    for(NiUInt32 ui = 0; ui < usNumGenerations; ui++)
    {
        ActorArray* pkPool = NiNew ActorArray();
        pkPool->SetSize(uiMaxNumParticles);

        m_kActorPools.SetAt(ui, pkPool);
    }
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3(100.0f, 100.0f, 100.0f);

    SetBit(bPhysXRotations, FLAG_PHYSX_ROTATIONS);
    SetBit(bPhysXSpace, FLAG_PHYSX_SPACE);
    SetBit(false, FLAG_NEEDS_UPDATE);

    // Reallocate the data buffers.
    AllocateDataBuffers(m_uiMaxNumParticles, HasLivingSpawner(), HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));

    m_pkProp = pkProp;
    if (pkProp)
    {
        // Set up source to out of PhysX.
        pkProp->AddSystem(this);
        m_spDest = NiNew NiPhysXPSParticleSystemDest(this);
        pkProp->AddDestination(m_spDest);

        // Attach the scene, which will fill actor pools if necessary
        if (pkProp->GetScene())
            SetScene(pkProp->GetScene());
    }
    else
    {
        m_spDest = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::AllocateDataBuffers(NiUInt32 uiBufferSize,
    bool bAllocateSpawnTime, bool bAllocateColors, bool bAllocateRotations,
    bool bAllocateRotationAxes)
{
    FreeDataBuffers();

    NiPSMeshParticleSystem::AllocateDataBuffers(uiBufferSize, bAllocateSpawnTime,
        bAllocateColors, bAllocateRotations, bAllocateRotationAxes, false);

    m_ppkActors = NiAlloc(NxActor*, uiBufferSize);
    m_pkLastVelocities = NiAlignedAlloc(NiPoint3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);

    m_pkActorPose = NiAlignedAlloc(NxMat34, uiBufferSize,
            NIPSKERNEL_ALIGNMENT);

    m_pkActorVelocity = NiAlignedAlloc(NxVec3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);

    memset(m_ppkActors, 0, sizeof(NxActor*) * uiBufferSize);
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::FreeDataBuffers()
{
    NiFree(m_ppkActors);
    m_ppkActors = NULL;

    NiAlignedFree(m_pkLastVelocities);
    m_pkLastVelocities = NULL;

    NiAlignedFree(m_pkActorPose);
    m_pkActorPose = NULL;
    
    NiAlignedFree(m_pkActorVelocity);
    m_pkActorVelocity = NULL;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::UpdateParticlesUponCompletion()
{
    NiPSMeshParticleSystem::UpdateParticlesUponCompletion();
    
    if (!m_pkScene)
        return;

    // Push GB particle state to PhysX.
    PhysXSDKManager::GetManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ui++)
    {
        NxActor* pkActor = m_ppkActors[ui];

        pkActor->addForce(m_pkActorVelocity[ui], NX_VELOCITY_CHANGE);
    }
    PhysXSDKManager::GetManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem::~NiPhysXPSMeshParticleSystem()
{
    // Must do this here so that the object is still alive to
    // complete simulation.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);
    
    // We need to remove the simulator modifier now, while the particle
    // system simulator still exists to detach.
    NiMeshModifier* pkSimulator =
        GetModifierByType(&NiPhysXPSSimulator::ms_RTTI);
    RemoveModifier(pkSimulator);

    NiUInt32 uiNumGenerations = m_kActorPools.GetSize();
    for (NiUInt32 ui = uiNumGenerations; ui > 0; ui--)
    {
        EmptyActorPool(ui - 1);
        NiDelete m_kActorPools.RemoveEnd();
    }

    DetachFromProp();
    m_spDest = 0;

    FreeDataBuffers();
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::AttachToProp(
    NiPhysXPSMeshParticleSystemProp* pkProp)
{
    DetachFromProp();
    
    AttachToScene(pkProp->GetScene());
    
    m_pkProp = pkProp;
    pkProp->AddSystem(this);
    m_spDest = NiNew NiPhysXPSParticleSystemDest(this);
    pkProp->AddDestination(m_spDest);
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::DetachFromProp()
{
    if (!m_pkProp)
        return;

    // Cache the prop pointer then set it to null. This function can get
    // called recursively and we don't want the second call to do anything.
    NiPhysXPSMeshParticleSystemProp* pkProp = m_pkProp;
    m_pkProp = 0;

    DetachFromScene();
    
    pkProp->RemoveSystem(this);
    pkProp->DeleteDestination(m_spDest);
    
    m_spDest = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::AttachToScene(NiPhysXScene* pkScene)
{
    if (m_pkScene)
        DetachFromScene();
        
    m_pkScene = pkScene;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::DetachFromScene(void)
{
    ResetParticleSystem();

    for (NiUInt32 ui = 0; ui < m_kActorPools.GetSize(); ui++)
        EmptyActorPool(ui);

    m_pkScene = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    NiPSMeshParticleSystem::InitializeParticle(uiNewParticle);
    
    if (!m_pkScene)
    {
        m_ppkActors[uiNewParticle] = NULL;
        return;
    }
    
    NiUInt32 uiFlags = m_puiFlags[uiNewParticle];
    NiUInt16 usGen = NiPSFlagsHelpers::GetGeneration(m_puiFlags[uiFlags]);
    if (usGen >= GetActorDescCount())
    {
        usGen = (NiUInt16)(GetActorDescCount() - 1);
    }

    NIASSERT(GetActorDescAt(usGen) &&
        "NULL actor descriptor in NiPhysXPSMeshParticleSystem");

    // Try to pull from the pool
    NxActor* pkActor = RemoveActorFromPool((NiUInt32)usGen);
    
    PhysXSDKManager::GetManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    if (!pkActor)
    {
        NxCompartment* pkCompartment = 0;
        if (m_uiCompartmentID)
        {
            pkCompartment =
                m_pkScene->GetCompartmentAt(m_uiCompartmentID);
        }
        
        pkActor = GetActorDescAt(usGen)->CreateActor(
            m_pkScene->GetPhysXScene(), pkCompartment, 0, 
            NiPhysXTypes::NXMAT34_ID, GetBit(FLAG_KEEP_ACTOR_MESHES));
    }
    pkActor->clearActorFlag(NX_AF_DISABLE_COLLISION);

    if (GetBit(FLAG_PHYSX_SPACE))
    {
        NxVec3 kVec;            
        if (m_pfRotationAngles && m_pkRotationAxes)
        {
            NiMatrix3 kRotate;
            kRotate.MakeRotation(m_pfRotationAngles[uiNewParticle],
                m_pkRotationAxes[uiNewParticle]);
            NiPoint3 kPosn = m_pkPositions[uiNewParticle] / m_fScalePToW;
            NxMat34 kPose;
            NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
            pkActor->setGlobalPose(kPose);
        }
        else
        {
            NiPoint3 kPosn = m_pkPositions[uiNewParticle] / m_fScalePToW;
            NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
            pkActor->setGlobalPosition(kVec);
        }        
        NiPhysXTypes::NiPoint3ToNxVec3(m_pkVelocities[uiNewParticle], kVec);
        kVec /= m_fScalePToW;
        pkActor->setLinearVelocity(kVec);
    }
    else
    {
        NiTransform m_kPSysToPhysX;
        m_kPhysXToPSys.Invert(m_kPSysToPhysX);

        NxVec3 kVec;            
        if (m_pfRotationAngles && m_pkRotationAxes)
        {
            NiMatrix3 kLocalRotate;
            kLocalRotate.MakeRotation(m_pfRotationAngles[uiNewParticle],
                m_pkRotationAxes[uiNewParticle]);
            NiMatrix3 kRotate = m_kPSysToPhysX.m_Rotate 
                * kLocalRotate;
            NiPoint3 kPosn = m_kPSysToPhysX * m_pkPositions[uiNewParticle];
            kPosn /= m_fScalePToW;
            NxMat34 kPose;
            NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
            pkActor->setGlobalPose(kPose);
        }
        else
        {
            NiPoint3 kPosn = m_kPSysToPhysX * m_pkPositions[uiNewParticle];
            kPosn /= m_fScalePToW;
            NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
            pkActor->setGlobalPosition(kVec);
        }
        NiPoint3 kVelocity = 
            m_kPSysToPhysX.m_Rotate * m_pkVelocities[uiNewParticle];
        kVelocity /= m_fScalePToW;
        NiPhysXTypes::NiPoint3ToNxVec3(kVelocity, kVec);
        pkActor->setLinearVelocity(kVec);
    }

    PhysXSDKManager::GetManager()->ReleaseSDKLock();

    m_ppkActors[uiNewParticle] = pkActor;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::ResetParticleSystem()
{
    // Completes tasks.
    m_pkSimulator->ResetSimulator(this);

    // Remove each particle, adding the actor back to the pool.
    for (NiUInt32 uiParticle = 0; uiParticle < m_uiNumParticles; ++uiParticle)
    {
        NiUInt16 usGen = NiPSFlagsHelpers::GetGeneration(
            m_puiFlags[uiParticle]);
        if (usGen >= GetActorDescCount())
        {
            usGen = GetActorDescCount() - 1;
        }

        NxActor* pkRemovedActor = m_ppkActors[uiParticle];
        if (pkRemovedActor)
        {
            // Add the actor to the appropriate pool. It will be deleted if
            // the pool is full
            AddActorToPool(usGen, pkRemovedActor);
        }
        m_ppkActors[uiParticle] = 0;
    }

    // Call the base class version of this function to actually reset the
    // particle counts.
    NiPSMeshParticleSystem::ResetParticleSystem();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSMeshParticleSystem);
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::CopyMembers(
    NiPhysXPSMeshParticleSystem* pkDest, NiCloningProcess& kCloning)
{
    NiPSMeshParticleSystem::CopyMembers(pkDest, kCloning);
    
    if (!NiIsKindOf(NiPhysXPSMeshParticleSystem, this))
        return;

    pkDest->m_pkProp = 0;
    pkDest->m_spDest = 0;

    // The cloned particle system must be attached to a prop and scene.
    pkDest->m_pkScene = 0;
    
    pkDest->m_uFlags = m_uFlags;
    SetBit(false, FLAG_NEEDS_UPDATE);

    pkDest->AllocateDataBuffers(m_uiMaxNumParticles, HasLivingSpawner(), HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));

    // Copy particle data. This also copies the actor pointers, which we want
    // to zero so that new actors are created later.
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ui++)
    {
        // Make sure the old actor is gone
        pkDest->m_ppkActors[ui] = 0;
    }

    pkDest->m_uiDefaultActorPoolSize = m_uiDefaultActorPoolSize;
    
    // We do not clone the pools - systems do not share pools
    NiUInt32 uiNumGenerations = GetNumGenerations();
    pkDest->m_kActorPools.SetSize(uiNumGenerations);

    // Create the individual pools for each generation
    for(NiUInt32 ui = 0; ui < uiNumGenerations; ui++)
    {
        ActorArray* pkPool = NiNew ActorArray();
        pkPool->SetSize(m_kActorPools.GetAt(ui)->GetAllocatedSize());

        pkDest->m_kActorPools.SetAt(ui, pkPool);
    }
    pkDest->m_kActorPoolCenter = m_kActorPoolCenter;
    pkDest->m_kActorPoolDim = m_kActorPoolDim;

    pkDest->m_kActorDescriptors.SetSize(m_kActorDescriptors.GetSize());
    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkDest->m_kActorDescriptors.SetAt(ui, 
                (NiPhysXActorDesc*)pkActorDesc->CreateSharedClone(kCloning));
        }
        else
        {
            pkDest->m_kActorDescriptors.SetAt(ui, 0);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSMeshParticleSystem::ProcessClone(kCloning);
    
    NiObject* pkClone = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    
    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkActorDesc->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSMeshParticleSystem);
//---------------------------------------------------------------------------
bool NiPhysXPSMeshParticleSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::LoadBinary(NiStream& kStream)
{
    NiPSMeshParticleSystem::LoadBinary(kStream);

    m_pkProp = (NiPhysXPSMeshParticleSystemProp*)kStream.ResolveLinkID();
    m_spDest = (NiPhysXPSParticleSystemDest*)kStream.ResolveLinkID();

    m_pkScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_uFlags);
    SetBit(false, FLAG_NEEDS_UPDATE);
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    NiUInt32 uiNumGenerations;
    NiUInt32 uiGenerationPoolSize;

    NiStreamLoadBinary(kStream, uiNumGenerations);

    // Load prototypical actors and set pools up
    m_kActorPools.SetSize(uiNumGenerations);
    for (NiUInt32 ui = 0; ui < uiNumGenerations; ui++)
    {
        NiStreamLoadBinary(kStream, uiGenerationPoolSize);

        // Create the Pool
        ActorArray* pkPool = NiNew ActorArray();
        pkPool->SetSize(uiGenerationPoolSize);

        m_kActorPools.SetAt(ui, pkPool);
    }
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kActorDescriptors.SetSize(uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = (NiPhysXActorDesc*)
            kStream.ResolveLinkID();
        m_kActorDescriptors.SetAt(ui, pkActor);
    }

    AllocateDataBuffers(m_uiMaxNumParticles, HasLivingSpawner(), HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::LinkObject(NiStream& kStream)
{
    NiPSMeshParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::PostLinkObject(NiStream& kStream)
{
    NiPSMeshParticleSystem::PostLinkObject(kStream);

    if (GetBit(FLAG_FILL_POOLS_ON_LOAD))
    {
        // Fill the Pools for each generation
        NiUInt32 uiGen;
        for (uiGen = 0; uiGen < GetNumGenerations(); uiGen++)
        {
            // Make sure we have a mesh to put in that pool
            if ((uiGen < m_kActorDescriptors.GetAllocatedSize()) &&
                m_kActorDescriptors.GetAt(uiGen))
            {
                FillActorPool(uiGen, m_kActorDescriptors.GetAt(uiGen));
            }
        }
    }
    
    // Can't stream  a particle system with active particles, otherwise we'd
    // need to initialize them here, or in a post-update for
    // NiPhysXPSMeshParticleSystem.
}
//---------------------------------------------------------------------------
bool NiPhysXPSMeshParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSMeshParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }
    
    if (m_pkProp)
        m_pkProp->RegisterStreamables(kStream);
    if (m_spDest)
        m_spDest->RegisterStreamables(kStream);

    if (m_pkScene) 
        m_pkScene->RegisterStreamables(kStream);
    
    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        if (pkActor)
        {
            pkActor->RegisterStreamables(kStream);
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::SaveBinary(NiStream& kStream)
{
    NiPSMeshParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkProp);
    kStream.SaveLinkID(m_spDest);

    kStream.SaveLinkID(m_pkScene);

    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    NiUInt32 uiNumGenerations = m_kActorPools.GetSize();
    NiStreamSaveBinary(kStream, uiNumGenerations);

    for (NiUInt32 ui = 0; ui < uiNumGenerations; ui++)
    {
        if (m_kActorPools.GetAt(ui))
        {
            NiStreamSaveBinary(kStream, 
                m_kActorPools.GetAt(ui)->GetAllocatedSize());
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    NiUInt32 uiSize = m_kActorDescriptors.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        kStream.SaveLinkID(pkActor);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXPSMeshParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiPSMeshParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSMeshParticleSystem* pkNewObject =
        (NiPhysXPSMeshParticleSystem*)pkObject;
    
    // some m_uFlags are dependent on Update so we check them individually
    if (GetFillActorPoolsOnLoad() != pkNewObject->GetFillActorPoolsOnLoad() ||
        GetKeepsActorMeshes() != pkNewObject->GetKeepsActorMeshes() ||
        GetUsesPhysXSpace() != pkNewObject->GetUsesPhysXSpace() ||
        GetUsesPhysXRotations() != pkNewObject->GetUsesPhysXRotations())
    {
        return false;
    }

    if (m_uiDefaultActorPoolSize != pkNewObject->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkNewObject->m_kActorPoolCenter ||
        m_kActorPoolDim != pkNewObject->m_kActorPoolDim)
    {
        return false;
    }
    
    for (NiUInt32 ui = 0; ui < m_kActorPools.GetSize(); ui++)
    {
        if (m_kActorPools.GetAt(ui))
        {
            if (!pkNewObject->m_kActorPools.GetAt(ui))
                return false;
                
            if (m_kActorPools.GetAt(ui)->GetAllocatedSize() !=
                pkNewObject->m_kActorPools.GetAt(ui)->GetAllocatedSize())
                return false;
        }
        else if (pkNewObject->m_kActorPools.GetAt(ui))
        {
            return false;
        }
    }

    if (m_kActorDescriptors.GetSize() != 
        pkNewObject->m_kActorDescriptors.GetSize())
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor1 = m_kActorDescriptors.GetAt(ui);
        NiPhysXActorDesc* pkActor2 = 
            pkNewObject->m_kActorDescriptors.GetAt(ui);
        if ((pkActor1 && !pkActor2) || (!pkActor1 && pkActor2) ||
            (pkActor1 && pkActor2 && !pkActor1->IsEqual(pkActor2)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSMeshParticleSystem::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSMeshParticleSystem::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(
        NiPhysXPSMeshParticleSystem::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("PhysXSpace",
        GetBit(FLAG_PHYSX_SPACE)));
    pkStrings->Add(NiGetViewerString("PhysXRotations",
        GetBit(FLAG_PHYSX_ROTATIONS)));
        
    if (m_spDest)
    {
        m_spDest->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spDest", 0));
    }

    pkStrings->Add(NiGetViewerString("FillActorPoolsOnLoad",
        GetBit(FLAG_FILL_POOLS_ON_LOAD)));
    pkStrings->Add(NiGetViewerString("KeepsActorMeshes",
        GetBit(FLAG_KEEP_ACTOR_MESHES)));
    pkStrings->Add(NiGetViewerString("DefaultActorPoolSize", 
        m_uiDefaultActorPoolSize));

    for (NiUInt32 ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkActorDesc->GetViewerStrings(pkStrings);
        }
        else
        {
            pkStrings->Add(NiGetViewerString("Missing actor", 0));
        }
    }
}
//---------------------------------------------------------------------------
