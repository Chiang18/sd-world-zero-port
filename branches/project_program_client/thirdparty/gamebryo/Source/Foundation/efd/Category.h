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
#ifndef EE_CATEGORY_H
#define EE_CATEGORY_H

#include <efd/UniversalID.h>
#include <efd/efdBaseIDs.h>
#include <efd/SystemFrameworkEnums.h>

namespace efd
{

/// @cond EMERGENT_INTERNAL 

/**
    Implements category masks for network messages.

    Category can only be masked using CategoryMask to provide additional type safety and avoid
    possible coding mistakes.  The 1 bits indicate that the corresponding bit in an associated
    Category is significant.  The 0 bits mask out corresponding bits, which become "don't care"
    values.
*/
class CategoryMask
{
public:
    /// @name Constructors.
    //@{
    inline CategoryMask();
    inline CategoryMask(const CategoryMask& i_other);
    inline CategoryMask(efd::UInt64 i_mask);
    //@}

    /// @name Assignment operators.
    //@{
    inline CategoryMask operator=(const CategoryMask& rhs);
    inline CategoryMask operator=(efd::UInt64 rhs);
    //@}

    /// @name Logical operators.
    //@{
    inline CategoryMask operator&(const CategoryMask& rhs) const;
    inline CategoryMask operator|(const CategoryMask& rhs) const;
    inline CategoryMask operator~() const;
    //@}

    /// @name Comparison operators.
    //@{
    inline bool operator==(const CategoryMask& rhs) const;
    inline bool operator!=(const CategoryMask& rhs) const;
    inline bool operator>(const CategoryMask& rhs) const;
    inline bool operator<(const CategoryMask& rhs) const;
    //@}

    /// Access category mask.
    inline efd::UInt64 GetValue() const;

    /**
        Returns a string of 64-bit category/mask.

        Used as a key for hash maps; also used for pretty print output logs.  Use .c_str() to get
        at the real string (for printf).
    */
    inline efd::utf8string ToString() const;

    /// Category mask.
    efd::UInt64 mask;
};

/// @endcond

/**
    A Category is used to determine the destination of a message.  A producer (sender) chooses what
    Category to send a message on while consumers (receivers) decide which categories they wish to
    receive messages from by subscribing.  This allows consumers to filter out messages they don't
    wish to receive and gives producers the ability to direct messages to groups of consumers that
    are interested.
*/
class Category : protected efd::UniversalID
{

public:
    /// Publicly expose some of the types from UniversalID.
    using UniversalID::ExpectedChannelUsage;

    /// Default constructor.
    inline Category();
    
    /**
        Constructor for Net Message category, using 64-bit unsigned int.

        @param cat 64-bit unsigned int used to initialize category or mask.
    */
    inline Category(efd::UInt64 cat);

    /**

        Constructor for Net Message category (clone operation).

        @param other Reference to existing category instance (clone operation).
    */
    inline Category(const Category &other);

    /**
        Constructor for Category, using usage, netID, and baseID.

        @param usage Expected channel usage.
        @param netID 32-bit unsigned int for net ID.
        @param baseID 32-bit unsigned int for base ID.
    */
    inline Category(ExpectedChannelUsage usage, efd::UInt32 netID, efd::UInt32 baseID);

    /// Empty destructor (avoids unused argument warnings).
    inline ~Category();

    /// @name Directly exposed members of UniversalID.
    /// @note Can get value, system, and type for verification, but cannot set them.
    //@{
    using efd::UniversalID::operator!;
    using efd::UniversalID::GetValue;
    using efd::UniversalID::GetSystem;
    using efd::UniversalID::GetType;
    using efd::UniversalID::GetUsage;
    using efd::UniversalID::SetUsage;
    //@}

    /// @name Assignment operators.
    //@{
    inline Category operator=(efd::UInt64 lhs);
    inline Category operator=(const Category& lhs);
    //@}

    /// @name Comparison operators.
    /// We don't want to be able to compare different types of IDs; we only share UniversalID so
    /// that we have a single place to put all the bit fields we use; we override other members
    /// with Category-specific implementations that still use the bit fields defined in
    /// UniversalID.
    //@{
    inline bool operator==(const Category& rhs) const;
    inline bool operator!=(const Category& rhs) const;
    inline bool operator>(const Category& rhs) const;
    inline bool operator<(const Category& rhs) const;
    //@}

    /**
        Determines whether this category represents a valid category.  To be valid, it must have
        the system bit set and be of type Channel or EntityID, and it must have some other bits
        set to non-zero values.

        @return True if this category represents a valid category.
    */
    inline bool IsValid() const;

    /**
        Tests for the specific case of a completely zero value, which is used for the sentinel
        value kCAT_INVALID.

        @note This function is not the opposite of IsValid().  A category may be set to all zero
        without assert, but any other invalid assignment will result in an assert.

        @return True if this category has a zero value.
    */
    inline bool IsZero() const;

    /**
        Gets the numeric representation of the NetID bits in the ID.

        @return Value of the Net ID that created this ID.
    */
    inline efd::UInt32 GetNetID() const;

    /**
        Sets the numeric representation of the NetID bits in the ID.
        
        The value will be ignored if it is outside of the valid range.

        @param i_netID 32-bit unsigned int representing the net ID.
        @return True if the input value is in range; false otherwise.
    */
    inline efd::Bool SetNetID(efd::UInt32 i_netID);

    /**
        Gets the numeric representation of the base bits in the ID.

        @return Value of the base ID without the extra info baked in.
    */
    inline efd::UInt64 GetBaseID() const;

    /**
        Sets the numeric representation of the base bits in the ID.
        
        The value will be ignored if it is outside of the valid range.

        @param i_base 64-bit unsigned int representing the base ID.
        @return True if the input value is in range; false otherwise.
    */
    inline efd::Bool SetBaseID(efd::UInt64 i_base);

    /**
        Returns a string of 64-bit category/mask.

        Used as key for hash maps; also used for pretty print output logs.  Use .c_str() to get at
        the real string (for printf).

        @return String representing the 64-bit category/mask.
    */
    inline efd::utf8string ToString() const;

public:
    /**
        You may cast directly to a Category Mask.

        This function is simply a convenience, so that you don't have to do "cat | kMASK_ALL" all
        over the place.
    */
    inline operator CategoryMask() const;

    /**
        Bitwise And operator.

        Keeps only bits identified by mask.

        @param mask Category identifying which bits are to be kept.
        @return 64-bit Resulting category mask.
    */
    inline CategoryMask operator&(const CategoryMask &mask) const;
};

/// @name Operators from Category.h
//@{

/// Insertion operator (<<) for inserting a Category in an output data stream.
EE_EFD_ENTRY efd::DataStream& operator<<(efd::DataStream& i_strm, const Category& i_val);

/// Extraction operator (>>) for extracting a Category from an input data stream.
EE_EFD_ENTRY const efd::DataStream& operator>>(const efd::DataStream& i_strm, Category& o_val);

/// Skip over a Category (UInt64) in an input data stream.
EE_EFD_ENTRY void DataStreamSkip(const efd::DataStream& i_strm, const Category&);

/// Insertion operator (<<) for inserting a CategoryMask in an output data stream.
EE_EFD_ENTRY efd::DataStream& operator<<(efd::DataStream& i_strm, const CategoryMask& i_val);

/// Extraction operator (>>) for extracting a CategoryMask from an input data stream.
EE_EFD_ENTRY const efd::DataStream& operator>>(const efd::DataStream& i_strm, CategoryMask& o_val);

/// Skip over a CategoryMask (UInt64) in an input data stream.
EE_EFD_ENTRY void DataStreamSkip(const efd::DataStream& i_strm, const CategoryMask&);

//@}

/// @name Common/system categories and masks from Category.h
//@{
static const efd::Category kCAT_INVALID = Category(0ull);

/// Special category used by the network system to communicate with itself.
static const efd::Category kCAT_NetSystem =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_NetSystem);

/// Special category used by the network system to notify the local system about network events.
static const efd::Category kCAT_NetEvent =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_NetEvent);

/// Special category used by the network system to implement ProducerSend.
static const efd::Category kCAT_SendToProducer =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_SendToProducer);

/// Category used by the log service.
static const efd::Category kCAT_LogService =
    Category(UniversalID::ECU_Any, kNetID_Any, kBASEID_LogService);
//@}

static const CategoryMask kMASK_ALL  = 0xFFFFFFFFFFFFFFFFull;
static const CategoryMask kMASK_NONE = 0ull;

} // end namespace efd

#include <efd/Category.inl>

#endif //EE_CATEGORY_H
