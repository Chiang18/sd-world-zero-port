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

#include "NiTerrain.h"
#include "NiTerrainSector.h"
#include "NiSurfaceMaskPixel.h"
#include "NiTerrainInteractor.h"
#include "NiTerrainConfiguration.h"
#include "NiTerrainXMLHelpers.h"
#include "NiTerrainCullingProcess.h"

#include <NiFixedString.h>

using namespace efd;

NiFogPropertyPtr NiTerrain::ms_spFogProperty = NULL;
NiTerrainConfiguration NiTerrain::ms_kDefaultConfiguration;

NiImplementRTTI(NiTerrain, NiNode);
//---------------------------------------------------------------------------
NiTerrain::NiTerrain(NiMetaDataStore* pkMetaDataStore) :
    m_pkMetaDataStore(pkMetaDataStore),
    m_spCustomSurfacePalette(0),
    m_bHasShapeChangedLastUpdate(false),
    m_kArchivePath(),
    m_uiBlockSize(32),
    m_uiNumLOD(5),
    m_fLODScale(NiSqrt(2.0f) * 2.0f),
    m_fLODShift(0.0f),
    m_uiLODMode(NiTerrainSectorData::LOD_MODE_2D |
        NiTerrainSectorData::LOD_MORPH_ENABLE),
    m_pkTerrainInteractor(NULL),
    m_kSectors(31)
{
    // Initial sector map size of 31, leaning toward optimal situation with 3^2
    // loaded sector. ie, 2 sectors in all directions around the active sector

    m_auiSectors[0] = 1;
    m_auiSectors[1] = 1;

    NiMetaDataStore::GetInstance();

    Initialize();
}
//---------------------------------------------------------------------------
void NiTerrain::Initialize()
{
    if (m_pkMetaDataStore == 0) 
    {
        m_pkMetaDataStore = NiMetaDataStore::GetInstance();
    }

    // Initial pool size
    m_pkTerrainInteractor = NiNew NiTerrainInteractor(2000);
    m_pkTerrainInteractor->SetTerrain(this);
}
//---------------------------------------------------------------------------
NiTerrain::~NiTerrain() 
{
    m_spCustomSurfacePalette = 0;
    m_kSectors.RemoveAll();

    NiDelete m_pkTerrainInteractor;
}
//---------------------------------------------------------------------------
void NiTerrain::_SDMInit()
{
    ms_spFogProperty = NiNew NiFogProperty();    
    ms_spFogProperty->SetFogFunction(NiFogProperty::FOG_RANGE_SQ);
    ms_spFogProperty->SetFogColor(NiColor(1.0f, 1.0f, 1.0f));
    ms_spFogProperty->SetFog(true);
    ms_spFogProperty->SetDepth(0.95f);
}
//---------------------------------------------------------------------------
void NiTerrain::_SDMShutdown()
{
    ms_spFogProperty = NULL;
}
//---------------------------------------------------------------------------
NiFogProperty* NiTerrain::GetGlobalFogProperty()
{
    return ms_spFogProperty;
}
//---------------------------------------------------------------------------
void NiTerrain::SetGlobalFogProperty(NiFogProperty* pkFogProp)
{
    NIASSERT(pkFogProp);
    ms_spFogProperty = pkFogProp;
}
//---------------------------------------------------------------------------
const NiFixedString NiTerrain::CreateSurfacePath(
    const NiFixedString& kIndexPath)
{
    NiString kTerrainPath = kIndexPath;
    NiFixedString kSurfaceRoot;

#ifndef _PS3
    kSurfaceRoot = kTerrainPath + "\\..\\";
#else
    kSurfaceRoot = kTerrainPath + "/../";
#endif
    
    if (NiPath::IsRelative(kSurfaceRoot))
    {
        char* pcAbsolutePath = NiAlloc(char, 512);
        NiPath::ConvertToAbsolute(
            pcAbsolutePath, 512,
            kSurfaceRoot, 0);

        kSurfaceRoot = NiFixedString(pcAbsolutePath);
        NiFree(pcAbsolutePath);
    }

    return kSurfaceRoot;
}
//---------------------------------------------------------------------------
bool NiTerrain::Save(const char* pcFileName, NiUInt32* puiErrorCode)
{
    bool bRes = true;
    
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        // Attempt to save all sectors, regardless of the other ones failures
        bRes &= pkSector->Save(pcFileName, puiErrorCode);
    }

    return bRes;
}
//---------------------------------------------------------------------------
bool NiTerrain::Load(const char* pcFileName, NiUInt32* puiErrorCode)
{
    bool bRes = true;
    
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        // Attempt to save all sectors, regardless of the other ones failures
        bRes &= pkSector->Load(pcFileName, puiErrorCode);
    }

    return bRes;
}
//---------------------------------------------------------------------------
bool NiTerrain::LoadDirectFromSectorFile(const char* pcFileName, NiInt16 sXIndex, NiUInt16 sYIndex, 
    NiUInt32* puiErrorCode)
{
    EE_ASSERT(pcFileName);
    EE_ASSERT(sXIndex == 0 && sYIndex == 0);

    NiUInt32 uiDummyError = 0;
    if (!puiErrorCode)
        puiErrorCode = &uiDummyError;

    NiTerrainSectorFilePtr spSectorFile = NiTerrainSectorFile::Open(pcFileName);
    if(!spSectorFile)
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_INVALID_FILE;
        return false;
    }

    // Check if the file is of the latest version
    if (spSectorFile->GetFileVersion() != NiTerrainSectorFile::GetCurrentVersion())
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_OUTOFDATE;
        return false;
    }

    m_uiNumLOD = spSectorFile->GetNumLOD();
    m_uiBlockSize = spSectorFile->GetBlockWidthInVerts() - 1;
    CreateSector(sXIndex, sYIndex);

    NiTerrainSector* pkSector = GetSector(sXIndex, sYIndex);
    EE_ASSERT(pkSector);

    if (!pkSector->LoadFromFile(spSectorFile, m_uiNumLOD, puiErrorCode))
        return false;

    NiString kArchivePath = pcFileName;
    kArchivePath.Replace("quadtree.dof", "");
    SetArchivePath((const char*)kArchivePath);
    pkSector->SetArchivePath((const char*)kArchivePath);
    
    if (pkSector->m_pkQuadMesh)
    {
        pkSector->m_pkQuadMesh->ResetLOD();

        // Signal this max LOD as having been loaded!
        pkSector->m_pkSectorData->SetHighestLoadedLOD(m_uiNumLOD);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrain::LoadSectorMaterialMasks(NiInt16 sXIndex, NiInt16 sYIndex, NiUInt32* puiErrorCode)
{
    NiSurfacePalette* pkSurfacePalette = GetActiveSurfacePalette();
    if (!pkSurfacePalette)
        return false;

    NiTerrainSector* pkSector = GetSector(sXIndex, sYIndex);
    if (!pkSector)
        return false;

    NiOutputDebugString("Loading surface masks\n");

    NiUInt32 uiDummyError = 0;
    if (!puiErrorCode)
        puiErrorCode = &uiDummyError;

    NiString kFileName = GetArchivePath();
    kFileName += "\\surfaces.xml";

    // Make a temporary copy of the file name to standardize the path.
    char* pcPathToStandardize = kFileName.MakeExternalCopy();
    NiPath::Standardize(pcPathToStandardize);

    kFileName = pcPathToStandardize;

    // Destroy the temporary copy of the file name.
    NiFree(pcPathToStandardize);

    // For now we only support materials on the top most level of the terrain quad tree.
    NiTerrainDataLeaf* pkCurrentLeaf = pkSector->m_pkQuadData;
    EE_ASSERT(pkCurrentLeaf);

    EE_ASSERT(pkSector->m_pkSectorData);

    TiXmlDocument kDoc((const char*)kFileName);
    if (NiTerrainXMLHelpers::LoadXMLFile(&kDoc))
    {
        TiXmlElement* pkCurElem = kDoc.FirstChildElement("Block");
        NIASSERT(pkCurElem);
        pkCurElem = pkCurElem->FirstChildElement("SurfaceMask");

        // Keep a track of surfaces, in their requested positions
        NiTPointerMap<NiInt32, NiSurface*> kSurfaces;
        NiUInt32 uiMaxLayerNum = 0;

        while (pkCurElem)
        {
            const char* pcPackage = pkCurElem->Attribute("package");
            if (pcPackage == 0)
                pcPackage = "";

            // Make sure we have the specified material package loaded.
            const char* pcName = pkCurElem->Attribute("name");
            if (pcName == 0)
                pcName = "";

            const char* pcLayerNum = pkCurElem->Attribute("position");
            if (pcLayerNum == 0)
                pcLayerNum = "";

            char acFullPath[NI_MAX_PATH];
            NiStrncpy(acFullPath, NI_MAX_PATH, pcPackage, NI_TRUNCATE);
            NiPath::ConvertToAbsolute(acFullPath, NI_MAX_PATH);
            
            NiString kMatName = acFullPath;
            NiUInt32 uiFirstPos = kMatName.FindReverse(EE_PATH_DELIMITER_CHAR);
            NiUInt32 uiSecondPos = kMatName.FindReverse('.');
            kMatName = kMatName.GetSubstring(uiFirstPos + 1, uiSecondPos);

            NiSurfacePackage* pkPackage = pkSurfacePalette->GetPackage((const char*)kMatName);
            if (!pkPackage)
            {
                *puiErrorCode |= NiTerrainSector::EC_SURFACE_INVALID_PACKAGE;
                pkCurElem = pkCurElem->NextSiblingElement();
                continue;
            }

            NiSurface* pkSurface = pkSurfacePalette->GetSurface(pkPackage->GetName(), pcName);
            if (!pkSurface)
            {
                *puiErrorCode |= NiTerrainSector::EC_SURFACE_FAILED_LOADSURFACE;
                pkCurElem = pkCurElem->NextSiblingElement();
                continue;
            }

            NiInt32 iPriority;
            if (!NiString(pcLayerNum).ToInt(iPriority))
            {
                *puiErrorCode |= NiTerrainSector::EC_SURFACE_INVALID_INDEX;
                pkCurElem = pkCurElem->NextSiblingElement();
                continue;
            }
            
            uiMaxLayerNum = NiMax(uiMaxLayerNum, iPriority);
            kSurfaces.SetAt(iPriority, pkSurface);
            pkCurElem = pkCurElem->NextSiblingElement();
        }
            
        NiInt32 iCurrPriority = 0;
        NiSurface* pkCurrSurface = NULL;
        NiUInt32 uiMaskSize = 0;
        for (NiUInt32 ui = 0; ui <= uiMaxLayerNum; ++ui)
        {
            if (!kSurfaces.GetAt(ui, pkCurrSurface))
                continue;

            EE_ASSERT(pkCurrSurface);

            // Load the mask
            NiDevImageConverter kImageConverter;
            NiString kPath;

            kPath.Format("%s\\%d.tga", (const char*)GetArchivePath(), ui);
            if (!kImageConverter.CanReadImageFile(kPath))
            {
                NiOutputDebugString((const char*)
                    (NiString("Failed to read mask: ") + 
                    NiString(kPath)));
                *puiErrorCode |= NiTerrainSector::EC_SURFACE_FAILED_READ_MASK; 
                continue;
            }

            NiPixelDataPtr spPixelData = NULL;
            spPixelData = kImageConverter.ReadImageFile(kPath, NULL);

            if (ui == 0)
            {
                uiMaskSize = spPixelData->GetWidth();
                SetDefaultMaskSize(uiMaskSize);
                pkSector->SetDefaultMaskSize(uiMaskSize);
            }
            else
            {
                // Make sure the masks for each layer are of equal size.
                EE_ASSERT(uiMaskSize == spPixelData->GetWidth());
                if (uiMaskSize != spPixelData->GetWidth())
                {
                    *puiErrorCode |= NiTerrainSector::EC_SURFACE_FAILED_READ_MASK;
                    return false;
                }
            }            
            
            NiSurfaceMask* pkMask = pkSector->CreateMask(spPixelData);
            EE_ASSERT(pkMask);

            // Add this surface to the leaf, and make an alpha layer
            pkCurrSurface->CompileSurface();
            pkCurrentLeaf->AddSurfaceMask(pkCurrSurface, pkMask, (NiUInt32)iCurrPriority);
            iCurrPriority++;
        }        
    }
    else
    {
        *puiErrorCode |= NiTerrainSector::EC_SURFACE_FAILED_LOADSURFACE;
    }

    if (*puiErrorCode != 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiTerrain::Unload()
{
    NiTMapIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        pkSector->Unload();
    }
}
//---------------------------------------------------------------------------
void NiTerrain::OnVisible(NiCullingProcess& kCuller)
{
    // If a terrain specific culler is in use, we can limit the LOD
    NiTerrainCullingProcess* pkTerrainCuller = NiDynamicCast(
        NiTerrainCullingProcess, &kCuller);
    
    bool bProcessBorders = true;
    NiUInt32 uiMaxLOD = NiTerrainUtils::ms_uiMAX_LOD;
    NiUInt32 uiMinLOD = 0;
    if (pkTerrainCuller)
    {
        uiMaxLOD = pkTerrainCuller->GetMaxTerrainLOD();
        uiMinLOD = pkTerrainCuller->GetMinTerrainLOD();
        bProcessBorders = pkTerrainCuller->GetUpdateGeometry();
    }

    // Perform LOD calculations
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    NiTerrainSector* pkSector = NULL;
    NiUInt32 ulIndex;
    while (kIterator)
    {
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        if (pkSector)
        {
            NiUInt32 uiOldMaxLOD = 
                pkSector->GetSectorData()->GetHighestVisibleLOD();
            NiUInt32 uiOldMinLOD = 
                pkSector->GetSectorData()->GetLowestVisibleLOD();

            pkSector->SetMinMaxVisibleLOD(uiMinLOD, uiMaxLOD);
            pkSector->OnVisible(kCuller);
            pkSector->SetMinMaxVisibleLOD(uiOldMinLOD, uiOldMaxLOD);
        }
    }

    if (bProcessBorders)
    {
        // Perform stitching of sectors
        kIterator = m_kSectors.GetFirstPos();
        while (kIterator)
        {
            m_kSectors.GetNext(kIterator, ulIndex, pkSector);
            pkSector->ProcessBorders();
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    DoUpdate();
    NiNode::UpdateDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate();
    }
    NiNode::UpdateSelectedDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate();
    }
    NiNode::UpdateRigidDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::DoUpdate(bool bUpdateWorldData) 
{
    // Set shape changed to false now (If a sector has changed it will
    // set it to true after this when we propagate the update)
    SetShapeChangedLastUpdate(false);

    // Update any sectors that may require updating
    if (HasSettingChanged())
    {       
        NiTListIterator kIterator = m_kSectors.GetFirstPos();
        while (kIterator)
        {
            NiTerrainSector* pkSector = NULL;
            NiUInt32 ulIndex;
            m_kSectors.GetNext(kIterator, ulIndex, pkSector);

            UpdateSector(pkSector);
            // Sector Update is called by the NiNode::Update code
            // DO NOT propagate the Update to children here! This
            // would create multiple update calls and break the update
            // order when using the NiEntity/Component system.
        }

        SetBit(false, SETTING_CHANGED);
    }

    if (bUpdateWorldData)
    {
        NiAVObject::UpdateWorldData();
    }
}
//---------------------------------------------------------------------------
void NiTerrain::ModifyVertexHeightFrom(NiTerrainSector* pkSector, 
        const NiTerrainVertex& kVertex, float fNewHeight, 
        bool bModifyParentLOD)
{
    pkSector->ModifyVertexHeightFrom(
        kVertex, kVertex.GetFirstContainingLeaf(), 
        fNewHeight,
        bModifyParentLOD);

    // Check if we are at the border of a sector:
    NiIndex kIndex = kVertex.GetIndex();
    NiUInt32 uiSectorSize = pkSector->GetSectorData()->GetSectorSize();
    if (kIndex.x == 0 || kIndex.x == uiSectorSize ||
        kIndex.y == 0 || kIndex.y == uiSectorSize)
    {
        NiTerrainSector* pkAdjacentSector = NULL;
        NiTerrainVertex kAdjacentVertex;
        NiIndex kAdjacentIndex;

        // Mark adjacent blocks in other sectors as changed too
        if (kIndex.x == 0)
        {
            // Modify sector's block on the Left border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_LEFT);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.x = uiSectorSize;
        } 
        else if (kIndex.x == uiSectorSize)
        {
            // Modify sector's block on the Right border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_RIGHT);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.x = 0;
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }

        if (kIndex.y == 0)
        {
            // Modify sector's block on the Bottom border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_BOTTOM);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.y = uiSectorSize;
        }
        else if (kIndex.y == uiSectorSize)
        {
            // Modify sector's block on the Top border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_TOP);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.y = 0;
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }

        // Corner Cases:
        if (kIndex.x == 0)
        {
            kAdjacentIndex.x = uiSectorSize;
            if (kIndex.y == 0)
            {
                // Modify sector's block on the LEFT - BOTTOM
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_LEFT |
                    NiTerrainDataLeaf::BORDER_BOTTOM);
                kAdjacentIndex.y = uiSectorSize;
            }
            else if (kIndex.y == uiSectorSize)
            {
                // Modify sector's block on the LEFT - TOP
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_LEFT |
                    NiTerrainDataLeaf::BORDER_TOP);              
                kAdjacentIndex.y = 0;
            }
        }else if (kIndex.x == uiSectorSize)
        {
            kAdjacentIndex.x = 0;
            if (kIndex.y == 0)
            {
                // Modify sector's block on the RIGHT - BOTTOM
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_RIGHT |
                    NiTerrainDataLeaf::BORDER_BOTTOM);              
                kAdjacentIndex.y = uiSectorSize;
            }
            else if (kIndex.y == uiSectorSize)
            {
                // Modify sector's block on the RIGHT - TOP
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_RIGHT |
                    NiTerrainDataLeaf::BORDER_TOP);              
                kAdjacentIndex.y = 0;
            }
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }
    }
}
//---------------------------------------------------------------------------
const NiTerrainConfiguration NiTerrain::GetDefaultConfiguration()
{
    return ms_kDefaultConfiguration;
}
//---------------------------------------------------------------------------
void NiTerrain::SetDefaultConfiguration(NiTerrainConfiguration kConfig)
{
    bool bValid = true;

    // We currently only support one valid configuration for the other platforms
    // which has all the flags set to true.
#ifndef _WII
    if (!kConfig.IsMorphingDataEnabled() || 
        !kConfig.IsLightingDataCompressionEnabled() ||
        !kConfig.IsTangentDataEnabled())
    {
        bValid = false;
    }
#endif

    if (!kConfig.ValidateConfiguration())
        bValid = false;

    if (bValid)
        ms_kDefaultConfiguration = kConfig;
}
//---------------------------------------------------------------------------
const NiTerrainConfiguration NiTerrain::GetConfiguration() const
{
    return ms_kDefaultConfiguration;
}
//---------------------------------------------------------------------------
