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
#ifndef EE_BASIC_UTF8STRING_H
#define EE_BASIC_UTF8STRING_H

#include <efd/String.h>
#include <efd/utf8string_iterator.h>
#include <efd/EEBasicString.h>      // defines efd::basic_string

namespace efd
{

/**
   When describing the "size" of a UTF string there are several ways to describe it:
   \li CT_LENGTH : equivalent to the return value from the length() function.  This is the number
        of characters in the string, but a single character can be multiple units long
   \li CT_SIZE : equivalent to the return value from the size() function.  This is the number of
        units that are required to store the string.  For utf8 a unit is one byte, for utf16
        a unit is 16 bytes.
   \li CT_BYTES : this is the size in bytes.  Similar to CT_SIZE, but different UTF encodings use 
        different unit sizes.  For UTF8 CT_BYTES and CT_SIZE are equivalent.  For UTF16 Bytes 
        should always be equivalent to CT_SIZE*2.
*/
enum _CountType { CT_LENGTH, CT_SIZE, CT_BYTES };


// basic_utf8string is not meant to be used directly, the utf8string typedef should instead be used
namespace details
{

/**
    A implementation of a std::string container that is UTF-8 aware.  The efd::utf8string type is
    simply a typedef for basic_utf8string with the default template parameters.  In general you
    should never directly use basic_utf8string but should use efd::utf8string directly instead.
    The efd::utf8string class is the default string used in Foundation and Game Framework.
 */
template<typename T = efd::CustomAllocator<char> >
class basic_utf8string
{
public:
    typedef efd::basic_string< char, efd::char_traits<char>, T > internal_string;
    typedef typename internal_string::size_type               size_type;
    typedef typename internal_string::difference_type         difference_type;
    typedef utf8char_t                                        value_type;
    typedef utf8char_t&                                       reference;
    typedef const utf8char_t&                                 const_reference;
    typedef utf8string_iterator<internal_string, typename internal_string::iterator>      iterator;
    typedef utf8string_iterator<internal_string, typename internal_string::const_iterator>  const_iterator;
    typedef EE_STL_NAMESPACE::reverse_iterator<iterator>          reverse_iterator;
    typedef EE_STL_NAMESPACE::reverse_iterator<const_iterator>    const_reverse_iterator;

#if defined(WIN32) || defined(EE_PLATFORM_XBOX360)
    static const size_type npos = static_cast<size_type>(internal_string::npos);
#else
    static const size_type npos;
#endif

    /// @name Construction and Destruction
    //@{

    /// Default ctor
    basic_utf8string();

    //1 dtor
    ~basic_utf8string();

    /// Construct a basic_utf8string<T> as a partial copy of a C string.
    /// @note This does NOT convert the string from the local codepage, it assumes the string
    /// already contains UTF8 formatted data!
    basic_utf8string(const char* src, _CountType ct, size_type count);

private:
    // Note: The following is an invalid constructor, it exists only so that:
    //      basic_utf8string( "char string", 4 );
    // will generate a compiler error instead of auto-promoting the char string to a basic
    // utf8 string and calling the basic_utf8string(const basic_utf8string<T>& src, size_type count_mb)
    // constructor.  It might be invalid to convert since the char string might not be NULL
    // terminated.  Call basic_utf8string(const char* src, _CountType ct, size_type count);
    // instead if you need this behavior.
    basic_utf8string(const char* src, size_type count_mb);
public:

    /// Construct a basic_utf8string<T> as a partial copy of another basic_utf8string<T>.
    basic_utf8string(
        const basic_utf8string<T> & src,
        size_type pos_mb,
        size_type count_mb = npos);

    /// Construct a basic_utf8string<T> as a copy of a C string.
    /// @note This does NOT convert the string from the local codepage, it assumes the string
    /// already contains UTF8 formatted data!
    basic_utf8string(const char* src_sz);

    /// Construct a basic_utf8string<T> as a copy of another basic_utf8string<T> (copy ctor).
    basic_utf8string(const basic_utf8string<T>& src_utf8);

    /// Construct a basic_utf8string<T> as multiple characters.
    basic_utf8string(size_type count_mb, const utf8char_t& uc);

    /// Construct a basic_utf8string<T> using printf-style formating.
    /// @note This does NOT convert strings from the local codepage, it assumes all strings
    /// already contain UTF8 formatted data!  See sprintf for more restrictions.
    basic_utf8string(_Formatted, const char* format_sz, ...);

    //@}

    /// Provide access to the raw string buffer as a C-style string.
    const char* c_str() const;

    /// Provide access to the raw string buffer as a C-style string.
    const char* data() const;

    /// Provide access to the raw string buffer as a C++-style string.
    const internal_string& raw() const;
    const internal_string& base() const;

    /// Create an iterator to the beginning of the string.
    iterator begin();

    /// Create an iterator at the end of the string.
    iterator end();

    /// Create a const iterator to the beginning of the string.
    const_iterator begin() const;

    /// Create a const iterator at the end of the string.
    const_iterator end() const;

    /// Create a reverse iterator at the beginning of the string.
    reverse_iterator rbegin();

    /// Create a reverse iterator at the end of the string.
    reverse_iterator rend();

    /// Create a const reverse iterator at the beginning of the string.
    const_reverse_iterator rbegin() const;

    /// Create a const reverse iterator at the end of the string.
    const_reverse_iterator rend() const;

    /// Read-only random access accessor
    const value_type operator[](size_type pos) const;

    /// Read-only random access accessor
    const value_type at(size_type pos) const;

    /// Returns a sub string of size count_mb that begins at pos_mb. 
    basic_utf8string<T> substr(
        size_type pos_mb = 0,
        size_type count_mb = npos) const;

    /// @name String comparison routines 
    //@{
    /// Compares this character string to the specified string.
    /// @param cmp_utf8 The string to compare against.
    /// @return An integer indicating the relationship between this string and the specified string:
    ///    @li < 0 Means this string is less than the specified string
    ///    @li 0 Means this string is identical to the specified string
    ///    @li > 0 Means this string is greater than the specified string
    int compare(const basic_utf8string<T>& cmp_utf8) const;
    
    /// @overload
    /// @param cmp_sz The string to compare against.
    int compare(const char* cmp_sz) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_utf8 The string to compare against.
    int compare(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& cmp_utf8) const;
 
    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_utf8 The string to compare against.
    /// @param pos2_mb The first character in the specified string at which the comparison starts.
    /// @param count2_mb The number of characters to consider in the specified string.
    int compare(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& cmp_utf8,
        size_type pos2_mb,
        size_type count2_mb) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_sz The string to compare against.
    /// @param count2_mb The number of characters to consider in the specified string.
    int compare(
        size_type pos_mb,
        size_type count_mb,
        const char* cmp_sz,
        size_type count2_mb) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_sz The string to compare against.
    int compare(
        size_type pos_mb,
        size_type count_mb,
        const char* cmp_sz) const;
    //@}

    /// @name Find routines
    //@{
    /// Finds the position of the specified string or character within this string.
    /// @param str_utf8 The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    /// @return The position of the specified string within this string.
    size_type find(
        const basic_utf8string<T>& str_utf8,
        size_type pos_mb = 0) const;

    /// @overload
    /// @param str_sz The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    /// @param count_mb The number of characters to consider in this string. 
    size_type find(
        const char* str_sz,
        size_type pos_mb,
        size_type count_mb) const;

    /// @overload
    /// @param str_sz The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    size_type find(
        const char* str_sz,
        size_type pos_mb = 0) const;

    /// @overload
    /// @param uc The character to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    size_type find(
        const utf8char_t& uc,
        size_type pos_mb = 0) const;

    /// Finds the position of the specified string or character within this string starting from 
    /// the end of the string.
    /// @param str_utf8 The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    /// @return The position of the specified string within this string.
    size_type rfind(
        const basic_utf8string<T>& str_utf8,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param str_sz The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    /// @param count_mb The number of characters to consider in this string. 
    size_type rfind(
        const char* str_sz,
        size_type pos_mb,
        size_type count_mb) const;

    /// @overload
    /// @param str_sz The string to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    size_type rfind(
        const char* str_sz,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param uc The character to find.
    /// @param pos_mb The position in this string to start looking for the specified string. 
    size_type rfind(
        const utf8char_t& uc,
        size_type pos_mb = npos) const;

    /// Finds the first occurrence of a specified character within this string.
    /// @param match_utf8 The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    /// @return The position of the character found within this string.
    typename basic_utf8string<T>::size_type find_first_of(
        const basic_utf8string<T>& match_utf8,
        typename basic_utf8string<T>::size_type pos_mb = 0) const;
   
    /// @overload
    /// @param match_sz The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    /// @param count_mb The number of characters to consider starting from the beginning of the 
    ///     specified string. 
    size_type find_first_of(
        const char* match_sz,
        size_type pos_mb,
        size_type count_mb) const;

    /// @overload
    /// @param match_sz The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    size_type find_first_of(
        const char* match_sz,
        size_type pos_mb = 0) const;

    /// @overload
    /// @param uc The character to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    size_type find_first_of(
        const utf8char_t& uc,
        size_type pos_mb = 0) const;

    /// Finds the last occurrence of a specified character within this string.
    /// @param match_utf8 The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    /// @return The position of the character found within this string.
    size_type find_last_of(
        const basic_utf8string<T>& match_utf8,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param match_sz The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    /// @param count_mb The number of characters to consider starting from the beginning of the 
    ///     specified string. 
    size_type find_last_of(
        const char* match_sz,
        size_type pos_mb,
        size_type count_mb) const;

    /// @overload 
    /// @param match_sz The string containing characters to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    size_type find_last_of(
        const char* match_sz,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param uc The character to find.
    /// @param pos_mb The position in this string to start looking for the specified character. 
    size_type find_last_of(
        const utf8char_t& uc,
        size_type pos_mb = npos) const;

    /// Finds the first occurrence of a character within this string that is \b not the specified 
    ///     character.
    /// @param match_utf8 The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    /// @return The position of the character found within this string.
    size_type find_first_not_of(
        const basic_utf8string<T>& match_utf8,
        size_type pos_mb = 0) const;

    /// @overload
    /// @param match_sz The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    /// @param count_mb The number of characters to consider starting from the beginning of the 
    ///     specified string. 
    size_type find_first_not_of(
        const char* match_sz,
        size_type pos_mb,
        size_type count_mb) const;

    /// @overload
    /// @param match_sz The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    size_type find_first_not_of(
        const char* match_sz,
        size_type pos_mb = 0) const;

    /// @overload
    /// @param uc The character to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    size_type find_first_not_of(
        const utf8char_t& uc,
        size_type pos_mb = 0) const;

    /// Finds the last occurrence of a character within this string that is \b not the specified 
    ///     character.
    /// @param match_utf8 The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    /// @return The position of the character found within this string.
    size_type find_last_not_of(
        const basic_utf8string<T>& match_utf8,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param match_sz The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    /// @param count_mb The number of characters to consider starting from the beginning of the 
    ///     specified string. 
    size_type find_last_not_of(
        const char* match_sz,
        size_type pos_mb,
        size_type count_mb) const;
    
    /// @overload
    /// @param match_sz The string containing characters to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    size_type find_last_not_of(
        const char* match_sz,
        size_type pos_mb = npos) const;

    /// @overload
    /// @param uc The character to look for.
    /// @param pos_mb The position in this string to start looking for characters that do \b not 
    ///     match the specified character. 
    size_type find_last_not_of(
        const utf8char_t& uc,
        size_type pos_mb = npos) const;
    //@}

    /// @name Assign overloads
    //@{
    /// Assign multiple copies of the given character *this.
    /// @param count_mb The number of copies of the character to assign to the string.
    /// @param uc The character.
    basic_utf8string<T>& assign(size_type count_mb, const utf8char_t& uc);

    /// Assign a string to *this.
    /// @param str_sz The string.
    basic_utf8string<T>& assign(const char* str_sz);

private:
    /// Assign a number of characters from the given string to *this.
    /// @param str_sz The string to assign from.
    /// @param count_mb The number of characters to assign. 
    // @note Don't call this flavor, use the following one instead.
    basic_utf8string<T>& assign(const char* str_sz, size_type count_mb);
public:

    /// Assign a number of characters from the given string to *this.
    /// @param str_sz The string to assign from.
    /// @param ct Specifies how the count parameter is interpreted. 
    /// @param count The number of characters to assign. 
    basic_utf8string<T>& assign(const char* str_sz, _CountType ct, size_type count);

    /// Assign a number of characters from the given string to *this.
    /// @param str_sz The string to assign from.
    /// @param pos_mb The position within the specified string to start assigning from. 
    /// @param count_mb The number of characters to assign. 
    basic_utf8string<T>& assign(
        const char* str_sz,
        size_type pos_mb,
        size_type count_mb);

    /// Assign the given string to *this.
    /// @param str_utf8 The string to assign from.
    basic_utf8string<T>& assign(const basic_utf8string<T>& str_utf8);

    /// Assign a number of characters from the given string to *this.
    /// @param str_utf8 The string to assign from.
    /// @param count_mb The number of characters to assign. 
    basic_utf8string<T>& assign(
        const basic_utf8string<T>& str_utf8,
        size_type count_mb);

    /// Assign a number of characters from the given string to *this.
    /// @param str_utf8 The string to assign from.
    /// @param pos_mb The position within the specified string to start assigning from. 
    /// @param count_mb The number of characters to assign. 
    basic_utf8string<T>& assign(
        const basic_utf8string<T>& str_utf8,
        size_type pos_mb,
        size_type count_mb);
    
    /// Assign a number of characters from the InputIterator to *this.
    /// @param itBegin The starting point to assign from.
    /// @param itEnd The ending point to assign from.
    template< typename InputIterator>
    basic_utf8string<T>& assign(InputIterator itBegin, InputIterator itEnd);
    //@}

    /// assignment operator
    basic_utf8string<T>& operator=(const basic_utf8string<T>& rhs_utf8);

    /// assignment operator
    basic_utf8string<T>& operator=(const internal_string& rhs_str);

    /// assignment operator
    /// @note rhs_sz is assumed to point to valid UTF8 formated data
    basic_utf8string<T>& operator=(const char* rhs_sz);

    /// assignment operator
    basic_utf8string<T>& operator=(const utf8char_t& uc);

    /// concatenation operator
    basic_utf8string<T>& operator+=(const basic_utf8string<T>& rhs_utf8);

    /// concatenation operator
    /// @note rhs_sz is assumed to point to valid UTF8 formated data
    basic_utf8string<T>& operator+=(const char* rhs_sz);

    /// concatenation operator
    basic_utf8string<T>& operator+=(const utf8char_t& uc);

    /// Add the character to this string.
    void push_back(const utf8char_t& uc);

    /// @name Append overloads
    //@{
    /// Append the specified string to this string.
    basic_utf8string<T>& append(const basic_utf8string<T>& src_utf8);

    /// Append the character to this string.
    basic_utf8string<T>& append(const utf8char_t& uc);

    /// Append a number of characters from the given string to *this.
    /// @param src_utf8 The string to append from.
    /// @param pos_mb The position within the specified string to start appending from. 
    /// @param count_mb The number of characters to append. 
    basic_utf8string<T>& append(
        const basic_utf8string<T>& src_utf8,
        size_type pos_mb,
        size_type count_mb);

    /// Append the specified string to this string.
    basic_utf8string<T>& append(const char* src_sz);

    /// Append a number of characters from the given string to *this.
    /// @param src_sz The string to append from.
    /// @param count_mb The number of characters to append. 
    basic_utf8string<T>& append(
        const char* src_sz,
        size_type count_mb);

    /// Append multiple copies of the given character *this.
    /// @param count_mb The number of copies of the character to append to this string.
    /// @param uc The character.
    basic_utf8string<T>& append(
        size_type count_mb,
        const utf8char_t& uc);

    /// Append a number of characters from the InputIterator to *this.
    /// @param itFirst The starting point to append from.
    /// @param itLast The ending point to append from.
    template < typename InputIterator>
    basic_utf8string<T>& append(InputIterator itFirst, InputIterator itLast);
    //@}

    /// @name Insert overloads
    //@{
    /// Inserts a string before the given position in this string.
    /// @param pos_mb The position to insert the given string.
    /// @param src_utf8 The string to insert.
    basic_utf8string<T>& insert(
        size_type pos_mb,
        const basic_utf8string<T>& src_utf8);

    /// Inserts a string before the given position in this string.
    /// @param pos_mb The position to insert the given string.
    /// @param src_sz The string to insert.
    basic_utf8string<T>& insert(
        size_type pos_mb,
        const char* src_sz);

    /// Inserts a substring before the given position in this string.
    /// @param pos_mb The position to insert the given substring.
    /// @param src_utf8 The string containing the substring.
    /// @param pos2_mb The starting position of the substring.
    /// @param count_mb The number of characters in the substring.
    basic_utf8string<T>& insert(
        size_type pos_mb,
        const basic_utf8string<T>& src_utf8,
        size_type pos2_mb,
        size_type count_mb);

    /// Inserts a substring before the given position in this string.
    /// @param pos_mb The position to insert the given substring.
    /// @param src_sz The string containing the substring.
    /// @param count_mb The number of characters in the substring.
    basic_utf8string<T>& insert(
        size_type pos_mb,
        const char* src_sz,
        size_type count_mb);

    /// Inserts multiple copies of the given character before the given position in this string.
    /// @param pos_mb The position to insert the characters.
    /// @param count_mb The number of copies of the character to insert.
    /// @param uc The character.
    basic_utf8string<T>& insert(
        size_type pos_mb,
        size_type count_mb,
        const utf8char_t& uc );

    /// Inserts the given character before the given position in this string.
    /// @param itPos The position to insert the character.
    /// @param uc The character.
    /// @return An iterator pointing to the newly-inserted character.
    iterator insert(
        iterator itPos,
        const utf8char_t& uc);

    /// Inserts multiple copies of the given character before the given position in this string.
    /// @param itPos The position to insert the characters.
    /// @param count_mb The number of copies of the character to insert.
    /// @param uc The character.
    void insert(
        iterator itPos,
        size_type count_mb,
        const utf8char_t& uc);

    /** Inserts the first count_mb characters of src_utf8 at pos_mb.
        @note This flavor of insert does not match any valid std::basic_string::insert function.
            In fact, some STL implementations might provide a default parameter to the:
                insert( size_type P0, const Self& _Str, size_type _Off, size_type _Count = npos )
            flavor, in which case you might expect this flavor to take an offset into the source
            string instead of a count of source characters.  Just be sure you mean to be calling
            this flavor.
            @param pos_mb The position to insert the given substring.
            @param src_utf8 The string containing the substring.
            @param count_mb The number of characters in the substring.
     */
    basic_utf8string<T>& insert(
        size_type pos_mb,
        const basic_utf8string<T>& src_utf8,
        size_type count_mb);

    /// Insert a number of characters from the InputIterator to *this.
    /// @param itPos The position to insert the characters.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    template< typename InputIterator>
    void insert(
        iterator itPos,
        InputIterator itBegin,
        InputIterator itEnd);
    //@}

    /// @name Replace overloads
    //@{
    /// Replaces a substring of *this with the given string.
    /// @param pos_mb The position to start replacing characters with the substring.
    /// @param count_mb The number of characters in the substring.
    /// @param src_utf8 The string containing the substring.
    basic_utf8string<T>& replace(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& src_utf8);

    /// Replaces a substring of *this with a substring of the given string.
    /// @param pos_mb The position to start replacing characters with the substring.
    /// @param count_mb The number of characters in the substring.
    /// @param src_utf8 The string containing the substring.
    /// @param pos2_mb The starting position of the substring from the given string.
    /// @param count2_mb The number of characters in the substring from the given string.
    basic_utf8string<T>& replace(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& src_utf8,
        size_type pos2_mb,
        size_type count2_mb);

    /// Replaces a substring of *this with the given string.
    /// @param pos_mb The position to start replacing characters with the substring.
    /// @param count_mb The number of characters in the substring.
    /// @param src_sz The string containing the substring.
    basic_utf8string<T>& replace(
        size_type pos_mb,
        size_type count_mb,
        const char* src_sz);

    /// Replaces a substring of *this with a substring of the given string.
    /// @param pos_mb The position to start replacing characters with the substring.
    /// @param count_mb The number of characters in the substring.
    /// @param src_utf8 The string containing the substring.
    /// @param count2_mb The number of characters in the substring starting from the 
    ///     beginning of the given string.
    basic_utf8string<T>& replace(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& src_utf8,
        size_type count2_mb);

    /// Replaces a substring of *this with multiple copies of the given character.
    /// @param pos_mb The position to start replacing characters with the substring.
    /// @param count_mb The number of characters in the substring.
    /// @param count2_mb The number of copies of the given character in the replacing substring.
    /// @param uc The character.
    basic_utf8string<T>& replace(
        size_type pos_mb,
        size_type count_mb,
        size_type count2_mb,
        const utf8char_t& uc);

    /// Replace a number of characters in *this with characters from the InputIterator.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    /// @param src_utf8 The string containing characters to replace.
    basic_utf8string<T>& replace(
        iterator itBegin,
        iterator itEnd,
        const basic_utf8string<T>& src_utf8);

    /// Replace a number of characters in *this with characters from the InputIterator.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    /// @param src_sz The string containing characters to replace.
    basic_utf8string<T>& replace(
        iterator itBegin,
        iterator itEnd,
        const char* src_sz);

    /// Replace substring of *this with the specified number of characters from the specified 
    /// string.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    /// @param src_sz The string containing characters to replace.
    /// @param count_mb The number of copies of the given character in the replacing substring.
    basic_utf8string<T>& replace(
        iterator itBegin,
        iterator itEnd,
        const char* src_sz,
        size_type count_mb);

    /// Replace substring of *this with the specified number of the specified character.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    /// @param count_mb The number of copies of the given character in the replacing substring.
    /// @param uc The character.
    basic_utf8string<T>& replace(
        iterator itBegin,
        iterator itEnd,
        size_type count_mb,
        const utf8char_t& uc);

    /// Replace substring of *this with the specified number of characters from the specified 
    /// string.
    /// @note This is a non-standard signature for replace.  It doesn't match any similar
    /// signature from std::basic_string::replace.
    /// @param itBegin The starting point to insert from.
    /// @param itEnd The ending point to insert from.
    /// @param src_utf8 The string containing characters to replace.
    /// @param count_mb The number of copies of the given character in the replacing substring.
    basic_utf8string<T>& replace(
        iterator itBegin,
        iterator itEnd,
        const basic_utf8string<T>& src_utf8,
        size_type count_mb);

    /// Replace substring of *this with characters from the specified range.
    /// @param itDestBegin The starting point to replace.
    /// @param itDestEnd The ending point to replace. 
    /// @param itSrcBegin The starting point to replace from.
    /// @param itSrcEnd The ending point to replace from.
    template< typename InputIterator>
    basic_utf8string<T>& replace(
        iterator itDestBegin,
        iterator itDestEnd,
        InputIterator itSrcBegin,
        InputIterator itSrcEnd);
    //@}

    /// Clears the string.
    void clear();

    /// @name Erase overloads
    //@{
    /// Erases a number of characters starting at the given position.
    /// @param pos_mb The position to start erasing.
    /// @param count_mb The number of characters to erase.
    basic_utf8string<T>& erase(
        size_type pos_mb,
        size_type count_mb = npos);

    /// Erases the entire string.
    basic_utf8string<T>& erase();

    /// Erases a number of characters starting at the given position.
    /// @param itPos The position to start erasing.
    iterator erase(iterator itPos);

    /// Erases a number of characters using iterators.
    /// @param itBegin The position to start erasing.
    /// @param itEnd The position to end erasing.
    iterator erase(
        iterator itBegin,
        iterator itEnd);
    //@}
    
    /// Returns true if the string is empty. 
    bool empty() const;

    /** Returns the number of octets in the string.  Unlike std::string,
        this is not necessarily the same as length(). */
    size_type size() const;

    /** Returns the number of characters in the string.  Unlike std::string,
        this is not necessarily the same as size(). */
    size_type length() const;

    /// Swap strings
    void swap(basic_utf8string<T>& rhs_utf8);

    /** Request that the string's capacity be changed.  The postcondition
        for this is, once called, capacity() >= count_sb.  
        @param count_sb The number of bytes of capacity requested.
        @note The argument for this call is in single bytes, not characters. */
    void reserve(size_type count_sb = 0);

    /** 
        Appends characters, or erases characters from the end, as necessary to make the string's 
            size exactly count_sb bytes.
        @param count_sb The number of bytes of capacity requested.
        @note The argument for this call is in single bytes, not characters. If,after resizing, 
            the trailing utf8 character has been severed, it will be removed entirely, such that 
            the final utf8 character of the string remains valid. 
    */
    void resize(size_type count_sb = 0);

    /// @overload
    /// @param count_sb The number of bytes of capacity requested.
    /// @param uc The character used to fill additional string space. 
    void resize(size_type count_sb, const utf8char_t& uc);

    /// Returns the size, in bytes, to which the string can grow before memory must be reallocated.
    size_type capacity() const;

    /// Returns the largest possible size of the string in bytes. 
    size_type max_size() const;

    /// Returns true if all chars in string are ASCII. 
    bool is_ascii() const;

    /// @name C Run-Time style formating routines
    //@{
    /**      
        @note Format strings are compiler specific, you must take care to pass in a valid
            string especially if you support multiple compilers.  For example 64bit integer
            tags vary between compiles ("%I64d" on MSVC verse "%lld" on gnu).
        @note Do NOT attempt to convert wide or narrow strings via formatting.  The result
            will not be a valid UTF8 string.  Do not use %ls, %ws, %S, %lc, or %C tags.  All
            char* strings passed in are assumed to already point to valid UTF8 formatted data.
        @note You cannot use utf8char as an argument to these format routines.  Instead pass
            in the result of utf8char::c_str() matched to a "%s" tag.
     */

    ///@param format The printf-style format string.
    ///@return The number of bytes (not characters) written.
    int sprintf( const char* format, ... );

    ///@param format The printf-style format string.
    ///@return The number of bytes (not characters) written.
    int sprintf( const basic_utf8string<T>& format, ... );

    ///@param format The printf-style format string.
    ///@param args Pointer to list of arguments.
    ///@return The number of bytes (not characters) written.
    int vsprintf( const char* format, va_list args );

    ///@param format The printf-style format string.
    ///@param args Pointer to list of arguments.
    ///@return The number of bytes (not characters) written.
    int vsprintf( const basic_utf8string<T>& format, va_list args );

    ///@param format The printf-style format string.
    ///@return The number of bytes (not characters) written.
    int sprintf_append( const char* format, ... );

    ///@param format The printf-style format string.
    ///@return The number of bytes (not characters) written.
    int sprintf_append( const basic_utf8string<T>& format, ... );

    ///@param format The printf-style format string.
    ///@param args Pointer to list of arguments.
    ///@return The number of bytes (not characters) written.
    int vsprintf_append( const char* format, va_list args );

    ///@param format The printf-style format string.
    ///@param args Pointer to list of arguments.
    ///@return The number of bytes (not characters) written.
    int vsprintf_append( const basic_utf8string<T>& format, va_list args );
    //@}

    /**
        Remove characters from the beginning and/or end of the string.

        @param t Either TrimFront, TrimBack, or TrimAll.
        @param set Characters to trim.  Defaults to set of standard whitespace characters
     */
    void trim( _Trim t = TrimAll, const basic_utf8string<T>& set = basic_utf8string<T>::WhiteSpace() );

    /**
        Remove characters from the beginning and/or end of the string.  Set must contain only
        single byte characters, to handle multi-byte use the other version.

        @param t Either TrimFront, TrimBack, or TrimAll.
        @param set Characters to trim.  Defaults to set of standard whitespace characters
     */
    void trim( _Trim t, const char* set );

    /// These are only placeholder functions, they do not behave correctly with respect to locale.
    /// Currently they will only effect ASCII characters according to English rules.
    void toupper();
    void tolower();

    /**
       Replace all occurrences of a substr with another string.  This will perform a single pass
       over the string from front to back replacing only those occurrences in the original string.
     
       Examples:
           "fooo".replace_substr( "oo", "o" ) => "foo"
           "foo".replace_substr( "o", "oo" ) => "foooo"
           "flolol".replace_substr( "lol", "l" ) => "flol"
      
      @return The number of replacements that were made.
     */
    int replace_substr(const char* find_sz, const char* replace_sz );
    int replace_substr(const basic_utf8string<T>& find_str, const basic_utf8string<T>& replace_str );

    /**
       Overlapping Replace Substring
       This will replace all occurrences of the find string with the replace string including any
       occurrences which are the result of a previous substitution.
       @note If the replacement string contains the find sting this function will return -1 and
       make no modifications to the string.
       Examples:
            "foooooo".ol_replace_substr( "oo", "o" ) => "fo"
            "flolol".replace_substr( "lol", "l" ) => "fl"
      @return The number of replacements that were made, or -1 if the arguments are invalid.
     */
    int ol_replace_substr(const char* find_sz, const char* replace_sz );
    int ol_replace_substr(const basic_utf8string<T>& find_str, const basic_utf8string<T>& replace_str );

    /// @name String comparison routines ignoring case
    //@{
    /// Compares this character string to the specified string ignoring case.
    /// @param cmp_utf8 The string to compare against.
    /// @return An integer indicating the relationship between this string and the specified string:
    ///    @li < 0 Means this string is less than the specified string
    ///    @li 0 Means this string is identical to the specified string
    ///    @li > 0 Means this string is greater than the specified string
    int icompare(const basic_utf8string<T>& cmp_utf8) const;

    /// @overload
    /// @param cmp_sz The string to compare against.
    int icompare(const char* cmp_sz) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_utf8 The string to compare against.
    int icompare(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& cmp_utf8) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_utf8 The string to compare against.
    /// @param pos2_mb The first character in the specified string at which the comparison starts.
    /// @param count2_mb The number of characters to consider in the specified string.
    int icompare(
        size_type pos_mb,
        size_type count_mb,
        const basic_utf8string<T>& cmp_utf8,
        size_type pos2_mb,
        size_type count2_mb) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_sz The string to compare against.
    /// @param count2_mb The number of characters to consider in the specified string.
    int icompare(
        size_type pos_mb,
        size_type count_mb,
        const char* cmp_sz,
        size_type count2_mb) const;

    /// @overload
    /// @param pos_mb The first character in this string at which the comparison starts. 
    /// @param count_mb The number of characters to consider in this string. 
    /// @param cmp_sz The string to compare against.
    int icompare(
        size_type pos_mb,
        size_type count_mb,
        const char* cmp_sz) const;
    //@}

    /**
        Split a string into multiple sub-strings by matching against a set of delimiters.
        Similar to strtok.
     */
    efd::UInt32 split(
        const basic_utf8string<T>& i_set,
        efd::vector< basic_utf8string<T> >& o_results) const;

    /**
        NullString is a global const string that is set to the empty or "null" string.  This is
        very helpful if you have a function that returns a "const basic_utf8string&" and you want
        to return a default or failure value.
     */
    static const basic_utf8string<T>& NullString()
    {
        static basic_utf8string<T> ns;
        return ns;
    }

    /**
        WhiteSpace is a global const string that contains " \t\n\r\l".  It is the default argument
        to the trim function.
     */
    static const basic_utf8string<T>& WhiteSpace()
    {
        static basic_utf8string<T> ws(" \t\n\r");
        return ws;
    }

    /** utf8_mb_to_sb()
        Translates between multibyte (character) position to byte offset within a UTF-8-encoded buffer.
        If the requested position is beyond sizelimit_sb then it will return npos
    */
    static size_type utf8_mb_to_sb(
        const char* buffer_sz,
        typename basic_utf8string<T>::size_type pos_mb,
        typename basic_utf8string<T>::size_type sizelimit_sb);

    /** utf8_sb_to_mb()
        Translates between byte offset and multibyte (character) position
        within a UTF-8-encoded buffer 
     */
    static size_type utf8_sb_to_mb(
        const char* buffer_sz,
        typename basic_utf8string<T>::size_type pos_sb,
        typename basic_utf8string<T>::size_type sizelimit_sb);


private:
    template< typename InputIterator > static
    basic_utf8string<T> StreamToString( InputIterator itBegin, InputIterator itEnd );

private:
    internal_string m_string;
};

// STATIC npos OBJECT
template<typename T>
const typename basic_utf8string<T>::size_type basic_utf8string<T>::npos =
    (typename basic_utf8string<T>::size_type)(-1);
} // end namespace details
} // end namespace efd

#include <efd/basic_utf8string.inl>

#endif  // EE_BASIC_UTF8STRING_H
