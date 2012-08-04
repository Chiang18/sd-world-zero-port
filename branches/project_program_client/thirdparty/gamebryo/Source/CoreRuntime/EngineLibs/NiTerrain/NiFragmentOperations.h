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

#ifndef NIFRAGMENTOPERATIONS_H
#define NIFRAGMENTOPERATIONS_H

#include "NiFragment.h"

class NITERRAIN_ENTRY NiFragmentOperations : public 
    NiFragment
{
public:

    /// Constructor
    NiFragmentOperations();

    enum 
    {
        /// The current vertex version. Adjusting this invalidates the vertex
        /// cache and forces new shaders to be generated.
        VERTEX_VERSION = 1,
        /// The current geometry version. Adjusting this invalidates the 
        /// geometry cache and forces new shaders to be generated.
        GEOMETRY_VERSION = 0,
        /// The current pixel version. Adjusting this invalidates the pixel
        /// cache and forces new shaders to be generated.
        PIXEL_VERSION = 1,
    };

    /**
        This method controls the application of alpha testing to the pixel 
        shade tree. This is currently only used for D3D10 shade trees.
    */
    bool HandleAlphaTest(Context& kContext,
        bool bAlphaTest,
        NiMaterialResource* pkAlphaTestInput);
        
    /**
        This is a utility method to add material nodes to normalize a float2,
        float3, or float4 vector.
    */
    bool NormalizeVector(Context& kContext, 
        NiMaterialResource*& pkVector);
        
    /**
        This is a utility method to add material nodes to add float, float2,
        float3, or float4 vectors.
    */
    bool AddVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to multiply float, 
        float2, float3, or float4 vectors.
    */
    bool MultiplyVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to multiply/add float3 
        vectors.
    */
    bool MultiplyAddVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource* pkVector3, NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to linearly interpolate
        float, float2, float3, or float4 vectors.
    */
    bool LerpVector(Context& kContext, 
        NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
        NiMaterialResource* pkLerpAmount, NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to multiply a float, 
        float2, float3, or float4 vector by a single float.
    */
    bool ScaleVector(Context& kContext, 
        NiMaterialResource* pkVector, NiMaterialResource* pkScale,
        NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to saturate float, 
        float2, float3, or float4 vector.
    */
    bool SaturateVector(Context& kContext, 
        NiMaterialResource* pkVector, NiMaterialResource*& pkValue);
        
    /**
        This is a utility method to add material nodes to multiply a float3 
        vector by the sum of another float3 vector and a scalar.
    */
    bool MultiplyScalarSatAddVector( 
        Context& kContext, NiMaterialResource* pkVector1, 
        NiMaterialResource* pkVector2, NiMaterialResource* pkScale, 
        NiMaterialResource*& pkValue);
        
    /**
        This method will add material nodes to split a float4 vector into 
        float3 and float vectors.
    */
    bool SplitColorAndOpacity(Context& kContext, 
        NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkColor,
        NiMaterialResource*& pkOpacity);
        
    /**
        This method will add material nodes to split a float4 vector into 
        float3 and float vectors.
    */
    bool SplitRGBA(Context& kContext, 
        NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkRed,
        NiMaterialResource*& pkGreen, NiMaterialResource*& pkBlue,
        NiMaterialResource*& pkAlpha);
        
    /**
        This utility method will invoke conversion functions when binding 
        resources with common float count mismatches, i.e. binding a float4
        to a float3 truncates the fourth float value. When the resources match
        types, a normal bind occurs.
    */
    bool TypeCastBind(Context& kContext, 
        NiMaterialResource* pkSrc, NiMaterialResource* pkDest);
};

#endif  //#ifndef NIFRAGMENTOPERATIONS_H
