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

#ifndef NIFRAGMENTLIGHTING_H
#define NIFRAGMENTLIGHTING_H

#include <NiBitfield.h>
#include "NiFragment.h"
#include "NiFragmentOperations.h"

#include <NiStandardMaterial.h>
#include <NiPSSMShadowClickGenerator.h>
#include <NiBitField.h>

/**
    NiFragmentLighting is an abstraction of the standard lighting pipeline
    for use in custom materials. It allows consistent dynamic lighting to be
    applied to a scene without having to implement the code for each material 
    written. The basic use involves instantiating the Fragment object in the
    material, setting the owner material with the setOwner function. Then
    generate the appropriate descriptor for lighting with GenerateDescriptor,
    and GenerateShaderDesc array. 

    The descriptors generated are generic and can be mapped into the material's
    descriptor easily either using the templated SetDescriptor functions, or
    by manually applying the descriptor bits in the material. To support the
    templated functions (including fallbacks) the following bits must be 
    declared in some form on the descriptors:

    Material Descriptor:
        1 bits - SPECULAR
        1 bits - VERTEXCOLORS
        2 bits - AMBDIFFEMISSIVE
        1 bits - LIGHTINGMODE
        1 bits - APPLYMODE
        1 bits - PERVERTEXFORLIGHT
        4 bits - POINTLIGHTCOUNT
        4 bits - SPOTLIGHTCOUNT
        4 bits - DIRLIGHTCOUNT
        8 bits - SHADOWMAPFORLIGHT
        2 bits - SHADOWTECHNIQUE
        1 bits - PSSMSLICETRANSITIONSENABLED
        3 bits - PSSMSLICECOUNT
        4 bits - PSSMWHICHLIGHT

    Vertex Descriptor:
        1 bits - OUTPUTWORLDPOS
        1 bits - OUTPUTWORLDNBT
        1 bits - OUTPUTWORLDVIEW
        1 bits - SPECULAR
        4 bits - POINTLIGHTCOUNT
        4 bits - SPOTLIGHTCOUNT
        4 bits - DIRLIGHTCOUNT
        1 bits - VERTEXCOLORS
        1 bits - VERTEXLIGHTSONLY
        2 bits - AMBDIFFEMISSIVE
        2 bits - LIGHTINGMODE
        1 bits - APPLYMODE

    Pixel Descriptor:
        1 bits - WORLDPOSITION
        1 bits - WORLDNORMAL
        1 bits - WORLDVIEW
        4 bits - POINTLIGHTCOUNT
        4 bits - SPOTLIGHTCOUNT
        4 bits - DIRLIGHTCOUNT
        8 bits - SHADOWMAPFORLIGHT
        2 bits - AMBDIFFEMISSIVE
        2 bits - LIGHTINGMODE
        1 bits - APPLYAMBIENT
        1 bits - APPLYEMISSIVE
        4 bits - SHADOWTECHNIQUE
        1 bits - PSSMSLICETRANSITIONSENABLED
        3 bits - PSSMSLICECOUNT
        4 bits - PSSMWHICHLIGHT
        1 bits - APPLYMODE
        1 bits - PERVERTEXLIGHTING
*/
class NITERRAIN_ENTRY NiFragmentLighting : public NiFragment
{
public:

    /// Constructor
    NiFragmentLighting();

    enum 
    {
        /// The standard pipeline maximum number of lights
        STANDARD_PIPE_MAX_LIGHTS = 8,
        /// The current vertex version. Adjusting this invalidates the vertex
        /// cache and forces new shaders to be generated.
        VERTEX_VERSION = 1,
        /// The current geometry version. Adjusting this invalidates the 
        /// geometry cache and forces new shaders to be generated.
        GEOMETRY_VERSION = 0,
        /// The current pixel version. Adjusting this invalidates the pixel
        /// cache and forces new shaders to be generated.
        PIXEL_VERSION = 1
    };

    // Reference all the relevant enumerations from standard material 
    typedef enum NiStandardMaterial::VertexColorType VertexColorType;
    typedef enum NiStandardMaterial::AmbDiffEmissiveEnum AmbDiffEmissiveEnum;
    typedef enum NiStandardMaterial::LightingModeEnum LightingModeEnum;
    typedef enum NiStandardMaterial::ApplyMode ApplyMode;
    typedef enum NiStandardMaterial::LightType LightType;
    typedef enum NiStandardMaterial::TextureMap TextureMap;
    typedef NiStandardMaterial EnumSource;

    /// Lighting descriptor used to describe the application of lighting
    /// in a material's implementation
    struct Descriptor
    {   
        Descriptor();   

        NiUInt32 uiNumPointLights;
        NiUInt32 uiNumDirectionalLights;
        NiUInt32 uiNumSpotLights;
        AmbDiffEmissiveEnum eAmbDiffEmissive;
        LightingModeEnum eLightingMode;
        ApplyMode eApplyMode;
        bool bApplyAmbient;
        bool bApplyEmissive;
        bool bSpecularOn;
        bool bPerVertexForLights;
        bool bPSSMSliceTransitionEnabled;
        NiUInt16 usPSSMSliceCount;
        NiUInt16 usPSSMWhichLight;
        NiUInt32 uiShadowMapBitfield;
        NiUInt16 usShadowTechnique;
        bool bVertexOnlyLights;
        bool bVertexColors;
    };

    /// Lighting pixel descriptor used to describe the application of lighting
    /// in the generation of a pixel shader (see Descriptor for more 
    /// information)
    struct PixelDescriptor
    {
        PixelDescriptor();

        // Pixel shader requires the following inputs
        bool bInputWorldView;
        bool bInputWorldPos;
        bool bInputWorldNormal;

        // Standard pixel shader lighting descriptor
        NiUInt32 uiNumPointLights;
        NiUInt32 uiNumDirectionalLights;
        NiUInt32 uiNumSpotLights;
        AmbDiffEmissiveEnum eAmbDiffEmissive;
        LightingModeEnum eLightingMode;
        ApplyMode eApplyMode;        
        bool bSpecularOn;

        bool bApplyAmbient;
        bool bApplyEmissive;
        bool bPerVertexForLights;
        bool bPSSMSliceTransitionEnabled;
        NiUInt16 usPSSMSliceCount;
        NiUInt16 usPSSMWhichLight;
        NiUInt32 uiShadowMapBitfield;
        NiUInt16 usShadowTechnique;
    };

    /// Lighting vertex descriptor used to describe the application of lighting
    /// in the generation of a vertex shader
    struct VertexDescriptor
    {   
        VertexDescriptor();

        // Vertex shader requires the following outputs
        bool bOutputWorldView;
        bool bOutputWorldPos;
        bool bOutputWorldNormal;

        // Standard vertex shader lighting descriptor
        NiUInt32 uiNumPointLights;
        NiUInt32 uiNumDirectionalLights;
        NiUInt32 uiNumSpotLights;
        AmbDiffEmissiveEnum eAmbDiffEmissive;
        LightingModeEnum eLightingMode;
        ApplyMode eApplyMode;
        bool bSpecularOn;
    
        bool bVertexOnlyLights;      
        bool bVertexColors;
    };

    /// Set a flag to force lighting to be done per-pixel.
    void SetForcePerPixelLighting(bool bForce);
    
    /// Get the value of the flag that forces per-pixel lighting.
    bool GetForcePerPixelLighting() const;

    /**
        Set the value of the flag that staturates the final lighting 
        coefficients. When set to true, the maximum contribution of light
        to a pixel will be at maximum (1,1,1). When set to false, it is possible
        to have many lights affecting a pixel to cause that pixel to 
        over expose and display pure white (what is called "White Saturation")
    */
    void SetSaturateShading(bool bSaturate);

    /**
        Get the value of the flag that staturates the final lighting 
        coefficients
    */
    bool GetSaturateShading() const;

    /**
        Set a flag to saturate texture accumulation values before applying the
        shader coefficients.
    */
    void SetSaturateTextures(bool bSaturate);

    /**
        Get the value of a flag that, when set, saturates the texture 
        accumulation values before applying the shader coefficients. 
    */
    bool GetSaturateTextures() const;
 
    /**
        Generate a lighting descriptor to handle the application of lighting
        based on the state of this geometry. 
    */
    bool GenerateDescriptor(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        Descriptor& kMaterialDesc, bool bNormals, bool bParallaxMapping, 
        bool bNormalMapping);

    /**
        By default this creates one render 
        pass with an NiStandardPixelProgramDescriptor and an 
        NiStandardVertexProgramDescriptor.
    */
    bool GenerateShaderDescArray(
        Descriptor& kMaterialDescriptor, 
        PixelDescriptor& kPixelDesc, VertexDescriptor& kVertexDesc,
        NiUInt32 uiPassIndex, NiUInt32& uiRequiredPassCount);

    /**
        Set the appropriate bits in a compliant material descriptor to
        match the details in a lighting descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void SetDescriptor(Type* pkDescriptor, 
        const Descriptor& kDescriptor);
    
    /**
        Get the appropriate bits in a compliant material descriptor and
        format them into a standard material descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void GetDescriptor(const Type* pkDescriptor, 
        Descriptor& kDescriptor);

    /**
        Set the appropriate bits in a compliant vertex descriptor to
        match the details in a lighting vertex descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void SetDescriptor(Type* pkVertexDesc, 
            const VertexDescriptor& kDescriptor);

    /**
        Set the appropriate bits in a compliant pixel descriptor to
        match the details in a lighting pixel descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void SetDescriptor(Type* pkPixelDesc, 
            const PixelDescriptor& kDescriptor);

    /**
        Get the appropriate bits in a compliant vertex descriptor and
        format them into a lighting vertex descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void GetDescriptor(const Type* pkVertexDesc, 
        VertexDescriptor& kDescriptor);
    
    /**
        Get the appropriate bits in a compliant pixel descriptor and
        format them into a lighting pixel descriptor. A description of the
        relevant bits to lighting can be found in the description of the
        NiFragmentLighting class.
    */
    template <class Type> static void GetDescriptor(const Type* pkPixelDesc, 
        PixelDescriptor& kDescriptor);

    /**
        This fallbacks structure is used to store the relevant fallback routine
        options available with NiFragmentLighting. These routines may be used
        with any compliant set of Descriptors (as described in the introduction
        to the NiFragmentLighting class) as it uses the SetDescriptor and 
        GetDescriptor functions. Alternativly, a developer may decide to write 
        their own fallback routine, in which case, these templated routines
        may be used as a base.
    */
    template <
        class MaterialDescriptorType,
        class VertexDescriptorType,
        class PixelDescriptorType>        
        struct Fallbacks
    {
        /**
            Create a new pass for each pass with lighting. The lights are evenly 
            divided from the original pass into the new passes. Note that this 
            only affects per-pixel lights. These passes will be additively 
            blended.
        */
        static bool SplitPerPixelLights(ReturnCode eFailedRC,
            NiUInt32 uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
            NiUInt32 uiMaxCount, NiUInt32& uiCount);

        // This technique will remove any PSSM enabled shadow map from the
        // failed passes.
        static bool DropPSSMShadowMaps(ReturnCode eFailedRC,
            NiUInt32 uiFailedPass, RenderPassDescriptor* pkRenderPasses);

        // This technique will remove the shadow maps from the failed passes.
        static bool DropShadowMaps(ReturnCode eFailedRC,
            NiUInt32 uiFailedPass, RenderPassDescriptor* pkRenderPasses);

        /**
            Create a new pass for each pass with lighting. The lights are evenly 
            divided from the original pass into the new passes. Note that this 
            only affects per-vertex lighting. These passes will be additively 
            blended.
        */
        static bool SplitPerVertexLights(ReturnCode eFailedRC,
            NiUInt32 uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
            NiUInt32 uiMaxCount, NiUInt32& uiCount);
    };
    
    /**
        This method adds the appropriate fragment to initialize the 
        ambient/diffuse/specular/emissive colors from per-vertex values or 
        NiMaterialProperty constants for the pixel shader or vertex
        shader depending on the stage in which lighting occurs.
    */
    bool HandleInitialSpecAmbDiffEmissiveColor(Context& kContext,
        bool bSpecularOn, AmbDiffEmissiveEnum eADF, 
        LightingModeEnum eLightingMode,
        NiMaterialResource*& pkDiffuseColorRes,
        NiMaterialResource*& pkSpecularColorRes,
        NiMaterialResource*& pkSpecularPowerRes,
        NiMaterialResource*& pkAmbientColorRes,
        NiMaterialResource*& pkEmissiveColorRes,
        NiMaterialResource*& pkOpacityRes);
    
    /**
        This method controls the lighting and material property pipeline 
        for the vertex shader. It controls the application of per-vertex 
        lighting or alternatively does the per-vertex work needed for per-pixel
        lighting.
    */
    bool HandleVertexLightingAndMaterials(Context& kContext,
        VertexDescriptor& kVertexDesc,
        NiMaterialResource* pkWorldPos,
        NiMaterialResource* pkWorldNormal,
        NiMaterialResource* pkWorldView);

    /**
        This method is called by GeneratePixelShadeTree to initialize most of 
        the material variables for the texture and lighting pipelines. This 
        also includes using the correct per-vertex parameters for per-pixel or
        per-vertex lighting. Variables are only instantiated if they are 
        requested by the descriptor.
    */
    bool HandlePixelMaterialInitialValues(Context& kContext,
        PixelDescriptor& kPixelDesc,
        NiMaterialResource*& pkMatDiffuse,
        NiMaterialResource*& pkMatSpecular,
        NiMaterialResource*& pkSpecularPower,
        NiMaterialResource*& pkGlossiness,
        NiMaterialResource*& pkMatAmbient,
        NiMaterialResource*& pkMatEmissive,
        NiMaterialResource*& pkTexDiffuseAccum,
        NiMaterialResource*& pkTexSpecularAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum,
        NiMaterialResource*& pkOpacityAccum,
        NiMaterialResource*& pkLightDiffuseAccum,
        NiMaterialResource*& pkLightSpecularAccum,
        NiMaterialResource*& pkLightAmbientAccum);

    /**
        This method combines the input material and lighting values to compute
        the specular and diffuse coefficients of the shading equation. This 
        method is called by either the vertex or pixel shader depending on
        the stage in which lighting is performed.
    */
    bool HandleShadingCoefficients(Context& kContext,
        bool bSaturateShading,
        NiMaterialResource* pkMatEmissive, 
        NiMaterialResource* pkMatDiffuse, NiMaterialResource* pkMatAmbient, 
        NiMaterialResource* pkMatSpecular, 
        NiMaterialResource* pkLightSpecularAccum,
        NiMaterialResource* pkLightDiffuseAccum, 
        NiMaterialResource* pkLightAmbientAccum,
        NiMaterialResource*& pkDiffuseCoeff, 
        NiMaterialResource*& pkSpecularCoeff);

    /**
        This method is simply an overload of the more specific HandleLighting
        function, it simply expands the data found in the PixelDescriptor
        to supply the other function with the required information.
    */
    bool HandleLighting(Context& kContext, 
        PixelDescriptor& kPixelDesc,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        NiMaterialResource* pkViewVector, 
        NiMaterialResource* pkSpecularPower,
        NiMaterialResource*& pkAmbientAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum);

    /**
        This method controls the contribution of lighting to the object.
        It can be called from either the vertex or pixel shader depending on
        the context in which lighting is occurring.
    */
    bool HandleLighting(Context& kContext, NiUInt32 uiShadowAtlasCells, 
        NiUInt32 uiPSSMWhichLight, bool bSliceTransitions,
        bool bSpecular, NiUInt32 uiNumPoint, NiUInt32 uiNumDirectional,
        NiUInt32 uiNumSpot, NiUInt32 uiShadowBitfield,
        NiUInt32 uiShadowTechnique,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        NiMaterialResource* pkViewVector, 
        NiMaterialResource* pkSpecularPower,
        NiMaterialResource*& pkAmbientAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum);

    /**
        This method computes the multiplier that a shadow applies to a given
        light. This method is called when generating a pixel shade tree.
    */
    bool HandleShadow(Context& kContext, NiUInt32 uiShadowAtlasCells, 
        bool bSliceTransitions, NiUInt32 uiLight, LightType eType, 
        NiUInt32 uiLightByType, NiMaterialResource* pkWorldPos, 
        NiMaterialResource* pkWorldNorm, NiUInt32 uiShadowTechnique, 
        NiMaterialResource*& pkShadow);

    /**
        This method controls the contribution of a single light to an object.
        This method can be called from either the vertex or pixel shade tree
        depending on where lighting is occurring.
    */
    bool HandleLight(Context& kContext, NiUInt32 uiLight,
        LightType eType, NiUInt32 uiLightByType, 
        NiMaterialResource* pkWorldPos, 
        NiMaterialResource* pkWorldNorm, NiMaterialResource* pkViewVector, 
        NiMaterialResource* pkSpecularPower,
        NiMaterialResource*& pkAmbientAccum,
        NiMaterialResource*& pkDiffuseAccum,
        NiMaterialResource*& pkSpecularAccum,
        bool bSpecular, NiMaterialResource* pkShadow);

    /**
        This method controls the combination of the shading coefficients from
        a material, textures, and lighting. This also handles the differences 
        in computation needed for per-pixel and per-vertex lighting.
    */
    bool HandleColorAccumulation(Context& kContext,
        PixelDescriptor& kPixelDesc,
        NiMaterialResource* pkMatEmissive, 
        NiMaterialResource* pkMatDiffuse, 
        NiMaterialResource* pkMatAmbient, 
        NiMaterialResource* pkMatSpecular, 
        NiMaterialResource* pkLightSpecularAccum, 
        NiMaterialResource* pkLightDiffuseAccum, 
        NiMaterialResource* pkLightAmbientAccum,
        NiMaterialResource* pkGlossiness, 
        NiMaterialResource* pkTexDiffuseAccum, 
        NiMaterialResource* pkTexSpecularAccum,
        NiMaterialResource*& pkSpecularAccum,
        NiMaterialResource*& pkDiffuseAccum);

    /**
        Get the name of the texture and its index for that texture name from 
        the texture enumeration.
    */
    static bool GetTextureNameFromTextureEnum(TextureMap eMap,
        NiFixedString& kString, NiUInt32& uiOccurance);

protected:
    
    /**
        Set the vertex color bitfield based on existence of a vertex color
        vertex stream on the mesh. If they exist, bVertexColors is true.
    */
    bool SetVertexColorDescriptor(Descriptor& kDesc, 
        const NiRenderObject* pkMesh,
        bool& bVertexColors);

    /**
        Set the AMBDIFFEMISSIVE and LIGHTINGMODE bitfield entries based on the
        NiVertexColorProperty attached to the mesh.
    */
    bool SetVertexColorPropertyDescriptor(Descriptor& kDesc, 
        const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState, bool bVertexColors);

    /**
        Set the SPECULAR entry based on the NiSpecularProperty attached to the
        mesh.
    */
    bool SetSpecularPropertyDescriptor(Descriptor& kDesc, 
        const NiRenderObject* pkMesh,     
        const NiPropertyState* pkPropState, bool& bSpecularEnabled);

    /**
        Since no bitfield entries are associated with the material property, 
        this method currently does nothing.
    */
    bool SetMaterialPropertyDescriptor(Descriptor& kDesc, 
        const NiRenderObject* pkMesh,
        const NiPropertyState* pkPropState);
    
    /**
        This method invokes the AddLight method for every light in the input 
        effect state.
    */
    bool SetLightsDescriptor(Descriptor& kDesc, const NiRenderObject* pkMesh, 
        const NiDynamicEffectState* pkEffectState);

    /**
        Add an NiLight to the bitfield. uiWhichLight is which slot to add the 
        light in. If the light has a shadow generator, then shadows will be 
        added as well. This function returns true if the light could be added.
    */
    bool AddLight(Descriptor& kDesc, NiLight* pkLight, 
        NiUInt32 uiWhichLight,
        const NiRenderObject* pkGeometry = NULL);

    /**
        If the specified light exists, determine its type. Returns true if the 
        light exists.
    */
    bool GetLightType(Descriptor& kDesc, NiUInt32 uiWhichLight,
        NiStandardMaterial::LightType& eLightType);

    /// Returns true if the specified light exists and it is a shadow caster.
    bool GetLightInfo(Descriptor& kDesc, NiUInt32 uiWhichLight, 
        bool& bShadowed);

    /// Set whether or not a light is a shadow caster.
    void SetLightInfo(Descriptor& kDesc, NiUInt32 uiWhichLight, 
        bool bShadowed);

    /// Standard shader operation fragments
    NiFragmentOperations m_kOperations;

    /// Flag for forcing per-pixel lighting in all shade trees
    bool m_bForcePerPixelLighting;

    /// Flag for forcing saturation of shading values
    bool m_bSaturateShading;
    
    /// Flag for forcing saturation of all texture lookups
    bool m_bSaturateTextures;
};

NiSmartPointer(NiFragmentLighting);

#include "NiFragmentLighting.inl"

#endif  //#ifndef NIFRAGMENTLIGHTING_H
