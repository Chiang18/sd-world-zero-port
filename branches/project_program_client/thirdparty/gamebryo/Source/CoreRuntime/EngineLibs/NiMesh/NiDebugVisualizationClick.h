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

#ifndef NIDEBUGGEOMETRYCLICK_H
#define NIDEBUGGEOMETRYCLICK_H

#include <NiRenderClick.h>
#include <NiImmediateModeAdapter.h>

NiSmartPointer(NiCamera);
NiSmartPointer(NiLight);
NiSmartPointer(NiAVObject);

/**
    This class implements a simple debug geometry visualizer that can be easily integrated
    into the Frame Rendering System to display additional information about a scene.

    Here is an example of how to use this code in an NiApplication derived application:
    @code
    // Create the debug click
    m_spDebugGeometryClick = NiNew NiDebugVisualizationClick();
    m_spDebugGeometryClick->SetName("Debug visualization click.");
    m_spDebugGeometryClick->SetCamera(m_spCamera);
    m_spDebugGeometryClick->SetClearAllBuffers(false);
    m_spDebugGeometryClick->ClearScene();

    // Create a render step to hold the click and append it to the frame
    NiDefaultClickRenderStep* pkDebugStep = NiNew NiDefaultClickRenderStep;
    pkDebugStep->SetName("Debug visualization step.");
    pkDebugStep->AppendRenderClick(m_spDebugGeometryClick);
    NIASSERT(m_spFrame);
    m_spFrame->AppendRenderStep(pkDebugStep);

    // Process the scene to build up a list of things to visualize
    m_spDebugGeometryClick->ProcessScene(m_spScene, NiDebugVisualizationClick::ALL_MASK);
    @endcode
    
*/
class NiDebugVisualizationClick : public NiRenderClick
{
    NiDeclareRTTI;
    NiDeclareFlags(unsigned char);
public:
    NiDebugVisualizationClick();

    /// Visualization mode flags.  These may be combined.
    enum Flags
    {
        /// Display point lights
        POINT_LIGHT_MASK       = 0x01,

        /// Display directional lights
        DIRECTIONAL_LIGHT_MASK = 0x02,

        /// Display spot lights
        SPOT_LIGHT_MASK        = 0x04,

        /// Display all lights (combination of light flags)
        LIGHT_MASK             = 0x07,

        /// Display cameras
        CAMERA_MASK            = 0x08,

        /// Display bounding spheres for nodes
        BOUNDS_MASK            = 0x10,

        /// Display bones
        BONES_MASK             = 0x20,

        /// Display all information (combination of other flags)
        ALL_MASK               = 0x3F
    };

    /**
        Empty one or more of the internal lists of debug visualization information.

        @param uiClearMask  A combination of one or more flags indicating which lists
                            should be cleared
    */
    void ClearScene(unsigned int uiClearMask = ALL_MASK);
    
    /**
        Internally records a list of items to visualize by recursively parsing the
        passed in scene.  Only objects which meet the processing criteria are added.

        @param pkObject       The scene to process
        @param uiProcessMask  A combination of one or more flags indicating what kinds
                              of information should be visualized
    */
    void ProcessScene(NiAVObject* pkObject, unsigned int uiProcessMask = ALL_MASK);


    /// @name Camera control
    //@{

    /// Set the camera to use when rendering the debug visualization.
    void SetCamera(NiCamera* pkCamera);

    /// Get the camera that will be used to render the debug visualization.
    const NiCamera* GetCamera() const;

    // Extend the near and far planes to encompass the debug visualization geometry.
    void ExtendCameraNearAndFar(NiCamera* pkCamera) const;

    //@}

    /// @name Screen scale control
    //@{

    /**
        The camera and light debug visualizations all have a default scale set by
        NiImmediateModeMacro::GetScreenScaleFactor().  This can be further scaled
        up or down by specifying a screen scale multipler

        @param fScreenMultiplier  Multiplier value (1.0 = no adjustment)
      */
    void SetScreenMultiplier(float fScreenMultiplier);

    /// Get the current screen scale multiplier
    float GetScreenMultiplier() const;
    
    //@}

    // Implementation of NiRenderClick API
    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;
protected:
    virtual void PerformRendering(unsigned int uiFrameID);

    static void ExpandFrustumNearFar(
        NiCamera* pkCamera,
        NiFrustum& kFrust,
        const NiPoint3& kCenter,
        float fRadius);

    float m_fCullTime;
    float m_fRenderTime;
    unsigned int m_uiNumObjectsDrawn;

    float m_fScreenMultiplier;

    NiTObjectSet<NiCameraPtr> m_kCameras;
    NiTObjectSet<NiLightPtr> m_kLights;
    NiTObjectSet<NiAVObjectPtr> m_kBounds;
    NiTObjectSet<NiMeshPtr> m_kHasBones;

    NiImmediateModeAdapter m_kAdapter;
};

NiSmartPointer(NiDebugVisualizationClick);

#endif  // #ifndef NIDEBUGGEOMETRYCLICK_H
