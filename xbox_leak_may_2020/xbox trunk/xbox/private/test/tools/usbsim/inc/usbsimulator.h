/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    USBSimulator.h

Abstract:

    Defines the Simulator control class and the Device base class.

    Library allows for this sort of usage:

        usbsim.FindSimulators();

        XIDDevice *duke = new XIDDevice();
        usbsim.Plug(1, 1, duke);
        Sleep(...)
        duke->SetInputReport(...); // button a down
        Sleep(...)
        duke->SetInputReport(...); // button a up, dpad left
        usbsim.Unplug(duke);
        delete duke;

        BulkDevice *mu = new BulkDevice();
        usbsim.Plug(2, 1, mu);
        Sleep(15000);              // let stuff happen
        usbsim.Unplug(mu);
        delete mu;

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _USB_SIMULATOR_H_
#define _USB_SIMULATOR_H_

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <stdio.h>
#include <crtdbg.h>

#ifdef _XBOX
    #include <xtl.h>
#else
    #include <windows.h>
#include <winsock2.h>
#endif

#include "CWorkerThread.h"
#include "SimPacket.h"
#include "consts.h"

#define ARRAYSIZE(ptr)      (sizeof(ptr) / sizeof(ptr[0]))


//
// Magic Numbers
//
#define USBSIM_MAX_SIMULATORS           4   // # ports on an xbox
#define USBSIM_MAX_USBPORTS             4   // # usb devices on a simulator
#define USBSIM_RECV_RETRYS              4   // # of times network code will retry an operation
#define USBSIM_UDP_DELAY              200   // milliseconds between fast sends (UDP stack on sim drops packets if too fast)
#define KILOBYTE                     1024   
#define MEGABYTE      (KILOBYTE*KILOBYTE)   



class Recorder
    {
    private:
        LARGE_INTEGER counterFrequency;
        LARGE_INTEGER startTime;

    public:
        int style;
        char filename[MAX_PATH];
        char fullFilename[MAX_PATH];

    public:
        Recorder();

    public:
        void SetFilename(char *name);
        void SetFormat(int formatType);
        void LogPacket(char *data, int len, DWORD deviceID, bool outGoing);
        unsigned LogPrint(char* format, ...);

    };
extern Recorder recorder; // global recording object


class USBDevice;

/*****************************************************************************

Class Description:

    USBSimulator

    This object manages information on remote simulators, and allows a device
    class to attach to a given simulator.

Notes:
    

*****************************************************************************/
class USBSimulator
    {
    public:
        DWORD ip[USBSIM_MAX_SIMULATORS]; // == to S_addr in the in_addr struct

    public:
        bool record;        // used to initialize the devices record settings

    public:
        USBSimulator();
        USBSimulator(bool startRecording, int recordingFormat);
        ~USBSimulator();

    public:
        DWORD Plug(int port, char simulator, USBDevice *vdevice);
        DWORD Unplug(USBDevice *vdevice);

    public:
        DWORD FindSimulators(void);
        DWORD SetIP(char simulator, char *ipAddr);
        template<class DataType> DWORD SendCommand(char simulator, unsigned __int8 command, unsigned __int8 subcommand, DataType &data)
            {
            // NOTE: This member function is inlined because of template limitations in VC
            SOCKADDR_IN dest;
            SOCKET sock;
            SimPacketHeader request = { command, subcommand, 0, 0 };
            SimPacketTyped<DataType> response;
            int addrsize=sizeof(SOCKADDR_IN);
            int err;

            if(simulator > USBSIM_MAX_SIMULATORS || simulator < 0 || ip[simulator] == 0) return USBSIM_ERROR_INVALID_SIMULATOR;

            sock = socket(AF_INET, SOCK_DGRAM, 0);
            dest.sin_family = AF_INET;
            dest.sin_port = htons(SIM_NETPORT_BASE);
            dest.sin_addr.s_addr = ip[simulator];
            err = sendto(sock, (char*)&request, sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, addrsize);
            if(err == SOCKET_ERROR)
                {
                shutdown(sock, SD_BOTH);
                closesocket(sock);
                return USBSIM_ERROR_SOCKET_ERROR;
                }

            TIMEVAL timeout;
            FD_SET bucket;
            bucket.fd_count = 1;
            bucket.fd_array[0] = sock;
            timeout.tv_sec = 0;
            timeout.tv_usec = 1000000; // 1 second
            err = select(0, &bucket, NULL, NULL, &timeout);
            if(err == 0 || err == SOCKET_ERROR)
                {
                shutdown(sock, SD_BOTH);
                closesocket(sock);
                return USBSIM_ERROR_TIMEOUT;
                }

            err = recvfrom(sock, (char*)&response, sizeof(response), 0, (SOCKADDR*)&dest, &addrsize);
            if(err == 0 || err == USBSIM_ERROR_SOCKET_ERROR)
                {
                shutdown(sock, SD_BOTH);
                closesocket(sock);
                return USBSIM_ERROR_SOCKET_ERROR;
                }

            shutdown(sock, SD_BOTH);
            closesocket(sock);

            memcpy(&data, &response, min(sizeof(DataType), err));
            return USBSIM_ERROR_OK;
            }

    public:
        friend class CSimulator;
    };


/*****************************************************************************

Class Description:

    USBDevice

    This class defines the core functionality for every "device". All devices
    are children of this object.

Notes:
    

*****************************************************************************/
class USBDevice : public CWorkerThread
    {
    private:
        TIMEVAL timeout;

    protected:
        bool record;            // wether or not to log traffic
        DWORD simIP;            // simulator IP address
        char usbPort;           // port we are on
        unsigned short destPort;// net port number in network byte order
        SOCKET sock;

    protected: // usb communication
        char *receiveBuffer;    // buffer which receives incomming data
        unsigned bufferSize;    // max size of above buffer
        bool dataToggle;        // keep track of DATA0(false) vs DATA1(true)
        unsigned __int8 deviceDescriptor[8];
        unsigned __int8 configurationDescriptor[32];

    public:
        USBDevice();
        virtual ~USBDevice();

    protected:
        int IsDataAvailable(void);
        DWORD GetPacket(void);
        DWORD WaitForUSBPacket(int command, int subcommand, int pid);
        int recv(SOCKET s, char *buf, int len, int flags);
        int send(SOCKET s, char *buf, int len, int flags);
        int sendto(SOCKET s, char *buf, int len, int flags, struct sockaddr *to, int tolen);
        int recvfrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen);

    public:
        DWORD Plug(void);
        DWORD Unplug(void);
        DWORD SetupEndpoint(unsigned endpoint, unsigned type, unsigned fifoSize, unsigned autoRepeat);

    // virtual member functions
    public:
        DWORD ThreadFunct(void);
        virtual DWORD EndpointConfig(unsigned config) = 0;
        virtual DWORD Receive(void) = 0;
        virtual const char* GetName(void) = 0;
        virtual bool IsEnumerated(void) = 0;

    public:
        friend class USBSimulator; // give the USBSimulator access to us
    };


//
// Debug output printing functions
//
void DebugPrint(char* format, ...);
void PrintPacket(SimPacket *packet);

//
// Utility functions
//
const char* const GetPIDName(unsigned char pid);

//
// Dealing with Endians
//
#define ENDIAN_SHIFT_32(v) ((((v) >> 24) & 0x000000FFL) | (((v) >>  8) & 0x0000FF00L) | (((v) <<  8) & 0x00FF0000L) | (((v) << 24) & 0xFF000000L))
#define ENDIAN_SHIFT_16(v) ((unsigned __int16)((((v) >> 8) & 0x00FF) | (((v) << 8) & 0xFF00)))
template <class T> class BigEndian
    {
    private:
        T value;

    public:
        // Read
        //operator unsigned __int16()             { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator unsigned short()               { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        //operator __int16()                      { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator short()                        { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }

        //operator unsigned __int32()             { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator unsigned long()                { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator unsigned int()                 { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        //operator __int32()                      { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator long()                         { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }
        operator int()                          { return sizeof(T)==2?ENDIAN_SHIFT_16(value):ENDIAN_SHIFT_32(value); }

        // Write
        //T operator=(unsigned __int16 newVal)    { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(unsigned short newVal)      { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        //T operator=(__int16 newVal)             { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(short newVal)               { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }

        //T operator=(unsigned __int32 newVal)    { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(unsigned long newVal)       { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(unsigned int newVal)        { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        //T operator=(__int32 newVal)             { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(long newVal)                { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
        T operator=(int newVal)                 { return value = (T)(sizeof(T)==2?ENDIAN_SHIFT_16(newVal):ENDIAN_SHIFT_32(newVal)); }
    };



//
// All child devices will go here:
//
#include "XIDDevice.h"      // Duke XID device
#include "BulkDevice.h"     // MU Bulk device
#include "IsocDevice.h"     // Isoc device


#endif // _USB_SIMULATOR_H_
