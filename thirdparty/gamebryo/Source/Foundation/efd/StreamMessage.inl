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

//-------------------------------------------------------------------------------------------------
inline void StreamMessage::SetStream(efd::DataStreamPtr datastream)
{
    m_spStream = datastream;
}


//-------------------------------------------------------------------------------------------------
inline void StreamMessage::GetStream(efd::DataStreamPtr& spDataStream)
{
    spDataStream = m_spStream;
}


//-------------------------------------------------------------------------------------------------
inline size_t StreamMessage::GetStreamSize() const
{
    if (m_spStream)
        return m_spStream->GetSize();
    else
        return 0;
}
//-------------------------------------------------------------------------------------------------

}   // End namespace efd
