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

#include "efdPhysXPCH.h"

#include <efd/Asserts.h>
#include <efd/MemHint.h>
#include <efd/MemoryDefines.h>

#include "PhysXAllocator.h"

using namespace efd;
using namespace efdPhysX;

//-------------------------------------------------------------------------------------------------
void* PhysXAllocator::malloc(size_t stSize)
{
    return EE_MALLOC2(stSize, MemHint::USAGE_PHYSICS); 
}
//-------------------------------------------------------------------------------------------------
void* PhysXAllocator::malloc(size_t stSize, NxMemoryType)
{
    return EE_MALLOC2(stSize, MemHint::USAGE_PHYSICS);
}
//-------------------------------------------------------------------------------------------------
void* PhysXAllocator::mallocDEBUG(size_t stSize, const char* pcFileName,
    int iLine)
{
    EE_UNUSED_ARG(iLine);
    EE_UNUSED_ARG(pcFileName);
#ifdef EE_USE_MEMORY_MANAGEMENT
    return efd::_Malloc(stSize, MemHint::USAGE_PHYSICS, pcFileName, iLine, __FUNCTION__);
#else
    return EE_MALLOC2(stSize, MemHint::USAGE_PHYSICS);
#endif // EE_USE_MEMORY_MANAGEMENT 
}    
//-------------------------------------------------------------------------------------------------
void* PhysXAllocator::mallocDEBUG(size_t stSize, const char* pcFileName,
    int iLine, const char*, NxMemoryType)

{
    EE_UNUSED_ARG(iLine);
    EE_UNUSED_ARG(pcFileName);
#ifdef EE_USE_MEMORY_MANAGEMENT
    return efd::_Malloc(stSize, MemHint::USAGE_PHYSICS, pcFileName, iLine, __FUNCTION__);
#else
    return EE_MALLOC2(stSize, MemHint::USAGE_PHYSICS);
#endif // EE_USE_MEMORY_MANAGEMENT 
}    
//-------------------------------------------------------------------------------------------------
void* PhysXAllocator::realloc(void* pvMemory, size_t stSize)
{
    return EE_REALLOC2(pvMemory, stSize, MemHint::USAGE_PHYSICS);
}   
//-------------------------------------------------------------------------------------------------
void PhysXAllocator::free(void* pvMemory)
{
    EE_MEMASSERT(NULL != pvMemory);
    EE_FREE(pvMemory);
}
//-------------------------------------------------------------------------------------------------
