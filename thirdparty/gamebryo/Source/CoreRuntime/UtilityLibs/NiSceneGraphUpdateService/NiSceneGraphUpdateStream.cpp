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

#include "NiSceneGraphUpdateStream.h"
#include "NiSceneGraphUpdateObjectId.h"
#include "NiSceneGraphUpdate.h"

#include <NiMemStream.h>
#include <NiMesh.h>

enum
{
    MAX_RTTI_LEN = 256,
    MAX_RTTI_ARG_COUNT = MAX_RTTI_LEN / 2
};

//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateStream::NiSceneGraphUpdateStream()
{
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateStream::~NiSceneGraphUpdateStream()
{
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateStream::Save(NiUInt32& uiBufferSize, char*& pcBuffer)
{

    NiMemStream kMemStream;
    m_pkOstr = &kMemStream;

    // If this platform is big endian, swap the bytes since the buffer is always saved in little
    // endian to make it more efficient in the most common case of running both the editor and
    // viewer on a PC. The viewer on the console will swap the buffer from little to big endian.
    bool bSwapBytes = !NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkOstr->SetEndianSwap(bSwapBytes);

    NIASSERT(m_kObjects.GetSize() == 0);
    NIASSERT(m_kRegisterMap.GetCount() == 0);

    RegisterObjects();

    NiUInt32 uiObjectCount = m_kObjects.GetSize();
    NiStreamSaveBinary(*this, uiObjectCount);

    SaveObjectIds();

    SaveRTTI();
    SaveFixedStringTable();

    m_kObjectSizes.SetSize(uiObjectCount);
    NiUInt32 uiStartingOffset = PreSaveObjectSizeTable();

    // save object groups
    UpdateObjectGroups();
    SaveObjectGroups();

    // save list of objects
    for (NiUInt32 ui = 0; ui < uiObjectCount; ui++)
    {
        NiUInt32 uiStartInBytes = m_pkOstr->GetPosition();
        NiSceneGraphUpdateObjectId kId = m_kObjectIds.GetAt(ui);
        NiUInt32 uiObjectState = m_kObjectStates.GetAt(ui);
        if (kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID ||
            NiSceneGraphUpdate::IsObjectStateDirty(uiObjectState))
        {
            NiObject* pkObject = m_kObjects.GetAt(ui);
            pkObject->SaveBinary(*this);
        }
        NiUInt32 uiSizeInBytes = m_pkOstr->GetPosition() - uiStartInBytes;
        m_kObjectSizes.SetAt(ui, uiSizeInBytes);
    }

    SaveTopLevelObjects();

    SaveObjectSizeTable(uiStartingOffset);

    m_kObjects.RemoveAll();
    m_kRegisterMap.RemoveAll();
    m_kObjectIds.RemoveAll();
    m_kObjectStates.RemoveAll();

    m_pkOstr = NULL;

    uiBufferSize = kMemStream.GetSize();
    pcBuffer = (char *) kMemStream.Str();

    return true;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateStream::SaveLinkID(const NiObject* pkObject)
{
    NiObject* pkNewObject = NULL;
    m_kReplaceObjects.GetAt(pkObject, pkNewObject);
    if (pkNewObject)
    {
        pkObject = pkNewObject;
    }
    NiUInt32 uiLinkID = GetLinkIDFromObject(pkObject);
    NiStreamSaveBinary(*this, uiLinkID);

    // This could be NULL because the object being referenced is not in the stream, but
    // it could exist in the replicated SceneGraph; thus, save the UpdateId to link it
    // up on the other side.
    if (uiLinkID == NULL_LINKID)
    {
        NiSceneGraphUpdateObjectId kId = NiSceneGraphUpdate::GetInstance()->GetObjectId(pkObject);
        NiUInt32 uiId = kId;
        NiStreamSaveBinary(*this, uiId);
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateStream::ReadLinkID()
{
    NiUInt32 uiLinkID;
    NiStreamLoadBinary(*this, uiLinkID);

    // Remove Update ID from stream, but don't do anything with it since this is just a null
    // pointer to no object in particular.
    if (uiLinkID == NULL_LINKID)
    {
        NiUInt32 uiId;
        NiStreamLoadBinary(*this, uiId);
        NiSceneGraphUpdateObjectId kId;
        kId = uiId;
        NIASSERT(kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID);
    }

    m_kLinkIDs.Add(uiLinkID);
}
//--------------------------------------------------------------------------------------------------
NiObject* NiSceneGraphUpdateStream::ResolveLinkID()
{
    NiObject* pkObject = NiStream::ResolveLinkID();

    // Link ID doesn't refer to object in the stream. Get the UpdateID and find
    // it in the replicated SceneGraph.
    if (pkObject == NULL)
    {
        NiUInt32 uiId;
        NiStreamLoadBinary(*this, uiId);
        NiSceneGraphUpdateObjectId kId;
        kId = uiId;

        if (kId != NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
        {
            NiSceneGraphUpdateObject* pkOtherObject = 
                NiSceneGraphUpdate::GetInstance()->GetObject(kId);
            if (pkOtherObject)
            {
                pkObject = pkOtherObject->GetObject();
            }
        }
    }

    return pkObject;
}
//--------------------------------------------------------------------------------------------------
// This class is meant to assist in parsing and handling RTTI objects
class LoadRTTIHelper : public NiMemObject
{
public:
    LoadRTTIHelper()
    {
        m_pfnFunction = NULL;
        m_pcArgs = NULL;
    }

    ~LoadRTTIHelper()
    {
        NiFree(m_pcArgs);
    }

    // This method will take the input buffer and loader map and 
    // finds the create function for that RTTI name. If any arguments
    // were specified, this method will store them for later use.
    bool ParseRTTINameAndArgs(char* pcBuffer, 
        NiUInt32 uiMaxBufferSize, 
        NiTStringPointerMap<NiStream::CreateFunction>* pkLoaders)
    {
        NIASSERT(pkLoaders);
        NIASSERT(pcBuffer);

        const char* pcName = pcBuffer;
        m_pcArgs = NULL;
        m_uiNumArgs = 0;
        m_pfnFunction = NULL;

        for (NiUInt32 ui = 0; ui < uiMaxBufferSize; ui++)
        {
            if (pcBuffer[ui] == NiStream::ms_cRTTIDelimiter)
            {
                pcBuffer[ui] = '\0';
                if (m_pcArgs == NULL)
                {
                    m_pcArgs = &pcBuffer[ui + 1];
                }
                else
                {
                    m_uiNumArgs++;
                }

            }
            else if (pcBuffer[ui] == '\0')
            {
                if (m_pcArgs != NULL)
                {
                    ptrdiff_t kDiff = (&(pcBuffer[ui]) - m_pcArgs);
                    const char* pcSrcArgs = m_pcArgs;
                    m_pcArgs = NiAlloc(char, kDiff + 1);
                    NiMemcpy(m_pcArgs, kDiff + 1, pcSrcArgs, kDiff + 1);
                    m_uiNumArgs++;
                    NIASSERT(MAX_RTTI_ARG_COUNT > m_uiNumArgs);
                }
                break;
            }
        }

        return pkLoaders->GetAt(pcName, m_pfnFunction);
    }

    // Determine whether or not the instance has a valid create function
    bool IsValid()
    {
        return m_pfnFunction != NULL;
    }

    // Create the object using the previously found create function.
    // If arguments were specified, they will be passed along to the create
    // function.
    NiObject* CreateObject(const char** pcArguments = NULL, NiUInt32 uiArgCount = 0)
    {
        NIASSERT(m_pfnFunction);
        if (m_uiNumArgs == 0)
        {
            return m_pfnFunction(pcArguments, uiArgCount);
        }
        else
        {
            const char* ppcArgs[MAX_RTTI_ARG_COUNT];
            memset(ppcArgs, 0, sizeof(char*) * MAX_RTTI_ARG_COUNT);
            char* pcArgs = m_pcArgs;
            char* pcCurrentArg = m_pcArgs;
            NiUInt32 uiArg = 0;

            while (uiArg != m_uiNumArgs)
            {
                if (*(pcArgs) == '\0')
                {
                    ppcArgs[uiArg] = pcCurrentArg;
                    pcCurrentArg = pcArgs + 1;
                    uiArg++;
                }
                pcArgs++;
            }
            return m_pfnFunction(ppcArgs, m_uiNumArgs);
        }
    }

    NiStream::CreateFunction m_pfnFunction;
    char* m_pcArgs;
    NiUInt32 m_uiNumArgs;
};
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateStream::LoadRTTI()
{
    unsigned short usRTTICount;
    NiStreamLoadBinary(*this, usRTTICount);

    LoadRTTIHelper* pkRTTICreate = NiNew LoadRTTIHelper[usRTTICount];
    NIASSERT(pkRTTICreate != NULL);

    NiUInt32 ui;
    for (ui = 0; ui < usRTTICount; ui++)
    {
        char aucRTTI[MAX_RTTI_LEN];
        LoadRTTIString(aucRTTI);

        // Determine if a create function exists for the RTTI name
        if (!pkRTTICreate[ui].ParseRTTINameAndArgs(aucRTTI, MAX_RTTI_LEN,
            ms_pkLoaders))
        {
            RTTIError(aucRTTI);
        }
    }

    for (ui = 0; ui < m_kObjects.GetAllocatedSize(); ui++)
    {
        unsigned short usRTTI;
        bool bSkippable = false;
     
        NiStreamLoadBinary(*this, usRTTI);
        if ((usRTTI & SKIPPABLE_MASK) != 0)
        {
            bSkippable = true;
            usRTTI &= ~SKIPPABLE_MASK;
        }

        NIASSERT(usRTTI < usRTTICount);
        bool bFound = pkRTTICreate[usRTTI].IsValid();
        if (!bFound && !bSkippable) // create not found and cannot be skipped
        {
            NiDelete [] pkRTTICreate;
            return false;
        }
        else if (!bFound) // create not found, but can be skipped, set to NULL
        {
            m_kObjects.SetAt(ui, NULL);
        }
        else // Create found, so create an object
        {
            NiObject* pkObject = m_kObjects.GetAt(ui);

            // Get object id and state
            NiSceneGraphUpdateObjectId kId = m_kObjectIds.GetAt(ui);
            NiUInt32 uiObjectState = m_kObjectStates.GetAt(ui);

            // Make sure we don't want to replace the object
            if (uiObjectState == NiSceneGraphUpdate::OS_REPLACE)
            {
                // If we are replacing add this object to the replacement map
                NIASSERT(pkObject);
                NiObject* pkOldObject = pkObject;
                pkObject = pkRTTICreate[usRTTI].CreateObject();
                m_kReplaceObjects.SetAt(pkOldObject, pkObject);
            }
            else if (uiObjectState == NiSceneGraphUpdate::OS_UPDATE)
            {
                if (pkObject)
                {
                    const char* pcArguments[2];
                    const NiInt32 iFlag = NI_RECREATE_FLAG;
                    pcArguments[0] = (char*)&iFlag;
                    pcArguments[1] = (char*)pkObject;
                    pkObject = pkRTTICreate[usRTTI].CreateObject(pcArguments, 2);
                }
            }

            // If we don't know what this is create it.
            // This is done for new and replaced objects.
            if (!pkObject)
            {
                pkObject = pkRTTICreate[usRTTI].CreateObject();
            }

            // Make sure this object is being managed and has been initialized.
            if (kId != NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
            {
                // If object does not exist add it to the manager.
                NiSceneGraphUpdate::GetInstance()->AddObject(kId);
                NiSceneGraphUpdate::GetInstance()->SetNiObject(kId, pkObject);
            }

            // Make sure the object matches
            m_kObjects.SetAt(ui, pkObject);
        }
    }

    NiDelete [] pkRTTICreate;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateStream::Load(const NiUInt32 uiBufferSize, const char* pcBuffer)
{
    NiMemStream kMemStream(pcBuffer, uiBufferSize);
    m_pkIstr = &kMemStream;

    // If this platform is big endian, swap the bytes since the buffer is always saved in little
    // endian to make it more efficient in the most common case of running both the editor and
    // viewer on a PC. The viewer on the console will swap the buffer from little to big endian.
    bool bSwapBytes = !NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkIstr->SetEndianSwap(bSwapBytes);

    // Initialize m_uiLoad, m_uiLink, and m_uiPostLink so that progress of
    // background loading thread can be estimated. This has to be postponed
    // until after the header has been read because 
    // m_kObjects.GetAllocatedSize is used in progress estimation.
    m_uiLoad = m_uiLink = m_uiPostLink = 0;

    //Removing all objects can release shared resources.  We must lock this
    //with the stream cleanup critical section.
    RemoveAllObjects();

    NiUInt32 uiObjectCount;
    NiStreamLoadBinary(*this, uiObjectCount);
    if (!uiObjectCount)
    {
        NIASSERT(0);
        m_pkIstr = NULL;
        return false;
    }

    m_kObjects.SetSize(uiObjectCount);

    if (!LoadObjectIds())
    {
        NIASSERT(0);
        m_pkIstr = NULL;
        return false;
    }

    if (!LoadRTTI())
    {
        NIASSERT(0);
        m_pkIstr = NULL;
        return false;
    }

    if (!LoadFixedStringTable())
    {
        NIASSERT(0);
        m_pkIstr = NULL;
        return false;
    }

    if (!LoadObjectSizeTable())
    {
        NIASSERT(0);
        m_pkIstr = NULL;
        return false;
    }

    // read object groups
    LoadObjectGroups();

    // read list of objects
    for ( ; m_uiLoad < uiObjectCount; m_uiLoad++)
    {
#if NIDEBUG
        NiUInt32 uiCurrentPos = m_pkIstr->GetPosition();
#endif

        NiSceneGraphUpdateObjectId kId = m_kObjectIds.GetAt(m_uiLoad);
        NiUInt32 uiObjectState = m_kObjectStates.GetAt(m_uiLoad);
        if (kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID ||
            NiSceneGraphUpdate::IsObjectStateDirty(uiObjectState))
        {
            NiObject* pkObject = m_kObjects.GetAt(m_uiLoad);
            if (pkObject)
            {
                pkObject->LoadBinary(*this);
            }
            else 
            {
                m_pkIstr->Seek(m_kObjectSizes.GetAt(m_uiLoad));
            }
        }

#if NIDEBUG
        if (m_kObjectSizes.GetSize() != 0)
        {
            NiUInt32 uiBytesRead = m_pkIstr->GetPosition() -
                uiCurrentPos;
            NiUInt32 uiStreamedSizeInBytes = 
                m_kObjectSizes.GetAt(m_uiLoad);
            NIASSERT(uiBytesRead == uiStreamedSizeInBytes);
        }
#endif
    }

    LoadTopLevelObjects();

    // linking phase
    for ( ; m_uiLink < uiObjectCount; m_uiLink++)
    {
        NiSceneGraphUpdateObjectId kId = m_kObjectIds.GetAt(m_uiLink);
        NiUInt32 uiObjectState = m_kObjectStates.GetAt(m_uiLink);
        if (kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID ||
            NiSceneGraphUpdate::IsObjectStateDirty(uiObjectState))
        {
            NiObject* pkObject = m_kObjects.GetAt(m_uiLink).GetObjectToLink();
            if (pkObject)
                pkObject->LinkObject(*this);
        }
    }

    // post-link phase
    for ( ; m_uiPostLink < uiObjectCount; m_uiPostLink++)
    {
        NiSceneGraphUpdateObjectId kId = m_kObjectIds.GetAt(m_uiPostLink);
        NiUInt32 uiObjectState = m_kObjectStates.GetAt(m_uiPostLink);
        if (kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID ||
            NiSceneGraphUpdate::IsObjectStateDirty(uiObjectState))
        {
            NiObject* pkObject = 
                (NiObject*)m_kObjects.GetAt(m_uiPostLink);
            if (pkObject)
                pkObject->PostLinkObject(*this);
        }
    }

    // Post-processing phase.
    if (ms_pkPostProcessFunctions->GetEffectiveSize() > 0)
    {
        for (NiUInt32 uj = 0; uj < ms_pkPostProcessFunctions->GetSize(); 
            uj++)
        {
            PostProcessFunction pfnFunc = ms_pkPostProcessFunctions->GetAt(uj);
            if (pfnFunc)
            {
                pfnFunc(*this, m_kTopObjects);
            }
        }

        if (ms_pkPostProcessFinalFunction)
        {
            ms_pkPostProcessFinalFunction(*this, m_kTopObjects);
        }
    }

    FreeLoadData();

    m_pkIstr = NULL;

    return true;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateStream::InsertObject(const NiSceneGraphUpdateObjectId& kId, 
    const NiUInt32 uiObjectState, NiObject* pkObject)
{
#ifdef NIDEBUG
    for (NiUInt32 ui = 0; ui < m_kTopObjectIds.GetSize(); ui++)
    {
        NIASSERT(kId != m_kTopObjectIds.GetAt(ui));
    }
#endif

    NIASSERT(m_kTopObjects.GetSize() == m_kTopObjectIds.GetSize());
    NIASSERT(m_kTopObjectIds.GetSize() == m_kTopObjectStates.GetSize());

    m_kTopObjectIds.Add(kId);
    m_kTopObjectStates.Add(uiObjectState);

    NiStream::InsertObject(pkObject);
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateStream::SaveObjectIds()
{
    NiUInt32 uiTopLevelCount = m_kTopObjects.GetSize();
    NIASSERT(uiTopLevelCount == m_kTopObjectIds.GetSize());
    NIASSERT(uiTopLevelCount == m_kTopObjectStates.GetSize());

    NiUInt32 uiObjectCount = m_kObjects.GetSize();
    m_kObjectIds.SetSize(uiObjectCount);
    m_kObjectStates.SetSize(uiObjectCount);
    for (NiUInt32 ui = 0; ui < uiObjectCount; ui++)
    {
        NiObject* pkObject = m_kObjects.GetAt(ui);
        NiSceneGraphUpdateObjectId kId = NiSceneGraphUpdateObjectId::NULL_OBJECT_ID;
        NiUInt32 uiObjectState = NiSceneGraphUpdate::OS_CLEAN;

        // Get the id if we are a top level object
        for (NiUInt32 uj = 0; uj < uiTopLevelCount; uj++)
        {
            if (m_kTopObjects.GetAt(uj) == pkObject)
            {
                kId = m_kTopObjectIds.GetAt(uj);
                uiObjectState = m_kTopObjectStates.GetAt(uj);
                break;
            }
        }

        if (kId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
        {
            kId = NiSceneGraphUpdate::GetInstance()->GetObjectId(pkObject);
            uiObjectState = NiSceneGraphUpdate::OS_CLEAN;
        }

        // If we are replacing add this object to the replacement map
        if (uiObjectState == NiSceneGraphUpdate::OS_REPLACE)
        {
            NiSceneGraphUpdateObject* pkOldObject = 
                NiSceneGraphUpdate::GetInstance()->GetObject(kId);
            if (pkOldObject && pkOldObject->GetObject())
                m_kReplaceObjects.SetAt(pkOldObject->GetObject(), pkObject);
            else
                uiObjectState = NiSceneGraphUpdate::OS_UPDATE;
        }

        m_kObjectIds.SetAt(ui, kId);
        m_kObjectStates.SetAt(ui, uiObjectState);

        NiUInt32 uiId = kId;
        NiStreamSaveBinary(*this, uiId);
        NiStreamSaveBinary(*this, uiObjectState);
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateStream::LoadObjectIds()
{
    NiUInt32 uiObjectCount = m_kObjects.GetAllocatedSize();
    m_kObjectIds.SetSize(uiObjectCount);
    m_kObjectStates.SetSize(uiObjectCount);
    for (NiUInt32 ui = 0; ui < uiObjectCount; ui++)
    {
        NiUInt32 uiId;
        NiUInt32 uiObjectState;
        NiStreamLoadBinary(*this, uiId);
        NiStreamLoadBinary(*this, uiObjectState);
        NiSceneGraphUpdateObjectId kId = uiId;

        NiSceneGraphUpdateObject* pkObject = NiSceneGraphUpdate::GetInstance()->GetObject(kId);
        if (pkObject)
            m_kObjects.SetAt(ui, pkObject->GetObject());
        else
            m_kObjects.SetAt(ui, NULL);

        m_kObjectIds.SetAt(ui, kId);
        m_kObjectStates.SetAt(ui, uiObjectState);
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
