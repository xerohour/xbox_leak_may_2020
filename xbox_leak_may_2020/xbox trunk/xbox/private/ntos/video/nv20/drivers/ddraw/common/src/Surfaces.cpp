/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
*                                                                           *
* Module: Surfaces.cpp                                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           3/26/98                                          *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (!defined(WINNT) && !defined(NVPE))

// we're going to use this definition so that we can
// do this under NT or Win9x "MULTI-MON" which is a cheeser way of saying
// that we're using the NValloc architecture
#define USE_NV_ALLOC_ARCH

// unit specific includes
#include "VidTex.h"

// Hey Folks some storage right here
extern vpSurfaces*          pMySurfaces;
//vpSurfaces            MySurfaces;

NvChannel*          pMyNvChan = NULL;
extern NvNotification*  nvMyVPNotifiers;

#define STARTINGBUF (99)
#define UNKNOWNBUF  (123)


// This is a hack to fix up stuff which a contradiction between the two known systems
// CODE this should be cleaned up as the NV4 style RM moves into NV3 land


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

    if( xIn < xOut ) {
        DPF ("   In dimension X is SMALLER than out X dimension %d, %d",xIn,xOut);
        return FALSE;
    }
    if( yIn < yOut ) {
        DPF ("   In dimension Y is SMALLER than out Y dimension %d, %d",yIn,yOut);
        return FALSE;
    }
    if( xIn % xOut != 0 ) {
        DPF ("   X dimension In does not evenly divide by X out %d, %d",xIn,xOut);
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
                    DPF ("   Not one of the good scale factors %d, %d, %d",xIn,xOut,xRatio);
                    return FALSE;
                }
                break;
    }

    return TRUE;
}


BOOL GetField(void) {
    BOOL    retvalue = TRUE;
    U032    whiletime = 0;
    FIELDTYPE aField = UNKNOWN;

    // we're not hitting the channel
    //INIT_FREE_COUNT(0);


    // clear the notifiers
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    // put the object into the channel
#ifdef NV3
    pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
    pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4

    // fire off the command
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[0] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[1] =  NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY;

    // wait for completion
    while ( ((nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
                 (nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS))    &&
                (whiletime++ < WHILETIMEOUT) );

    // only one of these should be running right now....
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)+NUM04DOFFSET].info16 == 0x1 /*NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET*/ ) {
        aField = pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[0]]->field;
    }
    if( nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)+NUM04DOFFSET].info16 == 0x1 /*NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET*/ ) {
        aField = pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[1]]->field;
    }

    if(aField == EVEN ) {
        retvalue = TRUE;
    } else {
        retvalue = FALSE;
    }

    return retvalue;
}

U032 checkNvAllocArchError(U032 error) {
    // returns true if there IS an error
    // otherwise false
    switch (error) {
            case NVOS04_STATUS_SUCCESS:
                return 0;
//              DPF("NVOS04_STATUS_SUCCESS");
                break;
            case NVOS04_STATUS_ERROR_OPERATING_SYSTEM:
                DPF("NVOS04_STATUS_ERROR_OPERATING_SYSTEM");
                return error;
                break;
            case NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT:
                DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT");
                return error;
                break;
            case NVOS04_STATUS_ERROR_BAD_OBJECT_NEW:
                DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_NEW");
                return error;
                break;
            case NVOS04_STATUS_ERROR_BAD_CLASS:
                DPF("NVOS04_STATUS_ERROR_BAD_CLASS");
                return error;
                break;
            case NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR:
                DPF("NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR");
                return error;
                break;
            case NVOS04_STATUS_ERROR_BAD_FLAGS:
                DPF("NVOS04_STATUS_ERROR_BAD_FLAGS");
                return error;
                break;
            case NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES:
                DPF("NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES");
                return error;
                break;
            default:
                    DPF("UNKNOWN ERROR");
                    return 1;
    }

    return 0;
}

DWORD SetupVP (LPDDHAL_CREATEVPORTDATA lpInput) {
    U032 error;
    U032 i;
    INIT_FREE_COUNT(0);

    pMyNvChan = pMySurfaces->pVPChanPtr;
    if(pMyNvChan == NULL ) {
        //CODE
        error = NvRmAllocChannelPio ((GLOBDATAPTR)->ROOTHANDLE,
                    NV_WIN_DEVICE,
                    MY_VPE_CHANNEL,
                    NV03_CHANNEL_PIO,
                    0,
                    (PVOID*)&(pMySurfaces->pVPChanPtr),
                    NVOS04_FLAGS_FIFO_RUNOUT_IGNORE);
        if(checkNvAllocArchError(error )) {
            DPF("   Warning: Unable to sucessfully complete NvRmAllocChannelPio");
            return DDHAL_DRIVER_NOTHANDLED;
        }


        pMyNvChan = pMySurfaces->pVPChanPtr;

        // Now store a copy in global land so that the brucerator's code can kill the channel on mode switches
        GLOBDATAPTR->NvDevFlatVPE = (U032) pMySurfaces->pVPChanPtr;

    }

    if(pMyNvChan == NULL ) {
        DPF("   Warning: the NV Channel for VPE is not initialized... sorry");
        return DDHAL_DRIVER_NOTHANDLED;
    } else {

            error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_VPE_CHANNEL,
                                MY_EXTERNAL_DECODER_OBJECT,
                                        NV03_EXTERNAL_VIDEO_DECODER
                                );

            if(checkNvAllocArchError(error )) {
                DPF("NVDD: Cannot allocate External Decoder Object %d",MY_EXTERNAL_DECODER_OBJECT);
                return FALSE;
            }
#ifdef NV3
            error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_VPE_CHANNEL,
                                MY_TIMER_OBJECT,
                                        NV_TIMER
                                      );
#else // !NV3
            error = NvRmAllocObject(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_VPE_CHANNEL,
                                MY_TIMER_OBJECT,
                                NV01_TIMER
                                );
#endif // !NV3
            if(checkNvAllocArchError(error )) {
                DPF("NVDD: Cannot allocate Timer Object %d",MY_TIMER_OBJECT);
                return FALSE;
            }

            // setup Notify Context

            if (NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_EXTERNAL_DECODER_NOTIFIER,
                                NV01_CONTEXT_DMA,
                                DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                (PVOID)(((U032)nvMyVPNotifiers) + NUM04DOFFSET*sizeof(NvNotification)),
                                (sizeof( NvNotification)*NUM04DNOTIFIER  - 1)
                         ) != NVOS03_STATUS_SUCCESS )
             {
                DPF((1, "NVDD: Cannot allocate notifier context"));
                return FALSE;
             }


            if (NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
                                MY_TIMER_NOTIFIER,
                                NV01_CONTEXT_DMA,
                                DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                (PVOID)(((U032)nvMyVPNotifiers) + NUM004OFFSET*sizeof(NvNotification)),
                                (sizeof( NvNotification)*NUM004NOTIFIER  - 1)
                         ) != NVOS03_STATUS_SUCCESS )
             {
                DPF((1, "NVDD: Cannot allocate notifier context"));
                return FALSE;
             }


            // Plug the notifiers into the main object
            // make sure the object is on the channel
#ifdef NV3
            pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
            pMyNvChan->subchannel[0].SetObject  = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4

            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaNotifies = MY_EXTERNAL_DECODER_NOTIFIER;

            // do something now to put the port into the proper mode...
            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
              if( lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth == 16 ) {
                  // this is the something
                  DPF((1, "NVDD: Now switching video port to 16 wide mode"));
                  pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImageConfig = 16 ;/* data width, task bit, null data  02f8-02fb*/
              } else if( lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth == 8 ) {
                  DPF((1, "NVDD: Now switching video port to 8 wide mode"));
                  pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImageConfig = 8 ;/* data width, task bit, null data  02f8-02fb*/
              } else {
                  DPF((1, "NVDD: invalid port width"));
              }
            }
#ifdef NV3
            pMyNvChan->subchannel[0].control.object = MY_TIMER_OBJECT;
            pMyNvChan->subchannel[0].timer.SetAlarmNotifyCtxDma = MY_TIMER_NOTIFIER;
#else
            pMyNvChan->subchannel[0].SetObject  = MY_TIMER_OBJECT;
            pMyNvChan->subchannel[0].nv01Timer.SetContextDmaNotifies = MY_TIMER_NOTIFIER;
#endif // NV4


 //**************************************************************************
 // Allocate a DMA context which points to all of video memory. The limit
 // must be page aligned: i.e. limit = (size in bytes of video mem rounded to the
 // closest page boundary) - 1.
 //**************************************************************************

        {
            void*  pFrameBuffer = (void*)(GLOBDATAPTR)->PBASEADDRESS;
            U032     dwFBLen= DWFBUFFERLEN;

            if (NvRmAllocContextDma(    (GLOBDATAPTR)->ROOTHANDLE,
                                            MY_IMAGE0_BUFFER_CONTEXT,
                                            NV01_CONTEXT_DMA,
                                            DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                            DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                            pFrameBuffer,
                                    dwFBLen
                                        ) != NVOS03_STATUS_SUCCESS )
            {
              DPF((1, "NVDD: Cannot allocate dma in memory context for MY_IMAGE0_BUFFER_CONTEXT"));
              return FALSE;
            }

             if (NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
                                            MY_IMAGE1_BUFFER_CONTEXT,
                                            NV01_CONTEXT_DMA,
                                            DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                            DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                    pFrameBuffer,
                                    dwFBLen
                                  ) != NVOS03_STATUS_SUCCESS )
              {
              DPF((1, "NVDD: Cannot allocate dma in memory context for MY_IMAGE1_BUFFER_CONTEXT"));
              return FALSE;
              }

            if (NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
                                  MY_VBI0_BUFFER_CONTEXT,
                                  NV01_CONTEXT_DMA,
                                  DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                  DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                  pFrameBuffer,
                                  dwFBLen
                                  ) != NVOS03_STATUS_SUCCESS )
            {
              DPF((1, "NVDD: Cannot allocate dma in memory context for MY_VBI0_BUFFER_CONTEXT"));
              return FALSE;
            }

            if (NvRmAllocContextDma(  (GLOBDATAPTR)->ROOTHANDLE,
                                  MY_VBI1_BUFFER_CONTEXT,
                                  NV01_CONTEXT_DMA,
                                  DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                                  DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                                  pFrameBuffer,
                                  dwFBLen
                                  ) != NVOS03_STATUS_SUCCESS )
            {
              DPF((1, "NVDD: Cannot allocate dma in memory context for MY_VBI1_BUFFER_CONTEXT"));
              return FALSE;
            }

        }


            CHECK_FREE_COUNT(pMyNvChan,10*5);

            // make sure decoder object is in the channel....
#ifdef NV3
            pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
            pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4
            // plug in our buffer contexts
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[0]     = MY_IMAGE0_BUFFER_CONTEXT;
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[1]     = MY_IMAGE1_BUFFER_CONTEXT;
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[0]       = MY_VBI0_BUFFER_CONTEXT;
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[1]       = MY_VBI1_BUFFER_CONTEXT;

        // Ok, done setting up the various contexts
        // now we should setup the class with some boring default values

            CHECK_FREE_COUNT(pMyNvChan,10*10);

            for( i = 0; i < 2; i++ ) {
                pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[i].sizeIn     = ((240 << 16 ) | 720 );   // *The next four method may fail if the scaling values do not work out properly.
                pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[i].sizeOut = ((240 << 16 ) | 720 );      // changing either In or Out values will modify the scaling values
                pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[i].offset        =0;         //  *indicates DMA "address" as on offset from frame buffer base

                //  we don't actually want to launch any captures at this point

                pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[i].offset      = 0;
                pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[i].size        = 0;
                //  we don't actually want to launch any captures at this point
            }


        // Ok, reset all the notifiers:

            for (i = 0; i < NUMVPNOTIFIERS ; i++ ) {
                nvMyVPNotifiers[i].status       = NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
                nvMyVPNotifiers[i].info32   = 0;
                nvMyVPNotifiers[i].info32   = 0;
                nvMyVPNotifiers[i].timeStamp.nanoseconds[0] = 0;
                nvMyVPNotifiers[i].timeStamp.nanoseconds[1] = 0;
            }


            // this means we're going to try for kevin's new cool callback dealy

            /*
             * NvAllocEvent() specifies a Windows event or callback for NV to send to the
             * application after requesting notifications with the
             * NV_OS_WRITE_THEN_AWAKEN style.
             *    first parameter is the NvChannel
             *    second is the object name
             *    third is the notify index
             *    fourth is the notify event type (NV_OS_EVENT_*)
             *    fifth is the lower 32bits of the 64bit event data
             *    sixth is the upper 32bits of the 64bit event data
             */

            // first object we're going to associate will be the MY_EXTERNAL_DECODER_OBJECT
            // we want to different notifiers associated here.

            pMySurfaces->bThreadDead = FALSE;
            pMySurfaces->bThreadRunning = TRUE;

            // I want VBI/Image/ 0 / 1  all to go to the same place


#ifdef NV3
// until someone gets this define in the right header file
#define  NV01_EVENT_KERNEL_CALLBACK                                (0x00000078)
#endif

            {
                U032 status = 0;

                status = NvRmAllocEvent (   (GLOBDATAPTR)->ROOTHANDLE,
                                        MY_EXTERNAL_DECODER_OBJECT,
                                MY_VBI0_BUFFER_EVENT,
                                NV01_EVENT_KERNEL_CALLBACK,
                                        NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0),
                                        (void*)pMySurfaces->pNotifyCallbackProcV0);

                if( status !=  0x0 /* meaning sucess */ ) {
                    DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
                }

                status = NvRmAllocEvent (   (GLOBDATAPTR)->ROOTHANDLE,
                                        MY_EXTERNAL_DECODER_OBJECT,
                                MY_VBI1_BUFFER_EVENT,
                                NV01_EVENT_KERNEL_CALLBACK,
                                        NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1),
                                        (void*)pMySurfaces->pNotifyCallbackProcV1);
               if( status !=  0x0 /* meaning sucess */ ) {
                    DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
                }
                status = NvRmAllocEvent (   (GLOBDATAPTR)->ROOTHANDLE,
                                        MY_EXTERNAL_DECODER_OBJECT,
                                MY_IMAGE0_BUFFER_EVENT,
                                NV01_EVENT_KERNEL_CALLBACK,
                                        NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0),
                                        (void*)pMySurfaces->pNotifyCallbackProcI0);
               if( status !=  0x0 /* meaning sucess */ ) {
                    DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
                }
                status = NvRmAllocEvent (   (GLOBDATAPTR)->ROOTHANDLE,
                                        MY_EXTERNAL_DECODER_OBJECT,
                                MY_IMAGE1_BUFFER_EVENT,
                                NV01_EVENT_KERNEL_CALLBACK,
                                        NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1),
                                        (void*)pMySurfaces->pNotifyCallbackProcI1);
               if( status !=  0x0 /* meaning sucess */ ) {
                    DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
                }

                 // And finally one for the timer
#ifdef NV3
#define NV004_NOTIFIERS_SET_ALARM_NOTIFY                           (1)
#endif
                 status = NvRmAllocEvent    (   (GLOBDATAPTR)->ROOTHANDLE,
                                        MY_TIMER_OBJECT,
                                MY_TIMER_EVENT,
                                NV01_EVENT_KERNEL_CALLBACK,
                                        NV004_NOTIFIERS_SET_ALARM_NOTIFY,
                                        (void*)pMySurfaces->pTimerNotifyProc);
               if( status !=  0x0 /* meaning sucess */ ) {
                    DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
                }

            }   // event alloc block




        // Decrement the number of ports available count
            (pMySurfaces->pDriverData)->dwVideoPortsAvailable--;


    }

    // this sets up the structures for ring0 data transfers
    SetupMTM();

    pMySurfaces->pVidTexSurf = NULL;
#ifndef NV3
    //initTextureSurf();
#endif

    pMySurfaces->SetupComplete = TRUE;
    return DDHAL_DRIVER_HANDLED;
}


void    tearDownVP(void) {
    U032    error;
    U032    i;
    U032    whiletime;
    INIT_FREE_COUNT(0);

    // stop things from running  ( check to make sure the channel is not null by which we'll assume
    //   that things are not running
    // tell the thread to exit
    pMySurfaces->bThreadRunning = FALSE;

    if(pMyNvChan != NULL)
        StopVP();

#ifndef NV3
    //DestroyTextureSurf();
#endif

    TearDownMTM();
#if 0
    {
             error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, NV_DD_DEV_VIDEO, MY_DVD_SUBPICTURE_OBJECT);


            if(checkNvAllocArchError(error )) {
                DPF("NVDD: Cannot deallocate DVD-subpicture Object %d",MY_DVD_SUBPICTURE_OBJECT);
            }
    }
#endif

    // this should wake up the thread
//  NvSetEvent( pMySurfaces->hVPInterruptEvent );

    // wait for it to die
    whiletime = 0;
    while( (!pMySurfaces->bThreadDead) &
            (whiletime++ < WHILETIMEOUT) );
    // make sure that the thread has really exited
    //TerminateThread((HANDLE)pMySurfaces->VpInterruptId,0);




    if(pMyNvChan != NULL ) {
        CHECK_FREE_COUNT(pMyNvChan,12*4);

        // stop the transfers
#ifdef NV3
        pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
        pMyNvChan->subchannel[0].SetObject  = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4

        if(pMySurfaces->dwVideoPortStatus != VP_NOT_RUNNING ) {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferImage   = 0;
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferVbi = 0;
        }

    }


#define NOEVENTFREEFUNCTIONYETFOLKS
#ifdef NOEVENTFREEFUNCTIONYETFOLKS
    DPF("Now ditching the EVENTs ");

    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_EXTERNAL_DECODER_OBJECT, MY_IMAGE0_BUFFER_EVENT);
   checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_EXTERNAL_DECODER_OBJECT, MY_IMAGE1_BUFFER_EVENT);
   checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_EXTERNAL_DECODER_OBJECT, MY_VBI0_BUFFER_EVENT);
   checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_EXTERNAL_DECODER_OBJECT, MY_VBI1_BUFFER_EVENT);
   checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_TIMER_OBJECT, MY_TIMER_EVENT);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, pMySurfaces->dwOverlayObjectID, MY_OVERLAY0_BUFFER_EVENT);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, pMySurfaces->dwOverlayObjectID, MY_OVERLAY1_BUFFER_EVENT);
    checkNvAllocArchError(error);


#endif

    DPF("Now setting notifiers to NULL");
    if(pMyNvChan != NULL ) {

// set notifiers to NULL context DMAs
#ifdef NV3
        pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
        pMyNvChan->subchannel[0].SetObject  = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaNotifies = 0;


#ifdef NV3
        pMyNvChan->subchannel[0].control.object = MY_TIMER_OBJECT;
        pMyNvChan->subchannel[0].timer.SetAlarmNotifyCtxDma = 0;
#else
        pMyNvChan->subchannel[0].SetObject  = MY_TIMER_OBJECT;
        pMyNvChan->subchannel[0].nv01Timer.SetContextDmaNotifies = 0;
#endif // NV4

// set DMA for transfers to NULL

#ifdef NV3
        pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
        pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;
#endif // NV4

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[0]     = 0;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[1]     = 0;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[0]       = 0;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[1]       = 0;




    // spin waiting for empty fifo
#ifdef NV3
        while(NvGetFreeCount(pMyNvChan, 0) < NV_GUARANTEED_FIFO_SIZE );
#else
        while(NvGetFreeCount(pMyNvChan, 0) < NV06A_FIFO_GUARANTEED_SIZE );
#endif

    }
//  I'm a bit concerned about syncing the FIFO with these NVRM API calls. but we'll let this slide for now

    DPF("Now ditching the Notifiers");
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_EXTERNAL_DECODER_NOTIFIER);
    checkNvAllocArchError(error);
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_TIMER_NOTIFIER);
    checkNvAllocArchError(error);



    DPF("Now ditching the Buffer contexts");
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_IMAGE0_BUFFER_CONTEXT);
    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_IMAGE0_BUFFER_CONTEXT");
    }
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_IMAGE1_BUFFER_CONTEXT);
    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_IMAGE1_BUFFER_CONTEXT");
    }
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_VBI0_BUFFER_CONTEXT);
    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_VBI0_BUFFER_CONTEXT");
    }
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, (GLOBDATAPTR)->ROOTHANDLE, MY_VBI1_BUFFER_CONTEXT);
    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_VBI1_BUFFER_CONTEXT");
    }




    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_VPE_CHANNEL, MY_TIMER_OBJECT);
    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_TIMER_OBJECT");
    }
    error = NvRmFree((GLOBDATAPTR)->ROOTHANDLE, MY_VPE_CHANNEL, MY_EXTERNAL_DECODER_OBJECT);

    if(checkNvAllocArchError(error)) {
        DPF("that was trouble freeing MY_EXTERNAL_DECODER_OBJECT");
    }



// increment the number of ports available
    (pMySurfaces->pDriverData)->dwVideoPortsAvailable++;
// shut down Media Port channel

    pMyNvChan = pMySurfaces->pVPChanPtr;
    {
        ULONG status;

        status = NvRmFree((GLOBDATAPTR)->ROOTHANDLE,
                                 NV_WIN_DEVICE,
                                 MY_VPE_CHANNEL);

        if( status == 0x0) {
            pMyNvChan = NULL;
            pMySurfaces->pVPChanPtr = NULL;
        } else {
            DPF("Uh Dude,  I couldn't deallocate the channel pointer... ");

        }
    }


    (pMySurfaces->pDriverData)->NvDevFlatVPE = (DWORD) NULL;
    pMyNvChan = NULL;

    // Ok, reset all the notifiers:

//  CLOSE_HANDLE(pMySurfaces->hThreadHandle);
    for (i = 0; i < NUMVPNOTIFIERS ; i++ ) {
        nvMyVPNotifiers[i].status       = NV04D_NOTIFICATION_STATUS_NOT_LAUNCHED;
        nvMyVPNotifiers[i].info32   = 0;
        nvMyVPNotifiers[i].info32   = 0;
        nvMyVPNotifiers[i].timeStamp.nanoseconds[0] = 0;
        nvMyVPNotifiers[i].timeStamp.nanoseconds[1] = 0;
    }
    pMySurfaces->SetupComplete = FALSE;

}


void StopVP(void) {
    U032    whiletime;

    INIT_FREE_COUNT(0);

#ifdef NV3
    NV_DD_DMA_PUSHER_SYNC();
#endif

    CHECK_FREE_COUNT(pMyNvChan,8);


#ifdef NV3
    pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
    pMyNvChan->subchannel[0].SetObject  = MY_EXTERNAL_DECODER_OBJECT;
#endif
    DPF ("   Firing Off Stop commands");

    if(pMySurfaces->dwVideoPortStatus != VP_NOT_RUNNING ) {
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferImage   = 0;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferVbi = 0;
    }
// wait for the last few notifies to show up ?

    pMySurfaces->bStopVP = TRUE;

    whiletime = 0;
    while ( ((nvMyVPNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
                 (nvMyVPNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
                (whiletime++ < WHILETIMEOUT) );

// CODE wait for VBI notifies.. too
//  while (nvMyVPNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS);
//  while (nvMyVPNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS);
    DPF ("   Done waiting for stops");

    // one more time to make sure those register get cleared
    if(pMySurfaces->dwVideoPortStatus != VP_NOT_RUNNING ) {
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferImage   = 0;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.StopTransferVbi = 0;
   }

}

DWORD getCurLine(void) {
    U032    whiletime;
    U032    possibleOffset = 0;
    U032    line;
    U032    returnLine;
    INIT_FREE_COUNT(0);

    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    returnLine = 0;

    // Put decoder object into the channel
    CHECK_FREE_COUNT(pMyNvChan,1*4);
#ifdef NV3
    pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
    pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;
#endif

    CHECK_FREE_COUNT(pMyNvChan,8);

    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[0]       = 0;            // *allows programming of either ODD or EVEN fields
    pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.GetImageOffsetNotify[1]       = 0;            // *allows programming of either ODD or EVEN fields

//CODE   there may still be in issue with divide by ZERO here.. I can't imagine pitch = 0.. but you never know...

    whiletime = 0;

    while ( ((nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
                 (nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
                (whiletime++ < WHILETIMEOUT) );


    if(nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)].info32 == 0x1 /*NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET*/ ) {
        possibleOffset = nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)].info32;
        line = possibleOffset - pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[0]]->dwOffset;
        returnLine = line / pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[0]]->dwPitch;
    }
    if(nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)].info32 == 0x1 /*NV04D_NOTIFICATION_INFO16_FIELD_VALID_OFFSET*/ ) {
        possibleOffset = nvMyVPNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(1)].info32;
        line = possibleOffset - pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[1]]->dwOffset;
        returnLine = line / pMySurfaces->Surfaces[pMySurfaces->CurrentBufferSurface[1]]->dwPitch;
    }

    return returnLine;
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
                    DPF ("   Problem... trying get an index for an unknown buffer notify");
                    return UNKNOWNBUF;
                }
    }
}


VPERRORS replaceSurfaceList(DWORD dwNumAutoflip,DWORD dwNumVBIAutoflip,
                                        LPDDRAWI_DDRAWSURFACE_INT   *lplpDDSurface,
                                        LPDDRAWI_DDRAWSURFACE_INT   *lplpDDVBISurface,
                                        U032 bAutoflip ) {
    VPERRORS    localError = NOVPERROR;
    U032    imageSurfCount = 0;
    U032    VBISurfCount = 0;
    U032    i;
    U032    next;

    pMySurfaces->surfMemAllocCount = 0;

    ASSERT(!((lplpDDSurface         != NULL) ^ (dwNumAutoflip > 0)));
    ASSERT(!((lplpDDVBISurface  != NULL) ^ (dwNumVBIAutoflip > 0)));
    if ((dwNumAutoflip == 0 ) && (dwNumVBIAutoflip ==0 )
         && (bAutoflip)) // H.AZAR (09/02/99): we must check NumAutoflip != 0 only if autoflip flag is set !!!
    {
      return CANTCREATESURFACE;
    }

    DPF ("   We have #Image=%d  #VB=%d autoflip surfaces",dwNumAutoflip,dwNumVBIAutoflip);

    localError = clearAllSurfaces();
    if( localError != NOVPERROR ) {
        return localError;
    }

    imageSurfCount = 0;

    if(lplpDDSurface != NULL) {
        // first count the surfaces
        imageSurfCount = 0;
        if(bAutoflip) {
            while( (imageSurfCount < dwNumAutoflip) &&(lplpDDSurface[imageSurfCount] != NULL )) {
                imageSurfCount++;
            }
        } else {
            while(lplpDDSurface[imageSurfCount] != NULL ) {
                imageSurfCount++;
            }
        }
        if(bAutoflip) {
            if(imageSurfCount > dwNumAutoflip ) {
                DPF("More in list than are autoflip!");
                imageSurfCount = dwNumAutoflip;
            }
        }
        // next add the surfaces to the list
        i = 0;
        while((lplpDDSurface[i] != NULL ) && (i <imageSurfCount) ) {
            next = i + 1;
            if( next == imageSurfCount )
                next = 0;
            localError = addSurface(createSurface(lplpDDSurface[i], bAutoflip, FALSE, NULL,next));
            i=i+1;
        }

        DPF("Added %d Image surfaces",imageSurfCount);
    }
    pMySurfaces->VBIstart = imageSurfCount;


    if(lplpDDVBISurface!=NULL ) {
        // now count VBI surfaces
        VBISurfCount = 0;
        // if we're auto flip then don't check beyond what numVBIAutoflip passes into us since
        // some lamers don't zero terminate their list in this case
        if(bAutoflip) {
            while((VBISurfCount < dwNumVBIAutoflip) && (lplpDDVBISurface[VBISurfCount] != NULL ) ) {
                VBISurfCount++;
            }
        } else {
            while(lplpDDVBISurface[VBISurfCount] != NULL ) {
                VBISurfCount++;
            }
        }
        if(bAutoflip) {
            if(VBISurfCount > dwNumVBIAutoflip ) {
                DPF("More in VBI list than are autoflip!");
                VBISurfCount = dwNumVBIAutoflip;
            }
        }
        // next add the surfaces to the list
        i = 0;
        // All the VBI surfaces are offset by a certain amount
        while((lplpDDVBISurface[i] != NULL ) && (i <VBISurfCount) ) {
            next = i + 1;
            if( next == VBISurfCount )
                next = 0;
            localError = addSurface(createSurface(lplpDDVBISurface[i], bAutoflip, TRUE, NULL,next+pMySurfaces->VBIstart));
            i=i+1;
        }
        DPF("Added %d VBI surfaces",VBISurfCount);
    }


    pMySurfaces->CurrentBufferSurface[0] = NOSURFACEPROGRAMED;
    pMySurfaces->CurrentBufferSurface[1] = NOSURFACEPROGRAMED;
    pMySurfaces->CurrentBufferSurface[2] = NOSURFACEPROGRAMED;
    pMySurfaces->CurrentBufferSurface[3] = NOSURFACEPROGRAMED;

    if( localError == NOVPERROR ) {
        DPF ("   Replaced surfaces - OK");
    } else {
        switch (localError) {
                case    TOOMANYSURFACESERROR:
                    DPF ("   Replaced surfaces - TOOMANYSURFACESERROR");
                    break;
                case CANTCREATESURFACE:
                    DPF ("   Replaced surfaces - CANTCREATESURFACE");
                    break;
                case SURFACENOTFOUND:
                    DPF ("   Replaced surfaces - SURFACENOTFOUND");
                    break;
                case NOSURFACEPROGRAMED:
                    DPF ("   Replaced surfaces - NOSURFACEPROGRAMED");
                    break;
        }

        DPF ("   Replaced surfaces there was a problem = %d",localError);
    }

#if 0
#ifdef DEBUG
    DPF("Now checking the surface thingie");
    {
        U032    i;
        for(i=0;i<pMySurfaces->numberOfSurfaces;i++) {
            DPF("#Surfaces = %d surface = %d, nextSurface = %d, prevSurcace = %d",pMySurfaces->numberOfSurfaces,i,getNextSurface(i),getPrevSurface(i));
        }
    }

    DPF("done checking");

#endif

#endif
    return localError;
}


VPERRORS    addSurface(vpSurfaceInfo*   pNewSurface) {
    ASSERT(pNewSurface  != NULL);

    if(pMySurfaces->numberOfSurfaces >= MAXVPSURFACES ) {
        return  TOOMANYSURFACESERROR;
    }
    if( pNewSurface == NULL) {
        return CANTCREATESURFACE;
    }

    pMySurfaces->Surfaces[pMySurfaces->numberOfSurfaces] = pNewSurface;
    pMySurfaces->numberOfSurfaces++;

    return NOVPERROR;
}

vpSurfaceInfo* createSurface(LPDDRAWI_DDRAWSURFACE_INT pNewSurface, U032 bAutoflip,
                                        U032 bVBI, NvNotification* pToNotify,U032 nextSurface) {


    vpSurfaceInfo*      pSurface;
    U032 Delta;

    ASSERT( pNewSurface != NULL);
    // unlikely to ever be not NULL
    //ASSERT( pToNotify != NULL);
    ASSERT( nextSurface < MAXVPSURFACES);

    pSurface = &pMySurfaces->surfMemory[pMySurfaces->surfMemAllocCount];
    pMySurfaces->surfMemAllocCount++;

    pSurface->surfHandle = pNewSurface;
    pSurface->bAutoFlip = bAutoflip;
    pSurface->pVidMem = VIDMEM_ADDR(pNewSurface->lpLcl->lpGbl->fpVidMem);
    pSurface->ddsCaps = pNewSurface->lpLcl->ddsCaps.dwCaps;
    pSurface->nextSurfaceNum = nextSurface;
    // store the offset of the intermediate hidden surface
    pSurface->dwRes1 = pNewSurface->lpLcl->lpGbl->dwReserved1;
//

    pSurface->dwPitch = pNewSurface->lpLcl->lpGbl->lPitch;



    // move things around in the surface
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
      Delta = 0;   // in NV10 the overlay can do these calculations..
    } else {
      Delta = pMySurfaces->dwOriginX*2 + pMySurfaces->dwOriginY*pSurface->dwPitch;
    }

    pSurface->dwOffset = (unsigned long) pSurface->pVidMem
                    - (unsigned long)(pMySurfaces->pDriverData)->BaseAddress
                    + Delta;


    if(bVBI) {
        pSurface->dwStartLine = pMySurfaces->dwVBIStartLine;
    } else {
        if( pMySurfaces->CurrentScanMode == INTERLEAVBUF) {
                // both field have bigger pitch
                pSurface->dwPitch = pNewSurface->lpLcl->lpGbl->lPitch * 2;
            }



        //if( pMySurfaces->dwImageStartLine < 6 )
        //  pSurface->dwStartLine = 6;
        //else
            if(pMySurfaces->dwImageStartLine == 1) {
                pSurface->dwStartLine = 16; //tried 17 no much- 15 can see stuff on top of intercast disk - 16?
            } else {
                pSurface->dwStartLine = pMySurfaces->dwImageStartLine;
            }
    }

#define VIDEOEXTRALEN (0)
    // make sure we're not trying to cram something into a surface that's too small
    if(bVBI ) {
        pSurface->dwHeight = pMySurfaces->dwVBIHeight;
    } else {
        if ( pMySurfaces->dwInHeight > pNewSurface->lpLcl->lpGbl->wHeight ) {
            pSurface->dwHeight = pNewSurface->lpLcl->lpGbl->wHeight + VIDEOEXTRALEN;
        } else {
            pSurface->dwHeight = pMySurfaces->dwInHeight + VIDEOEXTRALEN;
        }
    }
    if ( pMySurfaces->dwInWidth > pNewSurface->lpLcl->lpGbl->wWidth ) {
        pSurface->dwWidth = pNewSurface->lpLcl->lpGbl->wWidth;
    } else {
        pSurface->dwWidth = pMySurfaces->dwInWidth;
    }

    pSurface->dwPreScaleSize =  (pMySurfaces->dwPreWidth) | ((pMySurfaces->dwPreHeight+VIDEOEXTRALEN) << 16);
            // we know that there are TWO bytes per pixel in UYVY format
    //pSurface->dwLength = pNewSurface->lpLcl->lpGbl->lPitch * ((U032)pNewSurface->lpLcl->lpGbl->wHeight) * 2;
    pSurface->dwLength = pSurface->dwPitch * (((U032)pSurface->dwHeight) ) * 2;

    return pSurface;
}


VPERRORS replaceSurfaceWithSurface(U032 surfaceToReplace, LPDDRAWI_DDRAWSURFACE_LCL pNewSurface, U032  bVBI) {

    vpSurfaceInfo*      pSurface;
    U032                    Delta;

    ASSERT( pNewSurface != NULL);

    pSurface = pMySurfaces->Surfaces[surfaceToReplace];
// not sure if this is necessary or not....
//  pSurface->pNvNotify = &(nvMyVPNotifiers[notifyIndexFromBuffer(0)]);

    pSurface->surfHandle = pNewSurface;
    // pSurface->bAutoFlip = bAutoflip; // leave this the same!
    pSurface->pVidMem = VIDMEM_ADDR(pNewSurface->lpGbl->fpVidMem);
    pSurface->ddsCaps = pNewSurface->ddsCaps.dwCaps;
    // pSurface->nextSurfaceNum = nextSurface;      // leave this the same!

    pSurface->dwPitch = pNewSurface->lpGbl->lPitch;

    // move things around in the surface
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
      Delta = 0;   // in NV10 the overlay can do these calculations..
    } else {
      Delta = pMySurfaces->dwOriginX*2 + pMySurfaces->dwOriginY*pSurface->dwPitch;
    }
    pSurface->dwOffset = (unsigned long) pSurface->pVidMem
                    - (unsigned long)(pMySurfaces->pDriverData)->BaseAddress
                    + Delta;

    if( pMySurfaces->CurrentScanMode == INTERLEAVBUF) {
        // both field have bigger pitch
        pSurface->dwPitch = pNewSurface->lpGbl->lPitch * 2;
    }

    if(bVBI) {
        pSurface->dwStartLine = pMySurfaces->dwVBIStartLine;
    } else {
        //if( pMySurfaces->dwImageStartLine < 6 )
        //  pSurface->dwStartLine = 6;
        //else
            if(pMySurfaces->dwImageStartLine == 1) {
                pSurface->dwStartLine = 16; //tried 17 no much- 15 can see stuff on top of intercast disk - 16?
            } else {
                pSurface->dwStartLine = pMySurfaces->dwImageStartLine;
            }
    }

#define VIDEOEXTRALEN (0)
    // make sure we're not trying to cram something into a surface that's too small
    if(bVBI ) {
        pSurface->dwHeight = pMySurfaces->dwVBIHeight;
    } else {
        if ( pMySurfaces->dwInHeight > pNewSurface->lpGbl->wHeight ) {
            pSurface->dwHeight = pNewSurface->lpGbl->wHeight + VIDEOEXTRALEN;
        } else {
            pSurface->dwHeight = pMySurfaces->dwInHeight + VIDEOEXTRALEN;
        }
    }
    if ( pMySurfaces->dwInWidth > pNewSurface->lpGbl->wWidth ) {
        pSurface->dwWidth = pNewSurface->lpGbl->wWidth;
    } else {
        pSurface->dwWidth = pMySurfaces->dwInWidth;
    }

    pSurface->dwPreScaleSize =  (pMySurfaces->dwPreWidth) | ((pMySurfaces->dwPreHeight+VIDEOEXTRALEN) << 16);
            // we know that there are TWO bytes per pixel in UYVY format
    //pSurface->dwLength = pNewSurface->lpGbl->lPitch * ((U032)pNewSurface->lpGbl->wHeight) * 2;
    pSurface->dwLength = pSurface->dwPitch * (((U032)pSurface->dwHeight) ) * 2;

    return NOVPERROR;

}

VPERRORS clearAllSurfaces() {
    U032    i;

    for (i=0;i<pMySurfaces->numberOfSurfaces; i++ ) {
        pMySurfaces->Surfaces[i] = NULL;
    }

    pMySurfaces->surfMemAllocCount = 0;
    pMySurfaces->numberOfSurfaces = 0;

    return NOVPERROR;
}

U032    findSurface(LPDDRAWI_DDRAWSURFACE_LCL pNewSurface ) {
    U032    i;

    for(i = 0 ; i < pMySurfaces->numberOfSurfaces; i ++ ) {
        if( pNewSurface->lpGbl->fpVidMem == pMySurfaces->Surfaces[i]->pVidMem ) {
            // Ok, we found the surface!
            return i;
        }

    }

    return SURFACENOTFOUND;
}

#if 0

FIELDTYPE   getNextFieldType(FIELDTYPE lastFieldType) {

    switch (pMySurfaces->CurrentCaptureMode) {
            case EVENODD:
                if(lastFieldType ==EVEN ) {
                    return ODD;
                } else if( lastFieldType ==ODD ) {
                    return EVEN;
                } else {
                    return EVEN;
                }
                break;
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
                ASSERT(pMySurfaces->CurrentCaptureMode == ODDODD );
    }
}



U032    ProgramBufferWithSurface(U032   dwBufferIndex,U032  newSurfaceIndex, U032 bZeroLength) {
    // This function is responsible for programming up and then launching buffer/surface combination
    // This will finish Asyncronously, and either be relaunched in the case of auto-flip, or be able
    // to report back usefully with the VPE functions
    U032    nextField;
    U032    dwInterleaveAddition;
    INIT_FREE_COUNT(0);

    ASSERT(dwBufferIndex < 4 );
    ASSERT(newSurfaceIndex < MAXVPSURFACES );
    //Update the surface structures
        // notify


    pMySurfaces->Surfaces[newSurfaceIndex]->pNvNotify = &(nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)]);
    pMySurfaces->CurrentBufferSurface[dwBufferIndex] = newSurfaceIndex;


    // Figure out the whole field thing
    nextField = getNextFieldType(pMySurfaces->lastField);
    pMySurfaces->Surfaces[newSurfaceIndex]->field = nextField;
    pMySurfaces->lastField = nextField;
    pMySurfaces->curSurf = newSurfaceIndex;

#define PRINTPROGRAM
#ifdef PRINTPROGRAM
    DPF("   ProgramBufferWithSurface");
    DPF("           dwBufferIndex = %d",dwBufferIndex);
    DPF("         newSurfaceIndex = %d",newSurfaceIndex);
    DPF("             bZeroLength = %d",bZeroLength);
    switch (nextField) {
            case ODD:
                 DPF("                   field = ODD");
                break;
            case EVEN:
                 DPF("                   field = EVEN");
                break;
            case UNKNOWN:
                 DPF("                   field = UNKNOWN");
                break;
    }
#endif //PRINTPROGRAM

    // if we're interleaving AND we're on the EVEN field then move everything down a bit
    //   OOD field is on top
    if(( pMySurfaces->CurrentScanMode == INTERLEAVBUF) && (nextField == EVEN ) ) {
        dwInterleaveAddition = pMySurfaces->Surfaces[newSurfaceIndex]->dwPitch/2;
    } else {
        dwInterleaveAddition = 0;
    }
    // choose which type of surface this is
    if( dwBufferIndex <2 ) {
        // Image surface
        // setup the field polarity, offset and fire it off


        // Put decoder object into the channel
        CHECK_FREE_COUNT(pMyNvChan,1*4);
#ifdef NV3
        pMyNvChan->subchannel[0].control.object = MY_EXTERNAL_DECODER_OBJECT;
#else
        pMyNvChan->subchannel[0].SetObject = MY_EXTERNAL_DECODER_OBJECT;
#endif

        // for now use the same "in" as "out"

        CHECK_FREE_COUNT(pMyNvChan,5*4);

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImageStartLine =
            pMySurfaces->Surfaces[newSurfaceIndex]->dwStartLine;

// check for invalid scaling combo's

        // if prescale is not on 2:1 integer boundaries from output, then fail...
        {
            BOOL bScale = checkScalingOk((pMySurfaces->Surfaces[newSurfaceIndex]->dwWidth) |
                                                    (pMySurfaces->Surfaces[newSurfaceIndex]->dwHeight << 16)
                            ,pMySurfaces->Surfaces[newSurfaceIndex]->dwPreScaleSize);

            if(! bScale ) {
                DPF("   Invalid Scaling parameters in use...");

                return (-7);
            }
        }

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeIn =
            (pMySurfaces->dwInWidth) | (pMySurfaces->dwInHeight << 16);

        if( bZeroLength ) {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeOut = 0;
        } else {
            pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].sizeOut =
                pMySurfaces->Surfaces[newSurfaceIndex]->dwPreScaleSize;
        }

#ifdef PRINTPROGRAM
        DPF("                  offset = %x",pMySurfaces->Surfaces[newSurfaceIndex]->dwOffset + dwInterleaveAddition);
        DPF("                  pitch  = %x",pMySurfaces->Surfaces[newSurfaceIndex]->dwPitch);
#endif //PRINTPROGRAM
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].offset    =
                pMySurfaces->Surfaces[newSurfaceIndex]->dwOffset + dwInterleaveAddition;

        // There is some sly behavior in the field statement...
        // since progressive fields are expected to be delivered in the EVEN fields we only check for OOD
        // and then everything else is considered to be even
        nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetImage[dwBufferIndex].format =
                ( pMySurfaces->Surfaces[newSurfaceIndex]->dwPitch & 0xFFFF )  |
                ((( ( nextField == ODD ) ? NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD) << 16 ) & 0xFF0000 )  |
                ((NV_OS_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000 ) ;


    } else {
        CHECK_FREE_COUNT(pMyNvChan,3*4);

        // VBI surface
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].size  =
            (pMySurfaces->Surfaces[newSurfaceIndex]->dwHeight << 16) | (pMySurfaces->Surfaces[newSurfaceIndex]->dwStartLine) ;

        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].offset    =
            pMySurfaces->Surfaces[newSurfaceIndex]->dwOffset;

        nvMyVPNotifiers[notifyIndexFromBuffer(dwBufferIndex)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
        pMyNvChan->subchannel[0].nv03ExternalVideoDecoder.SetVbi[dwBufferIndex-2].format =
            ( pMySurfaces->Surfaces[newSurfaceIndex]->dwPitch & 0xFFFF )  |
            ((( ( nextField == ODD ) ? NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD) << 16 ) & 0xFF0000 )  |
            ((NV_OS_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000 ) ;

    }
    //Update the Capture Class

    return 0;
}


DWORD WINAPI VPInterruptThread(PVOID daParameter) {

    DWORD       waitReturnValue;
    HANDLE  myEvent = (HANDLE) daParameter;

    ASSERT(myEvent != NULL);

    DPF ("   |||||||| Thread Started");

    while(pMySurfaces->bThreadRunning) {
        // sit and wait...
        // we'll wake up on the event which can be triggered EITHER by the RM
        // OR by our parent telling us to wake up and then exit
        waitReturnValue = NvWaitEvent(myEvent,INFINITE);
        if( pMySurfaces->bThreadRunning ) {
            if(!IS_EVENT_ERROR(waitReturnValue)) {
                //DPF ("   interrupt");
                //HandleBufferInterrupt();
                if( pMySurfaces->myTable.handleBufferInterrupts != NULL ) {
                        U032 retval = (pMySurfaces->myTable.handleBufferInterrupts)();
                        if(retval != 0)
                            DPF("FUCKUP!!!!!!!!!   this is ab ad thing    what sometimes this is bad = %8x",retval);
                    } else {
                        ASSERT(1==0);
                        // massive failure... hang out here for a while
                    }
            }
        }
    }

    DPF ("   |||||||| Thread Ending");

    pMySurfaces->bThreadDead = TRUE;

    return 0;
}



#endif
#endif //(!defined(WINNT) && !defined(NVPE))
