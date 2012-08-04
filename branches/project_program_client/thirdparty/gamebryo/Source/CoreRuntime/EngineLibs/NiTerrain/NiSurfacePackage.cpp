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

#include "NiSurfacePackage.h"
#include "NiMetaDataStore.h"
#include "NiSurface.h"
#include "NiTerrainXMLHelpers.h"

using namespace efd;

//---------------------------------------------------------------------------
NiSurfacePackage::NiSurfacePackage() :
    m_bSurfaceChanged(false),
    m_kPackageName(""),
    m_kFileSrc(""),
    m_kSurfaces()
{
}
//---------------------------------------------------------------------------
NiSurfacePackage::NiSurfacePackage(const NiFixedString& kPackageName) :
    m_bSurfaceChanged(false),
    m_kPackageName(kPackageName),
    m_kFileSrc(""),
    m_kSurfaces()
{
}
//---------------------------------------------------------------------------
NiSurfacePackage::~NiSurfacePackage()
{
    UnloadSurfaces();
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::Save()
{
    // Create a DOM object    
    TiXmlDocument kDoc(m_kFileSrc);

    NiTMapIterator kIterator = m_kSurfaces.GetFirstPos();
    NiSurface* pkCurData;
    const char* kCurKey;

    // Write the header
    NiTerrainXMLHelpers::WriteXMLHeader(&kDoc);

    // Begin the new section:

    TiXmlElement* pkPackageElement = NiTerrainXMLHelpers::CreateElement(
        "Package", NULL);
    pkPackageElement->SetAttribute("name", m_kPackageName);
    kDoc.LinkEndChild(pkPackageElement);

    while (kIterator)
    {
        m_kSurfaces.GetNext(kIterator, kCurKey, pkCurData);

        // For each surface in this package, save!    
        TiXmlElement* pkSurfaceElement = NiTerrainXMLHelpers::CreateElement(
            "Surface", pkPackageElement);
        pkSurfaceElement->SetAttribute("name", kCurKey);
        pkCurData->Save(pkSurfaceElement);
    }

    return kDoc.SaveFile();
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::Load()
{
    return Load(m_kFileSrc);
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::Load(const char* pcFilename, 
    NiMetaDataStore* pkMetaDataStore)
{
    SetFilename(pcFilename);

    if (!m_kSurfaces.IsEmpty())
    {
        return false;
    }

    // Ensure meta data store is valid
    if (!pkMetaDataStore)
    {
        pkMetaDataStore = NiMetaDataStore::GetInstance();
    }

    TiXmlDocument kDoc(pcFilename);
    
    if (!NiTerrainXMLHelpers::LoadXMLFile(&kDoc)) {
        return false;
    }
    
    NiSurface* pkSurface = NULL;
    const char* pcName = NULL;

    TiXmlElement* pkCurElem = NULL;
    
    TiXmlElement* pkPackage = kDoc.FirstChildElement("Package");
    if (pkPackage == NULL)
        return false;

    pcName = pkPackage->Attribute("name");
    if (pcName == 0)
    {
        pcName = "";
    }
    m_kPackageName = NiFixedString(pcName);

    pkCurElem = pkPackage->FirstChildElement("Surface");

    while (pkCurElem)
    {
        pcName = pkCurElem->Attribute("name");
        if (pcName == 0)
        {
            pcName = "";
        }

        // Does a surface already exist by this name?
        if (m_kSurfaces.GetAt(pcName, pkSurface))
        {
            // Yes, a surface already exists, overwrite. Need to delete the
            // old surface, to make sure all the data is cleaned.
            m_kSurfaces.RemoveAt(pcName);
            NiDelete pkSurface;
        }

        pkSurface = NiNew NiSurface(pkMetaDataStore);

        pkSurface->Load(pkCurElem);
        pkSurface->SetPackage(this);
        pkSurface->SetName(pcName);

        m_kSurfaces.SetAt(pcName, pkSurface);


        pkCurElem = pkCurElem->NextSiblingElement();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::PrecompileSurfaces()
{
    if (!m_kSurfaces.GetCount())
        return false;

    NiTMapIterator kIterator = m_kSurfaces.GetFirstPos();
    NiSurface* pkCurrentSurface;
    const char* pcKey;

    while (kIterator)
    {
        m_kSurfaces.GetNext(kIterator, pcKey, pkCurrentSurface);
        pkCurrentSurface->CompileSurface();
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSurfacePackage::SetFilename(const char* pcFilename)
{
    m_kFileSrc = pcFilename;
}
//---------------------------------------------------------------------------
const char* NiSurfacePackage::GetFilename() const
{
    return m_kFileSrc;
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::RenameSurface(NiSurface* pkSurface,
        const NiFixedString& kNewSurfaceName)
{
    NiSurface *pkExistingSurface;
    NiFixedString kOldSurfaceName = pkSurface->GetName();

    // Check that no surfaces exist with the new name
    if (GetSurface(kNewSurfaceName, pkExistingSurface))
        return false;

    // Check that the surface is contained within this package to begin with
    GetSurface(pkSurface->GetName(), pkExistingSurface);
    if (pkExistingSurface != pkSurface)
        return false;

    // Map this surface to the new name
    m_kSurfaces.SetAt(kNewSurfaceName, pkSurface);
    pkSurface->SetName(kNewSurfaceName);

    // Remove the surface from the old name
    m_kSurfaces.RemoveAt(kOldSurfaceName);
            
    return true;
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::ClaimSurface(NiSurface* pkSurface)
{
    NIASSERT(pkSurface);

    NiSurfacePackage *pkOldPackage = pkSurface->GetPackage();
    NiSurfacePackage *pkNewPackage = this;

    // Check that these packages arn't the same ones!
    if (pkOldPackage == pkNewPackage)
        return true;

    // Check that there doesn't aready exist a surface by this name...
    NiSurface *pkExistingSurface; 
    if (GetSurface(pkSurface->GetName(), pkExistingSurface))
    {
        // A surface by this name already exists in this package
        return false; 
    }

    // Add this surface to this package:
    pkNewPackage->m_kSurfaces.SetAt(pkSurface->GetName(), pkSurface);
    pkSurface->SetPackage(pkNewPackage);

    // Remove this surface from the old package:
    if (pkOldPackage)
        pkOldPackage->m_kSurfaces.RemoveAt(pkSurface->GetName());

    return true;
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::ReleaseSurface(NiSurface* pkSurface)
{
    NiSurface* pkFoundSurface;
    if (GetSurface(pkSurface->GetName(), pkFoundSurface))
    {
        if (pkFoundSurface == pkSurface)
        {
            m_kSurfaces.RemoveAt(pkSurface->GetName());
            pkSurface->SetPackage(0);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiSurfacePackage::UnloadSurface(const NiFixedString& kName)
{
    NiSurface* pkSurface;
    if (GetSurface(kName, pkSurface))
    {
        m_kSurfaces.RemoveAt(kName);
        NiDelete pkSurface;
    }
}
//---------------------------------------------------------------------------
void NiSurfacePackage::UnloadSurfaces()
{
    NiTMapIterator kIterator = m_kSurfaces.GetFirstPos();
    NiSurface* pkCurrentSurface;
    const char* pcKey;

    while (kIterator)
    {
        m_kSurfaces.GetNext(kIterator, pcKey, pkCurrentSurface);
        m_kSurfaces.RemoveAt(pcKey);
        NiDelete pkCurrentSurface;
    }
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::CreateSurface(const NiFixedString& kName, 
    NiSurface*& pkSurface, NiMetaDataStore* pkMetaDataStore)
{
    if (pkMetaDataStore == 0)
        pkMetaDataStore = NiMetaDataStore::GetInstance();

    NIASSERT(pkMetaDataStore);

    // Make sure that no surfaces already exist by that name
    if (GetSurface(kName, pkSurface))
    {
        pkSurface = 0;
        return false;
    }

    pkSurface = NiNew NiSurface(pkMetaDataStore);
    pkSurface->SetName(kName);
    pkSurface->SetPackage(this);

    return true;
}
//---------------------------------------------------------------------------
bool NiSurfacePackage::GetSurface(const NiFixedString& kName, 
    NiSurface*& pkSurface) const
{
    if (m_kSurfaces.GetAt(kName, pkSurface))
    {
        return true;
    }
    else
    {
        pkSurface = 0;
        return false;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiSurfacePackage::GetLoadedSurfaces(
    NiTPrimitiveSet<NiSurface*>& kSurfaces) const
{
    NiTMapIterator kIterator = m_kSurfaces.GetFirstPos();
    NiSurface* pkCurrentSurface;
    const char* pcKey;
    NiUInt32 uiAdded = 0;

    while (kIterator)
    {
        m_kSurfaces.GetNext(kIterator, pcKey, pkCurrentSurface);
        kSurfaces.Add(pkCurrentSurface);
        ++uiAdded;
    }

    return uiAdded;
}
//---------------------------------------------------------------------------
const NiFixedString& NiSurfacePackage::GetName() const
{
    return m_kPackageName;
}
//---------------------------------------------------------------------------
void NiSurfacePackage::SetName(const NiFixedString& kNewName)
{
    m_kPackageName = kNewName;
}
