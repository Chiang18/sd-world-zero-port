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
#ifndef EE_REFERENCECOUNTING_H
#define EE_REFERENCECOUNTING_H

#include <efd/MemObject.h>
#include <efd/RefCount.h>

namespace efd
{
/// @name efd::IBase reference counting implementations
/// These macros provide implementations for common reference counting styles.  They are intended
/// for use inside of IBase derived classes.  If you have a non-IBase derived class for which
/// you want to implement reference counting, consider using the helper classes in RefCount.h
/// instead.
//@{

/// A helper macro for defining abstract classes that support reference counting.
#define EE_DECLARE_ABSTRACT_REFCOUNT \
    protected: \
        virtual void DeleteThis() const = 0; \
    public: \
        virtual void IncRefCount() const = 0;\
        \
        virtual void DecRefCount() const = 0; \
        virtual efd::UInt32 GetRefCount() const = 0;


//-------------------------------------------------------------------------------------------------

/// A helper macro that implements a reference count.  Use in classes derived from efd::IBase.
#define EE_DECLARE_CONCRETE_REFCOUNT_EX(ReferenceCountType) \
    public: \
        void IncRefCount() const \
        { \
            ++m_RefCount; \
        } \
        \
        void DecRefCount() const \
        { \
            if (--m_RefCount == 0) \
            { \
                DeleteThis(); \
            } \
        } \
        efd::UInt32 GetRefCount() const \
        { \
            return m_RefCount; \
        } \
    protected: \
        void DeleteThis() const \
        { \
            EE_DELETE this; \
        } \
        mutable ReferenceCountType m_RefCount;

#define EE_DECLARE_CONCRETE_REFCOUNT \
    EE_DECLARE_CONCRETE_REFCOUNT_EX(efd::NonAtomicRefCount)

#define EE_DECLARE_CONCRETE_ATOMIC_REFCOUNT \
    EE_DECLARE_CONCRETE_REFCOUNT_EX(efd::AtomicRefCount)

//-------------------------------------------------------------------------------------------------
/// Forwards the implementation of the reference count to a parent class.  Use in classes that
/// use multiple inheritance from efd::IBase where at least one base class already has a concrete
/// reference count.
#define EE_DECLARE_FORWARDED_CONCRETE_REFCOUNT(forwardClass) \
    protected: \
        void DeleteThis() const \
        { \
            forwardClass::DeleteThis(); \
        } \
    public: \
        void IncRefCount() const \
        { \
            forwardClass::IncRefCount(); \
        } \
        \
        void DecRefCount() const \
        { \
            forwardClass::DecRefCount(); \
        }\
        efd::UInt32 GetRefCount() const \
        { \
            return forwardClass::GetRefCount(); \
        }

//@}

//-------------------------------------------------------------------------------------------------

/**
    Helper template for creating a reference counted class from a non-reference counted base class.
    Usage:
    @code
        typedef efd::RefCounted<MyClass> MyRefCountedClass;
    @endcode
*/
template< typename BaseClass >
class RefCounted : public BaseClass
{
    EE_DECLARE_CONCRETE_REFCOUNT;
};

/**
    Helper template for creating a reference counted MemObject class from a base class that is
    not reference counted or a MemObject.
    Usage:
    @code
        typedef efd::RefCountedMemObj<MyClass> MyRefCountedMemObjectClass;
    @endcode
*/
template< typename BaseClass >
class RefCountedMemObj : public MemObject, public BaseClass
{
    EE_DECLARE_CONCRETE_REFCOUNT;
};

//-------------------------------------------------------------------------------------------------
} // end namespace efd

#endif // EE_IBASE_H

