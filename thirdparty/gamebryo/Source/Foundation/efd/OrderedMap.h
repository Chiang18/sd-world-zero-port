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
#ifndef EE_ORDEREDMAP_H
#define EE_ORDEREDMAP_H

#include <efd/StdContainers.h>



namespace efd
{

    // OrderedMap supports both list-like ordering and map-like key based lookup.
    template< typename KEY, typename VALUE >
    class OrderedMap
    {
    public:
        typedef EE_STL_NAMESPACE::pair<KEY, VALUE> NodeType;
        typedef efd::list< NodeType* > OMList;
        typedef efd::map< KEY, NodeType* > OMMap;
        typedef typename OMList::iterator iterator;

        /// Returns the begin iterator for the list member
        iterator begin()
        {
            return m_list.begin();
        }

        /// Returns the end iterator for the list member
        iterator end()
        {
            return m_list.end();
        }

        /**
            Adds value to both the map and list members
            @param i_key map key for the value to add
            @param i_value value to add to map and list
        */
        bool push_back( const KEY& i_key, const VALUE& i_value )
        {
            typename OMMap::iterator iter = m_map.find( i_key );
            if ( iter != m_map.end() )
            {
                return false;
            }

            NodeType* pNode = EE_EXTERNAL_NEW NodeType( i_key, i_value );
            m_list.push_back( pNode );
            m_map[i_key] = pNode;

            return true;
        }

        /**
            Finds a value in the map member.
            @param i_key the key to find in the map
        */
        NodeType* find( const KEY& i_key )
        {
            typename OMMap::iterator iter = m_map.find( i_key );
            if ( iter == m_map.end() )
            {
                return NULL;
            }
            return iter->second;
        }

        /**
            Finds a value in the map member.  Const version.
            @param i_key the key to find in the map
        */
        const NodeType* find( const KEY& i_key ) const
        {
            typename OMMap::iterator iter = m_map.find( i_key );
            if ( iter == m_map.end() )
            {
                return NULL;
            }
            return iter->second;
        }

        /**
            Removes a value from both the map and list members
            @param i_key the key to erase from both the map and list
        */
        bool erase( const KEY& i_key )
        {
            typename OMMap::iterator iter = m_map.find( i_key );
            if ( iter == m_map.end() )
            {
                return false;
            }

            NodeType* p = iter->second;
            m_list.remove( iter->second );
            m_map.erase( iter );
            EE_EXTERNAL_DELETE p;

            return true;
        }

        /// Clears both the map and list members.
        void clear()
        {
            m_list.clear();
            m_map.clear();
        }

        /// Returns the first element in the list member.
        const NodeType* front() const
        {
            if (m_list.empty())
            {
                return NULL;
            }
            const NodeType* p = m_list.front();
            return p;
        }

        /// Returns true if the list member is empty.
        bool empty()
        {
            return m_list.empty();
        }

        /// Removes the first element from both the map and list members.
        bool pop_front()
        {
            if (m_list.empty())
            {
                return false;
            }

            NodeType* p = m_list.front();
            m_map.erase( p->first );
            m_list.pop_front();
            EE_EXTERNAL_DELETE p;

            return true;
        }

        /**
            Removes the first element from both the map and list members.
            @param o_key map key for the first value in the list
            @param o_value the first value in the list
        */

        bool pop_front(KEY& o_key, VALUE& o_value)
        {
            if (m_list.empty())
            {
                return false;
            }

            NodeType* p = m_list.front();
            o_key = p->first;
            o_value = p->second;
            m_map.erase( p->first );
            m_list.pop_front();
            EE_EXTERNAL_DELETE p;

            return true;
        }

    protected:

        OMList m_list;
        OMMap  m_map;
    };


} // end namespace efd


#endif // EE_ORDEREDMAP_H
