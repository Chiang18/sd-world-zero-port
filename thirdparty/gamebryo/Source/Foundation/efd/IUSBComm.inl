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
inline USBInterruptCallback::~USBInterruptCallback()
{
}
//-------------------------------------------------------------------------------------------------
inline void IUSBComm::InitIUSBComm(efd::SmartPointer<IUSBComm> spUSBComm)
{
    if (!m_pIUSBComm)
    {
        m_pIUSBComm = spUSBComm;
        m_spIUSBComm = spUSBComm;
        m_spIUSBComm->InitUSB();
    }
}
inline void IUSBComm::ReInitIUSBComm()
{
    if (m_pIUSBComm)
    {
        m_spIUSBComm->InitUSB();
    }
}
//-------------------------------------------------------------------------------------------------
inline IUSBComm* IUSBComm::Instance()
{
    return m_pIUSBComm;
}
//-------------------------------------------------------------------------------------------------
inline void IUSBComm::_SDMShutdown()
{
    // release our SmartPointer, if anyone else is still holding a reference destruction will be
    // delayed and m_pIUSBComm will still be non-NULL
    m_spIUSBComm = NULL;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    PrimitiveQueue<T>::PrimitiveQueue()
    : m_Capacity(10), m_Head(0), m_Tail(0)
{
    m_Data = EE_EXTERNAL_ALLOC(T, m_Capacity);
    EE_ASSERT(m_Data);
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    PrimitiveQueue<T>::~PrimitiveQueue()
{
    EE_EXTERNAL_FREE(m_Data);
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    bool PrimitiveQueue<T>::Enqueue(const T data)
{
    if (IsFull())
        return false;

    m_Data[m_Tail] = data;
    m_Tail = (m_Tail + 1) % m_Capacity;
    return true;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    bool PrimitiveQueue<T>::Dequeue(T& data)
{
    if (IsEmpty())
        return false;
    data = Head();
    m_Head = (m_Head + 1) % m_Capacity;
    return true;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    T PrimitiveQueue<T>::Head() const
{
    return m_Data[m_Head];
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    bool PrimitiveQueue<T>::IsEmpty() const
{
    return m_Head == m_Tail;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    bool PrimitiveQueue<T>::IsFull() const
{
    return m_Head == ((m_Tail + 1) % m_Capacity);
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
    void PrimitiveQueue<T>::Clear()
{
    m_Head = m_Tail = 0;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
void PrimitiveQueue<T>::Read(UInt32 index, T& data)
{
    EE_ASSERT(index < m_Capacity);
    data = m_Data[index];
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
void PrimitiveQueue<T>::Write(UInt32 index, const T data)
{
    EE_ASSERT(index < m_Capacity);
    m_Data[index] = data;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
UInt32 PrimitiveQueue<T>::GetHeadPos()
{
    return m_Head;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
UInt32 PrimitiveQueue<T>::GetTailPos()
{
    return m_Tail;
}
/// Set the interrupt callback (optional)
inline void IUSBComm::SetUSBInterruptCallback(USBInterruptCallback* pUSBInterruptCallback)
{
    m_pUSBInterruptCallback = pUSBInterruptCallback;
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
AllocatedQueue<T>::AllocatedQueue()
{
    for (UInt32 i = 0; i < this->m_Capacity; ++i)
    {
        // placement new each object
        new(&this->m_Data[i]) T;
    }
}
//-------------------------------------------------------------------------------------------------
template<class T> inline
AllocatedQueue<T>::~AllocatedQueue()
{
    for (UInt32 i = 0; i < this->m_Capacity; ++i)
    {
        // manually call the destructor for each object
        this->m_Data[i].~T();
    }
}

}; // end namespace efd
