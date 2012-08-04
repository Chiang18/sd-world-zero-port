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
#ifndef EE_EEHELPERS_H
#define EE_EEHELPERS_H

// This header is simply a place to put universally helpful macros or functions.


/** @def EE_ARRAYSIZEOF( x )
    Determines the size of a static array at compile time.  Examples:

    @code
        EE_ASSERT( EE_ARRAYSIZEOF( "foo" ) == 4 );  // don't forget the implied terminating NULL
        static const char MyString[20] = "foo";
        EE_ASSERT( EE_ARRAYSIZEOF( MyString ) == 20 );  // size of the array, not length of the string
        int array[] = { 1, 2, 3, 4 };
        EE_ASSERT( EE_ARRAYSIZEOF( array ) == 4 );
        MyClass blah[4];
        EE_ASSERT( EE_ARRAYSIZEOF( blah ) == 4 );
    @endcode

    @note This macro does \b not work with pointers.  The size of a pointer is always 4 bytes on
    a 32bit platform so you end up with 4 divided by the size of your object type.
    @code
        char* pch = "Some String Longer Than Four";
        EE_ASSERT( EE_ARRAYSIZEOF( pch ) == 4 );
    @endcode
*/
#if defined( ARRAYSIZEOF )
#define EE_ARRAYSIZEOF(x) ARRAYSIZEOF(x)
#else
#define EE_ARRAYSIZEOF(x) (sizeof(x) / sizeof((x)[0]))
#endif


/// @def EE_OFFSETOF( CLASS, MEMBER )
/// Determines the offset of a member within a class or structure.
#if defined(offsetof)
#define EE_OFFSETOF( CLASS, MEMBER )  offsetof(CLASS,MEMBER)
#else
#define EE_OFFSETOF( CLASS, MEMBER )  (size_t)(&(((CLASS*)0)->MEMBER))
#endif


/// Deletes a pointer to a memory managed object (efd::MemObject) and sets the pointer value
/// to NULL to show that future access is invalid.
#define EE_SAFE_DELETE( x )  do { EE_DELETE (x); (x) = NULL; } while ( 0 )

/// Deletes a pointer to an external resource (allocated via EE_EXTERNAL_NEW) and sets the
/// pointer value to NULL to show that future access is invalid.
#define EE_SAFE_EXTERNAL_DELETE( x )  do { EE_EXTERNAL_DELETE (x); (x) = NULL; } while ( 0 )


/// If the pointer is non-NULL then decrements a pointer to an efd::RefCountImpl, efd::IRefCount,
/// or efd::IBase and sets the pointer to NULL to show that future access is invalid.  Note that
/// this is only needed in cases where an efd::SmartPointer<> is not being used.  Using the
/// SmartPointer class is the preferred method for dealing with reference counted objects.
#define EE_SAFE_DECREMENT( x ) do { if ( x ) { (x)->DecRefCount(); (x) = NULL; } } while ( 0 )


namespace efd
{
    // ... place more helpers here ...
}


#endif // EE_EEHELPERS_H
