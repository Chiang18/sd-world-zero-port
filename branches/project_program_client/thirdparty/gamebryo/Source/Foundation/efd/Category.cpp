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

#include <efd/Category.h>
#include <efd/DataStream.h>

//-------------------------------------------------------------------------------------------------
namespace efd
{

//-------------------------------------------------------------------------------------------------
efd::DataStream& operator<<(efd::DataStream& i_strm, const CategoryMask& i_val)
{
    i_strm << i_val.mask;
    return i_strm;
}

//-------------------------------------------------------------------------------------------------
efd::DataStream& operator>>(efd::DataStream& i_strm, CategoryMask& o_val)
{
    i_strm >> o_val.mask;
    return i_strm;
}

//-------------------------------------------------------------------------------------------------
void DataStreamSkip(const efd::DataStream& i_strm, const CategoryMask& i_val)
{
    i_strm.Skip(i_val.mask);
}

//-------------------------------------------------------------------------------------------------
efd::DataStream& operator<<(efd::DataStream& i_strm, const Category& i_val)
{
    i_strm << i_val.GetValue();
    return i_strm;
}

//-------------------------------------------------------------------------------------------------
const efd::DataStream& operator>>(const efd::DataStream& i_strm, Category& o_val)
{
    efd::UInt64 temp;
    i_strm >> temp;

    o_val = Category(temp);

    return i_strm;
}

//-------------------------------------------------------------------------------------------------
void DataStreamSkip(const efd::DataStream& i_strm, const Category& i_val)
{
    efd::UInt64 temp;
    i_strm.Skip(temp);

    EE_UNUSED_ARG(i_val);
}

} // namespace efd
