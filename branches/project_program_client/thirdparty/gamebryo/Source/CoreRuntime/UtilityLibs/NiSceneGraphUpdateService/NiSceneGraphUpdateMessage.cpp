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

#include "NiSceneGraphUpdateServicePCH.h"

#include "NiSceneGraphUpdateMessage.h"
#include "NiSceneGraphUpdateObject.h"
#include "NiSceneGraphUpdateStream.h"
#include "NiSceneGraphUpdate.h"

#include <NiMesh.h>

EE_IMPLEMENT_CONCRETE_CLASS_INFO(NiSceneGraphUpdateMessage);

//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateMessage::NiSceneGraphUpdateMessage()
{
    for (NiUInt32 ui = 0; ui < NiSceneGraphUpdate::MESSAGE_COUNT; ui++)
    {
        m_auiBufferSize[ui] = 0;
        m_apcBuffer[ui] = NULL;
    }

    NiSceneGraphUpdate::GetInstance()->PreHandleAddMessage();
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateMessage::~NiSceneGraphUpdateMessage()
{
    for (NiUInt32 ui = 0; ui < NiSceneGraphUpdate::MESSAGE_COUNT; ui++)
    {
        NiFree(m_apcBuffer[ui]);
    }

    NiSceneGraphUpdate::GetInstance()->PostHandleRemoveMessage();
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::PreSend()
{
    NiTMapIterator kUpdateIter = 
        m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT].GetFirstPos();
    NiTMapIterator kReplaceIter = 
        m_kObjects[NiSceneGraphUpdate::MESSAGE_REPLACE_OBJECT].GetFirstPos();
    if (kUpdateIter || kReplaceIter)
    {
        NiSceneGraphUpdateStream kUpdateStream;

        while (kUpdateIter)
        {
            NiSceneGraphUpdateObjectId kId;
            NiSceneGraphUpdateObjectPtr spObject;
            m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT].GetNext(kUpdateIter, kId, 
                spObject);
            NIASSERT(spObject && spObject->GetObject());

            kUpdateStream.InsertObject(kId, NiSceneGraphUpdate::OS_UPDATE, spObject->GetObject());
        }

        while (kReplaceIter)
        {
            NiSceneGraphUpdateObjectId kId;
            NiSceneGraphUpdateObjectPtr spObject;
            m_kObjects[NiSceneGraphUpdate::MESSAGE_REPLACE_OBJECT].GetNext(kReplaceIter, kId, 
                spObject);
            NIASSERT(spObject && spObject->GetObject());

            kUpdateStream.InsertObject(kId, NiSceneGraphUpdate::OS_REPLACE, spObject->GetObject());
        }

        NIASSERT(!m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT]);
        NiFree(m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT]);
        m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT] = NULL;
        m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT] = 0;

        kUpdateStream.Save(m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT],
            m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT]);
    }

    NiTMapIterator kSetAnimRangeIter = 
        m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS].GetFirstPos();
    if (kSetAnimRangeIter)
    {
        NIASSERT(!m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS]);
        NiFree(m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS]);
        m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS] = NULL;
        m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS] = 0;

        NiSceneGraphUpdate::GetInstance()->GetSettings().Save(
            m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS],
            m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS]);
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::PostSend()
{
    for (NiUInt32 uiType = 0; uiType < NiSceneGraphUpdate::MESSAGE_COUNT; uiType++)
    {
        m_kObjects[uiType].RemoveAll();

        NiFree(m_apcBuffer[uiType]);
        m_apcBuffer[uiType] = NULL;
        m_auiBufferSize[uiType] = 0;
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
efd::ClassID NiSceneGraphUpdateMessage::GetClassID() const 
{
    return CLASS_ID;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateMessage::ToStream(efd::DataStream& kDataStream) const
{
    efd::IMessage::ToStream(kDataStream);

    for (NiUInt32 uiType = 0; uiType < NiSceneGraphUpdate::MESSAGE_COUNT; uiType++)
    {
        NiUInt32 uiCount = m_kObjects[uiType].GetCount();
        kDataStream.Write(uiCount);

        NiTMapIterator kIter = m_kObjects[uiType].GetFirstPos();
        while (kIter)
        {
            NiSceneGraphUpdateObjectId kId;
            NiSceneGraphUpdateObjectPtr spObject;
            m_kObjects[uiType].GetNext(kIter, kId, spObject);

            NiUInt32 uiId = kId;
            kDataStream.Write(uiId);
        }

        // Write the object data stream
        kDataStream.Write(m_auiBufferSize[uiType]);
        if (m_auiBufferSize[uiType] > 0)
        {
            kDataStream.WriteRawBuffer(m_apcBuffer[uiType], m_auiBufferSize[uiType]);
        }
    }

    
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::FromStream(const efd::DataStream& kDataStream)
{
    efd::IMessage::FromStream(kDataStream);

    // Read all the object ids
    for (NiUInt32 uiType = 0; uiType < NiSceneGraphUpdate::MESSAGE_COUNT; uiType++)
    {
        NiUInt32 uiCount = 0;
        kDataStream.Read(uiCount);

        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            NiUInt32 uiId;
            kDataStream.Read(uiId);
            NiSceneGraphUpdateObjectId kId;
            kId = uiId;

            NiSceneGraphUpdateObject* pkObject = NiSceneGraphUpdate::GetInstance()->GetObject(kId);
            m_kObjects[uiType].SetAt(kId, pkObject);
        }

        // Destroy the data if it exists
        m_auiBufferSize[uiType] = 0;
        NiFree(m_apcBuffer[uiType]);
        m_apcBuffer[uiType] = NULL;

        // Read the buffer of the object data
        kDataStream.Read(m_auiBufferSize[uiType]);
        if (m_auiBufferSize[uiType] > 0)
        {
            m_apcBuffer[uiType] = NiAlloc(char, m_auiBufferSize[uiType]);
            NIASSERT(m_apcBuffer[uiType]);
            if (!m_apcBuffer[uiType])
                return false;
            kDataStream.ReadRawBuffer(m_apcBuffer[uiType], m_auiBufferSize[uiType]);
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsActive() const
{
    for (NiUInt32 uiType = 0; uiType < NiSceneGraphUpdate::MESSAGE_COUNT; uiType++)
    {
        if (m_kObjects[uiType].GetCount() > 0)
            return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsViewerAddedMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_VIEWER_ADDED].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsViewerRemovedMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_VIEWER_REMOVED].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsEditorStartedMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_EDITOR_STARTED].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsEditorStoppedMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_EDITOR_STOPPED].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsSetRootNodeMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ROOT_NODE].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsSetActiveCameraMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ACTIVE_CAMERA].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsUpdateObjectMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT].GetCount();
    if (uiCount)
        return true;

    uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_REPLACE_OBJECT].GetCount();
    if (uiCount)
        return true;

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsRemoveObjectMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_REMOVE_OBJECT].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsRequestObjectMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_REQUEST_OBJECT].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsSceneDirtyMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_SCENE_DIRTY].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsSceneCleanMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_SCENE_CLEAN].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsUpdateSettingsMsg() const
{
    NiUInt32 uiCount = m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS].GetCount();
    if (!uiCount)
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleUpdateObject() const
{
    if (!IsUpdateObjectMsg())
        return false;

    // Make sure all the modifiers are complete
    NiMesh::CompleteSceneModifiers(NiSceneGraphUpdate::GetInstance()->GetRootNode());

    // Now we read in the stream and fix up the objects
    NiSceneGraphUpdateStream kUpdateStream;
    kUpdateStream.Load(m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT],
        m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT]);

    // When changes are made to the scene graph there are chances that data was made stale.
    NiSceneGraphUpdate::GetInstance()->PostHandleUpdateObject();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleRemoveObject() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_REMOVE_OBJECT].GetFirstPos();
    if (!kIter)
        return false;

    // Remove all the objects in the message que
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject; // Is not needed can be null
        m_kObjects[NiSceneGraphUpdate::MESSAGE_REMOVE_OBJECT].GetNext(kIter, kId, spObject);
        NiSceneGraphUpdate::GetInstance()->RemoveObjectLocal(kId);
    }

    // When we delete something we need to know about it.
    NiSceneGraphUpdate::GetInstance()->PostHandleRemoveObject();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleRequestObject() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_REQUEST_OBJECT].GetFirstPos();
    if (!kIter)
        return false;

    // Remove all the objects in the message que
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject;
        m_kObjects[NiSceneGraphUpdate::MESSAGE_REQUEST_OBJECT].GetNext(kIter, kId, spObject);
        if (spObject && spObject->GetObject())
        {
            NiSceneGraphUpdate::GetInstance()->UpdateObject(kId, spObject->GetObject());
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleSetRootNode() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ROOT_NODE].GetFirstPos();
    if (!kIter)
        return false;

    // Set the active camera this should only be one.
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject; // Is not needed can be null
        m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ROOT_NODE].GetNext(kIter, kId, spObject);
        NiSceneGraphUpdate::GetInstance()->SetRootNode(kId);

        // When changes are made to the root node make sure things are up to date
        NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
        if (pkRootNode)
        {
            pkRootNode->Update(NiGetCurrentTimeInSec());
            NiMesh::CompleteSceneModifiers(pkRootNode);
            pkRootNode->UpdateProperties();
            pkRootNode->UpdateEffects();
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleSetActiveCamera() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ACTIVE_CAMERA].GetFirstPos();
    if (!kIter)
        return false;

    // Set the active camera this should only be one.
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject; // Is not needed can be null
        m_kObjects[NiSceneGraphUpdate::MESSAGE_SET_ACTIVE_CAMERA].GetNext(kIter, kId, spObject);
        NiSceneGraphUpdate::GetInstance()->SetActiveCamera(kId);
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleSceneDirty() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_SCENE_DIRTY].GetFirstPos();
    if (!kIter)
        return false;

    NiSceneGraphUpdate::GetInstance()->SetSceneDirty();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleSceneClean() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_SCENE_CLEAN].GetFirstPos();
    if (!kIter)
        return false;

    NiSceneGraphUpdate::GetInstance()->SetSceneClean();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::HandleUpdateSettings() const
{
    NiTMapIterator kIter = m_kObjects[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS].GetFirstPos();
    if (!kIter)
        return false;

    NiSceneGraphUpdate::GetInstance()->GetSettings().Load(
        m_auiBufferSize[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS],
        m_apcBuffer[NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS]);

    // Since we are likely updating playback settings, restart animations to pickup changes.
    NiSceneGraphUpdate::GetInstance()->PostHandleUpdateSettings();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateMessage::IsEqual(const NiSceneGraphUpdateMessage* pkMessage) const
{
    if (pkMessage->GetClassID() != GetClassID())
        return false;

    for (NiUInt32 ui = 0; ui < NiSceneGraphUpdate::MESSAGE_COUNT; ui++)
    {
        if (m_kObjects[ui].GetCount() != pkMessage->m_kObjects[ui].GetCount())
            return false;
    }

    for (NiUInt32 ui = 0; ui < NiSceneGraphUpdate::MESSAGE_COUNT; ui++)
    {
        NiTMapIterator kIter = m_kObjects[ui].GetFirstPos();
        NiTMapIterator kOtherIter = pkMessage->m_kObjects[ui].GetFirstPos();
        while (kIter && kOtherIter)
        {
            NiSceneGraphUpdateObjectId kId;
            NiSceneGraphUpdateObjectPtr spObject;
            m_kObjects[ui].GetNext(kIter, kId, spObject);

            NiSceneGraphUpdateObjectId kOtherId;
            NiSceneGraphUpdateObjectPtr spOtherObject;
            pkMessage->m_kObjects[ui].GetNext(kOtherIter, kId, spObject);

            if (kId != kOtherId)
                return false;

            if (spObject != spOtherObject)
                return false;
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------