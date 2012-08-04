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


//---------------------------------------------------------------------------
inline NiNoiseMap::NiNoiseMap() : m_spTexture(0)
{
}
//---------------------------------------------------------------------------
inline NiNoiseMap::~NiNoiseMap()
{
}
//---------------------------------------------------------------------------
inline void NiNoiseMap::_SDMInit()
{
    ms_akMapFromTypeName[NiStandardMaterial::NOISE_GREYSCALE] = 
        "GreyscaleNoise";
}
//---------------------------------------------------------------------------
inline void NiNoiseMap::_SDMShutdown()
{
    ms_akMapFromTypeName[NiStandardMaterial::NOISE_GREYSCALE] = NULL;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiNoiseMap::GetMapNameFromType(
    NiStandardMaterial::NoiseMapType eType)
{
    return ms_akMapFromTypeName[eType];
}
//---------------------------------------------------------------------------
inline bool NiNoiseMap::GetTypeIDFromName(
    const NiFixedString& kName, unsigned int& uiID)
{
    if (kName == ms_akMapFromTypeName[NiStandardMaterial::NOISE_GREYSCALE])
    {
        uiID = NiStandardMaterial::NOISE_GREYSCALE;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline NiSourceTexture* NiNoiseMap::GetTexture() const
{
    return m_spTexture;
}
//---------------------------------------------------------------------------
inline NiNoiseMap::NoiseType NiNoiseMap::GetNoiseType() const
{
    return (NoiseType)GetField(NOISETYPE_MASK, NOISETYPE_POS);
}
//---------------------------------------------------------------------------
inline void NiNoiseMap::SetNoiseType(NoiseType eNoiseType)
{
    SetField(NiInt32ToUInt16(eNoiseType), NOISETYPE_MASK, NOISETYPE_POS);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::ClampMode NiNoiseMap::GetClampMode() const
{
    return (NiTexturingProperty::ClampMode) GetField(CLAMPMODE_MASK,
        CLAMPMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiNoiseMap::SetClampMode(
    NiTexturingProperty::ClampMode eClampMode)
{
    SetField(NiInt32ToUInt16(eClampMode), CLAMPMODE_MASK, CLAMPMODE_POS);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty::FilterMode NiNoiseMap::GetFilterMode() const
{
    return (NiTexturingProperty::FilterMode) GetField(FILTERMODE_MASK,
        FILTERMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiNoiseMap::SetFilterMode(
    NiTexturingProperty::FilterMode eFilterMode)
{
    SetField(NiInt32ToUInt16(eFilterMode), FILTERMODE_MASK, FILTERMODE_POS);
}
//---------------------------------------------------------------------------
