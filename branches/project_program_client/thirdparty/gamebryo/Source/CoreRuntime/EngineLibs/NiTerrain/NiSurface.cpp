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


#include "NiTerrainXMLHelpers.h"

#include <NiFilename.h>
#include <NiBooleanExtraData.h>

#include "NiSurface.h"
#include "NiSurfacePackage.h"


using namespace efd;

NiFixedString* NiSurface::ms_pkSurfaceMapNames = 0;

//---------------------------------------------------------------------------
void NiSurface::_SDMInit()
{
    ms_pkSurfaceMapNames = NiNew NiFixedString[NUM_SURFACE_MAPS];
    ms_pkSurfaceMapNames[SURFACE_MAP_DIFFUSE] = "BaseMap";    
    ms_pkSurfaceMapNames[SURFACE_MAP_NORMAL] = "NormalMap";
    ms_pkSurfaceMapNames[SURFACE_MAP_DISTRIBUTION] = "DistributionMap";
}
//---------------------------------------------------------------------------
void NiSurface::_SDMShutdown()
{
    NiDelete[] ms_pkSurfaceMapNames;
}
//---------------------------------------------------------------------------
NiSurface::NiSurface(NiMetaDataStore* pkMetaDataStore) :
    m_bTagsChanged(true),
    m_kUVScale(128,128),    
    m_kUVOffset(0,0),    
    m_fDistributionMaskStrength(1.0f),
    m_pkPackage(0),
    m_kName(""),
    m_kDescription(""),
    m_kDiffuseMapFile(NULL),
    m_kNormalMapFile(NULL),    
    m_kMetaData(pkMetaDataStore),
    m_kTags(7),
    m_kRemovedTags(7),
    m_bCompiled(false)
{
    m_kMapParameters[0].Resize(7);
    m_kMapParameters[1].Resize(7);
    m_kMapParameters[2].Resize(7);

    m_aspShaderTextures[SURFACE_MAP_DIFFUSE] = NULL;
    m_aspShaderTextures[SURFACE_MAP_NORMAL] = NULL;
    m_aspShaderTextures[SURFACE_MAP_DISTRIBUTION] = NULL;
}
//---------------------------------------------------------------------------
NiSurface::~NiSurface()
{
    m_kTags.RemoveAll();
    m_kRemovedTags.RemoveAll();

    // remove all parameters
    for (NiUInt8 ui = 0; ui < NUM_SURFACE_MAPS; ++ui)
    {
        NiTMapIterator iterator = m_kMapParameters[ui].GetFirstPos();
        NiString* pStrValue;
        const char* pcName;
        while (iterator)
        {
            m_kMapParameters[ui].GetNext(iterator, pcName, pStrValue);
            NiDelete pStrValue;
        }

        m_kMapParameters[ui].RemoveAll();
    }
}
//---------------------------------------------------------------------------
void NiSurface::Load(TiXmlElement* pkRootElement)
{
    // Read the description:
    TiXmlElement* pkCurrentSection = 
        pkRootElement->FirstChildElement("Description");

    if (pkCurrentSection)
        m_kDescription = pkCurrentSection->GetText();

    // Read all the MAP data.
    pkCurrentSection = pkRootElement->FirstChildElement("DiffuseMap");
    if (pkCurrentSection)
    {
        LoadSurfaceMap(SURFACE_MAP_DIFFUSE, pkCurrentSection);
    }
    
    pkCurrentSection = pkRootElement->FirstChildElement("NormalMap");
    if (pkCurrentSection)
    {
        LoadSurfaceMap(SURFACE_MAP_NORMAL, pkCurrentSection);
    }

    pkCurrentSection = pkRootElement->FirstChildElement("DistributionMap");
    if (pkCurrentSection)
    {
        LoadSurfaceMap(SURFACE_MAP_DISTRIBUTION, pkCurrentSection);
    }

    // Read in the texture coordinate modifiers:
    NiTerrainXMLHelpers::ReadElement(pkRootElement, "UVScaleModifier", 
        m_kUVScale);
    
    NiTerrainXMLHelpers::ReadElement(pkRootElement, "UVOffsetModifier",
        m_kUVOffset);

    // Read in the distribution mask modifiers.
    NiTerrainXMLHelpers::ReadElement(pkRootElement, "DistMaskScale",
        m_fDistributionMaskStrength);
    
    pkCurrentSection = pkRootElement->FirstChildElement("tags");
    if (pkCurrentSection)
    {
        TiXmlElement* pkTagSection = pkCurrentSection->FirstChildElement();
        while (pkTagSection != NULL &&
            NiFixedString(pkTagSection->Value()) == "tag")
        {
            NiFixedString kTag = pkTagSection->GetText();
            if (kTag.Exists())
                AddTag(kTag);

            pkTagSection = pkTagSection->NextSiblingElement();
        }

    }
    
    // Load the MetaData
    pkCurrentSection = pkRootElement->FirstChildElement("MetaData");
    if (pkCurrentSection)
    {
        m_kMetaData.Load(pkCurrentSection);
    }
}
//---------------------------------------------------------------------------
void NiSurface::LoadSurfaceMap(SurfaceMapID eMapID, 
    TiXmlElement* pkRootElement)
{    
    // Any parameters?
    if (NULL != pkRootElement->FirstAttribute())
    {
        TiXmlAttribute* pAttribute = pkRootElement->FirstAttribute();

        while (pAttribute)
        {
            m_kMapParameters[eMapID].SetAt(
                pAttribute->Name(), NiNew NiString(pAttribute->Value()));

            pAttribute = pAttribute->Next();
        }

    }

    // Write the file
    NiFixedString kMapFile = pkRootElement->GetText();
    SetMapFile(eMapID, kMapFile);
}
//---------------------------------------------------------------------------
void NiSurface::Save(TiXmlElement* pkRootElement) const
{

    NiTerrainXMLHelpers::WriteElement(pkRootElement, "Description", 
        m_kDescription);
    
    // Write out the MAP information.
    TiXmlElement* pkCurrentElement = NiTerrainXMLHelpers::CreateElement(
        "DiffuseMap", pkRootElement);
    SaveSurfaceMap(SURFACE_MAP_DIFFUSE, pkCurrentElement);

    pkCurrentElement = NiTerrainXMLHelpers::CreateElement("NormalMap", 
        pkRootElement);
    SaveSurfaceMap(SURFACE_MAP_NORMAL, pkCurrentElement);

    pkCurrentElement = NiTerrainXMLHelpers::CreateElement("DistributionMap", 
        pkRootElement);
    SaveSurfaceMap(SURFACE_MAP_DISTRIBUTION, pkCurrentElement);

    // Write out the texture coordinate modifiers:
    NiTerrainXMLHelpers::WriteElement(pkRootElement, "UVScaleModifier",
        m_kUVScale);

    NiTerrainXMLHelpers::WriteElement(pkRootElement, "UVOffsetModifier",
        m_kUVOffset);

    NiTerrainXMLHelpers::WriteElement(pkRootElement, "DistMaskScale", 
        m_fDistributionMaskStrength);

    // Save out tags
    pkCurrentElement = NiTerrainXMLHelpers::CreateElement("tags", 
        pkRootElement);
    NiTMapIterator kIterator = m_kTags.GetFirstPos();
    const char* pcKey;
    bool kValue;

    while (kIterator)
    {
        m_kTags.GetNext(kIterator, pcKey, kValue);
        NiTerrainXMLHelpers::WriteElement(pkCurrentElement, "tag",
            pcKey);
    }


    // Save the metadata
    pkCurrentElement = NiTerrainXMLHelpers::CreateElement("MetaData", 
        pkRootElement);
    m_kMetaData.Save(pkCurrentElement);

}
//---------------------------------------------------------------------------
void NiSurface::SaveSurfaceMap(SurfaceMapID eMapID, 
    TiXmlElement* pkRootElement) const
{    
    NiFixedString kMapFile;
    GetMapFile(eMapID, kMapFile);
    
    if (!kMapFile.Exists())
        kMapFile = "";

    // Any parameters?
    NiTMapIterator iterator = m_kMapParameters[eMapID].GetFirstPos();
    NiString* pStrValue;
    const char* pcName;
    while (iterator)
    {
        m_kMapParameters[eMapID].GetNext(iterator, pcName, pStrValue);
        pkRootElement->SetAttribute(pcName, *pStrValue);
    }

    // Write the file
    NiTerrainXMLHelpers::WritePrimitive(pkRootElement, kMapFile);
}
//---------------------------------------------------------------------------
void NiSurface::SetPackage(NiSurfacePackage* pkPackage)
{
    // This is recursion safe.
    if (pkPackage)
    {
        pkPackage->ClaimSurface(this);
    }
    else if (m_pkPackage)
    {
        NiSurfacePackage* pkOldPackage = m_pkPackage;
        m_pkPackage = 0;
        pkOldPackage->UnloadSurface(GetName());
    }
    
    m_pkPackage = pkPackage;
}
//---------------------------------------------------------------------------
NiSurfacePackage* NiSurface::GetPackage() const
{
    return m_pkPackage;
}
//---------------------------------------------------------------------------
void NiSurface::SetName(const NiFixedString& kName)
{
    // This is recursion safe.
    if (m_pkPackage)
        m_pkPackage->RenameSurface(this, kName);

    m_kName = kName;
}
//---------------------------------------------------------------------------
const NiFixedString& NiSurface::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiSurface::SetDescription(const NiFixedString& kDescription)
{
    m_kDescription = kDescription;
}
//---------------------------------------------------------------------------
const NiFixedString& NiSurface::GetDescription() const
{
    return m_kDescription;
}
//---------------------------------------------------------------------------
bool NiSurface::AddTag(const NiFixedString& kTag)
{
    if (!kTag.Exists())
    {
        return false;
    }

    bool result = false;
    if (!HasTag(kTag))
    {
        DeleteTag(kTag);
        m_kTags.SetAt(kTag, true);

        // Set changed to true so the tagging index in the palette will know
        // to update
        m_bTagsChanged = true;
        if (m_pkPackage)
        {
            m_pkPackage->m_bSurfaceChanged = true;
        }

        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

//---------------------------------------------------------------------------
void NiSurface::RemoveAllTags()
{
    NiTMapIterator kIterator = m_kTags.GetFirstPos();
    const char* pcKey;
    bool kValue;

    while (kIterator)
    {
        m_kTags.GetNext(kIterator, pcKey, kValue);
        m_kRemovedTags.SetAt(pcKey, true);
    }

    m_kTags.RemoveAll();
    m_bTagsChanged = true;
    if (m_pkPackage)
    {
        m_pkPackage->m_bSurfaceChanged = true;
    }
}
//---------------------------------------------------------------------------
bool NiSurface::RemoveTag(const NiFixedString& kTag)
{
    if (!kTag.Exists())
        return false;

    if (HasTag(kTag))
    {
        m_kTags.RemoveAt(kTag);
        m_kRemovedTags.SetAt(kTag, true);

        m_bTagsChanged = true;
        m_pkPackage->m_bSurfaceChanged = true;
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiSurface::RemoveTag(const char* pcTag)
{
    return RemoveTag(NiFixedString(pcTag));
}
//---------------------------------------------------------------------------
bool NiSurface::DeleteTag(const NiFixedString& kTag)
{
    if (!kTag.Exists())
        return false;

    m_kTags.RemoveAt(kTag);
    m_kRemovedTags.RemoveAt(kTag);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiSurface::DeleteTag(const char* pcTag)
{
    if (pcTag == 0)
        return false;

    m_kTags.RemoveAt(pcTag);
    m_kRemovedTags.RemoveAt(pcTag);
    
    return true;
}
//---------------------------------------------------------------------------
void NiSurface::SetTag(
    const NiFixedString& kOldTag, const NiFixedString& kNewTag)
{
    if (!kNewTag.Exists())
        return;

    RemoveTag(kOldTag);
    AddTag(kNewTag);
}
//---------------------------------------------------------------------------
NiUInt32 NiSurface::GetTags(NiTObjectSet<NiFixedString>& kTags)
{
    NiTMapIterator kIterator = m_kTags.GetFirstPos();
    const char* pcKey;
    bool kValue;

    while (kIterator)
    {
        m_kTags.GetNext(kIterator, pcKey, kValue);
        kTags.AddUnique(NiFixedString(pcKey));
    }

    return m_kTags.GetCount();
}
//---------------------------------------------------------------------------
NiUInt32 NiSurface::GetRemovedTags(NiTObjectSet<NiFixedString>& kTags)
{
    NiTMapIterator kIterator = m_kRemovedTags.GetFirstPos();
    const char* pcKey;
    bool kValue;

    while (kIterator)
    {
        m_kRemovedTags.GetNext(kIterator, pcKey, kValue);
        kTags.AddUnique(NiFixedString(pcKey));
    }

    return m_kRemovedTags.GetCount();
}
//---------------------------------------------------------------------------
bool NiSurface::HasTag(const NiFixedString& kTag)
{
    if (!kTag.Exists())
        return false;

    bool bValue;
    return m_kTags.GetAt(kTag, bValue);
}
//---------------------------------------------------------------------------
bool NiSurface::HasTag(const char* pcTag)
{
    if (pcTag == 0)
        return false;

    bool bValue;
    return m_kTags.GetAt(pcTag, bValue);
}
//---------------------------------------------------------------------------
NiUInt32 NiSurface::GetTagCount()
{
    return m_kTags.GetCount();
}
//---------------------------------------------------------------------------
bool NiSurface::SetMapFile(SurfaceMapID eMapID, const NiFixedString& kMap)
{
    switch (eMapID)
    {
        case SURFACE_MAP_DIFFUSE:
            m_kDiffuseMapFile = kMap;
            break;        
        case SURFACE_MAP_NORMAL:
            m_kNormalMapFile = kMap;
            break;
        case SURFACE_MAP_DISTRIBUTION:
            m_kDistributionMapFile = kMap;
            break;
        default:
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSurface::GetMapFile(SurfaceMapID eMapID, NiFixedString& kMap) const
{
    switch (eMapID)
    {
        case SURFACE_MAP_DIFFUSE:
            kMap = m_kDiffuseMapFile;
            break;        
        case SURFACE_MAP_NORMAL:
            kMap = m_kNormalMapFile;
            break;
        case SURFACE_MAP_DISTRIBUTION:
            kMap = m_kDistributionMapFile;
            break;
        default:
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSurface::GetMapName(SurfaceMapID eMapID, NiFixedString& kMapName)
{
    if (eMapID < NUM_SURFACE_MAPS)
    {
        kMapName = ms_pkSurfaceMapNames[eMapID];
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiSurface::SetUVScale(NiPoint2 kScale)
{
    m_kUVScale = kScale;
}
//---------------------------------------------------------------------------
void NiSurface::SetUVOffset(NiPoint2 kOffset)
{
    m_kUVOffset = kOffset;
}
//---------------------------------------------------------------------------
NiPoint2 NiSurface::GetUVScale() const
{
    return m_kUVScale;
}
//---------------------------------------------------------------------------
NiPoint2 NiSurface::GetUVOffset() const
{
    return m_kUVOffset;
}
//---------------------------------------------------------------------------
float NiSurface::GetDistributionMaskStrength() const
{
    return m_fDistributionMaskStrength;
}
//---------------------------------------------------------------------------
void NiSurface::SetDistributionMaskStrength(float fValue)
{
    m_fDistributionMaskStrength = fValue;
}
//---------------------------------------------------------------------------
void NiSurface::CopySettings(const NiSurface* pkTemplate)
{
    m_kDiffuseMapFile = pkTemplate->m_kDiffuseMapFile;
    m_kNormalMapFile = pkTemplate->m_kNormalMapFile;
    m_kMetaData = pkTemplate->m_kMetaData;
        
    m_kUVScale = pkTemplate->m_kUVScale;
    m_kUVOffset = pkTemplate->m_kUVOffset;
    m_fDistributionMaskStrength = pkTemplate->m_fDistributionMaskStrength;

    for (NiUInt32 ui = 0; ui < NUM_SURFACE_MAPS; ++ui)
    {
        m_aspShaderTextures[ui] = pkTemplate->m_aspShaderTextures[ui];
    }
}
//---------------------------------------------------------------------------
NiMetaData& NiSurface::GetMetaData()
{
    return m_kMetaData;
}
//---------------------------------------------------------------------------
const NiMetaData& NiSurface::GetMetaData() const
{
    return m_kMetaData;
}
//---------------------------------------------------------------------------
void NiSurface::CompileSurface()
{   
    m_bCompiled = false;

    // We first reset all loaded textures.
    for (NiUInt32 ui = 0; ui < NUM_SURFACE_MAPS; ++ui)
    {
        m_aspShaderTextures[ui] = NULL;
    }

    NiTexture::FormatPrefs kFormatPrefs;
    kFormatPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::ALPHA_DEFAULT;
    kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::YES;
    kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;

    if (m_kDiffuseMapFile == NULL || m_kDiffuseMapFile.GetLength()==0)
    {
        if (m_kNormalMapFile == NULL || m_kNormalMapFile.GetLength() == 0)
        {
            // Making sure we have a default texture to render for the diffuse
            // map or other materials will be affected by the error.
            m_aspShaderTextures[SURFACE_MAP_DIFFUSE] = 
                NiSourceTexture::Create(NULL, kFormatPrefs);
            
            /* DT25526 fix error reporting here
            if (pkError == NULL)
                return;

            const char* pcPackageFilename = GetPackage()->GetFilename();
            const char* pcName = (const char*)m_kName;
            char acError[1024];
            NiSprintf(acError, 1024,
                "Surface material \"%s\" from the package \"%s\"\n" \
                "does not specify a diffuse or normal map texture. " \
                "As a result, the surface will not be visible until the " \
                "material is removed or has a valid texture.", pcName, 
                pcPackageFilename);

            
                pkError->ReportError("Package does not specify a diffuse or " \
                "normal map.", acError, NULL, NULL);*/

            return;
        }
    }

    //Figure out the package path
    NiString kPackageDir = GetPackage()->GetFilename();
    NiFilename kPackagePath = NiFilename(kPackageDir);
    kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
        (strlen(kPackagePath.GetFilename())) - 
        (strlen(kPackagePath.GetExt())));

    char* pcPathBuffer = NiAlloc(char, 512);
    NiPixelData* pkPixelData = NULL;
    const NiPixelFormat* pkPixelFormat = NULL;
    NiDevImageConverter kImageConverter;

    if (m_kDiffuseMapFile && m_kDiffuseMapFile.GetLength())
    {
        NiStrncpy(pcPathBuffer, 512, 
            (const char*)(kPackageDir + m_kDiffuseMapFile), NI_TRUNCATE);
        NiPath::RemoveDotDots(pcPathBuffer);

        pkPixelData = kImageConverter.ReadImageFile(pcPathBuffer, NULL);
        if (pkPixelData)
        {
            pkPixelFormat = &pkPixelData->GetPixelFormat();
            NIASSERT(pkPixelFormat);
            
            NiBooleanExtraData* pkBoolData = NiNew NiBooleanExtraData(false);
            if (pkPixelFormat->GetFormat() == NiPixelFormat::FORMAT_RGBA ||
                pkPixelFormat->GetFormat() == NiPixelFormat::FORMAT_DXT3 ||
                pkPixelFormat->GetFormat() == NiPixelFormat::FORMAT_DXT5)
            {
                pkBoolData->SetValue(true);
            }
            
            NiSourceTexture* pkBaseTex = NiSourceTexture::Create(pkPixelData, 
                kFormatPrefs);
            pkBaseTex->AddExtraData(pkBoolData);

            m_aspShaderTextures[SURFACE_MAP_DIFFUSE] = pkBaseTex;
        }
    }

    if (m_kDistributionMapFile && m_kDistributionMapFile.GetLength())
    {
        NiStrncpy(pcPathBuffer, 512, 
            (const char*)(kPackageDir + m_kDistributionMapFile), NI_TRUNCATE);
        NiPath::RemoveDotDots(pcPathBuffer);

        pkPixelData = kImageConverter.ReadImageFile(pcPathBuffer, NULL);
        if (pkPixelData)
        {
            pkPixelFormat = &pkPixelData->GetPixelFormat();
            NIASSERT(pkPixelFormat);

            // Create a texture object but we won't be loading it to the graphics 
            // card. We will combine the dist. mask for each layer into a single 
            // 4 channel texture.
            NiTexture::FormatPrefs kMaskFormatPref;

            kMaskFormatPref.m_ePixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
            kMaskFormatPref.m_eMipMapped = NiTexture::FormatPrefs::NO;
            kMaskFormatPref.m_eAlphaFmt = NiTexture::FormatPrefs::ALPHA_DEFAULT;

            bool bOldValue = NiSourceTexture::GetDestroyAppDataFlag();
            NiSourceTexture::SetDestroyAppDataFlag(false);
            NiSourceTexture* pkDistTex = NiSourceTexture::Create(pkPixelData, 
                kFormatPrefs);            
            NiSourceTexture::SetDestroyAppDataFlag(bOldValue);

            m_aspShaderTextures[SURFACE_MAP_DISTRIBUTION] = pkDistTex;
        }
    }
    
#ifndef _WII
    pkPixelData = NULL;
    pkPixelFormat = NULL;
    if (m_kNormalMapFile && m_kNormalMapFile.GetLength())
    {
        NiStrncpy(pcPathBuffer, 512, 
            (const char*)(kPackageDir + m_kNormalMapFile), NI_TRUNCATE);
        NiPath::RemoveDotDots(pcPathBuffer);

        
        pkPixelData = kImageConverter.ReadImageFile(pcPathBuffer, NULL);
        if (pkPixelData)
        {
            pkPixelFormat = &pkPixelData->GetPixelFormat();
            NIASSERT(pkPixelFormat);

            NiBooleanExtraData* pkBoolData = NiNew NiBooleanExtraData(false);
            if (pkPixelFormat->GetFormat() == NiPixelFormat::FORMAT_RGBA &&
                !pkPixelFormat->GetCompressed())
            {
                pkBoolData->SetValue(true);
            }

            NiSourceTexture* pkNormalTex = NiSourceTexture::Create(pkPixelData,
                kFormatPrefs);
            pkNormalTex->AddExtraData(pkBoolData);

            m_aspShaderTextures[SURFACE_MAP_NORMAL] = pkNormalTex;
        }
        else
        {
            /*DT25526 fix error reporting here
            if (pkError)
            {
                const char* pcName = (const char*)m_kName;
                const char* pcPackageFileName = GetPackage()->GetFilename();
                const char* pcNormalMapFile = (const char*)m_kNormalMapFile;
                char acError[1024];
                NiSprintf(acError, 1024,
                    "Surface material \"%s\" from the package \"%s\"\n" \
                    "specifies a normal map texture at \"%s\"\n" \
                    "but is unable  to locate the texture. " \
                    "As a result, the surface will not be render correctly " \
                    "until the normal map texture location is corrected.",
                    pcName, pcPackageFileName, pcNormalMapFile);
                pkError->ReportError("Unable to locate normal map texture",
                    acError, NULL, NULL);
            }
            */
        }
    }
#endif

    if (!m_aspShaderTextures[SURFACE_MAP_DIFFUSE])
    {
        m_aspShaderTextures[SURFACE_MAP_DIFFUSE] = 
            NiSourceTexture::Create(NULL, kFormatPrefs);
        /* DT25526 fix the error reporting here...
        if (pkError)
        {
            char acError[1024];
            const char* pcName = (const char*)m_kName;
            const char* pcPackageFilename = 
                (const char*)GetPackage()->GetFilename();
            const char* pcDiffuseMapFile = (const char*)m_kDiffuseMapFile;
            NiSprintf(acError, 1024,
                "Surface material \"%s\" from the package \"%s\"\n" \
                "specifies a diffuse map texture at \"%s\"\n" \
                "but is unable  to locate the texture. " \
                "As a result, the surface will not be visible until the " \
                "material is removed or has a valid diffuse texture.",
                pcName, pcPackageFilename, pcDiffuseMapFile);
            pkError->ReportError("Unable to locate diffuse map texture",
                acError, NULL, NULL);
        }
        */
    }

    pkPixelData = NULL;
    pkPixelFormat = NULL;
  
    NiFree(pcPathBuffer);
    m_bCompiled = true;
}
//---------------------------------------------------------------------------
bool NiSurface::IsCompiled() const
{
    return m_bCompiled;
}
//---------------------------------------------------------------------------
NiTexture* NiSurface::GetTexture(SurfaceMapID eMapID) const
{   
    NIASSERT(eMapID < NUM_SURFACE_MAPS);

    return m_aspShaderTextures[eMapID];
}
//---------------------------------------------------------------------------
bool NiSurface::SetMapParam(SurfaceMapID eMapID, 
    const NiFixedString& kParamName, NiFixedString& kValue)
{
    NIASSERT(eMapID < NUM_SURFACE_MAPS);
    if (eMapID >= NUM_SURFACE_MAPS)
        return false;

    NiString* pStrValue = 0;
    m_kMapParameters[eMapID].GetAt(kParamName, pStrValue);
    if (!pStrValue)
    {
        pStrValue = NiNew NiString(kValue);
        m_kMapParameters[eMapID].SetAt(kParamName, pStrValue);
    }
    else
        pStrValue->Format(kValue);

    return true;
}
//---------------------------------------------------------------------------
bool NiSurface::SetMapParam(SurfaceMapID eMapID, 
    const NiFixedString& kParamName,
    NiColorA& kValue)
{
    NIASSERT(eMapID < NUM_SURFACE_MAPS);
    if (eMapID >= NUM_SURFACE_MAPS)
        return false;

    NiString* pStrValue = 0;
    m_kMapParameters[eMapID].GetAt(kParamName, pStrValue);
    if (!pStrValue)
    {
        pStrValue = NiNew NiString(NiString::FromRGBA(kValue));
        m_kMapParameters[eMapID].SetAt(kParamName, pStrValue);
    }
    else
        pStrValue->FromRGBA(kValue);

    return true;
}
//---------------------------------------------------------------------------
bool NiSurface::GetMapParam(SurfaceMapID eMapID, 
    const NiFixedString& kParamName,
    NiColorA& kValue) const
{
    NIASSERT(eMapID < NUM_SURFACE_MAPS);
    if (eMapID >= NUM_SURFACE_MAPS)
        return false;

    NiString* pStrValue;
    if (!m_kMapParameters[eMapID].GetAt(kParamName, pStrValue))
        return false;

    return pStrValue->ToRGBA(kValue);
}
//---------------------------------------------------------------------------
bool NiSurface::RemoveMapParam(SurfaceMapID eMapID, 
    const NiFixedString& kParamName)
{
    NIASSERT(eMapID < NUM_SURFACE_MAPS);
    if (eMapID >= NUM_SURFACE_MAPS)
        return false;

    NiString* pStrValue;
    if (!m_kMapParameters[eMapID].GetAt(kParamName, pStrValue))
        return false;

    m_kMapParameters[eMapID].RemoveAt(kParamName);
    NiDelete pStrValue;

    return true;
}
