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
inline void NiPSSMShadowClickGenerator::SetActiveGenerator(
    NiShadowGenerator* pkGenerator)
{
    ms_pkActiveGenerator = pkGenerator;
}
//---------------------------------------------------------------------------
inline const NiShadowGenerator* NiPSSMShadowClickGenerator::GetActiveGenerator()
{
    return ms_pkActiveGenerator;
}
//---------------------------------------------------------------------------
inline bool NiPSSMShadowClickGenerator::LightSupportsPSSM(
    const NiShadowGenerator* pkGenerator, const NiLight* pkLight)
{
    if (pkGenerator != GetActiveGenerator())
        return false;

    if (pkLight->GetEffectType() != NiDynamicEffect::SHADOWDIR_LIGHT)
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSSMShadowClickGenerator::EncodeDescriptorSliceCount(
    NiUInt32 uiSliceCount)
{
    if (uiSliceCount <= 1)
        return 0;

    NIASSERT(uiSliceCount <= 16);
    return (uiSliceCount - 1) / 4 + 1;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSSMShadowClickGenerator::DecodeDescriptorMaxSliceCount(
    NiUInt32 uiEncodedSliceCount)
{
    if (uiEncodedSliceCount == 0)
        return 1;

    NIASSERT(uiEncodedSliceCount <= 4);
    return uiEncodedSliceCount * 4;
}
//---------------------------------------------------------------------------
inline NiNoiseMap* NiPSSMShadowClickGenerator::GetNoiseMap() const
{
    return m_pkNoiseMap;
}
//---------------------------------------------------------------------------