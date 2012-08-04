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
#ifndef EE_ISTREAMABLE_H
#define EE_ISTREAMABLE_H

#include <efd/efdLibType.h>
#include <efd/IBase.h>
#include <efd/efdClassIDs.h>

namespace efd
{

class DataStream;  // forward declare so compiler can see DataStream in To/FromStream methods

/** 
    The interface class for all objects that support streaming.
*/
class EE_EFD_ENTRY IStreamable : public efd::IBase
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_INTERFACE1(IStreamable, efd::kCLASSID_IStreamable, efd::IBase)
    /// @endcond

public:

    /**
        Write this class into a stream of atomic types.  If the class does not contains any data,
        such as a pure event class, then is it recommended and safe to add nothing to the stream.
     
        @param strm The stream to write the class into.
    */
    virtual void ToStream( efd::DataStream &strm ) const=0;

    /**
        Read this class from the stream of atomic types and recreate the class.  If the class does 
        not contains any data, such as a pure event class, then is it recommended and safe to 
        have an empty stream.  In that case this method should simple return true.
     
        @param strm The stream to read the class from.
     
        @return true if successful, false otherwise. 
    */
    virtual bool FromStream( const efd::DataStream &strm )=0;
};

///A smart pointer (reference counting, self deleting pointer) for the IStreamable class.
typedef efd::SmartPointer<IStreamable> IStreamablePtr;

} // end namespace efd

#endif // EE_ISTREAMABLE_H
