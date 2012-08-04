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
#ifndef EE_UTF8STRING_H
#define EE_UTF8STRING_H

#include <efd/basic_utf8string.h>
#include <efd/utf8char.h>

// include customalloc outside of namespace
#include <efd/customalloc.h>
 
namespace efd
{

/// utf8string is a basic_utf8string using our Emergent custom allocator.
typedef details::basic_utf8string< efd::CustomAllocator<char> > utf8string;

/// @name Standard Operators from utf8string.h

//@{



//-------------------------------------------------------------------------------------------------
// operator==()
//-------------------------------------------------------------------------------------------------
inline bool operator==(
                       const efd::utf8string& lhs_utf8,
                       const efd::utf8string& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) == 0;
}

//-------------------------------------------------------------------------------------------------
// operator==()
//-------------------------------------------------------------------------------------------------
inline bool operator==(
                       const efd::utf8string& lhs_utf8,
                       const char* rhs_c_str)
{
    return (rhs_c_str != NULL) && (lhs_utf8.compare(rhs_c_str) == 0);
}


//-------------------------------------------------------------------------------------------------
// operator==()
//-------------------------------------------------------------------------------------------------
inline bool operator==(const char* lhs_c_str, const efd::utf8string& rhs_utf8)
{
    return (lhs_c_str != NULL) && (rhs_utf8.compare(lhs_c_str) == 0);
}

//-------------------------------------------------------------------------------------------------
// operator!=()
//-------------------------------------------------------------------------------------------------
inline bool operator!=( const efd::utf8string& lhs_utf8,
                        const efd::utf8string& rhs_utf8 )
{
    return lhs_utf8.compare(rhs_utf8) != 0;
}

//-------------------------------------------------------------------------------------------------
// operator!=()
//-------------------------------------------------------------------------------------------------
inline bool operator!=( const efd::utf8string& lhs_utf8,
                        const char* rhs_c_str)
{
    return (rhs_c_str == NULL) || (lhs_utf8.compare(rhs_c_str) != 0);
}

//-------------------------------------------------------------------------------------------------
// operator!=()
//-------------------------------------------------------------------------------------------------
inline bool operator!=(const char* lhs_c_str, const efd::utf8string& rhs_utf8)
{
    return (lhs_c_str == NULL) || (rhs_utf8.compare(lhs_c_str) != 0);
}

//-------------------------------------------------------------------------------------------------
// operator<()
//-------------------------------------------------------------------------------------------------
inline bool operator<(
                      const efd::utf8string& lhs_utf8,
                      const efd::utf8string& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) < 0;
}

//-------------------------------------------------------------------------------------------------
// operator<()
//-------------------------------------------------------------------------------------------------
inline bool operator<( const efd::utf8string& lhs_utf8,
                       const char* rhs_c_str )
{
    return lhs_utf8.compare(rhs_c_str) < 0;
}

//-------------------------------------------------------------------------------------------------
// operator<()
//-------------------------------------------------------------------------------------------------
inline bool operator<( const char* lhs_c_str,
                       const efd::utf8string& rhs_utf8 )
{
    return rhs_utf8.compare(lhs_c_str) >= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator<=( const efd::utf8string& lhs_utf8,
                        const efd::utf8string& rhs_utf8 )
{
    return lhs_utf8.compare(rhs_utf8) <= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator<=( const efd::utf8string& lhs_utf8,
                        const char* rhs_c_str )
{
    return lhs_utf8.compare(rhs_c_str) <= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator<=( const char* rhs_c_str,
                        const efd::utf8string& lhs_utf8 )
{
    return lhs_utf8.compare(rhs_c_str) > 0;
}

//-------------------------------------------------------------------------------------------------
// operator>()
//-------------------------------------------------------------------------------------------------
inline bool operator>(
                      const efd::utf8string& lhs_utf8,
                      const efd::utf8string& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) > 0;
}

//-------------------------------------------------------------------------------------------------
// operator>()
//-------------------------------------------------------------------------------------------------
inline bool operator>( const efd::utf8string& lhs_utf8,
                       const char* rhs_c_str )
{
    return lhs_utf8.compare(rhs_c_str) > 0;
}

//-------------------------------------------------------------------------------------------------
// operator>()
//-------------------------------------------------------------------------------------------------
inline bool operator>( const char* lhs_c_str,
                       const efd::utf8string& rhs_utf8 )
{
    return rhs_utf8.compare(lhs_c_str) <= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator>=( const efd::utf8string& lhs_utf8,
                        const efd::utf8string& rhs_utf8 )
{
    return lhs_utf8.compare(rhs_utf8) >= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator>=(
                       const efd::utf8string& lhs_utf8,
                       const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) >= 0;
}

//-------------------------------------------------------------------------------------------------
// operator<=()
//-------------------------------------------------------------------------------------------------
inline bool operator>=( const char* lhs_c_str,
                        const efd::utf8string& rhs_utf8 )
{
    return rhs_utf8.compare(lhs_c_str) < 0;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                            const efd::utf8string& lhs_utf8,
                            const efd::utf8string& rhs_utf8)
{
    efd::utf8string temp(lhs_utf8);
    temp.append(rhs_utf8);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                            const efd::utf8string& lhs_utf8,
                            const char* rhs_c_str)
{
    efd::utf8string temp(lhs_utf8);
    temp.append(rhs_c_str);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                            const char* lhs_c_str,
                            const efd::utf8string& rhs_utf8)
{
    efd::utf8string temp(lhs_c_str);
    temp.append(rhs_utf8);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                            const efd::utf8string& lhs_utf8,
                            const efd::utf8char_t& rhs_utf8)
{
    efd::utf8string temp(lhs_utf8);
    temp.push_back(rhs_utf8);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                            const efd::utf8char_t& lhs_utf8,
                            const efd::utf8string& rhs_utf8)
{
    efd::utf8string temp;
    temp.push_back(lhs_utf8);
    temp.append(rhs_utf8);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator+()
//-------------------------------------------------------------------------------------------------
inline efd::utf8string operator+(
                                 const efd::utf8char_t& lhs_utf8,
                                 const char* rhs_utf8)
{
    efd::utf8string temp;
    temp.push_back(lhs_utf8);
    temp.append(rhs_utf8);
    return temp;
}

//-------------------------------------------------------------------------------------------------
// operator==()
//-------------------------------------------------------------------------------------------------
template <typename IT, typename IU>
inline bool operator==(
                       const efd::utf8string_iterator<efd::utf8string,IT>& lhs,
                       const efd::utf8string_iterator<efd::utf8string,IU>& rhs)
{
    return (lhs.base() == rhs.base());
}

//-------------------------------------------------------------------------------------------------
// operator!=()
//-------------------------------------------------------------------------------------------------
template <typename IT, typename IU>
inline bool operator!=(
                       const efd::utf8string_iterator<efd::utf8string,IT>& lhs,
                       const efd::utf8string_iterator<efd::utf8string,IU>& rhs)
{
    return (lhs.base() != rhs.base());
}

//-------------------------------------------------------------------------------------------------
// operator<()
//-------------------------------------------------------------------------------------------------
//template <typename IT>
inline bool operator<(
                      const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& lhs,
                      const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& rhs)
{
    return (lhs.base() < rhs.base());
}

//-------------------------------------------------------------------------------------------------
//  operator>()
//-------------------------------------------------------------------------------------------------
//template <typename IT>
inline bool operator>(
                      const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& lhs,
                      const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& rhs)
{
    return (lhs.base() > rhs.base());
}

//-------------------------------------------------------------------------------------------------
//  operator<=()
//-------------------------------------------------------------------------------------------------
//template <typename IT>
inline bool operator<=(
                       const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& lhs,
                       const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& rhs)
{
    return (lhs.base() <= rhs.base());
}

//-------------------------------------------------------------------------------------------------
//  operator>=()
//-------------------------------------------------------------------------------------------------
//template <typename IT>
inline bool operator>=(
                       const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& lhs,
                       const efd::utf8string_iterator<const efd::utf8string,const efd::utf8string::iterator>& rhs)
{
    return (lhs.base() >= rhs.base());
}

//@}

} // end namespace efd

#endif //EE_UTF8STRING_H
