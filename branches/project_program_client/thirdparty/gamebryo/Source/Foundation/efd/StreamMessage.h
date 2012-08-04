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
#ifndef EE_STREAMMESSAGE_H
#define EE_STREAMMESSAGE_H

#include <efd/IMessage.h>
#include <efd/DataStream.h>
#include <efd/efdClassIDs.h>

namespace efd
{

/**
    /brief A message class that can be used for fast prototyping.

    StreamMessage can be used to quickly prototype messages without the need to create a custom
    message type and implement ToStream and FromStream.  The drawback to this is that an 
    implementation detail of what order and what the data is stored in the message must be known
    everywhere the message is sent and received.  In a standard IMessage based custom message
    only ToStream and FromStream need to know the order data members are streamed.
*/
class EE_EFD_ENTRY StreamMessage : public IMessage
{
    /// @cond EMERGENT_INTERNAL

    EE_DECLARE_CLASS1(StreamMessage, efd::kCLASSID_StreamMessage, IMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;

    /// @endcond

public:
    StreamMessage();

    /// Virtual destructor.
    virtual ~StreamMessage();

    /**
        Implicit conversion to IDataStreams.

        @return Reference to the DataStream
    */
    virtual operator efd::DataStream&();

    /**
        Implicit conversion to IDataStreams.

        @return Reference to the DataStream
    */
    virtual operator const efd::DataStream&() const;

    /**
        Explicit conversion to IDataStreams.

        @return Reference to the DataStream
    */
    virtual efd::SmartPointer<const efd::DataStream> GetParameters() const;

    /// Resets the read and write cursors of the stream in this message.
    virtual void Reset() const;

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ToStream(efd::DataStream &strm) const;
    virtual bool FromStream(const efd::DataStream &strm);
    inline virtual void SetStream(efd::DataStreamPtr spDataStream);
    inline virtual void GetStream(efd::DataStreamPtr& spDataStream);

    /**
        Get the total number of bytes this stream uses.
        
        This count does not include padded space that is not yet allocated.

        @return Number of bytes used by this stream.
     */
    inline size_t GetStreamSize() const;

private:
    // Pointer to a data stream.
    efd::DataStreamPtr m_spStream;
};

/// Define a smart pointer for the StreamMessage class.
typedef efd::SmartPointer<StreamMessage> StreamMessagePtr;

}   // End namespace efd.

#include <efd/StreamMessage.inl>

#endif  // EE_STREAMMESSAGE_H
