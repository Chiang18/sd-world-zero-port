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

#pragma once
#ifndef NISHADOWGENERATORCOMPONENT_H
#define NISHADOWGENERATORCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityComponentInterface.h"
#include <NiFlags.h>
#include <NiShadowGenerator.h>
#include <NiPSSMConfiguration.h>
#include <NiPSSMShadowClickGenerator.h>
#include <NiTArray.h>
#include "NiEntityInterface.h"

NiSmartPointer(NiShadowGeneratorComponent);


class NIENTITY_ENTRY NiShadowGeneratorComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned int);

public:
    // Error strings.
    static NiFixedString ERR_SCENE_ROOT_POINTER_NOT_FOUND;
    static NiFixedString ERR_SCENE_ROOT_POINTER_NOT_A_DYNAMIC_EFFECT;

    // Semantic types.
    static NiFixedString SEMANTIC_SIZE_HINT;
    static NiFixedString SEMANTIC_SHADOW_TECHNIQUE;
    static NiFixedString SEMANTIC_DEPTH_BIAS;

    NiShadowGeneratorComponent(unsigned int uiCasterArraySize = 0,
        unsigned int uiCasterArrayGrowBy = 8,
        unsigned int uiUnaffectedReceiverArraySize = 0,
        unsigned int uiUnaffectedReceiverArrayGrowBy = 8);
    NiShadowGeneratorComponent(bool bCastShadows, bool bStaticShadows,
        unsigned short usSizeHint, bool bStrictlyObserveSizeHint,
        const NiFixedString& kShadowTechnique, bool bRenderBackfaces,
        float fDepthBias, bool bUseDefaultDepthBias,
        unsigned int uiCasterArraySize = 0,
        unsigned int uiCasterArrayGrowBy = 8,
        unsigned int uiUnaffectedReceiverArraySize = 0,
        unsigned int uiUnaffectedReceiverArrayGrowBy = 8,
        bool bPSSMEnabled = false,
        float fPSSMSliceTransitionSize = 300.0f, 
        float fPSSMSliceTransitionScale = 0.0015f,
        float fPSSMSliceLambda = 0.5f, unsigned short usPSSMNumSlices = 4,
        bool bPSSMSceneDependentFrustums = false, 
        bool bPSSMShimmerSuppression = true,
        bool bPSSMSliceTransition = false);

    virtual ~NiShadowGeneratorComponent();

    inline void SetCastShadows(bool bCastShadows);
    inline bool GetCastShadows() const;

    inline void SetStaticShadows(bool bStaticShadows);
    inline bool GetStaticShadows() const;

    inline void SetSizeHint(unsigned short usSizeHint);
    inline unsigned short GetSizeHint() const;

    inline void SetStrictlyObserveSizeHint(bool bStrictlyObserveSizeHint);
    inline bool GetStrictlyObserveSizeHint() const;

    inline unsigned int GetUnaffectedCasterCount() const;
    inline NiEntityInterface* GetUnaffectedCasterAt(unsigned int uiIndex) const;
    inline void AddUnaffectedCaster(NiEntityInterface* pkEntity);
    inline void RemoveUnaffectedCaster(NiEntityInterface* pkEntity);
    inline void RemoveUnaffectedCasterAt(unsigned int uiIndex);
    inline void RemoveAllUnaffectedCasters();

    inline unsigned int GetUnaffectedReceiverCount() const;
    inline NiEntityInterface* GetUnaffectedReceiverAt(unsigned int uiIndex) const;
    inline void AddUnaffectedReceiver(NiEntityInterface* pkEntity);
    inline void RemoveUnaffectedReceiver(NiEntityInterface* pkEntity);
    inline void RemoveUnaffectedReceiverAt(unsigned int uiIndex);
    inline void RemoveAllUnaffectedReceivers();

    inline void SetShadowTechnique(const NiFixedString& kShadowTechnique);
    inline const NiFixedString& GetShadowTechnique() const;

    inline void SetRenderBackfaces(bool bRenderBackfaces);
    inline bool GetRenderBackfaces() const;

    inline void SetDepthBias(float fDepthBias);
    inline float GetDepthBias() const;

    inline void SetUseDefaultDepthBias(bool bUseDefaultDepthBias);
    inline bool GetUseDefaultDepthBias() const;

    inline void SetPSSMEnabled(bool bEnabled);
    inline bool GetPSSMEnabled() const;

    inline void SetPSSMNumSlices(unsigned short uiNumSlices);
    inline unsigned short GetPSSMNumSlices() const;

    inline void SetPSSMSceneDependentFrustums(bool bEnabled);
    inline bool GetPSSMSceneDependentFrustums() const;

    inline void SetPSSMShimmerSupression(bool bEnabled);
    inline bool GetPSSMShimmerSuppression() const;

    inline void SetPSSMSliceTransition(bool bEnabled);
    inline bool GetPSSMSliceTransition() const;

    inline void SetPSSMSliceTransitionSize(float fSize);
    inline float GetPSSMSliceTransitionSize() const;

    inline void SetPSSMSliceTransitionScale(float fScale);
    inline float GetPSSMSliceTransitionScale() const;

    inline void SetPSSMSliceLambda(float fLambda);
    inline float GetPSSMSliceLambda() const;

    inline void SetPSSMCameraDistanceScaleFactor(float fScaleFactor);
    inline float GetPSSMCameraDistanceScaleFactor() const;

    inline NiShadowGenerator* GetShadowGenerator() const;
    inline NiPSSMConfiguration* GetConfiguration() const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiShadowGeneratorComponent(unsigned int uiCasterArraySize,
        unsigned int uiCasterArrayGrowBy,
        unsigned int uiUnaffectedReceiverArraySize,
        unsigned int uiUnaffectedReceiverArrayGrowBy,
        NiShadowGeneratorComponent* pkMasterComponent);

    // Flags.
    enum
    {
        SHADOW_GENERATOR_PROPERTIES_CHANGED_MASK    = 0x00001,
        UNAFFECTED_CASTERS_CHANGED_MASK             = 0x00002,
        UNAFFECTED_RECEIVERS_CHANGED_MASK           = 0x00004,
        CAST_SHADOWS_UNIQUE_MASK                    = 0x00008,
        STATIC_SHADOWS_UNIQUE_MASK                  = 0x00010,
        SIZE_HINT_UNIQUE_MASK                       = 0x00020,
        STRICTLY_OBSERVE_SIZE_HINT_UNIQUE_MASK      = 0x00040,
        SHADOW_TECHNIQUE_UNIQUE_MASK                = 0x00080,
        RENDER_BACKFACES_UNIQUE_MASK                = 0x00100,
        DEPTH_BIAS_UNIQUE_MASK                      = 0x00200,
        USE_DEFAULT_DEPTH_BIAS_MASK                 = 0x00400,
        PSSM_ENABLED_UNIQUE_MASK                    = 0x00800,
        PSSM_NUM_SLICES_UNIQUE_MASK                 = 0x01000,
        PSSM_SCENE_DEPENDENT_FRUSTUMS_UNIQUE_MASK   = 0x02000,
        PSSM_SHIMMER_SUPPRESSION_UNIQUE_MASK        = 0x04000,
        PSSM_SLICE_TRANSITION_UNIQUE_MASK           = 0x08000,
        PSSM_SLICE_TRANSITION_SIZE_UNIQUE_MASK      = 0x10000,
        PSSM_SLICE_TRANSITION_SCALE_UNIQUE_MASK     = 0x20000,
        PSSM_SLICE_LAMBDA_UNIQUE_MASK               = 0x40000,
        PSSM_PROPERTIES_CHANGED_MASK                = 0x80000,
        PSSM_SLICE_CAMERA_DIST_UNIQUE_MASK          = 0x100000,

    };
    inline NiBool GetShadowGeneratorPropertiesChanged() const;
    inline NiBool GetPSSMPropertiesChanged() const;
    inline void SetShadowGeneratorPropertiesChanged(
        bool bShadowGeneratorPropertiesChanged);
    inline void SetPSSMPropertiesChanged(bool bChanged);
    inline NiBool GetUnaffectedCastersChanged() const;
    inline void SetUnaffectedCastersChanged(bool bCastersChanged);
    inline NiBool GetUnaffectedReceiversChanged() const;
    inline void SetUnaffectedReceiversChanged(bool bUnaffectedReceiversChanged);
    inline NiBool GetCastShadowsUnique() const;
    inline void SetCastShadowsUnique(bool bCastShadowsUnique);
    inline NiBool GetStaticShadowsUnique() const;
    inline void SetStaticShadowsUnique(bool bStaticShadowsUnique);
    inline NiBool GetSizeHintUnique() const;
    inline void SetSizeHintUnique(bool bSizeHintUnique);
    inline NiBool GetStrictlyObserveSizeHintUnique() const;
    inline void SetStrictlyObserveSizeHintUnique(bool bStrictlyObserveSizeHintUnique);
    inline NiBool GetShadowTechniqueUnique() const;
    inline void SetShadowTechniqueUnique(bool bShadowTechniqueUnique);
    inline NiBool GetRenderBackfacesUnique() const;
    inline void SetRenderBackfacesUnique(bool bRenderBackfacesUnique);
    inline NiBool GetDepthBiasUnique() const;
    inline void SetDepthBiasUnique(bool bDepthBiasUnique);
    inline NiBool GetUseDefaultDepthBiasUnique() const;
    inline void SetUseDefaultDepthBiasUnique(bool bUseDefaultDepthBiasUnique);
    inline NiBool GetPSSMEnabledUnique() const;
    inline void SetPSSMEnabledUnique(bool bUnique);
    inline NiBool GetPSSMNumSlicesUnique() const;
    inline void SetPSSMNumSlicesUnique(bool bUnique);
    inline NiBool GetPSSMSceneDependentFrustumsUnique() const;
    inline void SetPSSMSceneDependentFrustumsUnique(bool bUnique);
    inline NiBool GetPSSMShimmerSuppressionUnique() const;
    inline void SetPSSMShimmerSuppressionUnique(bool bUnique);
    inline NiBool GetPSSMSliceTransitionUnique() const;
    inline void SetPSSMSliceTransitionUnique(bool bUnique);
    inline NiBool GetPSSMSliceTransitionSizeUnique() const;
    inline void SetPSSMSliceTransitionSizeUnique(bool bUnique);
    inline NiBool GetPSSMSliceTransitionScaleUnique() const;
    inline void SetPSSMSliceTransitionScaleUnique(bool bUnique);
    inline NiBool GetPSSMSliceLambdaUnique() const;
    inline void SetPSSMSliceLambdaUnique(bool bUnique);
    inline NiBool GetPSSMCameraDistanceScaleUnique() const;
    inline void SetPSSMCameraDistanceScaleUnique(bool bUnique);

    inline void CreateShadowGenerator(NiDynamicEffect* pkDynamicEffect);
    inline void ClearShadowGenerator();

    inline bool ShouldUpdateShadowGeneratorProperties() const;
    inline void RefreshCachedShadowGeneratorProperties();

    NiShadowGeneratorPtr m_spShadowGenerator;
    NiDynamicEffectPtr m_spLastDynamicEffect;
    NiShadowGeneratorComponentPtr m_spMasterComponent;
    NiPSSMConfigurationPtr m_spConfiguration;

    // Properties.
    NiTPrimitiveArray<NiEntityInterface*> m_kUnaffectedCasters;
    NiTPrimitiveArray<NiEntityInterface*> m_kUnaffectedReceivers;
    NiFixedString m_kShadowTechnique;
    float m_fDepthBias;
    float m_fPSSMSliceTransitionSize;
    float m_fPSSMSliceTransitionScale;
    float m_fPSSMSliceLambda;
    float m_fPSSMCameraDistanceScaleFactor;
    unsigned short m_usSizeHint;
    unsigned short m_usPSSMNumSlices;
    bool m_bCastShadows;
    bool m_bStaticShadows;
    bool m_bStrictlyObserveSizeHint;
    bool m_bRenderBackfaces;
    bool m_bUseDefaultDepthBias;
    bool m_bPSSMEnabled;
    bool m_bPSSMSceneDependentFrustums;
    bool m_bPSSMShimmerSuppression;
    bool m_bPSSMSliceTransition;

    // Class name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

    // Property names.
    static NiFixedString ms_kCastShadowsName;
    static NiFixedString ms_kStaticShadowsName;
    static NiFixedString ms_kSizeHintName;
    static NiFixedString ms_kStrictlyObserveSizeHintName;
    static NiFixedString ms_kUnaffectedCastersName;
    static NiFixedString ms_kUnaffectedReceiversName;
    static NiFixedString ms_kShadowTechniqueName;
    static NiFixedString ms_kRenderBackfacesName;
    static NiFixedString ms_kDepthBiasName;
    static NiFixedString ms_kUseDefaultDepthBiasName;
    static NiFixedString ms_kShadowGeneratorPointerName;
    static NiFixedString ms_kPSSMEnabledName;
    static NiFixedString ms_kPSSMNumSlicesName;
    static NiFixedString ms_kPSSMSceneDependentFrustumsName;
    static NiFixedString ms_kPSSMShimmerSupressionName;
    static NiFixedString ms_kPSSMSliceTransitionEnabledName;
    static NiFixedString ms_kPSSMSliceTransitionSizeName;
    static NiFixedString ms_kPSSMSliceTransitionScaleName;
    static NiFixedString ms_kPSSMSliceLambdaName;
    static NiFixedString ms_kPSSMCameraDistanceScaleFactor;

    // Property descriptions.
    static NiFixedString ms_kCastShadowsDescription;
    static NiFixedString ms_kStaticShadowsDescription;
    static NiFixedString ms_kSizeHintDescription;
    static NiFixedString ms_kStrictlyObserveSizeHintDescription;
    static NiFixedString ms_kUnaffectedCastersDescription;
    static NiFixedString ms_kUnaffectedReceiversDescription;
    static NiFixedString ms_kShadowTechniqueDescription;
    static NiFixedString ms_kRenderBackfacesDescription;
    static NiFixedString ms_kDepthBiasDescription;
    static NiFixedString ms_kUseDefaultDepthBiasDescription;
    static NiFixedString ms_kPSSMEnabledDescription;
    static NiFixedString ms_kPSSMNumSlicesDescription;
    static NiFixedString ms_kPSSMSceneDependentFrustumsDescription;
    static NiFixedString ms_kPSSMShimmerSupressionDescription;
    static NiFixedString ms_kPSSMSliceTransitionEnabledDescription;
    static NiFixedString ms_kPSSMSliceTransitionSizeDescription;
    static NiFixedString ms_kPSSMSliceTransitionScaleDescription;
    static NiFixedString ms_kPSSMSliceLambdaDescription;
    static NiFixedString ms_kPSSMCameraDistanceScaleFactorDescription;

    // PSSM Category name
    static NiFixedString ms_kPSSMCategoryName;

    // Dependent property names.
    static NiFixedString ms_kSceneRootPointerName;

    // Name of the PSSM Shadow Click Generator
    static NiFixedString ms_kPSSMClickGeneratorName;

public:
    // NiEntityComponentInterface overrides.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiEntityComponentInterface* GetMasterComponent() const;
    virtual void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent);
    virtual void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames);

    // NiEntityPropertyInterface overrides.
    virtual NiBool SetTemplateID(const NiUniqueID& kID);
    virtual NiUniqueID GetTemplateID();
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetClassName() const;
    virtual NiFixedString GetName() const;
    virtual NiBool SetName(const NiFixedString& kName);
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);
    virtual void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const;
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const;
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName);
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique);
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName);
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType);
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType);
    virtual NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription);
    virtual NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const;
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly);
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique);
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable);
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable);
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        unsigned int uiIndex, bool& bIsExternalAssetPath) const;
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName,
        unsigned int& uiCount) const;
    virtual NiBool SetElementCount(const NiFixedString& kPropertyName,
        unsigned int uiCount, bool& bCountSet);
    virtual NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        float& fData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float fData, unsigned int uiIndex = 0);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, unsigned int uiIndex = 0);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned short& usData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned short usData, unsigned int uiIndex = 0);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex);
};

#include "NiShadowGeneratorComponent.inl"

#endif  // #ifndef NISHADOWGENERATORCOMPONENT_H
