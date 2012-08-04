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

#include "NiDynamicEffectStateManager.h"
#include "NiDynamicEffectState.h"

NiDynamicEffectStateManager* 
    NiDynamicEffectStateManager::ms_pkDynamicEffectStateManager = NULL;

//---------------------------------------------------------------------------
void NiDynamicEffectStateManager::_SDMInit()
{
    ms_pkDynamicEffectStateManager = NiNew NiDynamicEffectStateManager();
}
//---------------------------------------------------------------------------
void NiDynamicEffectStateManager::_SDMShutdown()
{
    NiDelete ms_pkDynamicEffectStateManager;
}
//---------------------------------------------------------------------------
NiDynamicEffectStateManager::NiDynamicEffectStateManager()
{
    m_pkEffectStateMap = 
        NiNew NiTPointerMap<NiUInt32, ListEntry*>(DEFAULT_MAP_SIZE);
    NIASSERT(m_pkEffectStateMap);

    m_pkListEntryPool = 
        NiNew NiTObjectPool<ListEntry>(DEFAULT_LIST_ENTRY_POOL_SIZE);
    NIASSERT(m_pkListEntryPool);
}
//---------------------------------------------------------------------------
NiDynamicEffectStateManager::~NiDynamicEffectStateManager()
{
    NiDelete m_pkEffectStateMap;

    m_pkListEntryPool->PurgeAllObjects();
    NiDelete m_pkListEntryPool;
}
//---------------------------------------------------------------------------
NiDynamicEffectState* NiDynamicEffectStateManager::AddDynamicEffectState(
    NiDynamicEffectState* pkEffectState)
{
    if (!pkEffectState)
        return NULL;

    NiUInt32 uiHashKey = pkEffectState->GetHashKey();
    ListEntry* pkEntry = NULL;

    m_kEffectStateCriticalSection.Lock();

    // Check to see if an entry already exists with the same key.
    m_pkEffectStateMap->GetAt(uiHashKey, pkEntry);

    if (!pkEntry)
    {
        // No other entry contain the same key.
        pkEntry = m_pkListEntryPool->GetFreeObject();
        NIASSERT(pkEntry);

        pkEntry->m_pkNext = NULL;
        pkEntry->m_pkEffectState = pkEffectState;
        m_pkEffectStateMap->SetAt(uiHashKey, pkEntry);

        m_kEffectStateCriticalSection.Unlock();
        return pkEffectState;
    }

    // An set of entries was found with the same key. Iterate through
    // the list to deterimine if any of those entries are an exact match.
    while (pkEntry)
    {
        if (pkEntry->m_pkEffectState->Equal(pkEffectState))
        {
            // An identical effect state already exists. 
            NiDynamicEffectState* pkRet = pkEntry->m_pkEffectState;
            m_kEffectStateCriticalSection.Unlock();
            return pkRet;
        }

        if (!pkEntry->m_pkNext)
            break;

        pkEntry = pkEntry->m_pkNext;
    }

    pkEntry->m_pkNext = m_pkListEntryPool->GetFreeObject();
    pkEntry = pkEntry->m_pkNext;

    pkEntry->m_pkNext = NULL;
    pkEntry->m_pkEffectState = pkEffectState;

    m_kEffectStateCriticalSection.Unlock();
    return pkEffectState;
}
//---------------------------------------------------------------------------
bool NiDynamicEffectStateManager::RemoveDynamicEffectState(
    NiDynamicEffectState* pkEffectState)
{
    if (!pkEffectState)
        return false;

    NiUInt32 uiHashKey = pkEffectState->GetHashKey();

    m_kEffectStateCriticalSection.Lock();

    ListEntry* pkPrevEntry = NULL;
    ListEntry* pkEntry = NULL;
    m_pkEffectStateMap->GetAt(uiHashKey, pkEntry);

    while (pkEntry)
    {
        if (pkEntry->m_pkEffectState == pkEffectState)
        {
            if (pkPrevEntry)
            {
                pkPrevEntry->m_pkNext = pkEntry->m_pkNext;
            }
            else
            {
                // Handle case were pkEntry is either the first entry in 
                // list or the only entry in the list.
                m_pkEffectStateMap->SetAt(uiHashKey, pkEntry->m_pkNext);
            }

            m_pkListEntryPool->ReleaseObject(pkEntry);
            
            m_kEffectStateCriticalSection.Unlock();
            return true;
        }

        pkPrevEntry = pkEntry;
        pkEntry = pkEntry->m_pkNext;
    }

    // Effect State not found.
    m_kEffectStateCriticalSection.Unlock();
    return false;
}
//---------------------------------------------------------------------------
