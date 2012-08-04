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


namespace efd
{

namespace details
{

//==========================================================================
//  basic_utf8string<T> implementation
//==========================================================================

//-------------------------------------------------------------------------------------------------
//  A word about naming conventions in this file:
//
//  Variables suffixed with "mb" specify a multibyte value.  These are used
//  when referring to offsets with respect to code points.  For example,
//  (pos_mb = 6) might refer to the 6th multibyte character in the string.
//
//  Variables suffixed with "sb" specify a single-byte value.  These are
//  used when referring to offsets within the underlying byte buffer.  For
//  example, the 6th multibyte character in the string may be located at
//  (pos_sb = 24).
//
//  To translate between "mb" and "sb" (and vice-versa), use
//  utf8_mb_to_sb() and utf8_sb_to_mb().
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  StreamToString()  [private static]
//-------------------------------------------------------------------------------------------------
template< typename T >
template< typename InputIterator >
basic_utf8string<T> basic_utf8string<T>::StreamToString(
    InputIterator itBegin,
    InputIterator itEnd)
{
    basic_utf8string<T> tmp;

    while (itBegin != itEnd)
    {
        tmp += (*itBegin);
        ++itBegin;
    }

    return tmp;
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string()
: m_string()
{
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [copy ctor]
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(const basic_utf8string<T>& rhs)
: m_string( rhs.m_string )
{
}

//-------------------------------------------------------------------------------------------------
//  ~basic_utf8string<T>()  [dtor]
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::~basic_utf8string()
{
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as a partial copy of a C string.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(
    const char* src_sz,
    size_type count_mb)
{
    assign( src_sz, CT_LENGTH, count_mb );
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as a partial copy of a C string.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(
    const char* src_sz,
    _CountType ct,
    size_type count)
{
    assign( src_sz, ct, count );
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as a partial copy of another basic_utf8string<T>.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(
    const basic_utf8string<T>& src_utf8,
    size_type pos_mb,
    size_type count_mb)
{
    size_type pos_sb = utf8_mb_to_sb(src_utf8.data(), pos_mb, src_utf8.size());

    if ( pos_sb != npos )
    {
        EE_ASSERT( src_utf8.size() >= pos_sb );

        size_type remaining_buffer = src_utf8.size() - pos_sb;

        size_type count_sb =
            utf8_mb_to_sb(src_utf8.data() + pos_sb, count_mb, remaining_buffer);

        m_string.assign(src_utf8.m_string, pos_sb, count_sb);
    }
    else
    {
        // The starting position is beyond the end of the string, correct result is an empty
        // string so we don't have to do anything else.
        EE_ASSERT( m_string.empty() );
    }
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as a copy of a C string.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string( const char* src_sz )
    : m_string(src_sz)
{
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as multiple characters.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(
    size_type count_mb,
    const utf8char_t &uc)
{
    (*this).append(count_mb, uc);
}

//-------------------------------------------------------------------------------------------------
//  basic_utf8string<T>()  [ctor]
//  Construct a basic_utf8string<T> as multiple characters.
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>::basic_utf8string(
    _Formatted, const char* format_sz, ...)
{
    va_list args;
    va_start( args, format_sz );
    /*int result =*/ vsprintf( format_sz, args );
    va_end( args );
}

//-------------------------------------------------------------------------------------------------
//  raw()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const typename basic_utf8string<T>::internal_string& basic_utf8string<T>::raw() const
{
    return m_string;
}

//-------------------------------------------------------------------------------------------------
//  base()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const typename basic_utf8string<T>::internal_string& basic_utf8string<T>::base() const
{
    return m_string;
}

//-------------------------------------------------------------------------------------------------
//  c_str()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const char* basic_utf8string<T>::c_str() const
{
    return m_string.c_str();
}

//-------------------------------------------------------------------------------------------------
//  data()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const char* basic_utf8string<T>::data() const
{
    return m_string.data();
}

//-------------------------------------------------------------------------------------------------
//  begin()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::iterator basic_utf8string<T>::begin()
{
    return iterator(m_string.begin());
}

//-------------------------------------------------------------------------------------------------
//  end()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::iterator basic_utf8string<T>::end()
{
    return iterator(m_string.end());
}

//-------------------------------------------------------------------------------------------------
//  begin()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::const_iterator basic_utf8string<T>::begin() const
{
    return const_iterator(m_string.begin());
}

//-------------------------------------------------------------------------------------------------
//  end()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::const_iterator basic_utf8string<T>::end() const
{
    return const_iterator(m_string.end());
}

//-------------------------------------------------------------------------------------------------
//  rbegin()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::reverse_iterator basic_utf8string<T>::rbegin()
{
    return reverse_iterator(m_string.rbegin());
}

//-------------------------------------------------------------------------------------------------
//  rend()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::reverse_iterator basic_utf8string<T>::rend()
{
    return reverse_iterator(m_string.rend());
}

//-------------------------------------------------------------------------------------------------
//  rbegin()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::const_reverse_iterator basic_utf8string<T>::rbegin() const
{
    return const_reverse_iterator(m_string.rbegin());
}

//-------------------------------------------------------------------------------------------------
//  rend()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::const_reverse_iterator basic_utf8string<T>::rend() const
{
    return const_reverse_iterator(m_string.rend());
}

//-------------------------------------------------------------------------------------------------
//  operator[]
//  Provides read-only random access (requiring linear time).
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const typename basic_utf8string<T>::value_type basic_utf8string<T>::operator[](
    size_type pos_mb) const
{
    size_type offset_sb = utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size());
    // Lets assert if we walked past the end of our string:
    EE_ASSERT( offset_sb <= m_string.size() );
    return utf8char_t::from_stream((const char *)m_string.data() + offset_sb);
}

//-------------------------------------------------------------------------------------------------
//  at()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline const typename basic_utf8string<T>::value_type basic_utf8string<T>::at(
    size_type pos_mb) const
{
    return (*this)[pos_mb];
}

//-------------------------------------------------------------------------------------------------
//  substr()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T> basic_utf8string<T>::substr(
    size_type pos_mb,
    size_type count_mb) const
{
    basic_utf8string<T> tmp = basic_utf8string<T>(*this, pos_mb, count_mb);
    return tmp;
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(const basic_utf8string<T>& cmp_utf8) const
{
    return compare( cmp_utf8.data() );
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(const char* cmp_sz) const
{
    // Note: Our size does NOT include the NULL terminator, but we don't guarantee that there
    // IS a NULL terminator so we can't be 100% sure that size()+1 is valid.  Still, since
    // we are allowed to work on non-terminated strings we must pass in SOME size limit and
    // we can't pass in just size() or we'd consider the strings equal in the case where the
    // other string is longer than we are.  So we simply assume that we're actually NULL
    // terminated and we'll just have to rely on the user not calling this function when that
    // assumption isn't true.
    EE_ASSERT( 0 == m_string[m_string.length()] );
#if defined(WIN32)
    return ::_strncoll(m_string.data(), cmp_sz, size()+1);
#else
    return strncmp(m_string.data(), cmp_sz, size()+1);
#endif
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& cmp_utf8) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).compare(cmp_utf8);
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& cmp_utf8,
    size_type pos_mb2,
    size_type count_mb2) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).compare(
        basic_utf8string<T>(cmp_utf8, pos_mb2, count_mb2));
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(
    size_type pos_mb,
    size_type count_mb,
    const char* cmp_sz,
    size_type count_mb2) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).compare(
        basic_utf8string<T>(cmp_sz, CT_LENGTH, count_mb2));
}

//-------------------------------------------------------------------------------------------------
//  compare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::compare(
    size_type pos_mb,
    size_type count_mb,
    const char* cmp_sz) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).compare(cmp_sz);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(
    size_type count_mb,
    const utf8char_t& uc)
{
    basic_utf8string<T> tmp;
    tmp.reserve(count_mb * uc.size());
    for (unsigned i = 0; i < count_mb; ++i)
    {
        tmp.push_back(uc);
    }

    m_string.assign(tmp.raw());

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(const char* src_sz)
{
    m_string.assign(src_sz);

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(
    const char *src_sz,
    _CountType ct,
    typename basic_utf8string<T>::size_type count)
{
    if ( CT_LENGTH == ct )
    {
        count = utf8_mb_to_sb(src_sz, count, npos);
    }
    m_string.assign(src_sz, count);

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(
    const char *src_sz, size_type pos_mb, size_type count_mb)
{
    const size_type pos_sb = utf8_mb_to_sb(src_sz, pos_mb, npos);

    if ( npos == pos_sb )
    {
        // if the starting position is beyond the NULL terminator of the string then we'll
        // simply set ourselves to the empty string instead of, say, faulting.
        m_string.clear();
    }
    else
    {
        const size_type count_sb =
            utf8_mb_to_sb(src_sz + pos_sb, count_mb, npos);

        m_string.assign(src_sz + pos_sb, count_sb);
    }

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(const basic_utf8string<T>& src_utf8)
{
    m_string.assign(src_utf8.raw());
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(
    const basic_utf8string<T>& src_utf8,
    size_type count_mb)
{
    (*this).assign(src_utf8.raw().data(), CT_LENGTH, count_mb);
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::assign(
    const basic_utf8string<T>& src_utf8,
    size_type pos_mb,
    size_type count_mb)
{
    (*this).assign(src_utf8.raw().data(), pos_mb, count_mb);
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  assign()
//-------------------------------------------------------------------------------------------------
template< typename T>
template< typename InputIterator>
basic_utf8string<T>& basic_utf8string<T>::assign(InputIterator itBegin, InputIterator itEnd)
{
    clear();

    while (itBegin != itEnd)
    {
        push_back(*itBegin);
        ++itBegin;
    }

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator=(const basic_utf8string<T>& src_utf8)
{
    m_string = src_utf8.m_string;

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator=(
    const typename basic_utf8string<T>::internal_string& src_str)
{
    m_string = src_str;

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator=(const char* rhs_sz)
{
    m_string = rhs_sz;
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator=(const utf8char_t& ch)
{
    m_string = &(ch.bytes[0]);
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  find()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find(
    const basic_utf8string<T>& str_utf8,
    size_type pos_mb) const
{
    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    if ( npos == pos_sb )
    {
        return npos;
    }

    return utf8_sb_to_mb( m_string.c_str(),
                             m_string.find(str_utf8.m_string, pos_sb),
                             m_string.size() );
}

//-------------------------------------------------------------------------------------------------
//  find()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find(
    const char* str_sz,
    size_type pos_mb,
    size_type count_mb) const
{
    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    size_type count_sb = utf8_mb_to_sb(str_sz, count_mb, npos);
    size_type found_sb = m_string.find(str_sz, pos_sb, count_sb);
    size_type found_mb = utf8_sb_to_mb(m_string.c_str(), found_sb, m_string.size());

    return found_mb;
}

//-------------------------------------------------------------------------------------------------
//  find()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find(
    const char* str_sz,
    size_type pos_mb) const
{
    if (pos_mb == npos)
    {
        pos_mb = 0;
    }

    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    size_type found_sb = m_string.find(str_sz, pos_sb);
    size_type found_mb = utf8_sb_to_mb(m_string.c_str(), found_sb, m_string.size());

    return found_mb;
}

//-------------------------------------------------------------------------------------------------
//  find()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find(
    const utf8char_t& uc,
    size_type pos_mb) const
{
    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    return utf8_sb_to_mb( m_string.c_str(),
                             m_string.find(uc.c_str(), pos_sb),
                             m_string.size() );
}

//-------------------------------------------------------------------------------------------------
//  rfind()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type
basic_utf8string<T>::rfind(
    const basic_utf8string<T>& str_utf8,
    size_type pos_mb) const
{
    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    size_type found_sb = m_string.rfind(str_utf8.m_string, pos_sb);
    size_type found_mb = utf8_sb_to_mb(m_string.c_str(), found_sb, m_string.size());

    return found_mb;
}

//-------------------------------------------------------------------------------------------------
//  rfind()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type
basic_utf8string<T>::rfind(
    const char* str_sz,
    size_type pos_mb,
    size_type count_mb) const
{
    EE_ASSERT( str_sz );

    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    size_type count_sb = utf8_mb_to_sb(str_sz, count_mb, npos);
    size_type found_sb = m_string.rfind(str_sz, pos_sb, count_sb);
    size_type found_mb = utf8_sb_to_mb(m_string.c_str(), found_sb, m_string.size());

    return found_mb;
}

//-------------------------------------------------------------------------------------------------
//  rfind()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type
basic_utf8string<T>::rfind(const char* str_sz, size_type pos_mb) const
{
    EE_ASSERT(str_sz);

    size_type pos_sb = utf8_mb_to_sb(m_string.c_str(), pos_mb, m_string.size());
    size_type found_sb = m_string.rfind(str_sz, pos_sb);
    size_type found_mb = utf8_sb_to_mb(m_string.c_str(), found_sb, m_string.size());

    return found_mb;
}

//-------------------------------------------------------------------------------------------------
//  rfind()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type
basic_utf8string<T>::rfind(const utf8char_t& uc, size_type pos_mb) const
{
    return rfind(uc.c_str(), pos_mb);
}

//-------------------------------------------------------------------------------------------------
//  Find Helper Functions
//-------------------------------------------------------------------------------------------------
template< typename T >
bool _find_match(
    const char *szSubstr,
    const basic_utf8string<T>& match_utf8,
    bool invert)
{
    bool found = false;
    utf8char_t uc = utf8char_t::from_stream(szSubstr);

    typename basic_utf8string<T>::const_iterator i;
    for (i = match_utf8.begin(); i != match_utf8.end() && !found; ++i)
    {
        if (uc == (*i))
        {
            found = true;
        }
    }

    //  The "invert" flag is set for the "not_of" family of find functions.
    return (found ^ invert);
}

template< typename T >
typename basic_utf8string<T>::size_type _find_first_of(
    const basic_utf8string<T>& str_utf8,
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb,
    bool invert)
{
    if (pos_mb == basic_utf8string<T>::npos)
    {
        return basic_utf8string<T>::npos;
    }

    typename basic_utf8string<T>::size_type pos_sb =
        basic_utf8string<T>::utf8_mb_to_sb(str_utf8.data(), pos_mb, str_utf8.size());

    if (pos_sb == basic_utf8string<T>::npos)
    {
        return basic_utf8string<T>::npos;
    }

    const char* ptr = str_utf8.data() + pos_sb;
    const char* ptrEnd = str_utf8.data() + str_utf8.size();

    typename basic_utf8string<T>::size_type visited_mb = 0;
    bool found = false;

    while (ptr <= ptrEnd && visited_mb < count_mb)
    {
        if (_find_match(ptr, match_utf8, invert))
        {
            found = true;
            break;
        }

        //  Scan to the next character.
        ptr += basic_utf8string<T>::utf8_mb_to_sb(ptr, 1, ptrEnd - ptr);
        ++visited_mb;
    }

    return (found) ? (visited_mb + pos_mb) : (basic_utf8string<T>::npos);
}

template< typename T >
typename basic_utf8string<T>::size_type _find_last_of(
    const basic_utf8string<T>& str_utf8,
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb,
    bool invert)
{
    typename basic_utf8string<T>::size_type pos_sb;
    if (pos_mb != basic_utf8string<T>::npos)
    {
        pos_sb = basic_utf8string<T>::utf8_mb_to_sb(str_utf8.data(), pos_mb, str_utf8.size());
    }
    else
    {
        pos_mb = str_utf8.length();
        pos_sb = str_utf8.size();
    }

    const char* ptr = str_utf8.data() + pos_sb;
    const char* ptrHead = str_utf8.data();

    typename basic_utf8string<T>::size_type visited_mb = 0;
    bool found = false;

    // size_type is unsigned so npos is the maximum possible value:
    EE_ASSERT( basic_utf8string<T>::npos >= count_mb );

    while (ptr > ptrHead && visited_mb < count_mb)
    {
        //  Scan backwards to the previous character.
        while (ptr > ptrHead)
        {
            --ptr;

            if ((*ptr & '\xC0') != '\x80')
            {
                break;
            }
        }

        ++visited_mb;

        if (_find_match(ptr, match_utf8, invert))
        {
            found = true;
            break;
        }
    }

    return (found) ? (pos_mb - visited_mb) : (basic_utf8string<T>::npos);
}

//-------------------------------------------------------------------------------------------------
//  find_first_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_of(
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_first_of<T>(*this, match_utf8, pos_mb, npos, false);
}

//-------------------------------------------------------------------------------------------------
//  find_first_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb) const
{
    return _find_first_of<T>(*this, match_sz, pos_mb, count_mb, false);
}

//-------------------------------------------------------------------------------------------------
//  find_first_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_first_of<T>(*this, match_sz, pos_mb, npos, false);
}

//-------------------------------------------------------------------------------------------------
//  find_first_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_of(
    const utf8char_t& uc,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return find(uc.c_str(), pos_mb);
}

//-------------------------------------------------------------------------------------------------
//  find_last_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_of(
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_last_of<T>(*this, match_utf8, pos_mb, npos, false);
}

//-------------------------------------------------------------------------------------------------
//  find_last_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb) const
{
    return _find_last_of<T>(*this, match_sz, pos_mb, count_mb, false);
}

//-------------------------------------------------------------------------------------------------
//  find_last_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_last_of<T>(*this, match_sz, pos_mb, npos, false);
}

//-------------------------------------------------------------------------------------------------
//  find_last_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_of(
    const utf8char_t& uc,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_last_of<T>(*this, uc.c_str(), pos_mb, npos, false);
}

//-------------------------------------------------------------------------------------------------
//  find_first_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_not_of(
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_first_of<T>(*this, match_utf8, pos_mb, length(), true);
}


//-------------------------------------------------------------------------------------------------
//  find_first_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_not_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb) const
{
    return _find_first_of<T>(*this, match_sz, pos_mb, count_mb, true);
}


//-------------------------------------------------------------------------------------------------
//  find_first_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_not_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_first_of<T>(*this, match_sz, pos_mb, npos, true);
}

//-------------------------------------------------------------------------------------------------
//  find_first_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_first_not_of(
    const utf8char_t& uc,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    basic_utf8string<T> match_utf8;
    match_utf8 += uc;
    return _find_first_of<T>(*this, match_utf8, pos_mb, npos, true);
}

//-------------------------------------------------------------------------------------------------
//  find_last_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_not_of(
    const basic_utf8string<T>& match_utf8,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_last_of<T>(*this, match_utf8, pos_mb, npos, true);
}

//-------------------------------------------------------------------------------------------------
//  find_last_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_not_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb,
    typename basic_utf8string<T>::size_type count_mb) const
{
    return _find_last_of<T>(*this, match_sz, pos_mb, count_mb, true);
}

//-------------------------------------------------------------------------------------------------
//  find_last_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_not_of(
    const char* match_sz,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    return _find_last_of<T>(*this, match_sz, pos_mb, npos, true);
}

//-------------------------------------------------------------------------------------------------
//  find_last_not_of()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::find_last_not_of(
    const utf8char_t& uc,
    typename basic_utf8string<T>::size_type pos_mb) const
{
    basic_utf8string<T> match_utf8;
    match_utf8 += uc;
    return _find_last_of<T>(*this, match_utf8, pos_mb, npos, true);
}

//-------------------------------------------------------------------------------------------------
//  operator+=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator+=(const basic_utf8string<T>& src_utf8)
{
    m_string += src_utf8.m_string;
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator+=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator+=(const char* src_sz)
{
    m_string += src_sz;
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  operator+=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::operator+=(const utf8char_t& uc)
{
    m_string += uc.c_str();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  push_back()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::push_back(const utf8char_t& uc)
{
    m_string += uc.c_str();
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(const basic_utf8string<T>& src_utf8)
{
    m_string += src_utf8.raw();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(const utf8char_t& uc)
{
    m_string += uc.c_str();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(
    const basic_utf8string<T>& src_utf8,
    size_type pos_mb,
    size_type count_mb )
{
    m_string += ( src_utf8.substr(pos_mb, count_mb) ).raw();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(const char* src_sz)
{
    m_string += src_sz;
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(
    const char* src_sz,
    size_type count_mb)
{
    m_string += (basic_utf8string<T>(src_sz).substr(0, count_mb)).raw();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::append(
    size_type count_mb,
    const utf8char_t& uc)
{
    size_type currlen = m_string.size();
    m_string.reserve(currlen + count_mb * uc.size());

    if (uc.size() == 1)
    {
        m_string.append(count_mb, uc.bytes[0]);
    }
    else
    {
        while (count_mb--)
        {
            m_string.append(uc.c_str());
        }
    }

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  append()
//-------------------------------------------------------------------------------------------------
template< typename T >
template< typename InputIterator >
basic_utf8string<T>& basic_utf8string<T>::append(InputIterator itFirst, InputIterator itLast)
{
    while (itFirst != itLast)
    {
        (*this) += (*itFirst);
        ++itFirst;
    }

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    const basic_utf8string<T>& src_utf8)
{
    size_type pos_sb = utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size());
    m_string.insert(pos_sb, src_utf8.m_string);

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    const char* src_sz)
{
    size_type pos_sb = utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size());
    m_string.insert(pos_sb, src_sz);
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    const basic_utf8string<T>& src_utf8,
    size_type pos_mb2,
    size_type count_mb)
{
    m_string.insert(
        utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size()),
        src_utf8.substr(pos_mb2, count_mb).raw());

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    const char* src_sz,
    size_type count_mb)
{
    size_type pos_sb = utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size());
    if ( npos == pos_sb )
    {
        return append( src_sz, count_mb );
    }

    size_type count_sb = utf8_mb_to_sb(src_sz, count_mb, npos);
    if ( npos == count_sb )
    {
        m_string.insert(pos_sb, src_sz);
    }
    else
    {
        m_string.insert(pos_sb, src_sz, count_sb);
    }

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    size_type count_mb,
    const utf8char_t& uc)
{
    (*this).insert(pos_mb, basic_utf8string<T>(count_mb, uc));
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::iterator basic_utf8string<T>::insert(
    typename basic_utf8string<T>::iterator it,
    const utf8char_t& uc)
{
    const size_type pos_sb = it.base() - m_string.begin();
    EE_ASSERT((signed)pos_sb >= 0);

    m_string.insert(pos_sb, uc.c_str());

    return typename basic_utf8string<T>::iterator(m_string.begin() + pos_sb);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::insert(
    typename basic_utf8string<T>::iterator itPos,
    size_type count_mb,
    const utf8char_t& uc )
{
    const size_type pos_sb = itPos.base() - m_string.begin();
    EE_ASSERT((signed)pos_sb >= 0);

    m_string.insert(pos_sb, basic_utf8string<T>(count_mb, uc).data());
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::insert(
    size_type pos_mb,
    const basic_utf8string<T>& src_utf8,
    size_type count_mb )
{
    m_string.insert(
        utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size()),
        src_utf8.m_string,
        0,
        utf8_mb_to_sb(src_utf8.data(), count_mb, src_utf8.size()) );

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  insert()
//-------------------------------------------------------------------------------------------------
template< typename T >
template< typename InputIterator >
void basic_utf8string<T>::insert(
    iterator itPos,
    InputIterator itBegin,
    InputIterator itEnd )
{
    const size_type pos = itPos.base() - m_string.begin();
    m_string.insert(pos, StreamToString(itBegin, itEnd).raw());
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& src_utf8)
{
    size_type pos_sb = utf8_mb_to_sb( m_string.data(), pos_mb, m_string.size() );
    if ( npos != pos_sb )
    {
        size_type count_sb = utf8_mb_to_sb( m_string.data() + pos_sb, count_mb, m_string.size()-pos_sb );
        m_string.replace( pos_sb, count_sb, src_utf8.m_string );
    }
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& src_utf8,
    size_type pos_mb2,
    size_type count_mb2)
{
    (*this).replace(pos_mb, count_mb, src_utf8.substr(pos_mb2, count_mb2));
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& src_utf8,
    size_type count_mb2)
{
    (*this).replace(pos_mb, count_mb, src_utf8.substr(0, count_mb2));
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    size_type pos_mb,
    size_type count_mb,
    const char* src_sz)
{
    (*this).replace(pos_mb, count_mb, basic_utf8string<T>(src_sz));
    return (*this);
}


//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    size_type pos_mb,
    size_type count_mb,
    size_type count_mb2,
    const utf8char_t& uc)
{
    (*this).replace(pos_mb, count_mb, basic_utf8string<T>(count_mb2, uc));
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd,
    const basic_utf8string<T>& src_utf8)
{
    m_string.replace(itBegin.base(), itEnd.base(), src_utf8.raw());

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd,
    const char* src_sz,
    size_type count_mb)
{
    m_string.replace(
        itBegin.base(),
        itEnd.base(),
        src_sz,
        utf8_mb_to_sb(src_sz, count_mb, npos));

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd,
    const char* src_sz)
{
    m_string.replace(itBegin.base(), itEnd.base(), src_sz);
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd,
    size_type count_mb,
    const utf8char_t& uc)
{
    m_string.replace(
        itBegin.base(),
        itEnd.base(),
        basic_utf8string<T>(count_mb, uc).raw());

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd,
    const basic_utf8string<T>& src_utf8,
    size_type count_mb )
{
    m_string.replace(
        itBegin.base(),
        itEnd.base(),
        src_utf8.data(),
        utf8_mb_to_sb(src_utf8.data(), count_mb, src_utf8.size()) );

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  replace()
//-------------------------------------------------------------------------------------------------
template< typename T >
template< typename InputIterator >
basic_utf8string<T>& basic_utf8string<T>::replace(
    typename basic_utf8string<T>::iterator itDestBegin,
    typename basic_utf8string<T>::iterator itDestEnd,
    InputIterator itSrcBegin,
    InputIterator itSrcEnd )
{
    m_string.replace(
        itDestBegin.base(),
        itDestEnd.base(),
        StreamToString(itSrcBegin, itSrcEnd).raw());

    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  clear()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::clear()
{
    m_string.clear();
}

//-------------------------------------------------------------------------------------------------
//  erase()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::erase(
    size_type pos_mb,
    size_type count_mb)
{
    size_type pos_sb = utf8_mb_to_sb(m_string.data(), pos_mb, m_string.size());
    if ( npos != pos_sb )
    {
        size_type count_sb = utf8_mb_to_sb(m_string.data() + pos_sb,
                                              count_mb,
                                              m_string.size() - pos_sb );
        m_string.erase(pos_sb, count_sb);
    }
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  erase()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline basic_utf8string<T>& basic_utf8string<T>::erase()
{
    m_string.erase();
    return (*this);
}

//-------------------------------------------------------------------------------------------------
//  erase()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::iterator basic_utf8string<T>::erase(
    typename basic_utf8string<T>::iterator itPos)
{
    return iterator(
        m_string.erase(itPos.base(), itPos.base() + (*itPos).size()));
}

//-------------------------------------------------------------------------------------------------
//  erase()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::iterator basic_utf8string<T>::erase(
    typename basic_utf8string<T>::iterator itBegin,
    typename basic_utf8string<T>::iterator itEnd)
{
    return iterator(m_string.erase(itBegin.base(), itEnd.base()));
}

//-------------------------------------------------------------------------------------------------
//  empty()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool basic_utf8string<T>::empty() const
{
    return m_string.empty();
}

//-------------------------------------------------------------------------------------------------
//  size()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::size() const
{
    return m_string.size();
}

//-------------------------------------------------------------------------------------------------
//  length()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::length() const
{
    size_type len = 0;

    for (const_iterator i = begin(); i != end(); ++i)
    {
        ++len;
    }

    return len;
}

//-------------------------------------------------------------------------------------------------
//  swap()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::swap(basic_utf8string<T>& rhs)
{
    m_string.swap(rhs.m_string);
}

//-------------------------------------------------------------------------------------------------
//  reserve()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::reserve(size_type count_sb)
{
    m_string.reserve(count_sb);
}

//-------------------------------------------------------------------------------------------------
//  resize()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::resize(size_type count_sb)
{
    size_type oldCount_sb = m_string.size();
    m_string.resize(count_sb);

    if (count_sb < oldCount_sb)
    {
        //  Traverse from the end and ensure the last UTF8 character is
        //  valid, and erase it if it was severed in the resizing.

        unsigned fragment_sb = utf8_tail_fragments( m_string.data(), (unsigned)count_sb );

        m_string.erase(count_sb - fragment_sb, npos);
    }
}

//-------------------------------------------------------------------------------------------------
//  resize()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::resize(
    size_type count_sb,
    const utf8char_t& uc)
{
    size_type oldCount_sb = m_string.size();

    if (count_sb >= oldCount_sb)
    {
        m_string.reserve(count_sb);
        size_type mbPadding =
            (count_sb - oldCount_sb) / uc.size();
        (*this).append(mbPadding, uc);
    }
    else
    {
        m_string.resize(count_sb);

        //  Traverse from the end and ensure the last UTF8 character is
        //  valid, and erase it if it was severed in the resizing.

        unsigned fragment_sb = utf8_tail_fragments( m_string.data(), (unsigned)count_sb );

        m_string.erase(count_sb - fragment_sb, npos);
    }
}

//-------------------------------------------------------------------------------------------------
//  capacity()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::capacity() const
{
    return m_string.capacity();
}

//-------------------------------------------------------------------------------------------------
//  max_size()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline typename basic_utf8string<T>::size_type basic_utf8string<T>::max_size() const
{
    return m_string.max_size();
}

//-------------------------------------------------------------------------------------------------
//  is_ascii()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool basic_utf8string<T>::is_ascii() const
{
    const char* i = m_string.data();
    const char* const j = i + m_string.size();

    while (i != j)
    {
        if ((*i) & 0x80)
        {
            return false;
        }

        ++i;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
//  sprintf
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::sprintf( const char* format_sz, ... )
{
    va_list args;
    va_start( args, format_sz);
    int result = vsprintf( format_sz, args );
    va_end( args );
    return result;
}

template< typename T >
inline int basic_utf8string<T>::sprintf( const basic_utf8string<T>& format_str, ... )
{
    va_list args;
    va_start( args, format_str);
    int result = vsprintf( format_str.c_str(), args );
    va_end( args );
    return result;
}

template< typename T >
inline int basic_utf8string<T>::sprintf_append( const char* format_sz, ... )
{
    va_list args;
    va_start( args, format_sz);
    int result = vsprintf_append( format_sz, args );
    va_end( args );
    return result;
}

template< typename T >
inline int basic_utf8string<T>::sprintf_append( const basic_utf8string<T>& format_str, ... )
{
    va_list args;
    va_start( args, format_str);
    int result = vsprintf_append( format_str.c_str(), args );
    va_end( args );
    return result;
}

template< typename T >
inline int basic_utf8string<T>::vsprintf( const char* format_sz, va_list args )
{
    int bytesNeeded = efd::Vscprintf( format_sz, args );
    internal_string temp;
    temp.reserve( bytesNeeded + 1 );
    int bytesUsed = ::vsprintf( &temp[0], format_sz, args );
    EE_ASSERT( bytesUsed == bytesNeeded );
    EE_ASSERT( (size_type)bytesUsed < temp.capacity() );
    temp[bytesUsed] = 0;

    // This has to build the string into a temporary and then allocate and copy to the final
    // simply so that m_string will update its notion of how long it is (as it's impossible to
    // update the size() directly)
    m_string = temp.c_str();

    return bytesUsed;
}

template< typename T >
inline int basic_utf8string<T>::vsprintf( const basic_utf8string<T>& format_str, va_list args )
{
    return vsprintf( format_str.c_str(), args );
}

template< typename T >
inline int basic_utf8string<T>::vsprintf_append( const char* format_sz, va_list args )
{
    int bytesNeeded = efd::Vscprintf( format_sz, args );
    internal_string temp;
    temp.reserve( bytesNeeded + 1 );
    int bytesUsed = ::vsprintf( &temp[0], format_sz, args );
    EE_ASSERT( bytesUsed == bytesNeeded );
    EE_ASSERT( (size_type)bytesUsed < temp.capacity() );
    temp[bytesUsed] = 0;

    // This has to build the string into a temporary and then allocate and copy to the final
    // simply so that m_string will update its notion of how long it is (as it's impossible to
    // update the size() directly)
    m_string += temp.c_str();

    return bytesUsed;
}

template< typename T >
inline int basic_utf8string<T>::vsprintf_append( const basic_utf8string<T>& format_str, va_list args )
{
    return vsprintf_append( format_str.c_str(), args );
}

//-------------------------------------------------------------------------------------------------
//  trim
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::trim( _Trim t, const basic_utf8string<T>& set )
{
    size_type idxBegin = (t == TrimBack) ? 0 : find_first_not_of( set );
    if ( npos == idxBegin )
    {
        clear();
        return;
    }

    size_type idxEnd   = (t == TrimFront) ? npos-1 : find_last_not_of( set );
    ++idxEnd;

    EE_ASSERT( idxEnd >= idxBegin );
    *this = substr( idxBegin, idxEnd - idxBegin );
}

template< typename T >
inline void basic_utf8string<T>::trim( _Trim t, const char* set )
{
    size_type idxBegin = (t == TrimBack) ? 0 : m_string.find_first_not_of( set ) ;
    if ( npos == idxBegin )
    {
        clear();
        return;
    }

    size_type idxEnd = (t == TrimFront) ? npos-1 : m_string.find_last_not_of( set ) ;
    ++idxEnd;

    EE_ASSERT( idxEnd >= idxBegin );
    m_string = m_string.substr( idxBegin, idxEnd - idxBegin );
}

//-------------------------------------------------------------------------------------------------
//  toupper()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::toupper()
{
    // ensure string is NULL terminated:
    EE_ASSERT_MESSAGE( m_string[m_string.size()] == '\0', ("NULL terminated string required for %s", __FUNCTION__));

    // add 1 to string length to include null terminator
    efd::Strupr( &m_string[0], m_string.size()+1 );
}

//-------------------------------------------------------------------------------------------------
//  tolower()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline void basic_utf8string<T>::tolower()
{
    // ensure string is NULL terminated:
    EE_ASSERT_MESSAGE( m_string[m_string.size()] == '\0', ("NULL terminated string required for %s", __FUNCTION__));

    // add 1 to string length to include null terminator
    efd::Strlwr( &m_string[0], m_string.size()+1 );
}

//-------------------------------------------------------------------------------------------------
//  replace_substr()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::replace_substr(const char* find_sz, const char* replace_sz )
{
    int cReplacements = 0;

    size_type cbFind = strlen( find_sz );
    size_type cbReplace = strlen( replace_sz );

    size_type cbCurrent = 0;
    while ( basic_utf8string::npos != (cbCurrent = m_string.find( find_sz, cbCurrent )) )
    {
        m_string.replace( cbCurrent, cbFind, replace_sz, cbReplace );
        cbCurrent += cbReplace;
        ++cReplacements;
    }

    return cReplacements;
}

//-------------------------------------------------------------------------------------------------
//  replace_substr()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::replace_substr(const basic_utf8string<T>& find_str,
                                               const basic_utf8string<T>& replace_str )
{
    return replace_substr( find_str.c_str(), replace_str.c_str() );
}



//-------------------------------------------------------------------------------------------------
//  replace_substr()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::ol_replace_substr(const char* find_sz, const char* replace_sz )
{
    // If the replacement string contains the search string then we would get stuck in an
    // infinite loop if we continued, so we specifically check for this case:
    if ( strstr( replace_sz, find_sz ) )
    {
        return -1;
    }

    int cReplacements = 0;

    size_type cbFind = strlen( find_sz );
    size_type cbReplace = strlen( replace_sz );

    size_type cbCurrent = 0;
    while ( basic_utf8string::npos != (cbCurrent = m_string.find( find_sz, cbCurrent )) )
    {
        m_string.replace( cbCurrent, cbFind, replace_sz, cbReplace );
        ++cReplacements;
    }

    return cReplacements;
}

//-------------------------------------------------------------------------------------------------
//  replace_substr()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::ol_replace_substr(const basic_utf8string<T>& find_str,
                                               const basic_utf8string<T>& replace_str )
{
    return replace_substr( find_str.c_str(), replace_str.c_str() );
}


//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(const basic_utf8string<T>& cmp_utf8) const
{
    return icompare(cmp_utf8.data());
}

//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(const char* cmp_sz) const
{
    // Note: Our size does NOT include the NULL terminator, but we don't guarantee that there
    // IS a NULL terminator so we can't be 100% sure that size()+1 is valid.  Still, since
    // we are allowed to work on non-terminated strings we must pass in SOME size limit and
    // we can't pass in just size() or we'd consider the strings equal in the case where the
    // other string is longer than we are.  So we simply assume that we're actually NULL
    // terminated and we'll just have to rely on the user not calling this function when that
    // assumption isn't true.
    EE_ASSERT( 0 == m_string[m_string.size()] );
#if defined(WIN32) || defined(EE_PLATFORM_XBOX360)
    return ::_strnicoll(m_string.data(), cmp_sz, size()+1);
#else
    return strncasecmp(m_string.data(), cmp_sz, size()+1);
#endif
}

//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& cmp_utf8) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).icompare(cmp_utf8);
}

//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(
    size_type pos_mb,
    size_type count_mb,
    const basic_utf8string<T>& cmp_utf8,
    size_type pos_mb2,
    size_type count_mb2) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).icompare(
        basic_utf8string<T>(cmp_utf8, pos_mb2, count_mb2));
}

//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(
    size_type pos_mb,
    size_type count_mb,
    const char* cmp_sz,
    size_type count_mb2) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).icompare(
        basic_utf8string<T>(cmp_sz, count_mb2));
}

//-------------------------------------------------------------------------------------------------
//  icompare()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline int basic_utf8string<T>::icompare(
    size_type pos_mb,
    size_type count_mb,
    const char* cmp_sz) const
{
    return basic_utf8string<T>(*this, pos_mb, count_mb).icompare(cmp_sz);
}

} // namespace details

//-------------------------------------------------------------------------------------------------
// global operators

//-------------------------------------------------------------------------------------------------
//  operator==()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator==(
    const details::basic_utf8string<T>& lhs_utf8,
    const details::basic_utf8string<T>& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) == 0;
}

//-------------------------------------------------------------------------------------------------
//  operator==()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator==(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) == 0;
}

//-------------------------------------------------------------------------------------------------
//  operator==()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator==( const char* lhs_c_str, const details::basic_utf8string<T>& rhs_utf8 )
{
    return rhs_utf8.compare(lhs_c_str) == 0;
}

//-------------------------------------------------------------------------------------------------
//  operator!=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator!=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const details::basic_utf8string<T>& rhs_utf8)
{
    return !(lhs_utf8 == rhs_utf8);
}

//-------------------------------------------------------------------------------------------------
//  operator!=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator!=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const char* rhs_c_str)
{
    return !(lhs_utf8 == rhs_c_str);
}

//-------------------------------------------------------------------------------------------------
//  operator!=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator!=( const char* lhs_c_str, const details::basic_utf8string<T>& rhs_utf8 )
{
    return !(rhs_utf8 == lhs_c_str);
}

//-------------------------------------------------------------------------------------------------
//  operator<()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator<(
                      const details::basic_utf8string<T>& lhs_utf8,
                      const details::basic_utf8string<T>& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) < 0;
}

//-------------------------------------------------------------------------------------------------
//  operator<()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator<(
                      const details::basic_utf8string<T>& lhs_utf8,
                      const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) < 0;
}

//-------------------------------------------------------------------------------------------------
//  operator<=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator<=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const details::basic_utf8string<T>& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) <= 0;
}

//-------------------------------------------------------------------------------------------------
//  operator<=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator<=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) <= 0;
}

//-------------------------------------------------------------------------------------------------
//  operator>()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator>(
                      const details::basic_utf8string<T>& lhs_utf8,
                      const details::basic_utf8string<T>& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) > 0;
}

//-------------------------------------------------------------------------------------------------
//  operator>()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator>(
                      const details::basic_utf8string<T>& lhs_utf8,
                      const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) > 0;
}

//-------------------------------------------------------------------------------------------------
//  operator<=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator>=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const details::basic_utf8string<T>& rhs_utf8)
{
    return lhs_utf8.compare(rhs_utf8) >= 0;
}

//-------------------------------------------------------------------------------------------------
//  operator<=()
//-------------------------------------------------------------------------------------------------
template< typename T >
inline bool operator>=(
                       const details::basic_utf8string<T>& lhs_utf8,
                       const char* rhs_c_str)
{
    return lhs_utf8.compare(rhs_c_str) >= 0;
}


namespace details
{

//-------------------------------------------------------------------------------------------------
template< typename T >
typename basic_utf8string<T>::size_type basic_utf8string<T>::utf8_mb_to_sb(
    const char* buffer_sz,
    size_type pos_mb,
    size_type byteLimit )
{
    EE_ASSERT(buffer_sz);

    if (pos_mb == npos)
    {
        return npos;
    }

    // If the passed in byteLimit is npos then we are dealing with a NULL terminated string.
    // Otherwise we know the buffer size and will step over NULL characters.
    bool terminateOnNull = byteLimit == npos;

    const char* szIter = buffer_sz;
    const char* szEnd = terminateOnNull ? (const char*)-1 : szIter + byteLimit;

    while (pos_mb)
    {
        --pos_mb;

        if (terminateOnNull && 0 == szIter[0])
        {
            return npos;
        }

        if ( szIter > szEnd )
        {
            return npos;
        }

        unsigned int units = utf8_num_units(szIter);

#if defined(EE_EFD_CONFIG_DEBUG)
        // Lets make sure we really have a valid character.  Otherwise we could easily be
        // stepping beyond our memory allotment which could crash or lead to corruption.
        utf8_validate_char(szIter, units);
#endif

        szIter += units;
    }

    return (szIter - buffer_sz);
}

//-------------------------------------------------------------------------------------------------
//  utf8_sb_to_mb()
//-------------------------------------------------------------------------------------------------
template< typename T >
typename basic_utf8string<T>::size_type basic_utf8string<T>::utf8_sb_to_mb(
    const char* buffer_sz,
    size_type pos_sb,
    size_type byteLimit)
{
    EE_ASSERT(buffer_sz);

    if (pos_sb == npos)
    {
        return npos;
    }

    if (pos_sb > byteLimit)
    {
        return npos;
    }

    unsigned index = 0;
    size_type counter = 0;

    while (counter < pos_sb)
    {
        unsigned bytes = utf8_num_units( buffer_sz );
        buffer_sz += bytes;
        counter += bytes;
        ++index;
    }

    EE_ASSERT( counter <= byteLimit );

    return index;
}


template< typename T >
efd::UInt32 basic_utf8string<T>::split( const basic_utf8string<T>& i_set, efd::vector< basic_utf8string<T> >& o_results ) const
{
    o_results.clear();

    // in order to avoid changing the source string it must be copied since strtok directly
    // munges the string:
    basic_utf8string<T> temp = *this;

    efd::Char* pszContext;
    efd::Char* pszToken = efd::Strtok( const_cast<char*>(temp.c_str()), i_set.c_str(), &pszContext );

    efd::UInt32 i=0;
    while ( NULL != pszToken )
    {
        ++i;
        o_results.push_back(pszToken);
        pszToken = efd::Strtok( NULL, i_set.c_str(), &pszContext );
    }

    return i;
}

} // end namespace details

} // end namespace efd

