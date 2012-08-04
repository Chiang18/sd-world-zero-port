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

#include <NiFilename.h>

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

using namespace efdPhysX;

NiImplementRTTI(NiPhysXMeshDesc, NiObject);

char* NiPhysXMeshDesc::ms_pcPlatformSubDir;

//---------------------------------------------------------------------------
void NiPhysXMeshDesc::SDMInit()
{
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::SDMShutdown()
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
NiPhysXMeshDesc::NiPhysXMeshDesc() : m_kMeshName(NULL)
{
    m_uiMeshFlags = 0;
    m_eMeshPagingMode = NX_MESH_PAGING_MANUAL;
    m_uiMeshSize = 0;
    m_pucMeshData = 0;
    m_usVertexMapSize = 0;
    m_pusVertexMap = 0;

    SetIsConvex();
    m_pkConvexMesh = 0;
    SetBit(false, COOKED_FOR_WIN32_MASK);
    SetBit(false, COOKED_FOR_PS3_MASK);
    SetBit(false, COOKED_FOR_XENON_MASK);
    SetHardware(false);
}
//---------------------------------------------------------------------------
NiPhysXMeshDesc::~NiPhysXMeshDesc()
{
    NiFree(m_pucMeshData);
    
    // Remove the mesh from the cache.
    PhysXSDKManager* pkManager = PhysXSDKManager::GetManager();
    if (pkManager)
    {
        if (GetBit(IS_CONVEX_MASK) && m_pkConvexMesh)
        {
            efd::utf8string kMeshName =
                pkManager->GetConvexMeshName(m_pkConvexMesh);
            pkManager->RemoveConvexMesh(kMeshName);
        }
        else if (GetBit(IS_CLOTH_MASK) && m_pkClothMesh)
        {
            efd::utf8string kMeshName =
                pkManager->GetClothMeshName(m_pkClothMesh);
            pkManager->RemoveClothMesh(kMeshName);
        }
        else if (m_pkTriangleMesh)
        {
            efd::utf8string kMeshName =
                pkManager->GetTriangleMeshName(m_pkTriangleMesh);
            pkManager->RemoveTriangleMesh(kMeshName);
        }
    }

    NiFree(m_pusVertexMap);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiPhysXMeshDesc::ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
    const bool bKeepData, const bool bCheckPlatform)
{
    NIASSERT(GetBit(IS_CONVEX_MASK) && !GetBit(IS_CLOTH_MASK));
    
    if (bCheckPlatform && !IsValidPlatform())
    {
        const char *pcName =
            m_kMeshName.Exists() ? (const char*)m_kMeshName : "<null>";
        size_t stMsgSize = strlen(pcName) + 256;
        char* pcMsg = NiAlloc(char, stMsgSize);
        NiSprintf(pcMsg, stMsgSize, "NiPhysXMeshDesc::ToConvexDesc: "
            "PhysX Convex mesh %s was cooked for a different platform\n",
            pcName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        kConvexDesc.meshData = 0;
        return;
    }

    // Try creating from data or loading it from file
    if (m_uiMeshSize)
    {
        NiPhysXMemStream kMemStream(m_pucMeshData, (NiUInt32)m_uiMeshSize);
        m_pkConvexMesh = NiPhysXConvexMesh::Create(kMemStream, m_kMeshName);
        if (!bKeepData)
        {
            NiFree(m_pucMeshData);
            m_pucMeshData = 0;
            m_uiMeshSize = 0;
        }
    }
    else
    {
        m_pkConvexMesh = NiPhysXConvexMesh::Create(m_kMeshName);
    }

    kConvexDesc.meshData = m_pkConvexMesh;
    kConvexDesc.meshFlags = m_uiMeshFlags;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::ToTriMeshDesc(NxTriangleMeshShapeDesc& kTriMeshDesc, 
    const bool bKeepData, const bool bCheckPlatform)
{
    NIASSERT(!GetBit(IS_CONVEX_MASK) && !GetBit(IS_CLOTH_MASK));
    
    if (bCheckPlatform && !IsValidPlatform())
    {
        const char *pcName =
            m_kMeshName.Exists() ? (const char*)m_kMeshName : "<null>";
        size_t stMsgSize = strlen(pcName) + 256;
        char* pcMsg = NiAlloc(char, stMsgSize);
        NiSprintf(pcMsg, stMsgSize, "NiPhysXMeshDesc::ToTriMeshDesc: "
            "PhysX Triangle mesh %s was cooked for a different platform\n",
            pcName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        kTriMeshDesc.meshData = 0;
        return;
    }

    // Try creating from data or loading it from file
    if (m_uiMeshSize)
    {
        NiPhysXMemStream kMemStream(m_pucMeshData, (NiUInt32)m_uiMeshSize);
        m_pkTriangleMesh = NiPhysXTriangleMesh::Create(kMemStream, m_kMeshName);
        if (!bKeepData)
        {
            NiFree(m_pucMeshData);
            m_pucMeshData = 0;
            m_uiMeshSize = 0;
        }
    }
    else
    {
        m_pkTriangleMesh = NiPhysXTriangleMesh::Create(m_kMeshName);
    }

    kTriMeshDesc.meshData = m_pkTriangleMesh;
    kTriMeshDesc.meshFlags = m_uiMeshFlags;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::ToClothDesc(NxClothDesc& kClothDesc,
    const bool bKeepData, const bool bCheckPlatform)
{
    NIASSERT(!GetBit(IS_CONVEX_MASK) && GetBit(IS_CLOTH_MASK));
    
    if (bCheckPlatform && !IsValidPlatform())
    {
        const char *pcName =
            m_kMeshName.Exists() ? (const char*)m_kMeshName : "<null>";
        size_t stMsgSize = strlen(pcName) + 256;
        char* pcMsg = NiAlloc(char, stMsgSize);
        NiSprintf(pcMsg, stMsgSize, "NiPhysXMeshDesc::ToClothDesc: "
            "PhysX Cloth mesh %s was cooked for a different platform\n",
            pcName);
        NiOutputDebugString(pcMsg);
        NiFree(pcMsg);
        kClothDesc.clothMesh = 0;
        return;
    }

    // Try creating from data or loading it from file
    if (m_uiMeshSize)
    {
        NiPhysXMemStream kMemStream(m_pucMeshData, (NiUInt32)m_uiMeshSize);
        m_pkClothMesh = NiPhysXClothMesh::Create(kMemStream, m_kMeshName);
        if (!bKeepData)
        {
            NiFree(m_pucMeshData);
            m_pucMeshData = 0;
            m_uiMeshSize = 0;
        }
    }
    else
    {
        m_pkClothMesh = NiPhysXClothMesh::Create(m_kMeshName);
    }

    kClothDesc.clothMesh = m_pkClothMesh;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshDesc::IsValidPlatform() const
{
    if (!GetBit(COOKED_FOR_WIN32_MASK) &&
        !GetBit(COOKED_FOR_PS3_MASK) &&
        !GetBit(COOKED_FOR_XENON_MASK))
    {
        // Err on side of accept if there is no information. This maintains
        // back compatability of files.
        return true;
    }
    switch (NiSystemDesc::GetSystemDesc().GetPlatformID())
    {
        case NiSystemDesc::NI_WIN32:
            if (GetBit(COOKED_FOR_WIN32_MASK))
                return true;
            break;
        case NiSystemDesc::NI_PS3:
            if (GetBit(COOKED_FOR_PS3_MASK))
                return true;
            break;
        case NiSystemDesc::NI_XENON:
            if (GetBit(COOKED_FOR_XENON_MASK))
                return true;
            break;
        default:
            return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXMeshDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;
    
    kStream.RegisterFixedString(m_kMeshName);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kMeshName);
    NiStreamSaveBinary(kStream, m_uiMeshSize);
    if (m_uiMeshSize)
    {
        NiStreamSaveBinary(kStream, m_pucMeshData, 
            (unsigned int)m_uiMeshSize);
    }
    NiStreamSaveBinary(kStream, m_usVertexMapSize);
    for (NiUInt16 us = 0; us < m_usVertexMapSize; us++)
    {
        NiStreamSaveBinary(kStream, m_pusVertexMap[us]);
    }
    NiStreamSaveBinary(kStream, m_uiMeshFlags);
    NiStreamSaveEnum(kStream, m_eMeshPagingMode);

    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshDesc);
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() < kStream.GetVersion(20, 3, 0, 5))
    {
        NiBool bConvex;
        NiStreamLoadBinary(kStream, bConvex);
        if (bConvex)
            SetIsConvex();
        else
            SetIsTriMesh();
    }

    kStream.LoadFixedString(m_kMeshName);
    NiStreamLoadBinary(kStream, m_uiMeshSize);
    if (m_uiMeshSize)
    {
        m_pucMeshData = (unsigned char*)NiMalloc(m_uiMeshSize);
        NiStreamLoadBinary(kStream, m_pucMeshData,
            (unsigned int)m_uiMeshSize);
    }

    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 5))
    {
        NiStreamLoadBinary(kStream, m_usVertexMapSize);
        if (m_usVertexMapSize)
        {
            m_pusVertexMap = NiAlloc(NiUInt16, m_usVertexMapSize);
        }
        for (NiUInt16 us = 0; us < m_usVertexMapSize; us++)
        {
            NiStreamLoadBinary(kStream, m_pusVertexMap[us]);
        }
    }
    
    NiStreamLoadBinary(kStream, m_uiMeshFlags);
    
    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 1))
    {
        NiStreamLoadEnum(kStream, m_eMeshPagingMode);
    }
    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 2))
    {
        if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 5))
        {
            NiStreamLoadBinary(kStream, m_uFlags);
        }
        else
        {
            NiBool bHardware;
            NiStreamLoadBinary(kStream, bHardware);
            SetHardware(bHardware ? true : false);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMeshDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXMeshDesc* pkNewObject = (NiPhysXMeshDesc*)pkObject;

    if (m_kMeshName != pkNewObject->m_kMeshName ||
        m_uiMeshFlags != pkNewObject->m_uiMeshFlags ||
        m_eMeshPagingMode != pkNewObject->m_eMeshPagingMode ||
        m_uFlags != pkNewObject->m_uFlags)
    {
        return false;
    }

    if (pkNewObject->m_usVertexMapSize != m_usVertexMapSize)
        return false;
    for (NiUInt16 us = 0; us < m_usVertexMapSize; us++)
    {
        if (m_pusVertexMap[us] != pkNewObject->m_pusVertexMap[us])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NiPhysXMeshDesc::GetPlatformSpecificSubdirectory() 
{
    return ms_pcPlatformSubDir;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::SetPlatformSpecificSubdirectory(const char* pcSubDir)
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }

    if (pcSubDir)
    {
        size_t stLen = strlen(pcSubDir) + 1;
        ms_pcPlatformSubDir = NiAlloc(char, stLen);
        NiStrcpy(ms_pcPlatformSubDir, stLen, pcSubDir);
        NiPath::Standardize(ms_pcPlatformSubDir);
    }
    else
    {
        ms_pcPlatformSubDir = NULL;
    }
}
//---------------------------------------------------------------------------
char* NiPhysXMeshDesc::ConvertFilenameToPlatformSpecific(const char* pcPath)
{
    char* pcPlatformPath = NiAlloc(char, 260);
    if (ms_pcPlatformSubDir)
    {
        NiFilename kPlatform(pcPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(pcPlatformPath, 260);
    }
    else
    {
        NiStrncpy(pcPlatformPath, 260, pcPath, strlen(pcPath));
    }
    return pcPlatformPath;
}
//---------------------------------------------------------------------------
void NiPhysXMeshDesc::ConvertFilenameToPlatformSpecific(
    const NiFixedString& kPath, NiFixedString& kPlatformSpecificPath)
{
    if (ms_pcPlatformSubDir)
    {
        char acPlatformPath[260];
        NiFilename kPlatform((const char*)kPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(acPlatformPath, 260);
        kPlatformSpecificPath = acPlatformPath;
    }
    else
    {
        kPlatformSpecificPath = kPath;
    }
}
//---------------------------------------------------------------------------
