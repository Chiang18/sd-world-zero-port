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
#ifndef EE_NETENUMS_H
#define EE_NETENUMS_H

namespace efd
{
    enum NetFeatures
    {
        NET_UDP             = 0x00000001,
        NET_TCP             = 0x00000002,
        NET_RELIABLE        = 0x00000004,
        NET_AES             = 0x00000008,
        NET_SSL             = 0x00000010,
        NET_COMPRESSED      = 0x00000020,
        NET_CONNECTIONLESS  = 0x00000040,
        NET_LOCAL           = 0x00000080,
        NET_DEFAULT         = 0x00000100,
        NET_UNORDERED       = 0x00000200,
        //NET_BROADCAST       = 0x00000100,
    };
    enum QualityOfService
    {
        QOS_INVALID             = 0,
        //QOS_RELIABLE            = NET_UDP|NET_RELIABLE,
        QOS_RELIABLE            = NET_TCP|NET_RELIABLE|NET_DEFAULT, // our reliable is really TCP pretending to be reliable udp
        QOS_RELIABLE_AES        = NET_UDP|NET_RELIABLE|NET_AES,
        QOS_RELIABLE_COMPRESSED = NET_UDP|NET_RELIABLE|NET_COMPRESSED, //LLR2_UDP_RELIABLE_COMPRESSION, // not supported yet
        QOS_RELIABLE_COMPRESSED_AES = NET_UDP|NET_RELIABLE|NET_COMPRESSED|NET_AES, //LLR2_UDP_RELIABLE_AES_AND_COMPRESSION,// not supported yet
        QOS_UNRELIABLE          = NET_UDP|NET_UNORDERED,
        QOS_UNRELIABLE_ORDERED  = NET_UDP,
        QOS_CONNECTIONLESS      = NET_UDP|NET_UNORDERED|NET_CONNECTIONLESS,
        //QOS_BROADCAST           = NET_UDP|NET_CONNECTIONLESS|NET_BROADCAST,
        QOS_TCP                 = NET_TCP|NET_RELIABLE,
        QOS_TCP_AES             = NET_TCP|NET_RELIABLE|NET_AES,
        QOS_TCP_COMPRESSED      = NET_TCP|NET_RELIABLE|NET_COMPRESSED, // LLR2_TCP_MSG_COMPRESSION, // not supported yet
        QOS_TCP_COMPRESSED_AES  = NET_TCP|NET_RELIABLE|NET_COMPRESSED|NET_AES, // LLR2_TCP_MSG_AES_AND_COMPRESSION, // not supported yet
        QOS_TCP_SSL             = NET_TCP|NET_RELIABLE|NET_SSL, // Not supported yet
        QOS_LOCAL               = NET_LOCAL|NET_RELIABLE,
    };

} // end namespace efd

#endif //EE_NETENUMS_H
