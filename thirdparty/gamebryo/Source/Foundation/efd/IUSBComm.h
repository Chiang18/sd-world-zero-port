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
#ifndef EE_IUSBCOMM_H
#define EE_IUSBCOMM_H

#include <efd/RTLib.h>
#include <efd/CriticalSection.h>
#include <efd/StringUtilities.h>
#include <efd/ReferenceCounting.h>
#include <efd/SmartCriticalSection.h>

#if defined(EE_PLATFORM_WIN32)
#pragma warning(disable: 4275)
#endif

namespace efd
{
enum MessageType
{
    MSG_OPEN_FILE                       = 1,  //0x01
    MSG_READY_FOR_FILENAME_SIZE         = 2,  //0x02
    MSG_SENT_FILENAME_SIZE              = 3,  //0x03
    MSG_RECEIVED_FILENAME_SIZE          = 4,  //0x04
    MSG_SENT_FILENAME                   = 5,  //0x05
    MSG_SENT_FILE_INFO                  = 6,  //0x06
    MSG_SENT_FILE_READ_REQUEST          = 7,  //0x07
    MSG_SENT_FILE_READ_ACTUAL_BYTES     = 8,  //0x08
    MSG_SENT_FILE_WRITE_REQUEST         = 9,  //0x09
    MSG_SENT_FILE_DATA                  = 10, //0x0a
    MSG_SENT_FILE_WRITE_SUCCESS         = 11, //0x0b
    MSG_SENT_FILE_SEEK_REQUEST          = 12, //0x0c
    MSG_FILE_SEEK_COMPLETED             = 13, //0x0d
    MSG_FILE_CLOSE                      = 14, //0x0e
    MSG_ENSURE_DIRECTORY                = 15, //0x0f
    MSG_READY_FOR_DIRECTORY_SIZE        = 16, //0x10
    MSG_SENT_DIRECTORY_SIZE             = 17, //0x11
    MSG_RECEIVED_DIRECTORY_SIZE         = 18, //0x12
    MSG_SENT_DIRECTORY_NAME             = 19, //0x13
    MSG_DIRECTORY_ENSURED               = 20, //0x14
    MSG_RECEIVED_ACTUAL_BYTES           = 21, //0x15
    MSG_RECEIVED_FILE_WRITE_REQUEST     = 22, //0x16
    MSG_FILE_CLOSE_SUCCESS              = 23, //0x17
    MSG_SENT_FILE_FLUSH_REQUEST         = 24, //0x18
    MSG_FILE_FLUSH_COMPLETED            = 25, //0x19
    MSG_FILE_EXISTS_REQUEST             = 26, //0x1a
    MSG_FILE_EXISTS_REQUEST_READY       = 27, //0x1b
    MSG_SENT_FILE_EXISTS_RESPONSE       = 28, //0x1c
    MSG_RESERVED                        = 29, //0x1d
    MSG_RECEIVED_FILENAME               = 30, //0x1e
    MSG_SENT_FILE_OPEN_PARAMS           = 31, //0x1f
    MSG_BEGIN_TRANSACTION               = 32, //0x20
    MSG_BEGIN_TRANSACTION_ACK           = 33, //0x21
    MSG_END_TRANSACTION                 = 34, //0x22
    MSG_END_TRANSACTION_ACK             = 35, //0x23
    MSG_MAX                             = 36  //0x24
};

enum
{
    EE_USB_LEGACY_MAX = MSG_MAX,       // 36, //0x24 /// Maximum value usable by file interface
    EE_USB_OP_NONE,                    // 37, //0x25
    EE_USB_OP_FLUSH,                   // 38, //0x26
    EE_USB_OP_FLUSH_ACK,               // 39, //0x27
    EE_USB_OP_CONNECT,                 // 40, //0x28
    EE_USB_OP_CONNECT_ACK,             // 41, //0x29
    EE_USB_OP_RECONNECT                // 42, //0x30
};

class USBInterruptCallback
{
public:
    virtual ~USBInterruptCallback();
    virtual void HandleInterrupt() = 0;
};

/**
    This class provides an abstract interface for accessing a platform specific USB device with
    two UInt32 used for sending interrupt notifications and a shared buffer for communication.
*/
class EE_EFD_ENTRY IUSBComm
{
    EE_DECLARE_ABSTRACT_REFCOUNT;
public:
    
    /// @name Construction and Destruction
    //@{
    IUSBComm();
    virtual ~IUSBComm();
    //@}

    /// Should be called once at app startup to initialize USB.
    virtual bool InitUSB()=0;

    /// Called by legacy interface to begin a conversation
    virtual bool BeginTransaction()=0;

    /// Called by legacy interface to end a conversation
    virtual bool EndTransaction()=0;

    
    /// @name IUSBComm lock management
    //@{
    /// Attempt to obtain a lock on the USB device
    /// @return a SmartCriticalSection that is already locked
    virtual CriticalSection& SmartLockDevice()=0;

    /// Attempt to obtain a lock on the USB device.
    virtual bool LockDevice()=0;

    /// Release lock on the USB device.
    virtual bool UnlockDevice()=0;
    /**
        Check the status of the lock on the USB device.
        @return true if the device is locked.
    */
    virtual bool IsDeviceLocked()=0;
    //@}

    
    /// @name IUSBComm data transfer
    //@{

    /// Get Mail for the legacy synchronous communication system
    virtual bool GetLegacyMail(unsigned long& ulMsg)=0;
    /**
        Get the next pending queued message.
        @param ulMsg message sent from other side.
        @param readLegacyMail if true messages less than EE_USB_LEGACY_MAX will be returned and 
            a message greater than or equal to EE_USB_LEGACY_MAX at the head of the queue will be
            treated as if there are no messages waiting.  If false the behavior is reversed; if a
            message greater than or equal to EE_USB_LEGACY_MAX is waiting it will be returned.
            Otherwise it will be treated as if there is no message waiting.
        @return true if there is mail waiting, false if the queue is empty.
    */
    virtual bool GetMail(unsigned long& ulMsg)=0;
    /**
        Block until the message specified by ulMsg is received from the other side.
        @param ulMsg message sent from other side
        @return true if there is mail waiting, false if the queue is empty or the wrong message 
        was received
    */
    virtual bool WaitForMail(const unsigned long ulMsg)=0;
    /**
        Send a 32 bit mail message to the other end of communications.  This will generate an
        interrupt on the receiving side.
        @param ulMsg message to send
    */
    virtual bool SendMail(const unsigned long ulMsg)=0;
    /**
        Read a UInt out of the shared buffer at position 0 swapped for platform Endianness.
        @param uiData UInt32 to read into
        @return: true if int was successfully read from buffer
    */
    virtual bool GetUInt(unsigned int& uiData)=0;
    /**
        Read a UInt out of the shared buffer at position uiOffset swapped for platform Endianness.
        @param uiData UInt32 to read into.
        @param uiOffset offset into the buffer to read the UInt32.
        @return: true if int was successfully read from buffer.
    */
    virtual bool GetUInt(unsigned int& uiData, const unsigned int uiOffset)=0;
    /**
        Write a UInt into the shared buffer at position 0 swapped for platform Endianness.
        @param uiData UInt32 to write.
        @return: true if int was successfully written to buffer.
    */
    virtual bool SendUInt(const unsigned int uiData)=0;
    /**
        Write a UInt into the shared buffer at position uiOffset swapped for platform Endianness.
        @param uiData UInt32 to write.
        @param uiOffset offset into the buffer to write the UInt32.
        @return: true if int was successfully written to buffer.
    */
    virtual bool SendUInt(const unsigned int uiData, const unsigned int uiOffset)=0;
    /**
        Read data out of the shared buffer at position uiOffset.
        @param pData buffer to read into.
        @param uiOffset offset into the buffer to read.
        @param uiSize The size of data that will be read into pData in bytes.
        @return: true if data was successfully read from buffer.
    */
    virtual bool GetData(void* pData, const unsigned int uiOffset, const unsigned int uiSize)=0;
    /**
        Read data out of the shared buffer at position 0.
        @param pData buffer to read into.
        @param uiSize The size of data that will be read into pData in bytes.
        @return: true if data was successfully read from buffer.
    */
    virtual bool GetData(void* pData, const unsigned int uiSize)=0;
    /**
        Write data into the shared buffer at position uiOffset.
        @param pData buffer to write from.
        @param uiOffset offset into the buffer to write.
        @param uiSize The size of data that will be written from pData into the USB buffer.
        @return: true if data was successfully written to buffer.
    */
    virtual bool SendData(
        const void* pData,
        const unsigned int uiOffset,
        const unsigned int uiSize)=0;
    /**
        Write data into the shared buffer at position 0.
        @param pData buffer to write from.
        @param uiSize The size of data that will be written from pData into the USB buffer.
        @return: true if data was successfully written to buffer.
    */
    virtual bool SendData(const void* pData, const unsigned int uiSize)=0;
    //@}

    /// Get the maximum size of the USB buffer used to transfer data
    virtual UInt32 GetBufferSize()=0;

    /// Set the interrupt callback (optional)
    virtual void SetUSBInterruptCallback(USBInterruptCallback* pUSBInterruptCallback);

    /// returns true if the USB interface is initialized
    virtual bool IsInitialized()=0;

    /// returns true if the required dlls have been successfully loaded
    virtual bool ModulesLoaded()=0;

    /// Set the static singleton instance
    static void InitIUSBComm(efd::SmartPointer<IUSBComm> spUSBComm);

    /// to be called when USB needs to be reinitialized
    static void ReInitIUSBComm();

    /// IUSBComm is a singleton class.  m_spIUSBComm must be initialized externally using the 
    /// static InitIUSBComm function.  Otherwise this function will return NULL.
    static IUSBComm* Instance();

    /// Destroy the singleton
    static void _SDMShutdown();

protected:
    USBInterruptCallback* m_pUSBInterruptCallback;

    // Singleton instance
    static efd::SmartPointer<IUSBComm> m_spIUSBComm;

    // regular pointer to singleton instance.  Nulled out in destructor, if anyone anywhere has 
    // a SmartPointer to the singleton this pointer will still be valid
    static IUSBComm* m_pIUSBComm;
};
typedef efd::SmartPointer<IUSBComm> IUSBCommPtr;

/// This simple queue class makes a copy of any data
/// enqueued.  It should only be used with primitives.
template<class T>
class PrimitiveQueue
{
public:

    /// @name Construction and Destruction
    //@{
    PrimitiveQueue();
    virtual ~PrimitiveQueue();
    //@}

    /**
    Add an item to the queue.
    @param data item to add.
    */
    bool Enqueue(const T data);

    /// Remote the head item from the queue.
    /// @param data if successful contains the value from the head of the queue.
    bool Dequeue(T& data);

    /// Read at the specified index
    /// @param index index to read at
    /// @param data will contain the value at index
    void Read(UInt32 index, T& data);

    /// Write to the specified index
    /// @param index index to write to
    /// @param data data to write
    void Write(UInt32 index, const T data);

    /// Get the current index of the head
    UInt32 GetHeadPos();

    /// Get the current index of the tail
    UInt32 GetTailPos();

    /**
    Read the head item from the queue.
    @return: the head item from the queue.
    */
    T Head() const;

    /**
    Query if the queue is empty.
    @return: true if the queue is empty.
    */
    bool IsEmpty() const;

    /**
    Query if the queue is full.
    @return: true if the queue is full.
    */
    bool IsFull() const;

    /**
    Remove all items from the queue.
    */
    void Clear();

protected:

    /// max number of items in queue.
    UInt32 m_Capacity;

    /// index into m_Data of the head of the queue.
    UInt32 m_Head;

    /// index into m_Data of the tail of the queue.
    UInt32 m_Tail;

    /// simple array of items used to store the queue.
    T* m_Data;
};

template<class T>
class AllocatedQueue : public PrimitiveQueue<T>
{
public:
    AllocatedQueue();
    virtual ~AllocatedQueue();
};

}; // end namespace efd

#include <efd/IUSBComm.inl>

#endif // EE_IUSBCOMM_H

