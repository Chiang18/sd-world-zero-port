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
#ifndef EE_STANDARDALLOCATOR_H
#define EE_STANDARDALLOCATOR_H

#include <efd/IAllocator.h>
#include <efd/SmallObjectAllocator.h>
#include <efd/ExternalMemAllocator.h>

#if defined(EE_PLATFORM_WII)
#include <efd/Wii/WiiMemAllocator.h>
#endif

namespace efd
{

#define EE_USE_SMALL_OBJECT_ALLOCATOR

#if defined(EE_PLATFORM_WII)

/// Define a Wii-specific allocator for use on the Wii that is aware of MEM1 and MEM2 arenas and
/// uses a NiMemHint object passed to the allocator to allocate memory from an appropriate arena.
typedef SmallObjectAllocator<WiiMemAllocator> StandardAllocator;

#else

/// Define a general allocator for use with most platforms.
typedef SmallObjectAllocator<ExternalMemAllocator> StandardAllocator;

#endif

};    // End namespace efd.

#endif    // EE_STANDARDALLOCATOR_H
