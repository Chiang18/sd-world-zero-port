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

#pragma once
#ifndef NISMARTPOINTER_H
#define NISMARTPOINTER_H

#include <efd/SmartPointer.h>

//---------------------------------------------------------------------------

#define NiPointer efd::SmartPointer

#define NiSmartPointer(classname) \
    class classname; \
    typedef efd::SmartPointer<classname> classname##Ptr

// Use for casting a smart pointer of one type to a pointer or smart pointer
// of another type.
#define NiSmartPointerCast(type, smartptr) EE_UNSAFE_SMART_POINTER_CAST(type, smartptr)

//---------------------------------------------------------------------------

#endif // NISMARTPOINTER_H
