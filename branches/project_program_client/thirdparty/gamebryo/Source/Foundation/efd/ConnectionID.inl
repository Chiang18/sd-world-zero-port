/******************************************************************************
*
* EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
*
* This software is supplied under the terms of a license agreement or
* nondisclosure agreement with Emergent Game Technologies and may not
* be copied or disclosed except in accordance with the terms of that
* agreement.
*
*      Copyright (c) 1996-2009 Emergent Game Technologies.
*      All Rights Reserved.
*
* Emergent Game Technologies, Calabasas, CA 91302
* http://www.emergent.net
*
\******************************************************************************/

namespace efd
{

//-------------------------------------------------------------------------------------------------
inline efd::DataStream& operator<<( efd::DataStream& i_strm, const ConnectionID& i_val )
{
    i_strm << i_val.m_id;
    i_strm << (efd::SInt32)i_val.m_qos;
    return i_strm;
}


//-------------------------------------------------------------------------------------------------
inline const efd::DataStream& operator>>( const efd::DataStream& i_strm, ConnectionID& o_val )
{
    i_strm >> o_val.m_id;
    efd::SInt32 val;
    i_strm >> val;
    o_val.m_qos = (efd::QualityOfService)val;
    return i_strm;
}


//-------------------------------------------------------------------------------------------------
inline void DataStreamSkip( const efd::DataStream& i_strm, const ConnectionID& o_val )
{
    i_strm.Skip( o_val.m_id );
}


//-------------------------------------------------------------------------------------------------
} // end namespace efd


