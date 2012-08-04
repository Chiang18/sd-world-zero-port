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

#include "NiMainPCH.h"
#include "NiNoiseMap.h"

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiNoiseMap);
NiFixedString NiNoiseMap::ms_akMapFromTypeName[
    NiStandardMaterial::NOISE_MAX];

//---------------------------------------------------------------------------
bool NiNoiseMap::Initialize(NiNoiseMap* pkNoiseMap, NoiseType eNoiseType,
    NiTexturingProperty::FilterMode eFilterMode,
    NiTexturingProperty::ClampMode eClampMode,
    NiUInt32 uiTextureSize, NiUInt32 uiRandSeed)
{
    NIASSERT(pkNoiseMap);

    pkNoiseMap->SetNoiseType(eNoiseType);
    pkNoiseMap->SetFilterMode(eFilterMode);
    pkNoiseMap->SetClampMode(eClampMode);

    // Create the pixel data
    NiPixelFormat kPixelFormat(
        NiPixelFormat::FORMAT_ONE_CHANNEL,
        NiPixelFormat::COMP_INTENSITY,
        NiPixelFormat::REP_NORM_INT,
        8);

    NiPixelData* pkPixelData = NiNew NiPixelData(
        uiTextureSize, uiTextureSize, kPixelFormat);

    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::SINGLE_COLOR_8;

    // Create the noise
    unsigned char* pucPixels = pkPixelData->GetPixels();
    NiUInt32 uiNumPixels = uiTextureSize * uiTextureSize;

    switch (eNoiseType)
    {
    default:
    case NT_RAND:
        {
            NiSrand(uiRandSeed);
            for (NiUInt32 ui = 0; ui < uiNumPixels; ++ui)
            {
                NiUInt32 uiRand = NiRand();
                *pucPixels = (unsigned char)(uiRand >> 7 & 255);
                pucPixels++;
            }
        }
    }

    pkNoiseMap->m_spTexture = NiSourceTexture::Create(pkPixelData, kPrefs);
    return pkNoiseMap->m_spTexture != NULL;
}
//---------------------------------------------------------------------------
