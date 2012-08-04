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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiShadowClickValidator.h"
#include "NiShadowRenderClick.h"
#include "NiShadowGenerator.h"
#include "NiLODNode.h"

NiImplementRTTI(NiShadowClickValidator, NiRenderClickValidator);

//---------------------------------------------------------------------------
NiShadowClickValidator::NiShadowClickValidator() :
    m_spCamera(NULL),
    m_spCullingProcess(NULL)
{
}
//---------------------------------------------------------------------------
NiShadowClickValidator* NiShadowClickValidator::CreateShadowClickValidator()
{
    return NiNew NiShadowClickValidator();
}
//---------------------------------------------------------------------------
bool NiShadowClickValidator::ValidateClick(NiRenderClick* pkRenderClick,
    unsigned int)
{
    // Cast render click to an NiShadowRenderClick.
    NiShadowRenderClick* pkShadowClick = NiDynamicCast(NiShadowRenderClick,
        pkRenderClick);
    if (!pkShadowClick)
    {
        // If the render click is not a shadow click, allow rendering.
        return true;
    }
    else if (pkShadowClick->GetForceRender())
    {
        // If the shadow click is set to force render, allow rendering.
        pkShadowClick->MarkAsDirty();
        return true;
    }

    // If no camera or culling process have been set, allow rendering.
    if (!m_spCamera || !m_spCullingProcess)
    {
        pkShadowClick->MarkAsDirty();
        return true;
    }

    // Tell the culling process not to submit mesh modifiers. This allows
    // a culling pass to be performed without an associated rendering pass.
    bool bOldSubmitModifiers = m_spCullingProcess->GetSubmitModifiers();
    m_spCullingProcess->SetSubmitModifiers(false);

    m_kVisibleSet.RemoveAll();

    // Get shadow generator.
    NiShadowGenerator* pkGenerator = pkShadowClick->GetGenerator();
    NIASSERT(pkGenerator);

    bool bRetVal = false;

#if defined(_WII)
    // On the Wii, we don't want to waste resources rendering or applying a 
    // shadow map that has no shadow casters in it.  Since there are no 
    // shaders to recompile, there is no penalty for doing this.  The 
    // potentially visible geometry for the shadow frustum would have
    // to be calculated anyway when rendering the shadow click.
    NiTPointerList<NiRenderViewPtr>& kViewList = 
        pkShadowClick->GetRenderViews();
    NiTListIterator kViewIter = kViewList.GetHeadPos();
    bool bShadowCastersPresent = false;
    while (kViewIter && !bShadowCastersPresent)
    {
        NiRenderView* pkView = kViewList.GetNext(kViewIter);
        const NiVisibleArray& kVisible = 
            pkView->GetPVGeometry(NiRenderer::GetRenderer()->GetFrameID());
        bShadowCastersPresent = 
            bShadowCastersPresent || (kVisible.GetCount() > 0);
    }

    if (bShadowCastersPresent)
#endif
    {
        NiDynamicEffect* pkDynEffect = pkGenerator->GetAssignedDynamicEffect();
        NIASSERT(pkDynEffect);

        m_spCullingProcess->SetCamera(m_spCamera);
        m_spCullingProcess->SetFrustum(m_spCamera->GetViewFrustum());
        m_spCullingProcess->SetVisibleSet(&m_kVisibleSet);

        // Loop over shadow receivers (affected nodes list), culling each to
        // determine its visibility. If any are visible, allow rendering.
        const NiNodeList& kReceivers = pkDynEffect->GetAffectedNodeList();
        NiTListIterator kIter = kReceivers.GetHeadPos();
        while (kIter)
        {
            NiNode* pkReceiver = kReceivers.GetNext(kIter);

            // Cull the shadow receiver against the camera using the culling
            // process.
            pkReceiver->Cull(*m_spCullingProcess);

            // Determine whether or not the receiver is visible. If so, return
            // immediately allowing rendering.
            if (m_kVisibleSet.GetCount() > 0)
            {
                // Mark the shadow click as dirty.
                pkShadowClick->MarkAsDirty();

                bRetVal = true;
                break;
            }
        }
    }

    // Restore the previous state of the culling process.
    m_spCullingProcess->SetSubmitModifiers(bOldSubmitModifiers);

    return bRetVal;
}
//---------------------------------------------------------------------------
