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

#ifndef NINOISEMAP_H
#define NINOISEMAP_H

#include <NiMemObject.h>
#include "NiTexturingProperty.h"
#include "NiSourceTexture.h"
#include "NiStandardMaterial.h"

/**
    A noise map creates a texture containing random 8-bit values for each 
    pixel's channel.

    Currently only single channel (red) noise maps are supported
 */
class NIMAIN_ENTRY NiNoiseMap : public NiMemObject
{
    NiDeclareFlags(unsigned short);
    NiDeclareRootRTTI(NiNoiseMap);

public:

    // When adding new enumerations be sure to update the bitfield
    // mask & position enums below.
    enum NoiseType
    {
        NT_RAND   = 0,
        NT_MAX    = 1
    };

    /// Default constructor - creates a non-initialized noise map
    inline NiNoiseMap();
    /// Empty destructor.
    inline virtual ~NiNoiseMap();

    /**
        @return the current noise generation technique in use
     */
    inline NoiseType GetNoiseType() const;
    
    /**
        @return a fixed string representation of the given map type in
            NiStandardMaterial
     */
    inline static const NiFixedString& GetMapNameFromType(
        NiStandardMaterial::NoiseMapType eType);

    /**
        @return the NiStandardMaterial::NoiseMapType that corresponds to the 
            given type name
     */
    inline static bool GetTypeIDFromName(const NiFixedString& kName, 
        unsigned int& uiID);

    /**
        Get the generated noise NiTexture used by this map
     */
    inline NiSourceTexture* GetTexture() const;

    /**
        Clamping mode of the generated noise NiTexture
     */
    //@{
    inline NiTexturingProperty::ClampMode GetClampMode() const;
    inline void SetClampMode(NiTexturingProperty::ClampMode eClampMode);
    //@}

    /**
        Filtering mode of the generated noise NiTexture
     */
    //@{
    inline NiTexturingProperty::FilterMode GetFilterMode() const;
    inline void SetFilterMode(NiTexturingProperty::FilterMode eFilterMode);
    //@{

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

    /**
        Initializes the given noise map according to the given parameters. If 
        the given noise map has already been initialized, its existing data is 
        erased and it is re-initialized.
     */
    static bool Initialize(NiNoiseMap* pkNoiseMap, NoiseType eNoiseType,
        NiTexturingProperty::FilterMode eFilterMode,
        NiTexturingProperty::ClampMode eClampMode, 
        NiUInt32 uiTextureSize, NiUInt32 uiRandSeed = 0);

protected:

    // Flags
    enum
    {
        NOISETYPE_POS = 0,
        NOISETYPE_MASK = 0x0003,

        FILTERMODE_POS = 2,
        FILTERMODE_MASK = 0x01FC,

        CLAMPMODE_POS = 9,
        CLAMPMODE_MASK  = 0x1E00,
    };

    // Protected so that the user doesn't attempt to change the noise type after
    // the texture has been generated
    inline void SetNoiseType(NoiseType eNoiseType);

    static NiFixedString ms_akMapFromTypeName[
        NiStandardMaterial::NOISE_MAX];

    NiSourceTexturePtr m_spTexture;
};

#include "NiNoiseMap.inl"

#endif // NINOISEMAP_H
