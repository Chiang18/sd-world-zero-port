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
#include "NiParticlePCH.h"

#include "NiPoint2.h"
#include "NiPSAlignedQuadGenerator.h"
#include "NiPSParticleSystem.h"
#include <NiCommonSemantics.h>
#include "NiPSCommonSemantics.h"
#include <NiSPWorkflow.h>
#include <NiCullingProcess.h>

NiImplementRTTI(NiPSAlignedQuadGenerator, NiMeshModifier);

const NiUInt32 NiPSAlignedQuadGenerator::MAX_SUPPORTED_PARTICLES = UINT_MAX/6;
const NiUInt16 NiPSAlignedQuadGenerator::MAX_PARTICLES_FOR_16BIT_INDICES = USHRT_MAX/4;

//--------------------------------------------------------------------------------------------------
NiPSAlignedQuadGenerator::NiPSAlignedQuadGenerator() :
    m_pkGeneratorKernel(0),
    m_kGeneratorKernelStructIS(&m_kGeneratorKernelStruct, 1),
    m_pkTextureKernel(0),
    m_kTextureKernelStructIS(&m_kTextureKernelStruct, 1),
    m_b32BitIndices(false),
    m_bColors(false),
    m_bAttached(false)
{
   
    m_kGeneratorKernelStruct.m_kUp = NiPoint3(0.0f,1.0f,0.0f);
    m_kGeneratorKernelStruct.m_kNormal = NiPoint3(0.0f,0.0f,1.0f);
    m_kGeneratorKernelStruct.m_fScaleAmountU = 0.0f;
    m_kGeneratorKernelStruct.m_fScaleLimitU = 1.0e4;
    m_kGeneratorKernelStruct.m_fScaleRestU = 1.0f;
    m_kGeneratorKernelStruct.m_fScaleNormalizedU = 0.0f;
    m_kGeneratorKernelStruct.m_fScaleAmountV = 0.0f;
    m_kGeneratorKernelStruct.m_fScaleLimitV = 1.0e4;
    m_kGeneratorKernelStruct.m_fScaleRestV = 1.0f;
    m_kGeneratorKernelStruct.m_fScaleNormalizedV = 0.0f;
    m_kGeneratorKernelStruct.m_fCenterU = 0.0f;
    m_kGeneratorKernelStruct.m_fCenterV = 0.0f;

    m_kTextureKernelStruct.m_bUVScrolling = false;
    m_kTextureKernelStruct.m_uiNumFramesAcross = 1;
    m_kTextureKernelStruct.m_uiNumFramesDown = 1;
    m_kTextureKernelStruct.m_bPingPong = false;
    m_kTextureKernelStruct.m_fInitialTime = 0.0f;
    m_kTextureKernelStruct.m_fFinalTime = 1.0f;
    m_kTextureKernelStruct.m_uiInitialFrame = 0;
    m_kTextureKernelStruct.m_fInitialFrameVar = 0.0f;
    m_kTextureKernelStruct.m_uiNumFrames = 0;
    m_kTextureKernelStruct.m_fNumFramesVar = 0.0f;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::Attach(NiMesh* pkMesh)
{
    AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);
    AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;
    NIASSERT(pkParticleSystem);

    // Get maximum number of particles.
    const NiUInt32 uiMaxNumParticles = pkParticleSystem->GetMaxNumParticles();

    if (uiMaxNumParticles > MAX_SUPPORTED_PARTICLES)
    {
        NILOG("NiPSAlignedQuadGenerator Error: MaxNumParticles is %d, which "
            "is too large to support 6 indices per particle. The mesh "
            "modifier cannot be attached.\n", uiMaxNumParticles);
        return false;
    }
    else if (uiMaxNumParticles == 0)
    {
        NILOG("NiPSAlignedQuadGenerator Error: MaxNumParticles is 0. The mesh "
            "modifier will not be attached to a particle system that can "
            "contain no particles.\n");
        return false;
    }
    
    // Set the primitive type for the particle system.
    pkParticleSystem->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    const NiUInt32 uiNumVertices = uiMaxNumParticles * 4;
    const NiUInt32 uiNumIndices = uiMaxNumParticles * 6;

    // Add data stream for vertices.
    NiDataStream* pkDataStream = AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        uiNumVertices,
        NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        sizeof(NiPoint3));
    m_kVertexOS.SetDataSource(pkDataStream, false);
    m_kVertexOS.SetStride(sizeof(NiPoint3) * 4);

    // Add data stream for indices.
    if (uiMaxNumParticles < MAX_PARTICLES_FOR_16BIT_INDICES)
    {
        // If the number of vertices is less than 65536, 16-bit indices can
        // be used.
        NiUInt16* pusIndices = NiAlloc(NiUInt16, uiNumIndices);
        NiUInt16 usStartingVertex = 0;
        for (NiUInt32 ui = 0; ui + 5 < uiNumIndices; ui += 6)
        {
            pusIndices[ui] = usStartingVertex;
            pusIndices[ui + 1] = usStartingVertex + 1;
            pusIndices[ui + 2] = usStartingVertex + 2;
            pusIndices[ui + 3] = usStartingVertex + 2;
            pusIndices[ui + 4] = usStartingVertex + 3;
            pusIndices[ui + 5] = usStartingVertex;
            usStartingVertex += 4;
        }
        AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT16_1,
            uiNumIndices,
            NiDataStream::ACCESS_GPU_READ |
#if defined(_WII)
                NiDataStream::ACCESS_CPU_READ |
#endif
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            NiObject::CLONE_SHARE,
            sizeof(NiUInt16),
            pusIndices);
        NiFree(pusIndices);
    }
    else
    {
        m_b32BitIndices = true;

        NiUInt32* puiIndices = NiAlloc(NiUInt32, uiNumIndices);
        NiUInt32 uiStartingVertex = 0;
        for (NiUInt32 ui = 0; ui + 5 < uiNumIndices; ui += 6)
        {
            puiIndices[ui] = uiStartingVertex;
            puiIndices[ui + 1] = uiStartingVertex + 1;
            puiIndices[ui + 2] = uiStartingVertex + 2;
            puiIndices[ui + 3] = uiStartingVertex + 2;
            puiIndices[ui + 4] = uiStartingVertex + 3;
            puiIndices[ui + 5] = uiStartingVertex;
            uiStartingVertex += 4;
        }
        AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT32_1,
            uiNumIndices,
            NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX,
            NiObject::CLONE_SHARE,
            sizeof(NiUInt32),
            puiIndices);
        NiFree(puiIndices);
    }

    // Add data stream for normals. Size depends on alignment method.
    bool bNormalsPerParticle = (pkParticleSystem->GetNormalMethod() &
        NiPSParticleSystem::ALIGN_PER_PARTICLE) != 0;
    NiUInt32 uiNormalStreamSize = bNormalsPerParticle ? uiNumVertices : 1;
    NiDataStream* pkNormals = AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::NORMAL(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        uiNormalStreamSize,
        NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        sizeof(NiPoint3));

    // Add data stream for texture coordinates, ages and life spans.
    // Initial setup and access depends on texture animation flag
    NiPoint2* pkUVs = NiNew NiPoint2[uiNumVertices];

    NiUInt8 uiAccess = NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
    if (pkParticleSystem->HasAnimatedTextures())
    {
        m_kAgeIS.SetData(pkParticleSystem->GetAges()); 
        m_kLifeSpanIS.SetData(pkParticleSystem->GetLifeSpans());
        m_kVarianceIS.SetData(pkParticleSystem->GetVariance());
    }
    else
    {
        uiAccess = NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC;

        for (NiUInt32 ui = 0; ui + 3 < uiNumVertices; ui += 4)
        {
            pkUVs[ui] = NiPoint2::ZERO;
            pkUVs[ui + 1] = NiPoint2::UNIT_Y;
            pkUVs[ui + 2] = NiPoint2(1.0f, 1.0f);
            pkUVs[ui + 3] = NiPoint2::UNIT_X;
        }
    }

    pkDataStream = AddDataStream(
        pkParticleSystem,
        NiCommonSemantics::TEXCOORD(),
        0,
        NiDataStreamElement::F_FLOAT32_2,
        uiNumVertices,
        uiAccess,
        NiDataStream::USAGE_VERTEX,
         (pkParticleSystem->HasAnimatedTextures() ? NiObject::CLONE_BLANK_COPY : 
            NiObject::CLONE_SHARE),
        sizeof(NiPoint2),
        pkUVs);

    if (pkParticleSystem->HasAnimatedTextures())
    {
        m_kTexCoordOS.SetDataSource(pkDataStream, false);
        m_kTexCoordOS.SetStride(sizeof(NiPoint2) * 4);
    }
    NiDelete[] pkUVs;

    if (pkParticleSystem->HasColors())
    {
        m_bColors = true;

        // Add data stream for colors.
        pkDataStream = AddDataStream(
            pkParticleSystem,
            NiCommonSemantics::COLOR(),
            0,
            NiDataStreamElement::F_NORMUINT8_4,
            uiNumVertices,
            NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStream::USAGE_VERTEX,
            NiObject::CLONE_BLANK_COPY,
            sizeof(NiRGBA));
        m_kColorOS.SetDataSource(pkDataStream, false);
        m_kColorOS.SetStride(sizeof(NiRGBA) * 4);
    }

    // Set the number of submeshes on the particle system.
    pkParticleSystem->SetSubmeshCount(1);

    // Associate common particle data with Floodgate streams.
    m_kPositionIS.SetData(pkParticleSystem->GetPositions());
    if (bNormalsPerParticle)
    {
        m_kNormalOS.SetDataSource(pkNormals, false);
        m_kNormalOS.SetStride(sizeof(NiPoint3) * 4);
    }
    else
    {
        pkNormals->SetGPUConstantSingleEntry(true);
        m_kNormalOS.SetData(NULL);
    }
    m_kRadiusIS.SetData(pkParticleSystem->GetInitialSizes());
    m_kSizeIS.SetData(pkParticleSystem->GetSizes());
    m_kColorIS.SetData(pkParticleSystem->GetColors());
    m_kRotAngleIS.SetData(pkParticleSystem->GetRotationAngles());
 
    // If we are scaling quads based on velocity 
    if ((m_kGeneratorKernelStruct.m_fScaleAmountV != 0.0f) 
        || (m_kGeneratorKernelStruct.m_fScaleAmountU != 0.0f))
    {
        // Verify that ratio and limit are valid
        if ((m_kGeneratorKernelStruct.m_fScaleAmountU < 0.0f &&
            m_kGeneratorKernelStruct.m_fScaleLimitU >= m_kGeneratorKernelStruct.m_fScaleRestU)
            || (m_kGeneratorKernelStruct.m_fScaleAmountU > 0.0f &&
            m_kGeneratorKernelStruct.m_fScaleLimitU < m_kGeneratorKernelStruct.m_fScaleRestU)
            || (m_kGeneratorKernelStruct.m_fScaleAmountV < 0.0f &&
            m_kGeneratorKernelStruct.m_fScaleLimitV >= m_kGeneratorKernelStruct.m_fScaleRestV)
            || (m_kGeneratorKernelStruct.m_fScaleAmountV > 0.0f &&
            m_kGeneratorKernelStruct.m_fScaleLimitV < m_kGeneratorKernelStruct.m_fScaleRestV))
        {
            NIASSERT(0);
#ifndef NIDEBUG
            NILOG("NiPSAlignedQuadGenerator Error: For a positive scale amount, the "
                "limit must be greater than the rest scale. For a negative "
                "scale amount, the limit must be less than the rest scale.\n");
#endif
            return false;
        }
       
        m_kVelocityIS.SetData(pkParticleSystem->GetVelocities());
    }

    // Process data that depends on the alignment method
    if (!AttachAlignMethods(pkParticleSystem))
        return false;

    // Create Quad Generator Floodgate task
    m_spGeneratorTask = NiSPTask::GetNewTask(8, 3);
    m_spGeneratorTask->SetKernel(m_pkGeneratorKernel);
    m_spGeneratorTask->AddInput(&m_kGeneratorKernelStructIS);
    m_spGeneratorTask->AddInput(&m_kPositionIS);
    m_spGeneratorTask->AddInput(&m_kRadiusIS);
    m_spGeneratorTask->AddInput(&m_kSizeIS);
    m_spGeneratorTask->AddInput(&m_kColorIS);
    m_spGeneratorTask->AddInput(&m_kRotAngleIS);
    m_spGeneratorTask->AddInput(&m_kVelocityIS);
    m_spGeneratorTask->AddOutput(&m_kVertexOS);
    if (bNormalsPerParticle)
        m_spGeneratorTask->AddOutput(&m_kNormalOS);
    m_spGeneratorTask->AddOutput(&m_kColorOS);

    // Optionally create Animated Texture Floodgate task
    if (pkParticleSystem->HasAnimatedTextures())
    {
        m_pkTextureKernel = NiNew NiPSAlignedQuadTextureKernel();

        m_spTextureTask = NiSPTask::GetNewTask(3, 1);
        m_spTextureTask->SetKernel(m_pkTextureKernel);
        m_spTextureTask->AddInput(&m_kTextureKernelStructIS);
        m_spTextureTask->AddInput(&m_kAgeIS);
        m_spTextureTask->AddInput(&m_kLifeSpanIS);
        m_spTextureTask->AddInput(&m_kVarianceIS);
        m_spTextureTask->AddOutput(&m_kTexCoordOS);
    }

#ifdef _XENON
    // Set optimal block count for tasks.
    m_spGeneratorTask->SetOptimalBlockCount((NiUInt32) ceil(1536.0f /
        m_kColorIS.GetStride()));

#endif  // #ifdef _XENON

    m_bAttached = true;

    // Update the cached primitive count since POSITION & INDEX data streams
    // were added to the mesh.
    pkMesh->UpdateCachedPrimitiveCount();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::AttachAlignMethods(NiPSParticleSystem* pkParticleSystem)
{
    NiPSParticleSystem::AlignMethod eNormalMethod = pkParticleSystem->GetNormalMethod();
    NiPSParticleSystem::AlignMethod eUpMethod = pkParticleSystem->GetUpMethod();

    if (eNormalMethod & NiPSParticleSystem::ALIGN_PER_PARTICLE)
    {
        if (eUpMethod & NiPSParticleSystem::ALIGN_PER_PARTICLE)
        {
            m_pkGeneratorKernel = NiNew NiPSAlignedQuadGeneratorKernelVV();
        }
        else
        {
            m_pkGeneratorKernel = NiNew NiPSAlignedQuadGeneratorKernelVF();
        }
    }
    else
    {
        if (eUpMethod & NiPSParticleSystem::ALIGN_PER_PARTICLE)
        {
            m_pkGeneratorKernel = NiNew NiPSAlignedQuadGeneratorKernelFV();
        }
        else
        {
            m_pkGeneratorKernel = NiNew NiPSAlignedQuadGeneratorKernelFF();
        }
    }

    if (eNormalMethod == NiPSParticleSystem::ALIGN_LOCAL_POSITION)
    {
        // Position aligned
        m_kGeneratorKernelStruct.m_bNormalUsePosition = true;
    }
    else if (eNormalMethod == NiPSParticleSystem::ALIGN_LOCAL_VELOCITY)
    {
        // Velocity aligned
        m_kGeneratorKernelStruct.m_bNormalUsePosition = false;
        m_kVelocityIS.SetData(pkParticleSystem->GetVelocities());
    }

    if (eUpMethod == NiPSParticleSystem::ALIGN_LOCAL_POSITION)
    {
        // Position aligned
        m_kGeneratorKernelStruct.m_bUpUsePosition = true;
    }
    else if (eUpMethod == NiPSParticleSystem::ALIGN_LOCAL_VELOCITY)
    {
        // Velocity aligned
        m_kGeneratorKernelStruct.m_bUpUsePosition = false;
        m_kVelocityIS.SetData(pkParticleSystem->GetVelocities());
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::Detach(NiMesh*)
{
    m_spGeneratorTask = 0;
    NiDelete m_pkGeneratorKernel;

    m_spTextureTask = 0;
    NiDelete m_pkTextureKernel;

    m_b32BitIndices = false;
    m_bColors = false;
    m_bAttached = false;
    
    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::SubmitTasks(NiMesh* pkMesh,
    NiSyncArgs* pkArgs, NiSPWorkflowManager* pkWFManager)
{
    NIASSERT(pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_ANY ||
        pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE);
    
    // If a previous workflow is in flight from another culling operation, 
    // finish that workflow before restarting.
    if (m_spWorkflow != NULL)
    {
        CompleteTasks(pkMesh, pkArgs);
    }

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;
    NIASSERT(pkParticleSystem);

    // Get number of active particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();

    // Do not add task if there are no particles.
    if (uiNumParticles == 0)
    {
        return false;
    }
    NIASSERT(uiNumParticles < MAX_SUPPORTED_PARTICLES);

    const NiUInt32 uiVertexCount = uiNumParticles * 4;
    const NiUInt32 uiIndexCount = uiNumParticles * 6;

    // Update block count for Floodgate streams to the number of active
    // particles.
    m_kPositionIS.SetBlockCount(uiNumParticles);
    m_kRadiusIS.SetBlockCount(uiNumParticles);
    m_kSizeIS.SetBlockCount(uiNumParticles);
    m_kColorIS.SetBlockCount(uiNumParticles);
    m_kRotAngleIS.SetBlockCount(uiNumParticles);
    m_kVelocityIS.SetBlockCount(uiNumParticles);

    // Set region range for vertices.
    NiDataStreamRef* pkRef = NULL;
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiVertexCount);
    m_kVertexOS.SetBlockCount(uiNumParticles);

    // Set region range for indices.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiIndexCount);

    // Set region range for texture coordinates.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD());
    NIASSERT(pkRef);
    pkRef->SetActiveCount(0, uiVertexCount);

    // Set region range for colors.
    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::COLOR());
    if (pkRef)
    {
        pkRef->SetActiveCount(0, uiVertexCount);
        m_kColorOS.SetBlockCount(uiNumParticles);
    }

    if ((pkParticleSystem->GetNormalMethod() & NiPSParticleSystem::ALIGN_PER_PARTICLE) != 0)
    {
        m_kNormalOS.SetBlockCount(uiNumParticles);

        // Set region range for vertices.
        pkRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
        NIASSERT(pkRef);
        pkRef->SetActiveCount(0, uiVertexCount);
        m_kNormalOS.SetBlockCount(uiNumParticles);
    }
    else
    {
        // Set the normal explicitly, since there's only one piece of data and
        // there's no need to send it through NiFloodgate.
        NiDataStreamRef* pkNormRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
        NIASSERT(pkNormRef);

        NiDataStream* pkNorm = pkNormRef->GetDataStream();
        NIASSERT(pkNorm);

        NiPoint3* pkData = (NiPoint3*)pkNorm->LockWrite();
        NIASSERT(pkData);

        pkData[0] = m_kGeneratorKernelStruct.m_kNormal;

        pkNorm->UnlockWrite();
    }

    if(pkParticleSystem->HasAnimatedTextures())
    {
        // Update block count for Floodgate streams to the number of active particles.
        m_kAgeIS.SetBlockCount(uiNumParticles);
        m_kLifeSpanIS.SetBlockCount(uiNumParticles);
        m_kVarianceIS.SetBlockCount(uiNumParticles);

        // Set region range for texture coordinates.
        pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD());
        NIASSERT(pkRef);
        pkRef->SetActiveCount(0, uiVertexCount);
        m_kTexCoordOS.SetBlockCount(uiNumParticles);
    }

    // Tell the particle system to update its cached primitive count. This is
    // necessary because the primitive count is changing each frame.
    pkMesh->UpdateCachedPrimitiveCount();

    SubmitAlignMethods(pkParticleSystem, pkArgs);

    NiUInt32 uiTaskGroup = NiSyncArgs::GetTaskGroupID(
        NiSyncArgs::SYNC_VISIBLE, NiSyncArgs::SYNC_RENDER);
    m_spWorkflow = pkWFManager->AddRelatedTask(m_spGeneratorTask, uiTaskGroup, false);
    
    if (pkParticleSystem->HasAnimatedTextures())
    {
        m_spWorkflow->Add(m_spTextureTask);
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::SubmitAlignMethods(NiPSParticleSystem* pkParticleSystem,
    NiSyncArgs* pkArgs)
{
    NiPSParticleSystem::AlignMethod eNormalMethod = pkParticleSystem->GetNormalMethod();
    NiPSParticleSystem::AlignMethod eUpMethod = pkParticleSystem->GetUpMethod();

    NiPoint3 kUpDirection = pkParticleSystem->GetUpDirection();
    NiPoint3 kNormalDirection = pkParticleSystem->GetNormalDirection();

    // Stash camera values if needed.
    if (eNormalMethod == NiPSParticleSystem::ALIGN_CAMERA ||
        eUpMethod == NiPSParticleSystem::ALIGN_CAMERA)
    {
        // Get current camera.
        NIASSERT(pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE);
        NiCullingSyncArgs* pkCullingArgs = (NiCullingSyncArgs*) pkArgs;
        const NiCamera* pkCamera = pkCullingArgs->m_kCullingProcess.GetCamera();
        NIASSERT(pkCamera);

        NiPoint3 kCameraUp = pkCamera->GetWorldUpVector();

        if (pkParticleSystem->GetWorldSpace())
        {
            if (eUpMethod == NiPSParticleSystem::ALIGN_CAMERA)
                m_kGeneratorKernelStruct.m_kUp = kCameraUp;
            
            if (eNormalMethod == NiPSParticleSystem::ALIGN_CAMERA)
                m_kGeneratorKernelStruct.m_kNormal = 
                    pkCamera->GetWorldRightVector().Cross(kCameraUp);
        }
        else
        {
            NiMatrix3 kRotate;
            pkParticleSystem->GetWorldTransform().m_Rotate.Inverse(kRotate);
            
            if (eNormalMethod == NiPSParticleSystem::ALIGN_CAMERA)
            {            
                m_kGeneratorKernelStruct.m_kNormal = kRotate *
                    pkCamera->GetWorldRightVector().Cross(kCameraUp);
                m_kGeneratorKernelStruct.m_kNormal.Unitize();
            }

            if (eUpMethod == NiPSParticleSystem::ALIGN_CAMERA)
            {
                m_kGeneratorKernelStruct.m_kUp = kRotate * kCameraUp;
                m_kGeneratorKernelStruct.m_kUp.Unitize();
            }
        }
    }

    // Set the transforms to apply to the alignment directions.
    if (pkParticleSystem->GetWorldSpace())
    {
        switch (eNormalMethod)
        {
            case NiPSParticleSystem::ALIGN_LOCAL_FIXED:
            {
                // Transform the direction from local to world coordinates. Don't translate.
                m_kGeneratorKernelStruct.m_kNormal =
                    pkParticleSystem->GetOriginalWorldTransform().m_Rotate * kNormalDirection;
                m_kGeneratorKernelStruct.m_kNormal.Unitize(NiPoint3::UNIT_X);
            } break;
            
            case NiPSParticleSystem::ALIGN_LOCAL_POSITION:
            {
                // Position in the array is in world space, so it needs to be converted to
                // local space for use as the alignment direction, but then that direction needs
                // to be converted back into world space because the particle system transform
                // will be the identity except for scaling. THe following transform does the job.
                m_kGeneratorKernelStruct.m_kNormalTransform.m_Rotate.MakeIdentity();
                m_kGeneratorKernelStruct.m_kNormalTransform.m_fScale =
                    1.0f / pkParticleSystem->GetOriginalWorldTransform().m_fScale;
                m_kGeneratorKernelStruct.m_kNormalTransform.m_Translate =
                    pkParticleSystem->GetOriginalWorldTransform().m_Translate *
                    -m_kGeneratorKernelStruct.m_kNormalTransform.m_fScale; 
            } break;
            
            case NiPSParticleSystem::ALIGN_LOCAL_VELOCITY:
            {
                // Velocity in the array is in world space, so it needs to be converted to
                // local space for use as the alignment direction, but then that direction needs
                // to be converted back into world space because the particle system transform
                // will be the identity except for scaling. But note that we should not include
                // and translation in computing the velocity.
                m_kGeneratorKernelStruct.m_kNormalTransform.m_Rotate.MakeIdentity();
                m_kGeneratorKernelStruct.m_kNormalTransform.m_fScale =
                    1.0f / pkParticleSystem->GetOriginalWorldTransform().m_fScale;
                m_kGeneratorKernelStruct.m_kNormalTransform.m_Translate = NiPoint3::ZERO;
            } break;
                            
            default: // ALIGN_CAMERA handled above
                break;
        }

        switch (eUpMethod)
        {
            case NiPSParticleSystem::ALIGN_LOCAL_FIXED:
            {
                // Transform the direction from local to world coordinates. Don't translate.
                m_kGeneratorKernelStruct.m_kUp =
                    pkParticleSystem->GetOriginalWorldTransform().m_Rotate * kUpDirection;
                m_kGeneratorKernelStruct.m_kUp.Unitize(NiPoint3::UNIT_Z);
            } break;
            
            case NiPSParticleSystem::ALIGN_LOCAL_POSITION:
            {
                // Position in the array is in world space, so it needs to be converted to
                // local space for use as the alignment direction, but then that direction needs
                // to be converted back into world space because the particle system transform
                // will be the identity except for scaling. THe following transform does the job.
                m_kGeneratorKernelStruct.m_kUpTransform.m_Rotate.MakeIdentity();
                m_kGeneratorKernelStruct.m_kUpTransform.m_fScale =
                    1.0f / pkParticleSystem->GetOriginalWorldTransform().m_fScale;
                m_kGeneratorKernelStruct.m_kUpTransform.m_Translate =
                    pkParticleSystem->GetOriginalWorldTransform().m_Translate *
                    -m_kGeneratorKernelStruct.m_kUpTransform.m_fScale; 
            } break;
            
            case NiPSParticleSystem::ALIGN_LOCAL_VELOCITY:
            {
                // Velocity in the array is in world space, so it needs to be converted to
                // local space for use as the alignment direction, but then that direction needs
                // to be converted back into world space because the particle system transform
                // will be the identity except for scaling. But note that we should not include
                // and translation in computing the velocity.
                m_kGeneratorKernelStruct.m_kUpTransform.m_Rotate.MakeIdentity();
                m_kGeneratorKernelStruct.m_kUpTransform.m_fScale =
                    1.0f / pkParticleSystem->GetOriginalWorldTransform().m_fScale;
                m_kGeneratorKernelStruct.m_kUpTransform.m_Translate = NiPoint3::ZERO;
            } break;
                            
            default: // ALIGN_CAMERA handled above
                break;
        }
    }
    else
    {
        switch (eNormalMethod)
        {
            case NiPSParticleSystem::ALIGN_LOCAL_FIXED:
            {
                // Normal is already in correct coordinate system
                m_kGeneratorKernelStruct.m_kNormal = kNormalDirection;
                m_kGeneratorKernelStruct.m_kNormal.Unitize(NiPoint3::UNIT_X);
                break;
            }
            
            case NiPSParticleSystem::ALIGN_LOCAL_POSITION:
            case NiPSParticleSystem::ALIGN_LOCAL_VELOCITY:
            {
                m_kGeneratorKernelStruct.m_kNormalTransform.MakeIdentity();
            } break;
            
            default: // ALIGN_CAMERA handled above
                break;
        }

        switch (eUpMethod)
        {
            case NiPSParticleSystem::ALIGN_LOCAL_FIXED:
            {
                // Normal is already in correct coordinate system
                m_kGeneratorKernelStruct.m_kUp = kUpDirection;
                m_kGeneratorKernelStruct.m_kUp.Unitize(NiPoint3::UNIT_Z);
                break;
            }
            
            case NiPSParticleSystem::ALIGN_LOCAL_POSITION:
            case NiPSParticleSystem::ALIGN_LOCAL_VELOCITY:
            {
                m_kGeneratorKernelStruct.m_kUpTransform.MakeIdentity();
            } break;
            
            default: // ALIGN_CAMERA handled above
                break;
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::CompleteTasks(NiMesh*, NiSyncArgs*)
{
    if (m_spWorkflow)
    {
        NiStreamProcessor::Get()->Wait(m_spWorkflow);
        m_spWorkflow = 0;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::IsComplete(NiMesh* /*pkMesh*/, NiSyncArgs* /*pkArgs*/)
{
    if (m_spWorkflow != NULL)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::AreRequirementsMet(
    NiMesh* pkMesh, NiSystemDesc::RendererID) const
{
    if (!NiIsKindOf(NiPSParticleSystem, pkMesh))
    {
        return false;
    }
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

    const bool b16BitIndices = (pkParticleSystem->GetMaxNumParticles() <
        MAX_PARTICLES_FOR_16BIT_INDICES);

    const NiDataStreamRef* pkRef = pkMesh->FindStreamRef(
        NiCommonSemantics::INDEX());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX,
        NiObject::CLONE_SHARE,
        b16BitIndices ? NiDataStreamElement::F_UINT16_1 :
            NiDataStreamElement::F_UINT32_1))
    {
        NILOG("NiPSAlignedQuadGenerator> Indices are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::POSITION());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_FLOAT32_3))
    {
        NILOG("NiPSAlignedQuadGenerator> Positions are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::NORMAL());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_FLOAT32_3))
    {
        NILOG("NiPSAlignedQuadGenerator> Normals are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        (pkParticleSystem->HasAnimatedTextures() ? NiObject::CLONE_BLANK_COPY : 
            NiObject::CLONE_SHARE),
        NiDataStreamElement::F_FLOAT32_2))
    {
        NILOG("NiPSAlignedQuadGenerator> TexCoords are wrong format.\n");
        return false;
    }

    pkRef = pkMesh->FindStreamRef(NiCommonSemantics::COLOR());
    if (pkRef && !ValidateStream(pkRef, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
        NiDataStream::USAGE_VERTEX,
        NiObject::CLONE_BLANK_COPY,
        NiDataStreamElement::F_NORMUINT8_4))
    {
        NILOG("NiPSAlignedQuadGenerator> Colors are wrong format.\n");
        return false;
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::ValidateStream(const NiDataStreamRef* pkRef,
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, 
    NiObject::CloningBehavior eCloningBehavior, 
    NiDataStreamElement::Format eFormat)
{
    NIASSERT(pkRef);
    return pkRef->GetUsage() == eUsage &&
        pkRef->GetAccessMask() & uiAccessMask &&
        pkRef->GetElementDescCount() == 1 &&
        pkRef->GetElementDescAt(0).GetFormat() == eFormat &&
        pkRef->GetDataStream() != NULL && 
        pkRef->GetDataStream()->GetCloningBehavior() == eCloningBehavior;
}
//--------------------------------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::RetrieveRequirements(NiMeshRequirements&
    kRequirements) const
{
    for (NiUInt32 ui = 0; ui < 2; ui++)
    {
        NiUInt32 uiSet = kRequirements.CreateNewRequirementSet();

        // Add INDEX requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::INDEX(), 0, 
            NiMeshRequirements::STRICT_INTERLEAVE, 0, NiDataStream::
            USAGE_VERTEX_INDEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, m_b32BitIndices ?
            NiDataStreamElement::F_UINT32_1 : NiDataStreamElement::F_UINT16_1);

        // Add POSITION requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::POSITION(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 1, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE, NiDataStreamElement::
            F_FLOAT32_3);

        // Add NORMAL requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::NORMAL(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 2, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE, NiDataStreamElement::
            F_FLOAT32_3);

        // Add TEXCOORD requirement.
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::TEXCOORD(), 0,
            NiMeshRequirements::STRICT_INTERLEAVE, 3, NiDataStream::
            USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
            NiDataStreamElement::F_FLOAT32_2);

        // Add COLOR requirement.
        if (ui == 0)
        {
            kRequirements.AddRequirement(uiSet, NiCommonSemantics::COLOR(), 0,
                NiMeshRequirements::STRICT_INTERLEAVE, 4, NiDataStream::
                USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ | 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
                NiDataStreamElement::F_NORMUINT8_4);
        }
    }
}
//--------------------------------------------------------------------------------------------------
NiDataStream* NiPSAlignedQuadGenerator::AddDataStream(
    NiMesh* pkMesh,
    const NiFixedString& kSemantic,
    NiUInt32 uiSemanticIndex,
    NiDataStreamElement::Format eFormat,
    NiUInt32 uiCount,
    NiUInt8 uiAccessMask,
    NiDataStream::Usage eUsage,
    NiObject::CloningBehavior eCloningBehavior,
    size_t stElementSize,
    const void* pvData)
{
    NI_UNUSED_ARG(stElementSize);
    NIASSERT(pkMesh);

    NiDataStream* pkDataStream = NULL;

    NiDataStreamRef* pkStreamRef = pkMesh->FindStreamRef(kSemantic,
        uiSemanticIndex, eFormat);
    if (pkStreamRef)
    {
        pkDataStream = pkStreamRef->GetDataStream();
        NIASSERT(pkDataStream);

        // Assert that the stream found matches the one being asked for.
        NIASSERT(pkDataStream->GetAccessMask() & uiAccessMask);
        NIASSERT(pkDataStream->GetUsage() == eUsage);
        NIASSERT(pkDataStream->GetSize() == uiCount * stElementSize);
        NIASSERT(pkDataStream->GetCloningBehavior() == eCloningBehavior);
    }
    else
    {
        // Create data stream.
        pkDataStream = NiDataStream::CreateSingleElementDataStream(eFormat,
            uiCount, uiAccessMask, eUsage, pvData);
        NIASSERT(pkDataStream);
        pkDataStream->SetCloningBehavior(eCloningBehavior);

        // Create stream ref.
        pkStreamRef = pkMesh->AddStreamRef(pkDataStream, kSemantic,
            uiSemanticIndex);
        NIASSERT(pkStreamRef);
        pkStreamRef->BindRegionToSubmesh(0, 0);
        pkStreamRef->SetActiveCount(0, 0);
    }

    NIASSERT(pkDataStream);

    pkDataStream->SetStreamable(false);

    return pkDataStream;
}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Cloning
//--------------------------------------------------------------------------------------------------
NiImplementCreateClone(NiPSAlignedQuadGenerator);
//---------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::CopyMembers(
    NiPSAlignedQuadGenerator* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_kGeneratorKernelStruct.m_fScaleAmountU = m_kGeneratorKernelStruct.m_fScaleAmountU;
    pkDest->m_kGeneratorKernelStruct.m_fScaleLimitU = m_kGeneratorKernelStruct.m_fScaleLimitU;
    pkDest->m_kGeneratorKernelStruct.m_fScaleRestU = m_kGeneratorKernelStruct.m_fScaleRestU;
    pkDest->m_kGeneratorKernelStruct.m_fScaleAmountV = m_kGeneratorKernelStruct.m_fScaleAmountV;
    pkDest->m_kGeneratorKernelStruct.m_fScaleLimitV = m_kGeneratorKernelStruct.m_fScaleLimitV;
    pkDest->m_kGeneratorKernelStruct.m_fScaleRestV = m_kGeneratorKernelStruct.m_fScaleRestV;

    NormalizeScaleAmountU();
    NormalizeScaleAmountV();

    pkDest->m_kGeneratorKernelStruct.m_fCenterU = m_kGeneratorKernelStruct.m_fCenterU;
    pkDest->m_kGeneratorKernelStruct.m_fCenterV = m_kGeneratorKernelStruct.m_fCenterV;

    pkDest->m_kTextureKernelStruct.m_bUVScrolling = m_kTextureKernelStruct.m_bUVScrolling;
    pkDest->m_kTextureKernelStruct.m_uiNumFramesAcross = 
        m_kTextureKernelStruct.m_uiNumFramesAcross;
    pkDest->m_kTextureKernelStruct.m_uiNumFramesDown = 
        m_kTextureKernelStruct.m_uiNumFramesDown;
    pkDest->m_kTextureKernelStruct.m_bPingPong = m_kTextureKernelStruct.m_bPingPong;
    pkDest->m_kTextureKernelStruct.m_uiInitialFrame = m_kTextureKernelStruct.m_uiInitialFrame;
    pkDest->m_kTextureKernelStruct.m_fInitialFrameVar = m_kTextureKernelStruct.m_fInitialFrameVar;
    pkDest->m_kTextureKernelStruct.m_uiNumFrames = m_kTextureKernelStruct.m_uiNumFrames;
    pkDest->m_kTextureKernelStruct.m_fNumFramesVar = m_kTextureKernelStruct.m_fNumFramesVar;
    pkDest->m_kTextureKernelStruct.m_fInitialTime = m_kTextureKernelStruct.m_fInitialTime;
    pkDest->m_kTextureKernelStruct.m_fFinalTime = m_kTextureKernelStruct.m_fFinalTime;
}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiPSAlignedQuadGenerator);
//--------------------------------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleAmountU);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleLimitU);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleRestU);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleAmountV);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleLimitV);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fScaleRestV);

    NormalizeScaleAmountU();
    NormalizeScaleAmountV();

    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fCenterU);
    NiStreamLoadBinary(kStream, m_kGeneratorKernelStruct.m_fCenterV);

    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_bUVScrolling);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_uiNumFramesAcross);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_uiNumFramesDown);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_bPingPong);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_uiInitialFrame);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_fInitialFrameVar);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_uiNumFrames);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_fNumFramesVar);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_fInitialTime);
    NiStreamLoadBinary(kStream, m_kTextureKernelStruct.m_fFinalTime);
}
//--------------------------------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::RegisterStreamables(NiStream& kStream)
{
    return NiMeshModifier::RegisterStreamables(kStream);
}
//--------------------------------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleAmountU);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleLimitU);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleRestU);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleAmountV);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleLimitV);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fScaleRestV);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fCenterU);
    NiStreamSaveBinary(kStream, m_kGeneratorKernelStruct.m_fCenterV);

    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_bUVScrolling);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_uiNumFramesAcross);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_uiNumFramesDown);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_bPingPong);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_uiInitialFrame);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_fInitialFrameVar);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_uiNumFrames);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_fNumFramesVar);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_fInitialTime);
    NiStreamSaveBinary(kStream, m_kTextureKernelStruct.m_fFinalTime);
}
//--------------------------------------------------------------------------------------------------
bool NiPSAlignedQuadGenerator::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
        return false;
        
    NiPSAlignedQuadGenerator* pkOther = (NiPSAlignedQuadGenerator*)pkObject;
    
    if (m_kGeneratorKernelStruct.m_fScaleAmountU != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleAmountU ||
        m_kGeneratorKernelStruct.m_fScaleLimitU != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleLimitU ||
        m_kGeneratorKernelStruct.m_fScaleRestU != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleRestU ||
        m_kGeneratorKernelStruct.m_fScaleAmountV != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleAmountV ||
        m_kGeneratorKernelStruct.m_fScaleLimitV != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleLimitV ||
        m_kGeneratorKernelStruct.m_fScaleRestV != 
        pkOther->m_kGeneratorKernelStruct.m_fScaleRestV ||
        m_kGeneratorKernelStruct.m_fCenterU != 
        pkOther->m_kGeneratorKernelStruct.m_fCenterU ||
        m_kGeneratorKernelStruct.m_fCenterV != 
        pkOther->m_kGeneratorKernelStruct.m_fCenterV ||
        m_kTextureKernelStruct.m_bUVScrolling != 
        pkOther->m_kTextureKernelStruct.m_bUVScrolling ||
        m_kTextureKernelStruct.m_uiNumFramesAcross != 
        pkOther->m_kTextureKernelStruct.m_uiNumFramesAcross ||
        m_kTextureKernelStruct.m_uiNumFramesDown != 
        pkOther->m_kTextureKernelStruct.m_uiNumFramesDown ||
        m_kTextureKernelStruct.m_bPingPong != 
        pkOther->m_kTextureKernelStruct.m_bPingPong ||
        m_kTextureKernelStruct.m_uiInitialFrame != 
        pkOther->m_kTextureKernelStruct.m_uiInitialFrame ||
        m_kTextureKernelStruct.m_fInitialFrameVar != 
        pkOther->m_kTextureKernelStruct.m_fInitialFrameVar ||
        m_kTextureKernelStruct.m_uiNumFrames != 
        pkOther->m_kTextureKernelStruct.m_uiNumFrames ||
        m_kTextureKernelStruct.m_fNumFramesVar != 
        pkOther->m_kTextureKernelStruct.m_fNumFramesVar ||
        m_kTextureKernelStruct.m_fInitialTime != 
        pkOther->m_kTextureKernelStruct.m_fInitialTime ||
        m_kTextureKernelStruct.m_fFinalTime != 
        pkOther->m_kTextureKernelStruct.m_fFinalTime)
    {
        return false;
    }
    
    return true;
}
//--------------------------------------------------------------------------------------------------
// Viewer strings
//--------------------------------------------------------------------------------------------------
void NiPSAlignedQuadGenerator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSAlignedQuadGenerator::ms_RTTI.GetName()));
        
    pkStrings->Add(NiGetViewerString("ScaleAmountU", m_kGeneratorKernelStruct.m_fScaleAmountU));
    pkStrings->Add(NiGetViewerString("ScaleLimitU", m_kGeneratorKernelStruct.m_fScaleLimitU));
    pkStrings->Add(NiGetViewerString("ScaleRestU", m_kGeneratorKernelStruct.m_fScaleRestU));
    pkStrings->Add(NiGetViewerString("ScaleAmountV", m_kGeneratorKernelStruct.m_fScaleAmountV));
    pkStrings->Add(NiGetViewerString("ScaleLimitV", m_kGeneratorKernelStruct.m_fScaleLimitV));
    pkStrings->Add(NiGetViewerString("ScaleRestV", m_kGeneratorKernelStruct.m_fScaleRestV));
    pkStrings->Add(NiGetViewerString("CenterU", m_kGeneratorKernelStruct.m_fCenterU));    
    pkStrings->Add(NiGetViewerString("CenterV", m_kGeneratorKernelStruct.m_fCenterV));

    pkStrings->Add(NiGetViewerString("UVScrolling", m_kTextureKernelStruct.m_bUVScrolling));
    pkStrings->Add(NiGetViewerString("NumFramesAcross",
        m_kTextureKernelStruct.m_uiNumFramesAcross));
    pkStrings->Add(NiGetViewerString("NumFramesDown", 
        m_kTextureKernelStruct.m_uiNumFramesDown));
    pkStrings->Add(NiGetViewerString("PingPong", m_kTextureKernelStruct.m_bPingPong));
    pkStrings->Add(NiGetViewerString("InitialFrame", m_kTextureKernelStruct.m_uiInitialFrame));
    pkStrings->Add(NiGetViewerString("InitialFrameVar", m_kTextureKernelStruct.m_fInitialFrameVar));
    pkStrings->Add(NiGetViewerString("NumFrames", m_kTextureKernelStruct.m_uiNumFrames));
    pkStrings->Add(NiGetViewerString("NumFramesVar", m_kTextureKernelStruct.m_fNumFramesVar));
    pkStrings->Add(NiGetViewerString("InitialTime", m_kTextureKernelStruct.m_fInitialTime));
    pkStrings->Add(NiGetViewerString("FinalTime", m_kTextureKernelStruct.m_fFinalTime));
}
//--------------------------------------------------------------------------------------------------
