/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: vddsurfs.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           5/22/98                                          *
*                                                                           *
\***************************************************************************/

#define RESMANSIDE
#define INVDDCODE

#ifdef NV4_HW
#define NV4
#endif
#ifdef NV10_HW
#define NV4
#endif

#include "windows.h"
#include "ddraw.h"
#include "Ddrawp.h"
#include "ddrawi.h"
// #include "nvPusher.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
//  vdd specific MS includes
#include "minivdd.h"
#include "ddkmmini.h"

#include "vmm.h"

// NV includes
#include "nvwin32.h"
#include "nvhw.h"
#include <nv_ref.h>
#include "nvrmr0api.h"

#include "nvddobj.h"

// This is here because it is defined in nvrmarch.inc. We can't include that file here.
#define NV_WIN_DEVICE                            0x00002002

// unit specific includes
#include "surfaces.h"
#include "vddsurfs.h"
#include "VPcallbk.h"

#ifdef NV4
#include "VidTex.h"
#endif

//#define  DPF(a)    /* a */
#ifndef DPF

#if 1
#define  DPF(a) _Debug_Printf_Service(a)
#else
#define START_STR  ("VDDSURF - ")
#define END_STR ("\n")

void __cdecl DPF(LPSTR szFormat, ...)
{
    char str[256];

    lstrcpy(str, START_STR);
    wvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
    lstrcat(str, END_STR);
    OutputDebugString(str);
}
#endif  // if 1
#endif  // #ifndef DPF
//  things needing entry points
//        ProgramBufferWithSurface

#ifdef ASSERT
#undef ASSERT

#if 0
#define ASSERT(a)  NULL;
#else


#ifndef NV06A_FIFO_GUARANTEED_SIZE
#define NV06A_FIFO_GUARANTEED_SIZE  (NV_GUARANTEED_FIFO_SIZE)
#endif

#define ASSERT(a,b)   \
    if(a) {         \
    } else {        \
        DPF(b);   \
    }                
    
#endif  // 1

#endif  // ASSERT


#define NV3FBLIMIT  (0x3E8000)
void AssertFail(void) {
    DPF("AssertFail\n");
}

vpSurfaces             MySurfaces;
NvNotification        nvMyVPNotifiers[NUMVPNOTIFIERS];


FIELDTYPE    getFieldFromBuffer(U032 dwBuffIndex) {
    U032 daField;
    
    switch (MySurfaces.CurrentCaptureMode) {
            case EVENODD:
              if(dwBuffIndex == 0 ) {
                  daField = ODD;
                } else if(dwBuffIndex == 1 ) {
                  daField = EVEN;
                } else {
                  daField = UNKNOWN;
                }
                break;
            case EVENEVEN:
                daField = EVEN;
                break;
            case ODDODD:
                daField = ODD;
                break;
            default:
                ASSERT(MySurfaces.CurrentCaptureMode == ODDODD,"unknown field ASSERT failed\n" );
                daField = ODD;
    }
  return daField;
}
               
FIELDTYPE    getNextFieldType(FIELDTYPE lastFieldType,U032 dwBuffIndex) {

    switch (MySurfaces.CurrentCaptureMode) {
            case EVENODD:
// this is a fix which _SHOULD_ be benign for nv4 ( need to check it )
// but which protects us from NV10 behavior where field captures can start
// in the middle which can then set us into toxic behavior
// in which the buffer capture order is 0,0,1,1,0,0. and there are weird latency
// problems
#if 0

                if(lastFieldType ==EVEN ) {
                    return ODD;
                } else if( lastFieldType ==ODD ) {
                    return EVEN;
                } else {
                    return EVEN;
                }
                break;
#else
                if (dwBuffIndex == 0 ) {
                    return ODD;
                } else {
                    return EVEN;
                }
                break;
#endif
            case EVENEVEN:
                if(lastFieldType ==EVEN ) {
                    return EVEN;
                } else {
                    return EVEN;
                }    
                break;
            case ODDODD:
                if(lastFieldType ==ODD ) {
                    return ODD;
                } else {
                    return ODD;
                }    
                break;
            default:
                ASSERT(MySurfaces.CurrentCaptureMode == ODDODD,"unknown field ASSERT failed\n" );
                return ODD;
    }
}

U032  getSurfaceIndex(U032 pSurf ) {
    U032 i;
    // check for exact match
    for(i=0;i<MySurfaces.numberOfSurfaces;i++) {
        if((U032)(MySurfaces.Surfaces[i]->pVidMem) == ((U032) pSurf)) {
            return i;
        }
    }
    // check for something "within"
    for(i=0;i<MySurfaces.numberOfSurfaces;i++) {
        if( ((U032)(MySurfaces.Surfaces[i]->pVidMem) <= ((U032) pSurf))  &&
             ((U032)(MySurfaces.Surfaces[i]->pVidMem)+MySurfaces.Surfaces[i]->dwLength >= ((U032) pSurf)  )) {
                    return i;
        }
    }
    
    return 99;
}


U032    notifyIndexFromBuffer(U032 dwBuffer) {
    switch (dwBuffer) {
            case 0:
                return NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
                break;
            case 1:
                return NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
                break;
            case 2:
                return NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0);
                break;
            case 3:
                return NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1);
                break;
            default:
                {
                    DPF ("   Problem... trying get an index for an unknown buffer notify\n");
                    return UNKNOWNBUF;
                }
    }
}


U032    getNextSurface(U032 currentSurface, U032 bSingleAdvance) {
    U032    nextSurface;
    
    ASSERT(currentSurface < MAXVPSURFACES,"current surface to big ASSERT failed\n" );
    
    //Generally you are going to want to jump two surfaces ahead of the one you are on, but 
    // with a wrap around depending on what typeof surface you are
    // interleaved surface should only move ahead ONE buffer since both DMA engines are pointing
    // to the same surface
    if( currentSurface < MySurfaces.VBIstart ) {
        // we are an ImageSurface
        if(MySurfaces.CurrentScanMode == INTERLEAVBUF ) {
            if(bSingleAdvance) {
                // hopefully we don't get here....  this means our drop frame strategy for interelaved buffers is notworking
            }
            nextSurface = currentSurface + 1;
        } else {    
            if(bSingleAdvance) {
                nextSurface = currentSurface + 1;
            } else {
                nextSurface = currentSurface + 2;
            }
        }
        while(nextSurface >= MySurfaces.VBIstart) 
            nextSurface = nextSurface - MySurfaces.VBIstart;    
    } else {
        U032    temp;
        //we are a VBI surface
        nextSurface = currentSurface + 2 - MySurfaces.VBIstart;
        temp = MySurfaces.numberOfSurfaces - MySurfaces.VBIstart;
        while(nextSurface >= temp) 
            nextSurface = nextSurface - temp;
        nextSurface = nextSurface + MySurfaces.VBIstart;
    }
    return nextSurface;
}


U032    getPrevSurface(U032 currentSurface) {
    long    nextSurface;
    
    ASSERT(currentSurface < MAXVPSURFACES ,"current Surface to big ASSERT FAILED\n");
    
    //Generally you are going to want to jump two surfaces ahead of the one you are on, but 
    // with a wrap around depending on what typeof surface you are
    // interleaved surface should only move ahead ONE buffer since both DMA engines are pointing
    // to the same surface
    if( currentSurface < MySurfaces.VBIstart ) {
        // we are an ImageSurface
        if(MySurfaces.CurrentScanMode == INTERLEAVBUF ) {
            nextSurface = currentSurface - 1;
        } else {    
            nextSurface = currentSurface - 2;
        }
        while(nextSurface < 0) 
            nextSurface = nextSurface + MySurfaces.VBIstart;    
    } else {
        long    numVBI;
        //we are a VBI surface - never interleaved.. always subtract 2
        nextSurface = currentSurface - 2 - MySurfaces.VBIstart;
        numVBI = MySurfaces.numberOfSurfaces - MySurfaces.VBIstart;
        while(nextSurface < 0) 
            nextSurface = nextSurface + numVBI;
        nextSurface = nextSurface + MySurfaces.VBIstart;
    }
    return nextSurface;
}


U032    getPrevOverlaySurface(U032 currentSurface) {
    long    nextSurface;
    
    ASSERT(currentSurface < MAXVPSURFACES ,"ASSERT FAILED\n");
    
    //Generally you are going to want to jump two surfaces ahead of the one you are on, but 
    // with a wrap around depending on what typeof surface you are
    // interleaved surface should only move ahead ONE buffer since both DMA engines are pointing
    // to the same surface
    if( currentSurface < MySurfaces.VBIstart ) {
        // we are an ImageSurface
        nextSurface = currentSurface - 1;
        while(nextSurface < 0) 
            nextSurface = nextSurface + MySurfaces.VBIstart;    
    } else {
        ASSERT(1==0,"we should never try to flip the overlay on a VBI surface you numskull\n");
        DPF("we should never try to flip the overlay on a VBI surface you numskull\n");
    }
    return nextSurface;
}



BOOLEAN checkScalingOk(U032 in, U032 out ) {
    U032 xIn,yIn,xOut,yOut;
    U032    xRatio;
    /*  Scaling works like this... the output MUST be smaller than the input, and the X ratio must be an integer value
         there is no limit on the Y ratio since it's just a line dropping system
    */
    
    xIn  = in & 0xFFFF;
    yIn  = in >> 16;
    xOut = out & 0xFFFF;
    yOut = out >> 16;
    
    if( xOut <= 0 ) {
        return FALSE;
    }
    
    if( xIn < xOut ) {
        //DPF ("   In dimension X is SMALLER than out X dimension %d, %d",xIn,xOut);
        return FALSE;
    }
    if( yIn < yOut ) {
        //DPF ("   In dimension Y is SMALLER than out Y dimension %d, %d",xIn,xOut);
        return FALSE;
    }
    if( xIn % xOut != 0 ) {
        //DPF ("   X dimension In does not evenly divide by X out %d, %d",xIn,xOut);
        return FALSE;
    }    
    xRatio = xIn / xOut;
    
    switch (xRatio) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 6:
            case 8:
            case 12:
            case 16:
            case 24:
                break;
            default:
                {
                    //DPF ("   Not one of the good scale factors %d, %d, %d",xIn,xOut,xRatio);
                    return FALSE;
                }            
                break;
    }
    
    
    return TRUE;
}



U032    ProgramBufferWithSurface(U032    dwBufferIndex,U032    newSurfaceIndex, U032 bZeroLength) {
    // This function is responsible for programming up and then launching buffer/surface combination
    // This will finish Asyncronously, and either be relaunched in the case of auto-flip, or be able 
    // to report back usefully with the VPE functions
    U032    nextField;
    U032  nextVBIField;
    U032    dwInterleaveAddition;
    vpSurfaceInfo*        pSurface;
    NvChannel*    pMyNvChan = MySurfaces.pVPChanPtr;
    
    pSurface = MySurfaces.Surfaces[newSurfaceIndex];
    
        
    MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);
    
    ASSERT(dwBufferIndex < 4 ,"ASSERT FAILED\n");
    ASSERT(newSurfaceIndex < MAXVPSURFACES ,"ASSERT FAILED\n");
    //Update the surface structures
        // notify
    
    MySurfaces.Surfaces[newSurfaceIndex]->pNvNotify = &(nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)]);
    MySurfaces.CurrentBufferSurface[dwBufferIndex] = newSurfaceIndex;    
    
    // Figure out the whole field thing
    if( dwBufferIndex < 2 ) {
        // this is image
        nextField = getNextFieldType(MySurfaces.lastField, dwBufferIndex);
        MySurfaces.Surfaces[newSurfaceIndex]->field = nextField;
        MySurfaces.lastField = nextField;

        // if we're interleaving AND we're on the EVEN field then move everything down a bit
        //   OOD field is on top
        if(( MySurfaces.CurrentScanMode == INTERLEAVBUF) && (nextField == EVEN ) ) {
            dwInterleaveAddition = MySurfaces.Surfaces[newSurfaceIndex]->dwPitch/2;
        } else {
            dwInterleaveAddition = 0;
        }

        MySurfaces.curImageSurf = newSurfaceIndex;
    } else {
        // this is VBI
        nextVBIField = getNextFieldType(MySurfaces.lastField,dwBufferIndex-2);
        MySurfaces.Surfaces[newSurfaceIndex]->field = nextVBIField;
        MySurfaces.lastVBIField = nextVBIField;
        MySurfaces.curVBISurf = newSurfaceIndex;
    }
    
    MySurfaces.curSurf = newSurfaceIndex;
    
    // choose which type of surface this is

        // Put decoder object into the channel
        

    ASSERT(2*4 < NV06A_FIFO_GUARANTEED_SIZE ,"ASSERT FAILED\n");            
               
    while (MySurfaces.myFreeCount < 2*4) {
        NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
        MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);
    }            
    MySurfaces.myFreeCount -= 2*4;                                        

        

    pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;

        // these lines end up being the Y-Crop value.
        // therefore they're the deviding line between VBI and Image
    
        // check if VBI is running
    if(MySurfaces.VBIstart < MySurfaces.numberOfSurfaces ) {
        if( dwBufferIndex >= 2 ) {
        // VBI is running
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImageStartLine = 
                (MySurfaces.Surfaces[newSurfaceIndex]->dwHeight) +
                (MySurfaces.Surfaces[newSurfaceIndex]->dwStartLine) + 2;
        }
    } else {
        if( dwBufferIndex < 2) {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImageStartLine = 
                MySurfaces.Surfaces[newSurfaceIndex]->dwStartLine;
        }
    }

    if( dwBufferIndex <2 ) {
        

        // Image surface
         // setup the field polarity, offset and fire it off

        // if prescale is not on 2:1 integer boundaries from output, then fail...
        {
            BOOL bScale = checkScalingOk((MySurfaces.Surfaces[newSurfaceIndex]->dwWidth) | 
                                                    (MySurfaces.Surfaces[newSurfaceIndex]->dwHeight << 16)
                            ,MySurfaces.Surfaces[newSurfaceIndex]->dwPreScaleSize);
            
            if(! bScale ) {
                DPF("   Invalid Scaling parameters in use...\n");
    
                return (-7);        
            }
        }
        

        ASSERT(5*4 < NV06A_FIFO_GUARANTEED_SIZE ,"ASSERT FAILED\n");            

        while (MySurfaces.myFreeCount < 5*4) {
            NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
            MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);
        }            
        MySurfaces.myFreeCount -= 5*4;                                        
        if( bZeroLength) {
          MarkTiming(PROGRAMIMG,MySurfaces.Surfaces[newSurfaceIndex]->pVidMem, makeInd2(0,INCNOTAP,dwBufferIndex,newSurfaceIndex,( ( nextField == ODD ) ? 0 : 1)));
        } else {
          MarkTiming(PROGRAMIMG,MySurfaces.Surfaces[newSurfaceIndex]->pVidMem, makeInd2(0 | 0x80,INCNOTAP,dwBufferIndex,newSurfaceIndex,( ( nextField == ODD ) ? 0 : 1)));
        }

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeIn =
            (MySurfaces.dwInWidth) | (MySurfaces.dwInHeight << 16);
        if( bZeroLength ) {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeOut = 0;
        } else {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeOut = 
                MySurfaces.Surfaces[newSurfaceIndex]->dwPreScaleSize;        
        }
        
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].offset    =
                MySurfaces.Surfaces[newSurfaceIndex]->dwOffset + dwInterleaveAddition;
        // There is some sly behavior in the field statement...
        // since progressive fields are expected to be delivered in the EVEN fields we only check for ODD
        // and then everything else is considered to be even
        nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].format =        
                ( MySurfaces.Surfaces[newSurfaceIndex]->dwPitch & 0xFFFF )  |
                ((( ( nextField == ODD ) ? NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD) << 16 ) & 0xFF0000 )  |
                ((NV_OS_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000 ) ;
           

        
        
                
    } else {
        //MYCHECKFREECOUNT(pMyNvChan,4*4);
        

        ASSERT(4*4 < NV06A_FIFO_GUARANTEED_SIZE,"ASSERT FAILED\n" );            

        while (MySurfaces.myFreeCount < 4*4) {
            NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
            MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);
        }            
        MySurfaces.myFreeCount -= 4*4;                                        
        MarkTiming(PROGRAMVBI,MySurfaces.Surfaces[newSurfaceIndex]->pVidMem, makeInd(dwBufferIndex,newSurfaceIndex,INCNOTAP,INCNOTAP));

        // these lines end up being the Y-Crop value.
        // therefore they're the deviding line between VBI and Image
        
        

        // VBI surface    
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].size    =
            ((MySurfaces.Surfaces[newSurfaceIndex]->dwHeight+1) << 16) | (MySurfaces.Surfaces[newSurfaceIndex]->dwStartLine) ;
        
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].offset    =
            MySurfaces.Surfaces[newSurfaceIndex]->dwOffset;
        
        nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].format =        
            ( MySurfaces.Surfaces[newSurfaceIndex]->dwPitch & 0xFFFF )  |
            ((( ( nextVBIField == ODD ) ? NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD) << 16 ) & 0xFF0000 )  |
            ((NV_OS_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000 ) ;

    }
    //Update the Capture Class
    // hopefully make sure stuff REALLY get to the registers
    NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
    
    return 0;
}

    
void CheckAndDoCallbacks(U032        interruptingBuffer, U032 oldsurface){
    U032 newSurface = getNextSurface(oldsurface,FALSE);
    
    if(interruptingBuffer < 2) {
        if(MySurfaces.ImageCallbackRoutine != NULL ) {
            if( MySurfaces.Surfaces[oldsurface]->field == EVEN )
                MySurfaces.myIMGCallbackInfo.dwFlags = USINGFLAGS | IMAGESURFACE | BUFFADRLIN | EVENFIELD;
            else
                MySurfaces.myIMGCallbackInfo.dwFlags = USINGFLAGS | IMAGESURFACE | BUFFADRLIN | ODDFIELD;

            MySurfaces.myIMGCallbackInfo.lastBufferAddr = MySurfaces.Surfaces[oldsurface]->pVidMem;    // address of just finished buffer ( the one the client should unload )
            MySurfaces.myIMGCallbackInfo.curBufferAddr = MySurfaces.Surfaces[newSurface]->pVidMem;    // address of current input buffer ( one that is now being written to by MP )
            MySurfaces.myIMGCallbackInfo.intCount++;
        
            MySurfaces.ImageCallbackRoutine(&MySurfaces.myIMGCallbackInfo);
        }
    } else {
        if(MySurfaces.VBICallbackRoutine != NULL ) {
            if( MySurfaces.Surfaces[oldsurface]->field == EVEN )
                MySurfaces.myVBICallbackInfo.dwFlags = USINGFLAGS | VBISURFACE | BUFFADRLIN | EVENFIELD;
            else
                MySurfaces.myVBICallbackInfo.dwFlags = USINGFLAGS | VBISURFACE | BUFFADRLIN | ODDFIELD;
            
            MySurfaces.myVBICallbackInfo.lastBufferAddr = MySurfaces.Surfaces[oldsurface]->pVidMem;;    // address of just finished buffer ( the one the client should unload )
            MySurfaces.myVBICallbackInfo.curBufferAddr = MySurfaces.Surfaces[newSurface]->pVidMem;    // address of current input buffer ( one that is now being written to by MP )
            MySurfaces.myVBICallbackInfo.intCount++;
            MySurfaces.VBICallbackRoutine(&MySurfaces.myVBICallbackInfo);
        }
    }
}



U032 startSurfaces(U032    bInterleave, VPSTATE*    pVideoPortStatus ) {
    U032    rValue = DD_OK;
    VPSTATE    myState = *pVideoPortStatus;
    // figure out which buffers to program up and launch them...

    // zero out the last overlay scaler size so we're sure it gets reprogrammed
    MySurfaces.bThreadRunning = TRUE;
            
    MySurfaces.DropSystem.dwTargetSurface = 0;
    MySurfaces.DropSystem.dwbIsBobFromInterleave = 0;
    MySurfaces.DropSystem.dwFieldType = 0;
    MySurfaces.DropSystem.dwFullBool = 0;
    MySurfaces.DropSystem.dwDropFields = 0;

    if(MySurfaces.bInvertedFields ) {    
        MySurfaces.lastVBIField = ODD;
        MySurfaces.lastField = ODD;    // this should be even so we'll always start on odd ( hopefully... :-)
    } else { 
        MySurfaces.lastField = EVEN;
        MySurfaces.lastVBIField = EVEN;
    }
    MySurfaces.lastsrcDeltaY = 0;
    MySurfaces.bStopVP = FALSE;    

    // start first vbi
    if( (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart) > 0 ) {
        if( ProgramBufferWithSurface(2,MySurfaces.VBIstart,FALSE))
                rValue = DDERR_INVALIDPARAMS;
        myState |= VP_VBI_CAPTURING;
    }
    
    //start first image
    if( (MySurfaces.VBIstart > 0 ) ) {
        // there is at last one image surface
        if( ProgramBufferWithSurface(0,0,FALSE) )
                rValue = DDERR_INVALIDPARAMS;
            myState |= VP_IMAGE_CAPTURING;
    }
    // start second VBI
    if( (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart) > 0 ) {
    
        // now check to see if we have another surface
        if( (MySurfaces.numberOfSurfaces - MySurfaces.VBIstart) > 1 ) {
            if( ProgramBufferWithSurface(3,MySurfaces.VBIstart+1,FALSE))
                    rValue = DDERR_INVALIDPARAMS;
        } else {
            // set second buffer to first surface
            if( ProgramBufferWithSurface(3,MySurfaces.VBIstart+0,FALSE))
                    rValue = DDERR_INVALIDPARAMS;
        }
    }    
    // start second image
    if( (MySurfaces.VBIstart > 0 ) ) {
        
        if( bInterleave) {
            // since this is interleave always start the second buffer with the first surface
            if( ProgramBufferWithSurface(1,0,FALSE) )
                rValue = DDERR_INVALIDPARAMS;
            myState |= VP_IMAGE_CAPTURING;
        } else {    
            if( MySurfaces.CurrentCaptureMode == EVENODD ) {
              // if we're odd/even and not interleaved then we're seperate bob, which is where the
              // potential problem occurs
              MySurfaces.bFirstTime = TRUE;
            }
            // check for another surface
            if( MySurfaces.VBIstart > 1 ) {
                if( ProgramBufferWithSurface(1,1,FALSE) )
                    rValue = DDERR_INVALIDPARAMS;
            } else {
                // fire up the second buffer at the first surface
                DPF("this is kinda odd, only one surface, but also non-interleaved...");
                if( ProgramBufferWithSurface(1,0,FALSE) )
                    rValue = DDERR_INVALIDPARAMS;
            }
        }
    }
                    
    *pVideoPortStatus = myState;
    
    return rValue;
}



#define NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET               (0x0001)


BOOL GetField(void) {
    BOOL    retvalue = TRUE;
    U032    whiletime = 0;
    FIELDTYPE aField = UNKNOWN;
    NvChannel*    pMyNvChan = MySurfaces.pVPChanPtr;
        
    // we're not hitting the channel
    //INIT_FREE_COUNT(0);

    

    ASSERT(3*4 < NV06A_FIFO_GUARANTEED_SIZE,"ASSERT FAILED\n" );            

    while (MySurfaces.myFreeCount < 3*4)    {    
        NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
        MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);    
    }        
    MySurfaces.myFreeCount -= 3*4;                                        

    // clear the notifiers
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    // put the object into the channel
    pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;

    // fire off the command
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[0] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[1] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
    
    // wait for completion

    while ( ((nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
                 (nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS))    &&
                (whiletime++ < WHILETIMEOUT) )
                NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
        ;
    
    // only one of these should be running right now....
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].info16 == NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET ) {
        aField = MySurfaces.Surfaces[MySurfaces.CurrentBufferSurface[0]]->field;
    }
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].info16 == NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET ) {
        aField = MySurfaces.Surfaces[MySurfaces.CurrentBufferSurface[1]]->field;
    }
    if(aField == EVEN ) {
        retvalue = TRUE;
    } else {
        retvalue = FALSE;
    }
    
    return retvalue;
}


BOOL GetCurrentBuffers(void) {
    BOOL    retvalue = TRUE;
    U032    whiletime = 0;
    FIELDTYPE aField = UNKNOWN;
    NvChannel*    pMyNvChan = MySurfaces.pVPChanPtr;
        
    // we're not hitting the channel
    //INIT_FREE_COUNT(0);

    
    ASSERT(5*4 < NV06A_FIFO_GUARANTEED_SIZE ,"ASSERT FAILED\n");            
    while (MySurfaces.myFreeCount < 5*4)    {    
        NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
        MySurfaces.myFreeCount = NvGetFreeCount(pMyNvChan, 0);    
    }        
    MySurfaces.myFreeCount -= 5*4;                                        

    // clear the notifiers
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
//    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(0)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
//    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(1)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    // put the object into the channel
    pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;

    // fire off the command

    // NOTE we only need half of these since they return the same data as it stands now

    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[0] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
//    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[1] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetVbiOffsetNotify[0] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
//    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetVbiOffsetNotify[1] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
    
    // wait for completion

    while ( ((nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) &&
                 (nvMyVPNotifiers[NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(0)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS))    &&
                (whiletime++ < WHILETIMEOUT) )
                NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);                            
        ;
    
    // only one of these should be running right now....
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].info16 == NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET ) {
        aField = MySurfaces.Surfaces[MySurfaces.CurrentBufferSurface[0]]->field;
    }
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_VBI_OFFSET_NOTIFY(0)+NUM04DOFFSET].info16 == NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET ) {
        aField = MySurfaces.Surfaces[MySurfaces.CurrentBufferSurface[1]]->field;
    }
    if(aField == EVEN ) {
        retvalue = TRUE;
    } else {
        retvalue = FALSE;
    }
    
    return retvalue;
}


U032 vddhandleBufferInterrupts(U032 whichBuffer, U032 dummy2, U032 dummy3) {


    U032    i;
    U032    foundSomething;
    U032    reprogram;
    U032 interruptingBuffer = STARTINGBUF;
    U032 oldSurface;
    U032 newSurface;
    
    MySurfaces.myFreeCount = 0;

    // 15 long words is a bit arbitrary, but is based on the number of long words pushed in 
    // a normal update ie a VP reprogram and an overlay flip
    if (MySurfaces.myFreeCount < 15*4)                                
        MySurfaces.myFreeCount = NvGetFreeCount(MySurfaces.pVPChanPtr, 0);            
    
    {
        U032 aCount=0;
        while( MySurfaces.myFreeCount < 15*4 ) {
            DPF("No room on fifo.. try NVR0interrupt\n");
            NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);    
            MySurfaces.myFreeCount = NvGetFreeCount(MySurfaces.pVPChanPtr, 0);            
            aCount++;
            if(aCount > 5){
                DPF("STILL No room on fifo.. bail outta here... there's probably something wrong\n");
                return 0;
            }
        
        }
    }
    // I'm really NOT sure what sort of information I'm going to be getting at this point...
    // I need to determine which of the four possible buffers just finished...
    do  {
        U032    time0old = 0xFFFFFFFF;
        U032    time1old = 0xFFFFFFFF;
        U032  oldestBuffer = NOSURFACEPROGRAMED;
        
        reprogram = FALSE;
        foundSomething = FALSE;
        // check for GOOD completions and deal with these first

        // search for the oldest finished buffer
        for(i=0;i<4;i++) {
            
            if(( nvMyVPNotifiers[notifyIndexFromBuffer(i)].status != NV04D_NOTIFICATION_STATUS_IN_PROGRESS ) && 
                ( nvMyVPNotifiers[notifyIndexFromBuffer(i)].status != NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED )  ) 
            {
                // ok we've found one of the notifiers which has finished
                // now check the completion times to find the "oldest" one to preserve order
                U032 time1 = nvMyVPNotifiers[notifyIndexFromBuffer(i)].timeStamp.nanoseconds[1];
                U032 time0 = nvMyVPNotifiers[notifyIndexFromBuffer(i)].timeStamp.nanoseconds[0];
                
                // if the new time is not greater than the old time
                if( time1 <= time1old ) {
                    if( (( time1 == time1old ) && (time0 < time0old)) || ( time1 < time1old)) {
                        // we have a new winner
                        time0old = time0;
                        time1old = time1;
                        oldestBuffer = i;
                    }
                }
            }
        }
        
        
        if( oldestBuffer != whichBuffer ) {
            //DPF("Buffer callbacks are out of order\n");
        };
// This is an experiment.  check the free count.  if we have room, then process this request.  otherwise return
// now and deal with this notifier next time we get to this routine...
// this is put in to prevent a dead-lock between programmbufferswithsurface which is spinning waiting for room
// and the software method which is supposed to be MAKING room.                    
        if( oldestBuffer != NOSURFACEPROGRAMED ) {
            if (MySurfaces.myFreeCount < 15*4)                                
                MySurfaces.myFreeCount = NvGetFreeCount(MySurfaces.pVPChanPtr, 0);            
            {
                U032 aCount=0;
                while( MySurfaces.myFreeCount < 15*4 ) {
                    DPF("No room on fifo.. try NVR0interrupt\n");
                    NvRmR0Interrupt ((GLOBDATAPTR)->ROOTHANDLE, NV_WIN_DEVICE);    
                    MySurfaces.myFreeCount = NvGetFreeCount(MySurfaces.pVPChanPtr, 0);            
                    aCount++;
                    if(aCount > 5){
                        DPF("STILL No room on fifo.. bail outta here... there's probably something wrong\n");
                        return 0;
                    }
                
                }
            }
            // now verify that it was completed successfully
            if( nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)].status == NV04D_NOTIFICATION_STATUS_DONE_SUCCESS ) 
            {
                                         
                interruptingBuffer = oldestBuffer;
                oldSurface = MySurfaces.CurrentBufferSurface[interruptingBuffer];
                if( oldSurface != NOSURFACEPROGRAMED ) {

                    if(interruptingBuffer < 2)  {
                        //MarkTiming(NOTIFYIMG,MySurfaces.Surfaces[oldSurface]->pVidMem,makeInd(INCNOTAP,INCNOTAP,interruptingBuffer,oldSurface));
                        //MarkTimingNot(RMIMGPROG,&(nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(oldestBuffer)]),
                        //              nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(oldestBuffer)].info32,
                        //              makeInd(INCNOTAP,INCNOTAP,interruptingBuffer,oldSurface));
                        MarkTimingNot(NOTIFYIMG,&(nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)]),MySurfaces.Surfaces[oldSurface]->pVidMem,makeInd(INCNOTAP,INCNOTAP,interruptingBuffer,oldSurface));
                        //RecordImageNotify(nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)].timeStamp.nanoseconds[0],
                        //                  nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)].timeStamp.nanoseconds[1] );
                        
                    } else {
                        //MarkTiming(NOTIFYVBI,MySurfaces.Surfaces[oldSurface]->pVidMem,makeInd(interruptingBuffer,oldSurface,INCNOTAP,INCNOTAP));
                        MarkTimingNot(NOTIFYVBI,&(nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)]),MySurfaces.Surfaces[oldSurface]->pVidMem,makeInd(interruptingBuffer,oldSurface,INCNOTAP,INCNOTAP));
                    }
                    // onward
                    if(oldSurface == NOSURFACEPROGRAMED) {
                        //DPF("... that buffer should have had a surface..buffer =%d",interruptingBuffer);
                    }
                    nvMyVPNotifiers[notifyIndexFromBuffer(interruptingBuffer)].status = NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
                    foundSomething = TRUE;
                    if((oldSurface != NOSURFACEPROGRAMED ) & (!MySurfaces.bStopVP) )
                         reprogram = TRUE;
                } else {
                    DPF("spurious interrupt, with unrecognized surface");
                }
            } else {
                // completed with an error most likely
                oldSurface = MySurfaces.CurrentBufferSurface[oldestBuffer];
                reportError(interruptingBuffer,oldSurface);
                nvMyVPNotifiers[notifyIndexFromBuffer(oldestBuffer)].status = NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
                if( MySurfaces.Surfaces[oldSurface] == NULL )
                        foundSomething = FALSE;
                    else 
                        foundSomething = TRUE;    
                // for now do nothing
            }
        }

        // on the NV10, the buffers don't have a fixed capture order because of a bug in the HW
        // since I WANT a particular buffer to finish first, the only way to ensure this is to see if
        // the WRONG buffer finished first and if so reprogram it with the SAME settings.
        
        if((MySurfaces.bFirstTime) && (interruptingBuffer == 1)) {
            MarkTiming(ALMEMRESTART,MySurfaces.Surfaces[oldSurface]->pVidMem, makeInd2(0,INCNOTAP,interruptingBuffer,7,0));

            if( MySurfaces.VBIstart > 1 ) {
                ProgramBufferWithSurface(1,1,FALSE);
            } else {
                // fire up the second buffer at the first surface
                DPF("this is kinda odd, only one surface, but also non-interleaved...");
                ProgramBufferWithSurface(1,0,FALSE);
            }
            MySurfaces.bFirstTime = FALSE;
            // since we're going to pretend that this _didn't_ occur, we'll just return right now!
            return 0;
        }
        
        MySurfaces.bFirstTime = FALSE;
        // no go on with life as usual.

        if( reprogram) {
            U032 overlayFlipError = 0;
            
            // check if we're an image buffer and see if we need to schedule an overlay flip
            if( interruptingBuffer < 2 ) {
                FIELDTYPE interruptingField = UNKNOWN;
                
                interruptingField = getFieldFromBuffer(interruptingBuffer);
                
                
                if( MySurfaces.CurrentScanMode == SEPERATEBUF ) {
                    // scheduele flip!
                    // overlayFlipError = scheduleOverlayFlip(oldSurface,FALSE, interruptingField);
                    
                } else {
                    // we're in interleaved mode... 
                    
                    // Check to see if we're doing "bob from interleaved"
                    if ((MySurfaces.pDriverData)->vpp.dwOverlayMode & NV_VFM_FORMAT_BOBFROMINTERLEAVE) {
                        //Bob from interleaved
                        // we don't want the overlay to follow the surface, instead the one just behind
                        // this surface
        //                scheduleOverlayFlip(getPrevSurface(oldSurface),TRUE, interruptingField);

                        // this is an odd situation where we should delay one field on our updates...
                        // this causes overlay flip to a surface to occur only after BOTH fields in the surface
                        // have been received.
                        if( MySurfaces.lastSurface != NOSURFACEPROGRAMED) {
                            //overlayFlipError = scheduleOverlayFlip(MySurfaces.lastSurface,TRUE, interruptingField);
                            //overlayFlipError = scheduleOverlayFlip(oldSurface,TRUE, interruptingField);
                        }
                        MySurfaces.lastSurface = oldSurface;
                        MySurfaces.lastBuffer = interruptingBuffer;

                        if ( overlayFlipError == 15 ) {
                           // this indicates that we need to go into DropFrame mode.
                           // we need to prevent the flipping of the NEXT field to complete
                           // and we need to reprogram the NEXT TWO fields to have lengths of zero
                           MySurfaces.DropSystem.dwDropFields = 2;
                           
                        }
                    } else {
                        // weave from interleaved
                        // Check to see if the other buffer is working on this surface, if it is then don't flip
                        // flip only when ODD field was the last programmed. which likey means this is an EVEN
                        // field which completed and therefore is the second field in the surface
                        if(  MySurfaces.lastField == ODD ) {
                            // overlayFlipError = scheduleOverlayFlip(oldSurface,FALSE, interruptingField);
                        } else {
                        }
                    }
                }
                
            // however smac@microsoft.com ( head VPE guru )
            // says that autoflip means we should continue capturing fields but they should end up in the SAME surface
            // so off we go to reprogram
        
                if( MySurfaces.DropSystem.dwDropFields > 0  ){
                    
                    // then call ProgramBuffer with Surface
                    // program up this buffer to send to a "null" surface
                    // reprogram the old surface, but with a zero length so nothing get written into
                    // the surface, and things can progress properly if needed
                    
                    //  I think this might be throwing out the field just following the field
                    // that we want to throw out, because I'm programming one field ahead...
                    ProgramBufferWithSurface(interruptingBuffer,oldSurface, TRUE);
                    
                    // we'll only drop two of these count them down
                    MySurfaces.DropSystem.dwDropFields--;
                    
                } else if( MySurfaces.bSkipNextField ) {
                    // then call getNextSurface
                    if( MySurfaces.Surfaces[oldSurface]->bAutoFlip ) {
                        newSurface = getNextSurface(oldSurface,FALSE);
                    } else {
                        MySurfaces.bFlipRequested = FALSE;
                        newSurface = oldSurface;
                    }
                    // then call ProgramBuffer with Surface
                    // program up this buffer to send to a "null" surface
                    // reprogram the old surface, but with a zero length so nothing get written into
                    // the surface, and things can progress properly if needed
                    
                    //  I think this might be throwing out the field just following the field
                    // that we want to throw out, because I'm programming one field ahead...
                    ProgramBufferWithSurface(interruptingBuffer,oldSurface, TRUE);
                    MySurfaces.bSkipNextField = FALSE;
                } else {
                  // this is the normal calm pathway
                  if( MySurfaces.Surfaces[oldSurface]->bAutoFlip ) {
                      newSurface = getNextSurface(oldSurface,FALSE);
                  } else {
                      MySurfaces.bFlipRequested = FALSE;
                      newSurface = oldSurface;
                  }// then call ProgramBuffer with Surface
                  ProgramBufferWithSurface(interruptingBuffer,newSurface, FALSE);
                    
                }
                 
                // Do Vid Texture now...  this will be purely field based. and only ODD field
                if(  MySurfaces.lastField == ODD ) {

                    // until this code is written
                }
                
            } else {
                // this is a VBI buffer... so just check if it's autoflip or not
                if( MySurfaces.Surfaces[oldSurface]->bAutoFlip ) {
                    newSurface = getNextSurface(oldSurface,FALSE);
                } else {
                    MySurfaces.bFlipRequested = FALSE;
                    newSurface = oldSurface;
                }
                // then call ProgramBuffer with Surface
                ProgramBufferWithSurface(interruptingBuffer,newSurface, FALSE);
            
            }
            CheckAndDoCallbacks(interruptingBuffer,oldSurface);
            
            
            // At this pointer we've checked and call the 3rd party callbacks.
            //   then we've flipped the overlay if we determined that to be important
            //   finally we've reprogrammed our hardware to go off and do whatever
            
            // now we'll call MS's callback to tell them that something has happened.
            
            // we should ONLY do this ONCE per field on the LAST of the two notifies.
            // So if it's VBI only then that's when the vbi is done
            // otherwise when IMAGE is done
            
            if( (( MySurfaces.VBIstart == 0 ) && ((whichBuffer == 2) || (whichBuffer == 3))) ||
                (( MySurfaces.VBIstart != 0 ) &&((whichBuffer == 0) || (whichBuffer == 1)))   ){
                if((void*)MySurfaces.pDXIRQCallback != NULL) {
                    if( MySurfaces.DXIRQCallbackSources & DDIRQ_VPORT0_VSYNC ) {
                        // basically since we're auto flipping, we're only going to tell them when this is done and nothing else
                            if(MySurfaces.pDXIRQCallback != 0) {
                                // here's the one line of docs that we have:
                                // When calling the IRQCallback, EAX should contain the DDIRQ_xxxx flags indicating 
                                // what caused the IRQ and EBX should contain the specified dwContext. 
                                
                                
                                U032 temp1 = MySurfaces.dwIrqContext;        //  the same context which dd gave me before
                                U032 temp2 = DDIRQ_VPORT0_VSYNC;                // at this point we only have an interrupt for ONE event
    
    // to allow chris's editor to deal with funky assembly code
#define SKIPOMPARSE
#ifdef SKIPOMPARSE        
                            
                                // push register values
                                _asm mov EAX, temp2
                                _asm mov EBX, temp1
#endif                            
                                // call the callback
                                ((ACALLBACKTYPEPROC)MySurfaces.pDXIRQCallback)();
                            };
    
                    }
                
                }
            }
        } else {
            if(interruptingBuffer == STARTINGBUF ) {
                //DPF ("   We got an interrupt but no buffer's were in state STATUS_DONE_SUCCESS \n");
            //    DPF ("           or had ERRORS.  We may be trying to terminate\n");
            }
            interruptingBuffer = UNKNOWNBUF;
            if(oldSurface == NOSURFACEPROGRAMED) {
                DPF ("   We got an event wherein the surface which we just claimed to  \n");
                DPF ("           have filled was invalid\n");
            }
        }
        
        
        
//    } while (foundSomething);
    } while (FALSE);
    
    return 0;
}



void timerNotifyProc(void) {
    // this gets called when we want to start the video on NV3
    // check for null channel pointer
    if( (MySurfaces.pDriverData)->dwGlobalStructSize != sizeof(GLOBALDATA) ) {
        U032 size = sizeof(GLOBALDATA);
        
        DPF("Warning Warning Warning  -  Globaldata structure compiled with different length prepare to die!");
        return;
    }
    
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }

    if( (MySurfaces.pDriverData)->NvDevVideoFlatPio == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    
    startSurfaces(MySurfaces.dwVPFlags & DDVP_INTERLEAVE, &(MySurfaces.dwVideoPortStatus) );

    return;
}

void wrapperProcI0(void) {
    
    // check for null channel pointer
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    if( (MySurfaces.pDriverData)->NvDevVideoFlatPio == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    
    // only do something here if we know that the "thread is running"
    if(MySurfaces.bThreadRunning)
        vddhandleBufferInterrupts(0,0,0);
    return;
}

void wrapperProcI1(void) {
    
    // check for null channel pointer
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    if( (MySurfaces.pDriverData)->NvDevVideoFlatPio == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    
    // only do something here if we know that the "thread is running"
    if(MySurfaces.bThreadRunning)
        vddhandleBufferInterrupts(1,0,0);
    return;
}

void wrapperProcV0(void) {
    
    // check for null channel pointer
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    if( (MySurfaces.pDriverData)->NvDevVideoFlatPio == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    
    // only do something here if we know that the "thread is running"
    if(MySurfaces.bThreadRunning)
        vddhandleBufferInterrupts(2,0,0);
    return;
}

void wrapperProcV1(void) {
    
    // check for null channel pointer
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    if( (MySurfaces.pDriverData)->NvDevVideoFlatPio == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    
    // only do something here if we know that the "thread is running"
    if(MySurfaces.bThreadRunning)
        vddhandleBufferInterrupts(3,0,0);
    return;
}

void MTMNotifyProc(void) {
    if( (MySurfaces.pDriverData)->NvDevFlatVPE == (U032)NULL ) {
        // how do we indicate error here?  uh I don't know yet
        return ; 
    }
    // Uh... is there more to do here?  Probably we should check and update the status field...
    if( UpdateStatus() ) {
        // something completed... see if we want to launch another
        CheckForLaunch();
    } else {
        // it's still running this is a superflous interrupt!
    }
}


U032 vddSetCallback(inSCBStruct*    inbuffer, outSCBStruct* outbuffer) {

    // remember in  buffer is 4 bytes
    //          out buffer is 4 bytes

    
    // ignore incoming value
    
    // setout outgoing value to the function pointer
    
    
        

    if(((U032*)inbuffer)[0] == IMAGESURFACE ) {
        // this is the IMAGE callback
        if(MySurfaces.ImageCallbackRoutine == NULL ) {
            MySurfaces.ImageCallbackRoutine = (CALLBPROC) ((U032*)inbuffer)[1];
            ((U032*)outbuffer)[0] = 0;
        } else {
            ((U032*)outbuffer)[0] = 13;
        }
    } else if(((U032*)inbuffer)[0] == VBISURFACE  ) {
        // this is the VBI callback
        if(MySurfaces.VBICallbackRoutine == NULL ) {
            MySurfaces.VBICallbackRoutine = (CALLBPROC)  ((U032*)inbuffer)[1];
            ((U032*)outbuffer)[0] = 0;
        } else {
            ((U032*)outbuffer)[0] = 13;
        }
    } else {
        // we don't recognize the surface type... let's bail with an error
            ((U032*)outbuffer)[0] = 13;
    }
    
    return 0;
}



U032 GetFlatCodeSelector()
{
    DWORD    Selector;
// to allow chris's editor to deal with funky assembly code
#define SKIPOMPARSE
#ifdef SKIPOMPARSE        
    _asm xor eax, eax
    _asm mov ax, cs
    _asm mov Selector, eax
#endif
    return (Selector);
}



U032 vddgetStoragePointer(U032 dummy1, U032 dummy2, U032 dummy3) {
//    MySurfaces.marker = 0xDEADBEEF;
//    MySurfaces.storCount = MAXDATASTORE;
    MySurfaces.lastSurfaceFlippedTo = 1000;    // for the overlay
    
    MySurfaces.pTimerNotifyProc = (U032) timerNotifyProc;
    MySurfaces.pNotifyCallbackProcI0 = (U032) wrapperProcI0;
    MySurfaces.pNotifyCallbackProcI1 = (U032) wrapperProcI1;
    MySurfaces.pNotifyCallbackProcV0 = (U032) wrapperProcV0;
    MySurfaces.pNotifyCallbackProcV1 = (U032) wrapperProcV1;
    // MySurfaces.pNotifyOverlay0NotifyProc = (U032) Overlay0NotifyProc;
    // MySurfaces.pNotifyOverlay1NotifyProc = (U032) Overlay1NotifyProc;
    MySurfaces.CallbackSelector = GetFlatCodeSelector();
    MySurfaces.pTheNotifiers = nvMyVPNotifiers;
    MySurfaces.pMTMNotifyCallback = (U032) MTMNotifyProc;
    return ((U032)&MySurfaces);
}



void    reportError(U032 interruptingBuffer,U032 oldSurface) {
    V016    status;
    V032 info32;                  
     V016 info16;

    info32 = nvMyVPNotifiers[notifyIndexFromBuffer(interruptingBuffer)].info32;
    info16 = nvMyVPNotifiers[notifyIndexFromBuffer(interruptingBuffer)].info16;

    status = nvMyVPNotifiers[notifyIndexFromBuffer(interruptingBuffer)].status;

    if( NV04D_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT & status ) {
        DPF ("   ERROR DETECTED BY NOTIFICATION:              Error Type: ERROR_PROTECTION_FAULT\n");
    }
    if( NV04D_NOTIFICATION_STATUS_ERROR_BAD_ARGUMENT & status ) {
        DPF ("   ERROR DETECTED BY NOTIFICATION:              Error Type: ERROR_BAD_ARGUMENT\n");
    }
    if( NV04D_NOTIFICATION_STATUS_ERROR_INVALID_STATE & status ) {
        DPF ("   ERROR DETECTED BY NOTIFICATION:              Error Type: ERROR_INVALID_STATE\n");
    }
    if( NV04D_NOTIFICATION_STATUS_ERROR_STATE_IN_USE & status ) {
        DPF ("   ERROR DETECTED BY NOTIFICATION:              Error Type: ERROR_STATE_IN_USE\n");
    }
    if( NV04D_NOTIFICATION_STATUS_WARNING_INVALID_DATA & status ) {
        DPF ("   ERROR DETECTED BY NOTIFICATION:              Error Type: WARNING_INVALID_DATA\n");
    }

    switch (info32) {
            case MY_EXTERNAL_DECODER_OBJECT:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_EXTERNAL_DECODER_OBJECT\n");    
                break;
            case MY_EXTERNAL_DECODER_NOTIFIER:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_EXTERNAL_DECODER_NOTIFIER\n");
                break;
            case MY_IMAGE0_BUFFER_CONTEXT:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_IMAGE0_BUFFER_CONTEXT\n");
                break;
            case MY_IMAGE1_BUFFER_CONTEXT:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_IMAGE1_BUFFER_CONTEXT\n");
                break;
            case MY_VBI0_BUFFER_CONTEXT:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_VBI0_BUFFER_CONTEXT\n");
                break;
            case MY_VBI1_BUFFER_CONTEXT:
                DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: MY_VBI1_BUFFER_CONTEXT\n");
                break;
            default:        
                //DPF ("   ERROR DETECTED BY NOTIFICATION:    Error Class Instance: Unknown Class Instance: %x",info32);        
                break;
    }
    
    //DPF ("   ERROR DETECTED BY NOTIFICATION:     Error Method Number: %x",info16);
    //DPF ("   ERROR DETECTED BY NOTIFICATION:     Interrupting Buffer: %x",interruptingBuffer);
    if( oldSurface == NOSURFACEPROGRAMED) {
        //DPF ("   ERROR DETECTED BY NOTIFICATION:       Surface Attempted: SURFACE NOT PROGRAMMED\n");
    } else {
        //DPF ("   ERROR DETECTED BY NOTIFICATION:       Surface Attempted: %x",oldSurface);
    }    

}

