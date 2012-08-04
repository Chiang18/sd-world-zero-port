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
#ifndef EE_POINT4_H
#define EE_POINT4_H

#include <efd/Point3.h>
#include <efd/OS.h>

#ifdef __SPU__
#include <vectormath/cpp/vectormath_aos.h>
#endif

namespace efd
{

#if defined (EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
__declspec(align(16))
#elif defined (EE_PLATFORM_PS3) || defined(EE_PLATFORM_WII) || defined(EE_PLATFORM_LINUX)
#else 
    #error unknown platform
#endif

/**
    This class encapsulates a 4 value point with coordinates x,y,z,w.
    The interface is generally cross platform while the implementation
    is platform specific.
*/
class EE_EFD_ENTRY Point4
{
public:
    /// Constructor
    inline Point4();
    /// Constructor
    inline Point4(float fX, float fY, float fZ, float fW);
    /// Constructor
    inline Point4(const efd::Point3& kPt, float fW = 1.0f);
    /// Constructor
    inline Point4(const efd::Point4& kPt);

    /// Conversion from Point4 to Point3. Simply drops the w component.
    inline operator Point3();

#if defined (EE_PLATFORM_WIN32) || defined(EE_PLATFORM_WII) || defined(EE_PLATFORM_LINUX)
#elif defined(EE_PLATFORM_XBOX360)
    /// Constructor
    inline Point4(XMVECTOR vPt);

    // type conversion operators
    inline operator const XMVECTOR () const { return m_vfPt; }

    static void UnitizeVectors(
        Point4* p4In, 
        unsigned int uiCount,
        efd::Point3* p3Out);
#elif defined (EE_PLATFORM_PS3)
    static void PointsPlusEqualFloatTimesPoints(Point4* pkDst, float f,
        const Point4* pkSrc, unsigned int uiVerts);
#ifdef __SPU__
    /// Constructor
    inline Point4(vec_float4 vPt);
#else
    /// Constructor
    inline Point4(__vector float vPt);
#endif
#else 
    #error unknown platform
#endif

    /// Zeros the fields of the Point4
    inline void MakeZero();

    /// Set the Point4 values
    inline void Set(float fX, float fY, float fZ, float fW);
    /// Set the Point4 values
    inline void Set(const efd::Point3& kPt, float fW);

    /// Set3VectorW0 functions set x, y, and z, with w set to 0.
    inline void Set3VectorW0(const Point3& kPt);
    /// Set3VectorW0 functions set x, y, and z, with w set to 0.
    inline void Set3VectorW0(float fX, float fY, float fZ);

    // element access
    float operator[](int i) const;
    float Set(unsigned int uiComponent, float fValue);

    inline float X() const;
    inline float Y() const;
    inline float Z() const;
    inline float W() const;
    inline void SetX(float fX);
    inline void SetY(float fY);
    inline void SetZ(float fZ);
    inline void SetW(float fW);

    inline Point4 operator- () const;
    inline Point4 operator+ (const Point4& kPt) const;
    inline Point4 operator- (const Point4& kPt) const;
    inline Point4 operator* (float fScalar) const;
    inline Point4 operator/ (float fScalar) const;
    inline Point4 operator+= (const Point4& kPt);
    inline Point4 operator-= (const Point4& kPt);
    inline Point4 operator*= (float fScalar);
    inline Point4 operator/= (float fScalar);

    inline bool operator==(const Point4& kPt) const;
    inline bool operator!=(const Point4& kPt) const;

    /// Assignment operator
    inline Point4& operator=(const Point4& kPt);

    /*
        Given a normalized efd::Point3, scales the X and Y so that Z equals 1.
        Sets our X = point.X, Y = point.Y
    */
    inline void CompressNormalizedIntoXY(const efd::Point3& point);

    /*
        Given a normalized efd::Point3, scales the X and Y so that Z equals 1.
        Sets our Z = point.X, W = point.Y
    */
    inline void CompressNormalizedIntoZW(const efd::Point3& point);

    inline void DecompressXYIntoNormalized(efd::Point3& point);

    inline void DecompressZWIntoNormalized(efd::Point3& point);

    inline void DecompressHighDetailIntoNormalized(efd::Point3& point);

    static const Point4 ZERO;


    union
    {
        // Use of these members is not portable, 
        // only use them in platform specific code.
#if defined (EE_PLATFORM_WIN32) || defined(EE_PLATFORM_WII) || defined(EE_PLATFORM_LINUX)
#elif defined(EE_PLATFORM_XBOX360)
        XMVECTOR m_vfPt;
#elif defined (EE_PLATFORM_PS3)
#   ifdef __SPU__
        vec_float4 m_vfPt;
#   else
        __vector float m_vfPt;
#   endif
#else 
#error unknown platform
#endif

        // *** begin Emergent internal use only ***
    
        // Use of this member is strongly discouraged. 
        // Future platforms may not support this union, and its use may be 
        // less efficient then API above.
        float m_afPt[4];

        // *** end Emergent internal use only ***
    };
};

} // end namespace efd

#if defined (EE_PLATFORM_WIN32)
#include <efd/Win32/Point4_Win32.inl>
#elif defined (EE_PLATFORM_XBOX360)
#include <efd/XBox360/Point4_XBox360.inl>
#elif defined (EE_PLATFORM_PS3)
#include <efd/PS3/Point4_PS3.inl>
#elif defined (EE_PLATFORM_LINUX)
#include <efd/Linux/Point4_Linux.inl>
#elif defined (EE_PLATFORM_WII)
#include <efd/Wii/Point4_Wii.inl>
#endif

#include <efd/Point4.inl>

#endif // EE_POINT4_H
