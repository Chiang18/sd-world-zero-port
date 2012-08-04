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

#include <direct.h>
#include <process.h>
#include <efd/Asserts.h>

namespace efd
{

//-------------------------------------------------------------------------------------------------
inline efd::SInt32 efd::AtomicIncrement(efd::SInt32& value)
{
    return InterlockedIncrement((LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 efd::AtomicDecrement(efd::SInt32& value)
{
    return InterlockedDecrement((LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 efd::AtomicIncrement(volatile efd::SInt32& value)
{
    return InterlockedIncrement((volatile LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 efd::AtomicDecrement(volatile efd::SInt32& value)
{
    return InterlockedDecrement((volatile LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 efd::AtomicIncrement(efd::UInt32& value)
{
    return InterlockedIncrement((LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 efd::AtomicDecrement(efd::UInt32& value)
{
    EE_ASSERT(value > 0);
    return InterlockedDecrement((LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 efd::AtomicIncrement(volatile efd::UInt32& value)
{
    return InterlockedIncrement((volatile LONG*)&value);
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 efd::AtomicDecrement(volatile efd::UInt32& value)
{
    EE_ASSERT(value > 0);
    return InterlockedDecrement((volatile LONG*)&value);
}
//-------------------------------------------------------------------------------------------------

} // end namespace efd
