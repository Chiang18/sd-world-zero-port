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
//  NiTObjectAllocator inline functions
//---------------------------------------------------------------------------
template <class T> inline
void* NiTObjectAllocator<T>::Allocate()
{
#if !EE_USE_PER_THREAD_ALLOCATOR_POOLS
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Lock();
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS

#ifdef NIDEBUG
    unsigned int uiAllocated = NiTAbstractPoolAllocator<T>::ms_uiAllocated;
    unsigned int uiMaxAllocated = NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated;

    NiTAbstractPoolAllocator<T>::ms_uiAllocated = ++uiAllocated;

    if (uiAllocated > uiMaxAllocated)
    {
        NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated = uiAllocated;
    }
#endif

    if (!NiTAbstractPoolAllocator<T>::ms_pkFreeMem)
    {
#if EE_USE_PER_THREAD_ALLOCATOR_POOLS
        NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Lock();
        NiTObjectAllocator<T>::GetFreeMemFromChain();
        NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Unlock();
#else
        NiTObjectAllocator<T>::CreateFreeMem();
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS
    }

    typename NiTAbstractPoolAllocator<T>::AllocNode* pTmp = 
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem;

    typename NiTAbstractPoolAllocator<T>::AllocNode* pkFreeMem = 
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem;
    NiTAbstractPoolAllocator<T>::ms_pkFreeMem = pkFreeMem->m_pkNext;

    pTmp->m_pkNext = 0;
    pTmp->m_pkData = 0;

#if !EE_USE_PER_THREAD_ALLOCATOR_POOLS
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Unlock();
#endif

    return pTmp;
} 
//---------------------------------------------------------------------------
template <class T> inline
void NiTObjectAllocator<T>::Deallocate(void* pkDel) 
{
#if !EE_USE_PER_THREAD_ALLOCATOR_POOLS
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Lock();
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS

#ifdef NIDEBUG
    unsigned int uiAllocated = NiTAbstractPoolAllocator<T>::ms_uiAllocated;
    NiTAbstractPoolAllocator<T>::ms_uiAllocated = --uiAllocated;
#endif

    // Node being freed - Just set the freepointer
    // here and the next to the previous free
    // In debug, memset(0).
    typename NiTAbstractPoolAllocator<T>::AllocNode* pDel = 
        (typename NiTAbstractPoolAllocator<T>::AllocNode*)pkDel;

    //pDel->m_pkData = 0;
    pDel->m_pkNext = NiTAbstractPoolAllocator<T>::ms_pkFreeMem;
    NiTAbstractPoolAllocator<T>::ms_pkFreeMem = pDel;

#if !EE_USE_PER_THREAD_ALLOCATOR_POOLS
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Unlock();
#endif // EE_USE_PER_THREAD_ALLOCATOR_POOLS
}
//---------------------------------------------------------------------------
