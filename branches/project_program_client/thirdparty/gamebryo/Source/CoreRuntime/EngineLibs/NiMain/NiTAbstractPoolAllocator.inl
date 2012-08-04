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
//---------------------------------------------------------------------------
//  NiTAbstractPoolAllocator inline functions

//---------------------------------------------------------------------------

#include "NiTAbstractPoolAllocatorFuncStorage.h"
#include <NiMemoryDefines.h>
#include <NiDebug.h>

template <class T> inline
void NiTAbstractPoolAllocator<T>::EnsureFreeMem()
{
    if (!ms_pkFreeMem)
        CreateFreeMem();  
}
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::CreateFreeMem()
{
    NIASSERT(ms_pkFreeMem == 0);
    NIASSERT(ms_uiBlockSize >= 2);

#if EE_USE_PER_THREAD_ALLOCATOR_POOLS
    if(!m_sbIsPerThreadShutdown)
    {
        m_sbIsPerThreadShutdown = true;
        NiTAbstractPoolAllocatorFuncStorage::AddPerThreadShutdown(
            NiTAbstractPoolAllocator<T>::_SDMPerTheadShutdown);
    }
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS

    AllocNode* pkFreeMem = 
        NiNew typename NiTAbstractPoolAllocator<T>::AllocNode[ms_uiBlockSize];

    // Handle end point
    typename NiTAbstractPoolAllocator<T>::AllocNode* pkCurr = 
        pkFreeMem + ms_uiBlockSize - 1;
    pkCurr->m_pkNext = 0;

    unsigned int i = 1;

    do
    {
        pkCurr = pkFreeMem + i;
        pkCurr->m_pkNext = pkCurr + 1;
    } while(i++ < ms_uiBlockSize - 2);

    if (ms_pkBlockHeader)
    {
        // add block to head of list
        pkFreeMem->m_pkNext = ms_pkBlockHeader;
        ms_pkBlockHeader = pkFreeMem;
    }
    else
    {
        ms_pkBlockHeader = pkFreeMem;
        ms_pkBlockHeader->m_pkNext = 0;
    }

    pkFreeMem = pkFreeMem + 1;
    ms_pkFreeMem = pkFreeMem;
}
//---------------------------------------------------------------------------
#if EE_USE_PER_THREAD_ALLOCATOR_POOLS
template <class T> inline
void NiTAbstractPoolAllocator<T>::GetFreeMemFromChain()
{
    if (!ms_pkFreeChainList)
    {
        CreateFreeMem();
    }
    else
    {
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem = ms_pkFreeChainList->m_pkData;
        FreeChain* pFreeList = ms_pkFreeChainList;
        ms_pkFreeChainList = ms_pkFreeChainList->m_pkNext;
        NiDelete pFreeList;
    }
}
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::_SDMShutdown()
{
    typename NiTAbstractPoolAllocator<T>::AllocNode* pkCurr = ms_pkBlockHeader;

    while (pkCurr)
    {
        typename NiTAbstractPoolAllocator<T>::AllocNode* pkNext = 
            pkCurr->m_pkNext;
        NiDelete [] pkCurr;
        pkCurr = pkNext;
    }

#if EE_USE_PER_THREAD_ALLOCATOR_POOLS

    FreeChain* pkDel = ms_pkFreeChainList;
    while (pkDel)
    {
        FreeChain* pkNext = pkDel->m_pkNext;
        NiDelete pkDel;
        pkDel = pkNext;
    }

    ms_pkFreeChainList = NULL;

    if (m_sbIsPerThreadShutdown)
    {
        m_sbIsPerThreadShutdown = false;
        NiTAbstractPoolAllocatorFuncStorage::RemovePerThreadShutdown(
            NiTAbstractPoolAllocator<T>::_SDMPerTheadShutdown);
    }
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS

    ms_pkBlockHeader = NULL;
    ms_pkFreeMem = NULL;    
}
//---------------------------------------------------------------------------
#if EE_USE_PER_THREAD_ALLOCATOR_POOLS
template <class T> inline
void NiTAbstractPoolAllocator<T>::_SDMPerTheadShutdown()
{    
    // critical section is above
    typename NiTAbstractPoolAllocator<T>::AllocNode* pkFreeMem = 
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem;

    if (pkFreeMem != NULL)
    {
        unsigned int uiCounter = 0;
        typename NiTAbstractPoolAllocator<T>::AllocNode* pkCurr = pkFreeMem;
        typename NiTAbstractPoolAllocator<T>::AllocNode* pkTmp = pkFreeMem;
        while (pkCurr)
        {
            typename NiTAbstractPoolAllocator<T>::AllocNode* pkNext = 
                pkCurr->m_pkNext;

            uiCounter++;

            bool bMaxSize = uiCounter >= ms_uiBlockSize;
            if (bMaxSize)
            {
                pkTmp = pkFreeMem;
                pkFreeMem = pkNext;
                pkCurr->m_pkNext = 0;
                uiCounter = 0;
            }

            pkCurr = pkNext;
            if (pkCurr == NULL || bMaxSize)
            {
                if (ms_pkFreeChainList)
                {
                    FreeChain* pkNewChain = NiNew FreeChain;
                    pkNewChain->m_pkData = pkTmp;
                    pkNewChain->m_pkNext = 0;

                    FreeChain* pTmpChain = ms_pkFreeChainList;
                    ms_pkFreeChainList = pkNewChain;
                    ms_pkFreeChainList->m_pkNext = pTmpChain;
                }
                else
                {
                    ms_pkFreeChainList = NiNew FreeChain;
                    ms_pkFreeChainList->m_pkData = pkTmp;
                    ms_pkFreeChainList->m_pkNext = 0;
                }
            }
        }

        ms_pkFreeMem = NULL;
    }
}
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::SetBlockSize(unsigned int uiBlockSize)
{
    NIASSERT(uiBlockSize >= 2);
    ms_uiBlockSize = uiBlockSize; 
}
