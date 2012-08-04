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


#include "NiPhysXPCH.h"

#include <efd/utf8string.h>
#include <efdPhysX/PhysXSDKManager.h>

#include "NiPhysXFileStream.h"
#include "NiPhysXClothMesh.h"
#include "NiPhysXMeshDesc.h"

using namespace efd;
using namespace efdPhysX;

//---------------------------------------------------------------------------
NxClothMesh* NiPhysXClothMesh::Create(const NiFixedString& kFilename)
{
    NiFixedString kFinalName = kFilename;
    NiStandardizeFilePath(kFinalName);
    NiPhysXMeshDesc::ConvertFilenameToPlatformSpecific(kFinalName, kFinalName);

    PhysXSDKManager* pkManager = PhysXSDKManager::GetManager();

    utf8string kMeshName = (const efd::Char*)kFinalName;
    NxClothMesh* pkResult = pkManager->GetClothMesh(kMeshName);
    
    if (pkResult)
        return pkResult;

    NiPhysXFileStream* pkStream =
        NiNew NiPhysXFileStream(kFinalName, NiFile::READ_ONLY);
    if (!pkStream->Ready())
    {
        size_t stSize = strlen((const char *)kFinalName) + 256;
        char* pcMsg = (char*)NiMalloc(sizeof(char) * stSize);
        NiSprintf(pcMsg, stSize,
            "NiPhysXClothMesh::CreateMesh cannot open file %s\n",
            (const efd::Char*)kFinalName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        NiDelete pkStream;
        return 0;
    }

    pkManager->WaitSDKLock();
    pkResult = pkManager->m_pPhysXSDK->createClothMesh(*pkStream);
    pkManager->ReleaseSDKLock();

    if (!pkResult)
    {
        size_t stSize = strlen((const char *)kFinalName) + 256;
        char* pcMsg = (char*)NiMalloc(sizeof(char) * stSize);
        NiSprintf(pcMsg, stSize,
            "NiPhysXClothMesh::CreateMesh mesh creation failed for file %s\n",
            (const efd::Char*)kFinalName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        NiDelete pkStream;
        return 0;
    }

    pkManager->SetClothMesh(kMeshName, pkResult);

    NiDelete pkStream;

    return pkResult;
}
//---------------------------------------------------------------------------
NxClothMesh* NiPhysXClothMesh::Create(NiPhysXMemStream& kStream,
    const NiFixedString& kName)
{
    PhysXSDKManager* pkManager = PhysXSDKManager::GetManager();

    utf8string kMeshName = (const efd::Char*)kName;
    NxClothMesh* pkResult = pkManager->GetClothMesh(kMeshName);
    
    if (pkResult)
        return pkResult;

    pkManager->WaitSDKLock();
    pkResult = pkManager->m_pPhysXSDK->createClothMesh(kStream);
    pkManager->ReleaseSDKLock();

    if (!pkResult)
    {
        size_t stSize = strlen((const char *)kName) + 256;
        char* pcMsg = (char*)NiMalloc(sizeof(char) * stSize);
        NiSprintf(pcMsg, stSize,
            "NiPhysXClothMesh::CreateMesh mesh creation failed for mesh %s\n",
            (const efd::Char*)kName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        return 0;
    }

    pkManager->SetClothMesh(kMeshName, pkResult);

    return pkResult;
}
//---------------------------------------------------------------------------
float NiPhysXClothMesh::ComputeArea(NxClothMesh* pkMesh)
{
    NxClothMeshDesc kClothMeshDesc;
    pkMesh->saveToDesc(kClothMeshDesc);

    NIASSERT(kClothMeshDesc.pointStrideBytes == sizeof(NxVec3));

    float fArea = 0.0f;
    NxVec3* pkVertices = (NxVec3*)kClothMeshDesc.points;
    for (NiUInt32 ui = 0; ui < kClothMeshDesc.numTriangles; ui++)
    {
        // The returned descriptor seems to have 32 bit indices, even though
        // the flags say 16 bit and the mesh was created with 16 bit.

        NiUInt32 uiI0 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui);
        NiUInt32 uiI1 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui +
                kClothMeshDesc.triangleStrideBytes / 3);
        NiUInt32 uiI2 = *(NiUInt32*)(
                ((char*)kClothMeshDesc.triangles) +
                kClothMeshDesc.triangleStrideBytes * ui +
                2 * kClothMeshDesc.triangleStrideBytes / 3);

        NxVec3 kE1 = pkVertices[uiI1] - pkVertices[uiI0];
        NxVec3 kE2 = pkVertices[uiI2] - pkVertices[uiI0];

        NxVec3 kCross = kE1 ^ kE2;
        fArea += 0.5f * kCross.magnitude();
    }

    return fArea;
}
//---------------------------------------------------------------------------
