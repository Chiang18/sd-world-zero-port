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
#ifndef EE_ATOMICOPERATIONS_H
#define EE_ATOMICOPERATIONS_H

#include <efd/OS.h>
#include <efd/UniversalTypes.h>
#include <efd/Asserts.h>

namespace efd
{
/// atomically increment a variable
inline efd::SInt32 AtomicIncrement(efd::SInt32 &value);

/// atomically decrement a variable
inline efd::SInt32 AtomicDecrement(efd::SInt32 &value);

/// atomically increment a variable
inline efd::UInt32 AtomicIncrement(efd::UInt32 &value);

/// atomically decrement a variable
inline efd::UInt32 AtomicDecrement(efd::UInt32 &value);

/// atomically increment a variable
inline efd::SInt32 AtomicIncrement(volatile efd::SInt32 &value);

/// atomically decrement a variable
inline efd::SInt32 AtomicDecrement(volatile efd::SInt32 &value);

/// atomically increment a variable
inline efd::UInt32 AtomicIncrement(volatile efd::UInt32 &value);

/// atomically decrement a variable
inline efd::UInt32 AtomicDecrement(volatile efd::UInt32 &value);


#if defined(EE_PLATFORM_WII) || defined(__SPU__)
    /// atomically increment a variable
    inline size_t AtomicIncrement(size_t &value);

    /// atomically decrement a variable
    inline size_t AtomicDecrement(size_t &value);

    /// atomically increment a variable
    inline size_t AtomicIncrement(volatile size_t &value);

    /// atomically decrement a variable
    inline size_t AtomicDecrement(volatile size_t &value);
#endif

#if defined(EE_ARCH_64) || defined(EE_PLATFORM_PS3) || defined(EE_PLATFORM_WII)
    /// atomically increment a variable
    inline efd::SInt64 AtomicIncrement(efd::SInt64 &value);

    /// atomically decrement a variable
    inline efd::SInt64 AtomicDecrement(efd::SInt64 &value);

    /// atomically increment a variable
    inline efd::UInt64 AtomicIncrement(efd::UInt64 &value);

    /// atomically decrement a variable
    inline efd::UInt64 AtomicDecrement(efd::UInt64 &value);

    /// atomically increment a variable
    inline efd::SInt64 AtomicIncrement(volatile efd::SInt64 &value);

    /// atomically decrement a variable
    inline efd::SInt64 AtomicDecrement(volatile efd::SInt64 &value);

    /// atomically increment a variable
    inline efd::UInt64 AtomicIncrement(volatile efd::UInt64 &value);

    /// atomically decrement a variable
    inline efd::UInt64 AtomicDecrement(volatile efd::UInt64 &value);
#endif

} // end namespace efd

// Include the platform specific inline functions
#include EE_PLATFORM_SPECIFIC_INCLUDE(efd,AtomicOperations,inl)

#endif // EE_ATOMICOPERATIONS_H

