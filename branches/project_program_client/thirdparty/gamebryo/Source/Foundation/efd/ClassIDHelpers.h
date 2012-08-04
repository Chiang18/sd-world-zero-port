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
#ifndef EE_CLASSIDHELPERS_INCLUDE
#define EE_CLASSIDHELPERS_INCLUDE

#include <efd/efdPrimitiveTypeIDs.h>
#include <efd/utf8string.h>

namespace efd
{
    /**
        A series of overloaded functions that can return a ClassID for
        a primitive type, or a type derived from IBase.
    */

/// @name ClassID definitions and functions from ClassIDHelpers.h
//@{
    const efd::ClassID CLASS_ID_BOOL        = efd::kTypeID_Bool;
    const efd::ClassID CLASS_ID_CHAR        = efd::kTypeID_Char;
    const efd::ClassID CLASS_ID_SINT8       = efd::kTypeID_SInt8;
    const efd::ClassID CLASS_ID_UINT8       = efd::kTypeID_UInt8;
    const efd::ClassID CLASS_ID_SINT16      = efd::kTypeID_SInt16;
    const efd::ClassID CLASS_ID_UINT16      = efd::kTypeID_UInt16;
    const efd::ClassID CLASS_ID_SINT32      = efd::kTypeID_SInt32;
    const efd::ClassID CLASS_ID_UINT32      = efd::kTypeID_UInt32;
    const efd::ClassID CLASS_ID_SINT64      = efd::kTypeID_SInt64;
    const efd::ClassID CLASS_ID_UINT64      = efd::kTypeID_UInt64;
    const efd::ClassID CLASS_ID_FLOAT32     = efd::kTypeID_Float32;
    const efd::ClassID CLASS_ID_FLOAT64     = efd::kTypeID_Float64;
    const efd::ClassID CLASS_ID_UTF8STRING  = efd::kTypeID_utf8string;
    const efd::ClassID CLASS_ID_POINT2      = efd::kTypeID_Point2;
    const efd::ClassID CLASS_ID_POINT3      = efd::kTypeID_Point3;
    const efd::ClassID CLASS_ID_POINT4      = efd::kTypeID_Point4;
    const efd::ClassID CLASS_ID_MATRIX3     = efd::kTypeID_Matrix3;
    const efd::ClassID CLASS_ID_COLOR       = efd::kTypeID_Color;
    const efd::ClassID CLASS_ID_COLORA      = efd::kTypeID_ColorA;
    const efd::ClassID CLASS_ID_QUATERNION  = efd::kTypeID_Quaternion;
    const efd::ClassID CLASS_ID_ASSETID     = efd::kTypeID_AssetID;


    /// IBase requires some special handling in order to implement the ClassID query methods that
    /// we use in the templated Property methods:
    inline efd::ClassID GetClassIDFromObject(const efd::IBase& var)
    {
        return var.GetClassDesc()->GetClassID();
    }
    inline efd::ClassID GetClassIDFromObject(const efd::IBase* const var)
    {
        return var->GetClassDesc()->GetClassID();
    }


    /// template function to return CLASS_ID for a type T.  The default implementation assumes that
    /// the type T derives from IBase.
    template<typename T>
    inline efd::ClassID GetClassIDFromType()
    {
        return T::CLASS_ID;
    }


    /// All the plain old data types and most simple non-IBase classes can simply use this macro
    /// to define the required ClassID mapping functions:
    #define EE_SPECIALIZE_TYPE( TYPE, CID )                                                     \
    inline efd::ClassID GetClassIDFromObject( const TYPE& )                                     \
    {                                                                                           \
        return CID;                                                                             \
    }                                                                                           \
    template<>                                                                                  \
    inline efd::ClassID GetClassIDFromType< TYPE >()                                            \
    {                                                                                           \
        return CID;                                                                             \
    }

    /// ClassID mapping for efd::Bool
    EE_SPECIALIZE_TYPE( efd::Bool, CLASS_ID_BOOL );
    /// ClassID mapping for efd:SInt8
    EE_SPECIALIZE_TYPE( efd::SInt8, CLASS_ID_SINT8 );
    /// ClassID mapping for efd:UInt8
    EE_SPECIALIZE_TYPE( efd::UInt8, CLASS_ID_UINT8 );
    /// ClassID mapping for efd::SInt16
    EE_SPECIALIZE_TYPE( efd::SInt16, CLASS_ID_SINT16 );
    /// ClassID mapping for efd::UInt16
    EE_SPECIALIZE_TYPE( efd::UInt16, CLASS_ID_UINT16 );
    /// ClassID mapping for efd::SInt32
    EE_SPECIALIZE_TYPE( efd::SInt32, CLASS_ID_SINT32 );
    /// ClassID mapping for efd::UInt32
    EE_SPECIALIZE_TYPE( efd::UInt32, CLASS_ID_UINT32 );
    /// ClassID mapping for efd::SInt64
    EE_SPECIALIZE_TYPE( efd::SInt64, CLASS_ID_SINT64 );
    /// ClassID mapping for efd::UInt64
    EE_SPECIALIZE_TYPE( efd::UInt64, CLASS_ID_UINT64 );
    /// ClassID mapping for efd::Float32
    EE_SPECIALIZE_TYPE( efd::Float32, CLASS_ID_FLOAT32 );
    /// ClassID mapping for efd::Float64
    EE_SPECIALIZE_TYPE( efd::Float64, CLASS_ID_FLOAT64 );
    /// ClassID mapping for efd::utf8string
    EE_SPECIALIZE_TYPE( efd::utf8string, CLASS_ID_UTF8STRING );
    /// ClassID mapping for efd::utf8char_t
    EE_SPECIALIZE_TYPE( efd::utf8char_t, CLASS_ID_CHAR );

    // Forward declares:
    class Point2;
    class Point3;
    class Point4;
    class Matrix3;
    class Color;
    class ColorA;
    class Quaternion;
    class AssetID;

    /// ClassID mapping for efd::Point2
    EE_SPECIALIZE_TYPE( efd::Point2, CLASS_ID_POINT2 );
    /// ClassID mapping for efd::Point3
    EE_SPECIALIZE_TYPE( efd::Point3, CLASS_ID_POINT3 );
    /// ClassID mapping for efd::Point4
    EE_SPECIALIZE_TYPE( efd::Point4, CLASS_ID_POINT4 );
    /// ClassID mapping for efd::Matrix3
    EE_SPECIALIZE_TYPE( efd::Matrix3, CLASS_ID_MATRIX3 );
    /// ClassID mapping for efd::Color
    EE_SPECIALIZE_TYPE( efd::Color, CLASS_ID_COLOR );
    /// ClassID mapping for efd::ColorA
    EE_SPECIALIZE_TYPE( efd::ColorA, CLASS_ID_COLORA );
    /// ClassID mapping for efd::Quaternion
    EE_SPECIALIZE_TYPE( efd::Quaternion, CLASS_ID_QUATERNION );
    /// ClassID mapping for efd::AssetID
    EE_SPECIALIZE_TYPE( efd::AssetID, CLASS_ID_ASSETID );

//@}

} // end namespace efd


#endif

