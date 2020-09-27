//******************************************************************************
//
// Module Name: DMAMACRO.H
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1998 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/
 
#define DECLARE_DMA_FIFO                                                \
    ULONG       nvDmaCount;                                             \
    ULONG       *nvDmaFifo                                              \
                         
                         
#define INIT_LOCAL_DMA_FIFO                                             \
    nvDmaCount = ppdev->nvDmaCount;                                     \
    nvDmaFifo = ppdev->nvDmaFifo;                                      \
    
#define NV_DMA_FIFO                 nvDmaFifo[nvDmaCount++]

#define UPDATE_PDEV_DMA_COUNT       ppdev->nvDmaCount  = nvDmaCount

#define UPDATE_LOCAL_DMA_COUNT      nvDmaCount = ppdev->nvDmaCount   


//******************************************************************************
// DMA Pusher Control DWORD macros
//******************************************************************************

#define NV_DMAPUSH_CONTROL_WORD(NUM_DWORD, SUBCH, METHOD_OFFSET)            \
            ( ((NUM_DWORD)<<18) | ((SUBCH)<<13) | (METHOD_OFFSET) )

#define NV_DMAPUSH_START(NUM_DWORD, SUBCH, METHOD_OFFSET)                   \
{\
            nvDmaFifo[nvDmaCount++] = NV_DMAPUSH_CONTROL_WORD(NUM_DWORD,    \
                                                              SUBCH,        \
                                                              METHOD_OFFSET); \
}                                                              

#define NV_DMAPUSH_SET_DEST_BASE(ppdev, Offset, Stride)                     \
{                                                                           \
    if ((Offset != ppdev->CurrentDestOffset) ||                             \
        (Stride != (LONG)ppdev->CurrentDestPitch))                          \
        {                                                                   \
        (ppdev->pfnSetDestBase)(ppdev, Offset, Stride);                     \
        }                                                                   \
}                                                              

#define NV_DMAPUSH_SET_SOURCE_BASE(ppdev, Offset, Stride)                   \
{                                                                           \
    if ((Offset != ppdev->CurrentSourceOffset) ||                           \
        (Stride != (LONG)ppdev->CurrentSourcePitch))                        \
        {                                                                   \
        (ppdev->pfnSetSourceBase)(ppdev, Offset, Stride);                   \
        }                                                                   \
}                                                              

//******************************************************************************
// DMAPUSH_WRITE1 macro 
//
// Writes a control dword followed by ONE method value
// 
// SUBCH            = subchannel for the methods placed into the push buffer
// METHOD_OFFSET    = offset to the first method in the subchannel to
//                    be filled in by the push buffer commands
// METHOD_VALUE     = Actual value sent to hardware
// 
//
//
// Example Usage of sending MULTIPLE method values:
//
//            NV_DMAPUSH_START(NUM_DWORDS, SUBCHANNEL, STARTING_OFFSET)
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE1
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE2
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE3
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE4
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE5
//            nvDmaFifo[nvDmaCount++] = METHOD_VALUE6
//            ... up to N methods
//
//******************************************************************************

#define NV_DMAPUSH_WRITE1(SUBCH, METHOD_OFFSET, METHOD_VALUE)               \
{\
            NV_DMAPUSH_START(1, SUBCH, METHOD_OFFSET);                      \
            nvDmaFifo[nvDmaCount++] = METHOD_VALUE;                         \
}


//******************************************************************************
// CREATE OBJECT macro
//
// SUBCHANNEL to be used
// CLASS type to create
// INSTANCE name
//******************************************************************************

//#define NV_CREATE_OBJECT(SUBCH,CLASS,INSTANCE)                              \
//{\
//            NV_DMAPUSH_WRITE1(SUBCH, NVFFF_SET_OBJECT, CLASS);              \
//            NV_DMAPUSH_WRITE1(SUBCH, NVFFF_CREATE, INSTANCE);               \
//}            

//******************************************************************************
// DESTROY OBJECT macro
//
// SUBCHANNEL to be used
// CLASS type to destroy
// INSTANCE name
//******************************************************************************

#define NV_DESTROY_OBJECT(SUBCH,CLASS,INSTANCE)                              \
{\
            NV_DMAPUSH_WRITE1(SUBCH, NVFFF_SET_OBJECT, CLASS);               \
            NV_DMAPUSH_WRITE1(SUBCH, NVFFF_DESTROY, INSTANCE);               \
}            


//******************************************************************************
// Used to reset the DMA buffer address back to the beginning
//******************************************************************************

#define NV4_JUMP(ADDR)                              (0x20000000 | (ADDR))


//******************************************************************************
//
// NV_DMAPUSH_CHECKFREE() macro
//
//
//         WRAP_FLAG = FALSE
//
//         ---------  0  
//        |         |    
//        |         |    
//        |         |    
//        |         |  
//        |         |  
//        |         |  
//        |  Push   |  
//        | Buffer  |  
//        |         |  <----- Get Ptr (Normally, the GET ptr will be 'less'
//        |         |    |             than the PUT ptr. It constantly tries
//        |         |    v             to catchup to the PUT ptr.
//        |         |
//        |         |  <----- Cached Put Ptr 
//        |         |    | 
//        |         |    v
//        |         |  <----- DMA Count (Where CPU writes data)  
//        |         |    
//         ---------    
//
//         Total Size of Push Buffer
//
//
//  ---------------------------------------------------------------------------
//
//
//         WRAP_FLAG = TRUE
//          
//         ---------  0  
//        |         |    
//        |         |    
//        |         |  <----- Cached Put Ptr (Where the last hardware PUT occurred)  
//        |         |    |                   (PUT ptr must not get ahead of the GET ptr)  
//        |         |    v
//        |         |  
//        |  Push   |  <----- DMA Count (Where CPU currently writes data)
//        | Buffer  |    |
//        |         |    |
//        |         |    v            
//        |         |                
//        |         |  <----- Get Ptr   
//        |         |    | 
//        |         |    v
//        |         |    
//        |         |    
//         ---------    
//
//         Total Size of Push Buffer
//
//
//  Current Setings:    DmaPushBufTotalSize = 512k
//    
//******************************************************************************

//******************************************************************************
// PADDING is needed because when we wraparound, 
// we sometimes insert 1 JUMP dword at the end (need an extra dword)
// (Specified in DWORDS)
//******************************************************************************


#define NV_DMAPUSH_CHECKFREE(DWORDS_NEEDED)               \
    {\
\
    /***************************************************************************/ \
    /* 2 scenarios:                                                            */ \
    /*      1) DMA Count Ptr is ahead of the Get Ptr.  (DmaCount >= Get Ptr)   */ \
    /*      2) Get Ptr is ahead of the DMA Count Ptr   (DmaCount < Get Ptr)    */ \
    /***************************************************************************/ \
\
    if (ppdev->nvDmaWrapFlag == FALSE)\
        {\
\
        /***********************************************************************/ \
        /* WRAP_FLAG = FALSE                                                   */ \
        /* Check if we need to wraparound.                                     */ \
        /* Convert PushBufTotalSize to DWORDS                                  */ \
        /***********************************************************************/ \
        if ( (nvDmaCount + DWORDS_NEEDED) > (ppdev->DmaPushBufCanUseSize) )       \
            {\
            /*******************************************************************/ \
            /* Make sure to update the PDEV copy of the DmaCount first!!       */ \
            /*******************************************************************/ \
            ppdev->nvDmaCount = nvDmaCount;\
\
            NV_DmaPush_Wrap(ppdev,(ULONG)(DWORDS_NEEDED));\
\
            /*******************************************************************/ \
            /* Make sure to update the LOCAL copy of the DmaCount!!            */ \
            /*******************************************************************/ \
            nvDmaCount=ppdev->nvDmaCount;\
            }\
\
        }\
\
    else\
\
        {\
        /***********************************************************************/ \
        /* Make sure to update the PDEV copy of the DmaCount first!!           */ \
        /***********************************************************************/ \
        ppdev->nvDmaCount = nvDmaCount;\
        /***********************************************************************/ \
        /* WRAP_FLAG = TRUE                                                    */ \
        /* We are currently 'wrapped around' (DMA Count < Get Ptr)             */ \
        /* Make sure DMA Count does NOT go past GET ptr                        */ \
        /***********************************************************************/ \
        NV_DmaPush_CheckWrapped(ppdev,(ULONG)(DWORDS_NEEDED));\
\
        /***********************************************************************/ \
        /* Make sure to update the LOCAL copy of the DmaCount!!                */ \
        /***********************************************************************/ \
        nvDmaCount=ppdev->nvDmaCount;\
        }\
    }

                    


