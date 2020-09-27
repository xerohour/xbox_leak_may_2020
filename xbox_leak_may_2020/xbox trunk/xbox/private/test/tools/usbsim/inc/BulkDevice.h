/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    BulkDevice.h

Abstract:

    USB Device child class that handles MU enumeration and storage

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _BULKDEVICE_H_
#define _BULKDEVICE_H_

#pragma pack(push, 1) // align structure members on a byte boundry
struct CBW
    {
    unsigned __int8  pid;

    // header
    unsigned __int32 sig;           // CBW Signature - same for all transactions
    unsigned __int32 tag;           // CBW Tag - unique identifier for a transaction, returned in CSW
    unsigned __int32 dataLength;    // Length (in bytes) of the data stage of the transaction
    unsigned __int8  flag;          // Sets data direction among other things, 0x00-WRTIE, 0x80-READ
    unsigned __int8  drive;         // Sets Logical Unit - something to do with have multiple drives in on device
    unsigned __int8  length;        // Length of the valid portion of the RBC

    // RBC
    unsigned __int8  opCode;        // Command ID
    unsigned __int8  rbcDrive;      // Bitwise field containing LUN
    BigEndian<unsigned __int32> address;       // Logical Block Address - Address in Block numbers of the destination
    unsigned __int8  reserved2;     
    unsigned __int16 param2;        // RBC Length - Length in Block numbers of the data
    unsigned __int8  control;       
    unsigned __int8  fill[6];       
    };
struct CSW
    {
    unsigned __int8  pid;

    // header
    unsigned __int32 sig;
    unsigned __int32 tag;
    unsigned __int32 dataLength;
    unsigned __int8  result;
    };
struct MUCapacity
    {
    BigEndian<unsigned __int32> numBlocks;
    BigEndian<unsigned __int16> logicalSize;
    BigEndian<unsigned __int16> blockSize;
    };
#pragma pack(pop)


/*****************************************************************************

Class Description:

    BulkDevice

    USB Device child class that handles MU enumeration and storage

Notes:
    

*****************************************************************************/
class BulkDevice : public USBDevice
    {
    public:
        unsigned enumerateState[10];

    protected:
        MUCapacity capacity;
        unsigned __int8 *storage;

    public:
        BulkDevice();
        ~BulkDevice();

    // virtual member functions
    public:
        DWORD EndpointConfig(unsigned config);
        DWORD Receive(void);
        const char* GetName(void);
        bool IsEnumerated(void);

    protected:
        DWORD HandleSETUP(void);
        DWORD HandleOUT(void);
        DWORD HandleIN(void);

    public:
        void SetCapacity(DWORD numBlocks, unsigned short blockSize, unsigned short logicalSize);
        DWORD StoreMU(char *filename);
        DWORD LoadMU(char *filename);

    public:
        friend class CMU; // give the COM object access to us
    };

#endif // _BULKDEVICE_H_
