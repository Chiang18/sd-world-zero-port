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

#include "efdPCH.h"

#include <efd/RTLib.h>

#include <efd/customalloc.h>
#include <efd/utf8string.h>

using namespace efd;
//-------------------------------------------------------------------------------------------------
// Static definitions
//-------------------------------------------------------------------------------------------------
#if defined(linux)
template<> const efd::utf8string::size_type efd::utf8string::npos = efd::utf8string::internal_string::npos;
#endif
//template<> const efd::basic_utf8string<efd::CustomAllocator<char> >::size_type efd::basic_utf8string<efd::CustomAllocator<char> >::npos;

//const efd::basic_utf8string< efd::NiCustomAllocator<char> > efd::basic_utf8string< efd::NiCustomAllocator<char> >::NullString;
//const efd::basic_utf8string< efd::NiCustomAllocator<char> > efd::basic_utf8string< efd::NiCustomAllocator<char> >::WhiteSpace( " \t\n\r" );

