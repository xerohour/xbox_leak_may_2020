#ifndef __VPCALLBK_H__
#define __VPCALLBK_H__

/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: VPcallbk.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           5/28/98                                          *
*                                                                           *
\***************************************************************************/



typedef struct _sCallBack {
    DWORD       dwSize;             // should be set to the size of this structure
    DWORD       dwVersion;          // set this to 0x100
    DWORD       dwFlags;                //
    DWORD       lastBufferAddr; // address of just finished buffer ( the one the client should unload )
    DWORD    curBufferAddr;    // address of current input buffer ( one that is now being written to by MP )
    DWORD       intCount;           // free running interrupt counter - incremented for each time the callback is made
} sCallBack;

/* callback procedure pointer definition */

typedef VOID (* CALLBPROC)(sCallBack*);

/*  These macros should will be ORed together to form the flags field  */

/* field type */                        // Bit 0
#define     ODDFIELD            0x1
#define     EVENFIELD       0x0
/* address type */                  // bits 1-2 
                                            // indicates that the buffer address really an offset from the base of the frame buffer memory
#define     BUFFADROFFSET   0x2 
                                            // indicates that the buffer address is a Physical address
#define     BUFFADRPHYS     0x4
                                            // indicates that the buffer address is a linear address ( see note below)
#define     BUFFADRLIN      0x6
                                            // indicates that the buffer address is an index into the list from DDRAW
#define     BUFFADRINDEX    0x0     
/* surface type */                  //bits 4
                                            // indicates that the surface contains VBI data
#define     VBISURFACE      0x10        
                                            // indicates that the surface contains IMAGE data
#define     IMAGESURFACE    0x00        
                                            
                                            // should always be ORed in to indicate that the flags are valid
#define     USINGFLAGS      0x80        

/*   NOTES

Notes for the client:

Allways hold the callback routine in locked memory.
When setting the callback, it can be called even before SetCallback returned. In this
case the callback routine cannot know about the context. So the callback routine needs
to ignore calls until it knows about the context.

Notes about Linear Address:

If a linear address is used, this address must be a ring 3 addressable Address in System range.
It should be the same address as ddSurface->Lock method returns.

Notes about Surface Index:

If this is to be used the index order should be the determined by the
attachment order when the surfaces are created (FrontBuffer = 0,BackBuffer = 1 ....)
This is nornally indetically to the flipping order. ???

*/



// USE THIS IOCTL CODE TO GET YOUR SETCALLBACK FUNCTION POINTER:
#define VDD_IOCTL_NV_GETSETCALLBACK  0x20000668     
//  This IOCTL allows you to get an address for a routine which can be called to set the callback.
//  The idea behind the indirection is that an application can use the IOCTL to get an address
//  which an associated VXD can use to set callbacks into itself.  The VXD is also capable of resetting
//  the callbacks in the situation where the application has crashed and the VXD is unloading itself.
//  input to IOCTL is U032 and is ignored
//  output to IOCTL is U032 and is a function pointer to a function ie  SETCALLBPROC



// USE the following structures to actually SET a callback
// input pointer points to a structure
typedef struct _inSCBStruct {
    DWORD       flags;
    CALLBPROC   pTheCallback;
} inSCBStruct;
// output pointer points to a structure
typedef struct _outSCBStruct {
    DWORD   returnValue;
} outSCBStruct;

// USE THIS FUNCTION POINTER TO SET YOUR CALLBACK:

//DWORD vddSetCallback(inSCBStruct* inbuffer, outSCBStruct* outbuffer);
typedef VOID (* SETCALLBPROC)(inSCBStruct*  inbuffer, outSCBStruct* outbuffer);


/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


// Here is some real basic example code

#ifdef JUST_EXAMPLE_CODE
        HANDLE bVXDHandle;
        U032     inBuffer;
        U032     outputSize;
        SETCALLBPROC    pSetCallback;
        
        inBuffer = 0;       
        
#ifdef NV3      
        bVXDHandle = CreateFile("\\\\.\\NV3", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#else
        bVXDHandle = CreateFile("\\\\.\\NV4", 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
#endif  // NV4      
        if((bVXDHandle != NULL ) && (bVXDHandle != INVALID_HANDLE_VALUE ) ){
            if(!DeviceIoControl(bVXDHandle,
                                        VDD_IOCTL_NV_GETSETCALLBACK,        // control code of operation to perform
                                        &inBuffer,                          // pointer to buffer to supply input data
                                        4,                                      // size of input buffer
                                        &pSetCallback,                      // pointer to buffer to receive output data
                                        4,                                      // size of output buffer
                                        &outputSize,                        // pointer to variable to receive output byte count
                                        NULL                                    // pointer to overlapped structure for asynchronous operation
                                    )
                 ){                 
                        // things worked, the
                        inSCBStruct     inBuf;
                        outSCBStruct    outBuf;
                        // NOTE USE OF FLAGS FROM VPCALLBk.h
                        inBuf.flags = IMAGESURFACE;
                        inBuf.pTheCallback = JoeCoolDudesCallbackRoutine;
                        
                        (pSetCallback)(&inBuf,&outBuf);
                        
            } else {
                        
                        // bummer, for some reason, we were unable to set your callback
            
            }
                    
            CloseHandle(bVXDHandle);
                                    
        }
        
        
#endif  //JUST_EXAMPLE_CODE 
    
    

#endif //__VPCALLBK_H__