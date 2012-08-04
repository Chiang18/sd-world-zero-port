// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiMainPCH.h"

#include "NiNode.h"
#include "NiShadowVisitor.h"
#include "NiShadowGenerator.h"

NiImplementRTTI(NiShadowVisitor, NiObject);

//---------------------------------------------------------------------------
NiShadowVisitor::~NiShadowVisitor()
{
}
//---------------------------------------------------------------------------
void NiShadowVisitor::PopulateCasterList(NiShadowGenerator* pkGenerator, 
    NiAVObjectRawList& kCasterList)
{
    kCasterList.RemoveAll();

    NiDynamicEffect* pkDynEffect = pkGenerator->GetAssignedDynamicEffect();

    // Can not process without an assigned light.
    if (!pkDynEffect)
        return;

    // Flatten the affected node list of the shadow generator's light into a 
    // set of only NiRenderObject objects.
    const NiNodeList& kAffectedNodeList = pkDynEffect->GetAffectedNodeList();
    NiTListIterator kIter = kAffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kAffectedNodeList.GetNext(kIter);
        AdditiveListHelper(pkNode, &kCasterList);
    }

    // Now remove all NiRenderObject objects from the casters list that are 
    // included in the light's unaffected node list.
    const NiNodeList& kUnaffectedNodeList = 
        pkDynEffect->GetUnaffectedNodeList();
    kIter = kUnaffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedNodeList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kCasterList);
    }

    // Now remove all NiRenderObject objects from the casters list that are 
    // included in the generators unaffected caster list.
    const NiNodeList& kUnaffectedCasterList = 
        pkGenerator->GetUnaffectedCasterNodeList();

    kIter = kUnaffectedCasterList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedCasterList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kCasterList);
    }
}
//---------------------------------------------------------------------------
void NiShadowVisitor::PopulateReceiverList(NiShadowGenerator* pkGenerator,
    NiAVObjectRawList& kReceiverList)
{
    kReceiverList.RemoveAll();

    NiDynamicEffect* pkDynEffect = pkGenerator->GetAssignedDynamicEffect();

    // Can not process without an assigned light.
    if (!pkDynEffect)
        return;

    // Flatten the affected node list of the shadow generator's light into a 
    // set of only NiRenderObject objects.
    const NiNodeList& kAffectedNodeList = pkDynEffect->GetAffectedNodeList();
    NiTListIterator kIter = kAffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kAffectedNodeList.GetNext(kIter);
        AdditiveListHelper(pkNode, &kReceiverList);
    }

    // Now remove all NiRenderObject objects from the receiver list that are 
    // included in the light's unaffected node list.
    const NiNodeList& kUnaffectedNodeList = 
        pkDynEffect->GetUnaffectedNodeList();
    kIter = kUnaffectedNodeList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedNodeList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kReceiverList);
    }

    // Now remove all NiRenderObject objects from the receiver list that are 
    // included in the generators unaffected receiver list.
    const NiNodeList& kUnaffectedReceiverList = 
        pkGenerator->GetUnaffectedReceiverNodeList();
    
    kIter = kUnaffectedReceiverList.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedReceiverList.GetNext(kIter);
        SubtractiveListHelper(pkNode, &kReceiverList);
    }

}
//---------------------------------------------------------------------------
void NiShadowVisitor::AdditiveListHelper(NiNode* pkNode,
    NiAVObjectRawList* pkObjList)
{
    if (!pkNode)
        return;

    // Add just the provided node to the list if it's children are not 
    // suppose to be processed.
    if (!pkNode->GetAppCulled())
    {
        if (!ShouldProcessNodeChildren(pkNode))
        {
            pkObjList->AddTail((NiRenderObject*)pkNode);
            return;
        }
    }
    else 
    {
        // Node is app-culled so we ignore it.
        return;
    }

    // Recurse to children
    unsigned int uiChildCount = pkNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            if (NiIsKindOf(NiRenderObject, pkChild) &&
                !pkChild->GetAppCulled())
            {
                pkObjList->AddTail((NiRenderObject*)pkChild);
            }
            else if (pkChild->IsNode())
            {
                AdditiveListHelper((NiNode*)pkChild, pkObjList);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiShadowVisitor::SubtractiveListHelper(NiNode* pkNode,
    NiAVObjectRawList* pkGeomList)
{
    if (!pkNode)
        return;

    if (!ShouldProcessNodeChildren(pkNode))
        pkGeomList->Remove((NiRenderObject*)pkNode);

    // Recurse to children
    unsigned int uiChildCount = pkNode->GetArrayCount();
    for (unsigned int i = 0; i < uiChildCount; i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            if (NiIsKindOf(NiRenderObject, pkChild))
            {
                pkGeomList->Remove((NiRenderObject*)pkChild);
            }
            else if (pkChild->IsNode())
            {
                SubtractiveListHelper((NiNode*)pkChild, pkGeomList);
            }
        }
    }

}
//---------------------------------------------------------------------------
