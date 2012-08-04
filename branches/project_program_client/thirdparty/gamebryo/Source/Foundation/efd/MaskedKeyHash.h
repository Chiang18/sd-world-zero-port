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
#ifndef EE_MASKEDKEYHASH_H
#define EE_MASKEDKEYHASH_H

#include <efd/IBase.h>
#include <efd/Metrics.h>

namespace efd
{

/// Templated class for a hash table of masked categories to subscribers (listeners).
template<typename KeyType, typename ValueType>
class MaskedKeyHash : public efd::MemObject
{
    /// @cond EMERGENT_INTERNAL

    EE_DECLARE_CONCRETE_REFCOUNT;

    /// @endcond

public:
    /// Constructor.
    MaskedKeyHash();

    /// Virtual destructor.
    virtual ~MaskedKeyHash();

    /// Masked key entry data structure.
    struct MaskedKey
    {
        MaskedKey(KeyType setKey, KeyType setMask):
          nodeKey(setKey),
          nodeMask(setMask) {};

        /// Category (key).
        KeyType nodeKey;

        /// Category mask.
        KeyType nodeMask;

        //@{
        /// Comparison operators
        bool operator==(const MaskedKey& ptr) const
        {
            return ((nodeKey == ptr.nodeKey) && (nodeMask == ptr.nodeMask));
        };
        bool operator!=(const MaskedKey& ptr) const
        {
            return ((nodeKey != ptr.nodeKey) || (nodeMask != ptr.nodeMask));
        };
        bool operator>(const MaskedKey& ptr) const
        {
            return ((nodeKey >  ptr.nodeKey)
                || ((nodeKey == ptr.nodeKey) && (nodeMask > ptr.nodeMask)));
        };
        bool operator<(const MaskedKey& ptr) const
        {
            return ((nodeKey <  ptr.nodeKey)
                || ((nodeKey == ptr.nodeKey) && (nodeMask < ptr.nodeMask)));
        };
        //@}
    };

    //@{
    /// Define value set, value list, and masked key set.
    typedef efd::RefCountedMemObj< efd::set<ValueType> > ValueSet;
    typedef efd::RefCountedMemObj< efd::list<ValueType> > ValueList;
    typedef efd::RefCountedMemObj< efd::set<MaskedKey> > MaskedKeySet;
    //@}

    //@{
    /// Define associated smart pointers.
    typedef efd::SmartPointer<ValueSet> ValueSetPtr;
    typedef efd::SmartPointer<ValueList> ValueListPtr;
    typedef efd::SmartPointer<MaskedKeySet> MaskedKeySetPtr;
    //@}

    /**
        Add a masked category and subscriber (listener) to the hash table.

        @param key Category.
        @param mask Category mask.
        @param value Subscriber (listener).
    */
    void AddValue(
        KeyType key,
        KeyType mask,
        ValueType value);

    /**
        Remove a subscriber (listener) from the hash table, given category, mask, and subscriber.

        @param key Category.
        @param mask Category mask.
        @param value Subscriber (listener).
    */
    void RemoveValue(
        KeyType key,
        KeyType mask,
        ValueType value);

    /**
        Remove a subscriber (listener) from the hash table, given only the subscriber.

        @param value Subscriber (listener).
    */
    void RemoveValue(ValueType value);

    /**
        Get a set of subscribers (listeners), given a category.

        @param key Category (hash key).
        @return Set of subscribers.
    */
    ValueSetPtr GetValues(KeyType key);

    /**
        Get the full set of masked keys.

        @return Set of masked keys.
    */
    MaskedKeySetPtr GetMaskedKeys();

    /// Clear the hash table.
    void clear();

protected:
    typedef efd::RefCountedMemObj< efd::map<KeyType, ValueListPtr> > MaskedKeyToValueListMapType;

    class MaskedKeyToValueListMap : public MaskedKeyToValueListMapType
    {
        EE_DECLARE_CONCRETE_REFCOUNT;
    public:
        /// Constructor.
        MaskedKeyToValueListMap(const KeyType& mask);

        /// Actual mask corresponding to category map.
        KeyType m_mask;
    };

    typedef efd::SmartPointer<MaskedKeyToValueListMap> MaskedKeyToValueListMapPtr;

    typedef efd::RefCountedMemObj< efd::map<KeyType, MaskedKeyToValueListMapPtr> >
        KeyToMaskedKeyToValueListMapMap;

    // Masked key storage.
    KeyToMaskedKeyToValueListMapMap m_maskedKeyStorage;
};

};  // End namespace efd.

#include <efd/MaskedKeyHash.inl>

#endif  //EE_MASKEDKEYHASH_H
