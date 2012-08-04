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
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef EE_DEFAULTINITIALIZEMEMORYMANAGER_H
#define EE_DEFAULTINITIALIZEMEMORYMANAGER_H

#include <efd/MemTracker.h>
#include <efd/StandardAllocator.h>
#include <efd/MemManager.h>

#if defined (EE_PLATFORM_WII)
#include <efd/Wii/WiiHeapManager.h>
#endif

namespace efd
{
    /**
        Default implementation of the CreateGlobalMemoryAllocator() function.
        
        To use the default implementation in a statically linked application, invoke the macro
        EE_USE_DEFAULT_ALLOCATOR in a source file linked into the application.  

        This implementation is always used in dynamically linked versions
        (see MemManager.cpp for more details).

        @return Pointer to an IAllocator instance.
    */
    inline IAllocator* CreateDefaultGlobalMemoryAllocator();

} // end namespace efd


#if defined(__CWCC__)
    /**
        Optional implementation of CodeWarrior __sys_alloc and __sys_free, 
        which are used by CRT malloc and free functions. These functions must 
        precede CodeWarrior implementation in the link order. This can be done
        by simply including this macro in the application's initialization code
        source file. If an application does not use this implementation, it 
        must explicitly call NiWiiHeapManager::Initialize(...) prior to calling
        EE_INIT() / NiInit() to ensure Gamebryo has access to at least one Wii
        memory heap.

        This macro is automatically invoked by EE_UseDefaultAllocatorImplementation.
     */
    #define EE_WiiSysAllocAndSysFreeImplementation \
        extern "C" { \
        void* __sys_alloc(__std(size_t) blocksize) \
        { \
            if (!efd::WiiHeapManager::IsInitialized()) \
            { \
                efd::WiiHeapManager::Initialize(); \
            } \
            return MEMAllocFromExpHeap(efd::WiiHeapManager::GetHeap( \
                efd::EE_WII_MEM2_ARENA), blocksize); \
        } \
        void __sys_free(void* block) \
        { \
            if (!efd::WiiHeapManager::IsInitialized()) \
            { \
                efd::WiiHeapManager::Initialize(); \
            } \
            MEMFreeToExpHeap(efd::WiiHeapManager::GetHeap(efd::EE_WII_MEM2_ARENA), \
                block); \
        } \
        } //extern "C"
#else
    #define EE_WiiSysAllocAndSysFreeImplementation
#endif // defined(__CWCC__)

/**
    Include this macro in a source file to get the default memory allocator.  If you want
    to implement a custom Gamebryo allocator, implement a CreateGlobalMemoryAllocator method
    that returns the custom allocator.

    This macro also invokes EE_WiiSysAllocAndSysFreeImplementation, saving a separate invocation.
*/
#if defined(EE_EFD_IMPORT) || ((defined(_WINDLL) || defined(USEDLL)) && !defined(EE_EFD_EXPORT))
    // The macro does nothing in DLL import builds since it is already included via
    // the DLL build of MemManager.cpp
    #define EE_USE_DEFAULT_ALLOCATOR
#else
    #define EE_USE_DEFAULT_ALLOCATOR \
        EE_WiiSysAllocAndSysFreeImplementation; \
        EE_EFD_ENTRY efd::IAllocator* efd::CreateGlobalMemoryAllocator() \
        { \
            return efd::CreateDefaultGlobalMemoryAllocator(); \
        }
#endif

#include <efd/DefaultInitializeMemoryManager.inl>

#endif // #ifndef EE_DEFAULTINITIALIZEMEMORYMANAGER_H
