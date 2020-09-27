/******************************************************************************
 *                                                                            *
 *                      Voxware Proprietary Material                          *
 *                      Copyright 1996, Voxware, Inc.                         *
 *                           All Rights Resrved                               *
 *                                                                            *
 *                     DISTRIBUTION PROHIBITED without                        *
 *                   written authorization from Voxware                       *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  testRT.c                                                        *
 * PURPOSE:   To demonstrate the use of the Voxware RT29 (+RT24), BC29 codec. *
 *                                                                            *
 * AUTHOR:    Epiphany Vera, Ilan Berci                                       *
 *                                                                            *
 * $Header:   P:/r_and_d/archives/realtime/testcode/testRT.c_v   1.11   23 Jul 1998 10:00:08   weiwang  $
 *****************************************************************************/

/** ************************************************************************ **/
/**                              Standard headers                            **/
/** ************************************************************************ **/
/* This section defines name changes and standard header files needed for the */
/* DSP Research Quick Start (QS) library. If compiling the code to run on a   */
/* DSP Research board that uses the QS library, then compile with the _QS     */
/* option turned on.                                                          */

/** ************************************************************************ **/
/**                              Standard headers                            **/
/** ************************************************************************ **/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h> 
#include <assert.h>
#include <ctype.h>

/* For every codec that you would like to include in the executable, */
#include "metatest.h"

/** ************************************************************************ **/
/**                    Voxware Core Interface Headers                        **/
/** ************************************************************************ **/
#include "vci.h" 
#include "vciError.h"
#include "vciPlus.h"
#if(RT24==1)      
#include "vciRT24.h"
#endif
#if(RT28==1) 
#include "vciRT28.h"
#endif
#if(RT29==1)      
#include "vciRT29.h"
#endif
#if(VR12==1) 
#include "vciVR12.h"
#include "vciPVR12.h"
#endif
#if(VR15==1) 
#include "vciVR15.h"
#include "vciPVR15.h"
#endif

#if(VFII==1) 
#include "vciVFII.h"
#endif

#if(RT24==1||RT24LD==1||RT28==1||RT29==1||RT29LD==1)
#include "vciPlsRT.h"    /* Voxware Core Interface Extension Manager Warping  */
#endif
#if(RT29==1)      
#include "PsQuan29.h"    /* Model Pseudo Quantization */
#endif

/** ************************************************************************ **/
/**                             Windows Specific                             **/
/** ************************************************************************ **/

#ifdef WIN32
#include "NspTimer.h"
#endif

/** ************************************************************************ **/
/**                            Macintosh Specific                            **/
/** ************************************************************************ **/

#define VOXWARE_MAC 0

#if (VOXWARE_MAC==1)
#include <console.h>
#endif

/** ************************************************************************ **/
/**                              Global Defines                              **/
/** ************************************************************************ **/

#ifdef WIN32
#define TIMER_NO   1
#define TEST_TIMES 5
#define MAX_FRAME_NO 6000
#endif

#define RATE8K 8000.0F

enum {VOICE_FONTS=0x02, PRINT=0x04, NUMBERS=0x08, WARP=0x10,
      BYTE_SWAP=0x20, DECODE=0x40, ENCODE=0x80, BENCH_MARK=0x100};

FILE *InFile, *OutFile, *VoxFileIn=NULL, *VoxFileOut=NULL; /* PCM files for both input and output. */

typedef struct tagCodecFuncPtrBlk {
   VCI_RETCODE (*pVciEncode)(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciEncodeIOBlk);
   VCI_RETCODE (*pVciDecode)(void *hCodecMemBlk, VCI_CODEC_IO_BLOCK *hvciDecodeIOBlk);

   VCI_RETCODE (*pVciInitEncode)(void **hCodecMemBlk);
   VCI_RETCODE (*pVciInitDecode)(void **hCodecMemBlk);
                                                      
   VCI_RETCODE (*pVciFreeEncode)(void **hCodecMemBlk);
   VCI_RETCODE (*pVciFreeDecode)(void **hCodecMemBlk);

   VCI_RETCODE (*pVciSetWarpFactor)(void *hCodecMemBlk, float wWarpFactor);
   VCI_RETCODE (*pVciDecodeNeedsInput)(void *hCodecMemBlk, unsigned char *pUnPack);

   unsigned int numOfBits;
   unsigned int numOfBytes;
   unsigned int frameLen;
} CodecFuncPtrBlk; 

#ifdef WIN32
static float ENCODER_TIMER[MAX_FRAME_NO];
static float DECODER_TIMER[MAX_FRAME_NO];
#endif

/** ************************************************************************ **/
/**                           Private Funtion Defs                           **/
/** ************************************************************************ **/
void InitInOutDevices(int argc, char **argv, FILE **InFile, FILE **OutFile, unsigned int echo);
static unsigned short 
   CommandLineParse(int argc, char **argv, int* pNumOfFrames, CodecFuncPtrBlk* pCodecFuncPtrBlk);
static float setWarp(void);
void byteSwap(short *sound_buffer, unsigned int bufferSize);
static int bufToFile(unsigned char *pBuf, FILE *pFile, int bits, short last);
static int fileToCBuf(VCI_CODEC_IO_BLOCK *hvciIOBlk, FILE *pFile, int bytes);


/** ************************************************************************ **/
/**                              MAIN FUNCTION                               **/
/** ************************************************************************ **/

void RTmain(int argc, char** argv)  /* MSVC arg1: input.pcm  arg2: output.pcm */
{
   short done=0;                      /* Exit flag.                           */
   int frame=0;                      /* Frame counter.                       */
   void *hCodecMemblk1;               /* Handle to encoder instance of codec. */
   void *hCodecMemblk2;               /* Handle to decoder instance of codec. */
   unsigned short wCommandLineParse;  /* Parse the options from the com. line */
   int NumOfFrames=0;
   unsigned char wNeedsInputFlag=0;
   CodecFuncPtrBlk codecFuncPtrBlk;

   int currentBit = 0;
   double averageBitE = 0.0;
   int minBitE = 1000, maxBitE = 0;
   double averageBitD = 0.0;
   int minBitD = 1000, maxBitD = 0;
   int maxReadByte;
   int lastReadByteOffset = 0, lastReadBitOffset = 0;
#ifdef WIN32
    int EncoderPeakIndex=0, DecoderPeakIndex=0;
    float DecoderPeak=0.0, EncoderPeak=0.0;
    float DecoderAverage=0.0, EncoderAverage=0.0;
    float tempClock=0.0;
    unsigned int uiTestCount=0;
    int i;
#endif


   VCI_RETCODE wVciRetcode = VCI_NO_ERROR;

   VCI_CODEC_IO_BLOCK hvciEncodeIOBlk;    /* Encoder interface structure.     */
   VCI_CODEC_IO_BLOCK hvciDecodeIOBlk;    /* Decoder interface structure.     */


#if VFII == 1
   void *pFontChunkMemBlk = NULL;
   short FontSlection     = 10;
#endif

#if (VOXWARE_MAC==1)
/*     argc = ccommand(&argv);*/

#endif

#ifdef WIN32
   do {
#endif
      /* Parse the rest of the command line */
      wCommandLineParse=CommandLineParse(argc, argv, &NumOfFrames, 
                                         &codecFuncPtrBlk);

      maxReadByte = codecFuncPtrBlk.numOfBits/8;
      if (8*maxReadByte < (int)codecFuncPtrBlk.numOfBits)
	maxReadByte ++;

      hvciEncodeIOBlk.wVoxBufferSize = (short)(maxReadByte + 2);
      hvciDecodeIOBlk.wVoxBufferSize = hvciEncodeIOBlk.wVoxBufferSize;

      done=0;
      frame=0;

      /* Initialize an instance of the codec as an encoder. */
      if(ENCODE&wCommandLineParse) {  
         if(!(hvciEncodeIOBlk.pPCMBuffer
            =calloc(codecFuncPtrBlk.frameLen,sizeof(short)))) 
         {
            printf("\nFailed to allocated memory! .. exiting");
            exit(1);
         }
         if(!(hvciEncodeIOBlk.pVoxBuffer=calloc(hvciEncodeIOBlk.wVoxBufferSize,
                                        sizeof(unsigned char)))) 
         {
            printf("\nFailed to allocated memory! .. exiting");
            exit(1);
         }
         if(PRINT&wCommandLineParse)
            printf("\n\nInitializing: Encoder.\n");
         if(codecFuncPtrBlk.pVciInitEncode(&hCodecMemblk1)) 
          {
            printf ("Memory Allocation error.\n");
            printf ("Failed to intitialize memory for encoder instance. \n");
            exit(1);
          }
         hvciEncodeIOBlk.wVoxBufferReadByteOffset  = 0;
         hvciEncodeIOBlk.chVoxBufferReadBitOffset  = 0;
         hvciEncodeIOBlk.wVoxBufferWriteByteOffset = 0;
         hvciEncodeIOBlk.chVoxBufferWriteBitOffset = 0;
      }
      
      /* Initialize an instance of the codec as a decoder. */
      if(DECODE&wCommandLineParse) {
         if(!(hvciDecodeIOBlk.pPCMBuffer=calloc(codecFuncPtrBlk.frameLen,sizeof(short))))
         {
            printf("\nFailed to allocated memory! .. exiting");
            exit(1);
         }
         if(!(hvciDecodeIOBlk.pVoxBuffer=calloc(hvciDecodeIOBlk.wVoxBufferSize, 
                                                sizeof(unsigned char)))) 
         {
            printf("\nFailed to allocated memory! .. exiting");
            exit(1);
         }
         if(PRINT&wCommandLineParse)
             printf("Initializing: Decoder.\n");
         if(codecFuncPtrBlk.pVciInitDecode(&hCodecMemblk2)) {
            printf ("Memory Allocation error.\n");
            printf ("Failed to intitialize memory for decoder instance. \n");
            codecFuncPtrBlk.pVciFreeEncode(&hCodecMemblk1); /* woops! time to free encoder and leave! */
            exit(1);
         }

         hvciDecodeIOBlk.wVoxBufferReadByteOffset = 0;
         hvciDecodeIOBlk.chVoxBufferReadBitOffset = 0;
         hvciDecodeIOBlk.wVoxBufferWriteByteOffset = 0;
         hvciDecodeIOBlk.chVoxBufferWriteBitOffset = 0;
      }

#if (VFII==1)
      if(VOICE_FONTS&wCommandLineParse) {
        if(vciEnableVoiceFontsVFII(hCodecMemblk1))  {
           printf("Failed to initialize memory for vciEnableVoiceFontsVFII routine. \n");
           exit(1);
         }
         if(vciEnableVoiceFontsVFII(hCodecMemblk2)) {
           printf ("Failed to intitialize memory for voice font instance. \n");
           exit(1);
         } 
         if(vciGetVoiceFontChunkVFII(FontSlection ,&pFontChunkMemBlk)) {
           printf ("Failed to intitialize memory for voice font chunk. \n");
           exit(1);
         } 
         if(vciSetVoiceFontVFII(hCodecMemblk1,pFontChunkMemBlk)) {
           printf ("Failed to set voice font for encode. \n");
           exit(1);
         } 
         if(vciSetVoiceFontVFII(hCodecMemblk2,pFontChunkMemBlk)) {
           printf ("Failed to set voice font for decode. \n");
           exit(1);
         } 
       }
#endif /* (VFII==1) */
     
      /** ****************************************************************** **/
      /**                       Encode-Decode Loop                           **/
      /** ****************************************************************** **/
      /* This loop reads input from some input device, compresses it, passes  */
      /* the output bit-stream to the decoder, the decoder decodes it and     */
      /* writes its PCM output to the output device.                          */

      if(PRINT&wCommandLineParse && ENCODE&wCommandLineParse)  
	printf(" Encoding ... \n");

      if(PRINT&wCommandLineParse && DECODE&wCommandLineParse)  
	printf(" Decoding ... \n");

      while(!done&&(frame<NumOfFrames||NumOfFrames==0)) {
         if(NUMBERS&wCommandLineParse && PRINT&wCommandLineParse)
            printf("\nframe: %d : ",frame);

         if(ENCODE&wCommandLineParse) {
            /* This implementation uses the VoxBuffer as a simple/linear      */
            /* buffer. Thus after reading the encoder output, we have to      */
            /* reset the write offsets for the encoder VoxBuffer to zero.     */
            /* This way the encoder always writes from the beginning of the   */
            /* buffer.                                                        */
            hvciEncodeIOBlk.wVoxBufferWriteByteOffset = 0;
            hvciEncodeIOBlk.chVoxBufferWriteBitOffset = 0;

            /* Get a frame of speech speech for compressing                   */
            if(NUMBERS&wCommandLineParse && PRINT&wCommandLineParse)
               printf(" Reading ...");

            memset(hvciEncodeIOBlk.pPCMBuffer, 0, 
                   sizeof(short)*codecFuncPtrBlk.frameLen);
            if(fread(hvciEncodeIOBlk.pPCMBuffer, sizeof(short), 
               codecFuncPtrBlk.frameLen, InFile)!= codecFuncPtrBlk.frameLen)
               done=1;

            if(BYTE_SWAP&wCommandLineParse)
               byteSwap(hvciEncodeIOBlk.pPCMBuffer, codecFuncPtrBlk.frameLen);

	    /* clean Vox buffer */
	    memset(hvciEncodeIOBlk.pVoxBuffer, 0, hvciEncodeIOBlk.wVoxBufferSize
		   *sizeof(unsigned char));

            /** ************************************************************ **/
            /**                            Encode                            **/
            /** ************************************************************ **/
            if((NUMBERS&wCommandLineParse) && (PRINT&wCommandLineParse))
               printf(" Encoding ...");

               if(BENCH_MARK&wCommandLineParse) {
               /*** start TIMER for ENCODE**/
#ifdef WIN32
                tstTimerStart(TIMER_NO);
#endif
            }
               wVciRetcode = codecFuncPtrBlk.pVciEncode(hCodecMemblk1, &hvciEncodeIOBlk);

               if(BENCH_MARK&wCommandLineParse) {
            /*** stop TIMER for ENCODE ***/
#ifdef WIN32
                tstTimerStop(TIMER_NO);
                 tempClock = tstTimerSec(TIMER_NO);

                   if((tempClock < ENCODER_TIMER[frame] || uiTestCount==0)&&frame<MAX_FRAME_NO)
                        ENCODER_TIMER[frame] = tempClock;
#endif
            }

            if(wVciRetcode) {
               printf("\nEncoder error : %d", wVciRetcode);
               printf("\nIoBlk:");
               printf("\n   Read bit offset: %d", 
                      hvciEncodeIOBlk.chVoxBufferReadBitOffset);
               exit(1);
            }

            currentBit = (int)(hvciEncodeIOBlk.wVoxBufferWriteByteOffset)*8
	                 + (int)hvciEncodeIOBlk.chVoxBufferWriteBitOffset; 
            if (currentBit < minBitE)
               minBitE = currentBit;
            if (currentBit > maxBitE)
               maxBitE = currentBit;
            averageBitE += (double)currentBit;

            if (NUMBERS&wCommandLineParse && PRINT&wCommandLineParse)
               printf("Current Bit = %d\n", currentBit);

            if(VoxFileOut) {
               if(bufToFile(hvciEncodeIOBlk.pVoxBuffer, VoxFileOut, currentBit, done)) 
                  done=1;
	    }
         }

         if(DECODE&wCommandLineParse) {
            if(VoxFileIn) {
               if(fileToCBuf(&hvciDecodeIOBlk, VoxFileIn, maxReadByte))  
                  done=1;
            } else {
               memcpy(hvciDecodeIOBlk.pVoxBuffer, hvciEncodeIOBlk.pVoxBuffer, 
                  hvciDecodeIOBlk.wVoxBufferSize);

	       /* In this example, the decoder VoxBuffer is also used as a simple/linear */
	       /* buffer. Thus the decoder must always read from the beggining of the    */
	       /* buffer.                                                                */
	       hvciDecodeIOBlk.wVoxBufferReadByteOffset  = 0;
	       hvciDecodeIOBlk.chVoxBufferReadBitOffset  = 0;

               hvciDecodeIOBlk.chVoxBufferWriteBitOffset=hvciEncodeIOBlk.chVoxBufferWriteBitOffset;
               hvciDecodeIOBlk.wVoxBufferWriteByteOffset=hvciEncodeIOBlk.wVoxBufferWriteByteOffset;
            }

            /** ************************************************************ **/
            /**                            Decode                            **/
            /** ************************************************************ **/
            if(WARP&wCommandLineParse)
               codecFuncPtrBlk.pVciSetWarpFactor(hCodecMemblk2, setWarp());
            else
               codecFuncPtrBlk.pVciSetWarpFactor(hCodecMemblk2, 1.0F);

            do {
 	       if(NUMBERS&wCommandLineParse && PRINT&wCommandLineParse)
                   printf(" Decoding ...");

               if(BENCH_MARK&wCommandLineParse) {
               /** start TIMER for DECODER **/
#ifdef WIN32
                  tstTimerStart(TIMER_NO);
#endif
               }
               wVciRetcode = codecFuncPtrBlk.pVciDecode(hCodecMemblk2, &hvciDecodeIOBlk);

               if(BENCH_MARK&wCommandLineParse) {
                  /** stop TIMER for DECODER **/
#ifdef WIN32
                  tstTimerStop(TIMER_NO);
                  tempClock = tstTimerSec(TIMER_NO);

                  if((tempClock < DECODER_TIMER[frame] || uiTestCount==0)&&frame<MAX_FRAME_NO)
                    DECODER_TIMER[frame] = tempClock;
#endif
               }

               if(wVciRetcode) {
                  printf("\nDecoder error : %d", wVciRetcode);
                  exit(1);
               }

               /* If warping is turned off, the decoder always outputs 180    */
               /* samples, where 180 is the number of samples per frame. If   */
               /* warping is less than 1.0, i.e. during speedup, the number   */
               /* of output samples is variable and is sometimes zero.        */
               if(hvciDecodeIOBlk.wSamplesInPCMBuffer) { 
                  /* If the number of samples is non-zero, write to output.   */
                  if(NUMBERS&wCommandLineParse && PRINT&wCommandLineParse)
		     printf(" Writing ");
                  if(BYTE_SWAP&wCommandLineParse)
                     byteSwap(hvciDecodeIOBlk.pPCMBuffer, codecFuncPtrBlk.frameLen);
                  fwrite(hvciDecodeIOBlk.pPCMBuffer, sizeof(short), 
                         hvciDecodeIOBlk.wSamplesInPCMBuffer, OutFile);
               }
               codecFuncPtrBlk.pVciDecodeNeedsInput(hCodecMemblk2,&wNeedsInputFlag);
            }while(!wNeedsInputFlag); 
            /* If warping is greater than 1.0, i.e. during slow-down, there   */
            /* are cases whereby the decoder does not need to take input in   */
            /* order to give output. The while loop above makes sure that all */
            /* the output the decoder has is read before the decoder is given */
            /* more input.                                                    */

	    /* write out the frame bits */
            currentBit = (int)(hvciDecodeIOBlk.wVoxBufferReadByteOffset)*8
                  + (int)hvciDecodeIOBlk.chVoxBufferReadBitOffset 
                  - (int)(lastReadByteOffset*8) - (int)lastReadBitOffset;

            if (currentBit < 0)
                currentBit += (hvciDecodeIOBlk.wVoxBufferSize*8);
		
            if (currentBit < minBitD)
                minBitD = currentBit;
            if (currentBit > maxBitD)
                maxBitD = currentBit;
            averageBitD += (double)currentBit;

            if (NUMBERS&wCommandLineParse && PRINT&wCommandLineParse) {
               printf("Current Bit = %d\n", currentBit);
            }

            if (VoxFileIn) {
	      lastReadByteOffset = hvciDecodeIOBlk.wVoxBufferReadByteOffset;
      	      lastReadBitOffset = hvciDecodeIOBlk.chVoxBufferReadBitOffset;
	    }


         } /* end of 'if (DECODE&wCommandLineParse) '*/
         frame++; 
      } /* while(!done) */

       /* VoiceFonts */
#if (VFII==1)
       if(VOICE_FONTS&wCommandLineParse) {
       vciDisableVoiceFontsVFII(hCodecMemblk1);
       vciDisableVoiceFontsVFII(hCodecMemblk2);
       vciFreeVoiceFontChunkVFII(pFontChunkMemBlk);
       }
#endif /* (VFII==1) */
     
      if(ENCODE&wCommandLineParse) {
	if(PRINT&wCommandLineParse) {
            printf("\nFreeing Encoder ");

	    averageBitE /= (double)frame;

     	    printf("\naverage bps = %.2f\nminimum bps = %.2f\nmaximum bps = %.2f\n", 
		 averageBitE*RATE8K/(float)codecFuncPtrBlk.frameLen, 
		 (float)minBitE*RATE8K/(float)codecFuncPtrBlk.frameLen, 
		 (float)maxBitE*RATE8K/(float)codecFuncPtrBlk.frameLen);
	}

         codecFuncPtrBlk.pVciFreeEncode(&hCodecMemblk1); 
         free(hvciEncodeIOBlk.pPCMBuffer);
         free(hvciEncodeIOBlk.pVoxBuffer);
      }
  
      if(DECODE&wCommandLineParse) {
	if(PRINT&wCommandLineParse) {
          printf("\nFreeing Decoder ");

          averageBitD /= (double)frame;

	  printf("\naverage bps = %.2f\nminimum bps = %.2f\nmaximum bps = %.2f\n", 
		 averageBitD*RATE8K/(float)codecFuncPtrBlk.frameLen,
		 (float)minBitD*RATE8K/(float)codecFuncPtrBlk.frameLen,
		 (float)maxBitD*RATE8K/(float)codecFuncPtrBlk.frameLen);
	}

         codecFuncPtrBlk.pVciFreeDecode(&hCodecMemblk2);
         free(hvciDecodeIOBlk.pPCMBuffer);
         free(hvciDecodeIOBlk.pVoxBuffer);
      }

      if(InFile)
         fclose(InFile);
      if(OutFile)
         fclose(OutFile);

      if(VoxFileIn)
         fclose(VoxFileIn);
      if(VoxFileOut)
         fclose(VoxFileOut);

#ifdef WIN32
   } while(!(!(BENCH_MARK&wCommandLineParse) || (++uiTestCount>=TEST_TIMES)));
#endif

#ifdef WIN32
   if(BENCH_MARK&wCommandLineParse) {
        frame=(frame>MAX_FRAME_NO)?MAX_FRAME_NO:frame;
        for(i=1;i<frame;i++) {
            if(ENCODER_TIMER[i]>EncoderPeak)
            {
                EncoderPeak = ENCODER_TIMER[i];
                EncoderPeakIndex = i;
            }
            if(DECODER_TIMER[i]>DecoderPeak)
            {
                DecoderPeak = DECODER_TIMER[i];
                DecoderPeakIndex = i;
            }

          EncoderAverage += ENCODER_TIMER[i];
          DecoderAverage += DECODER_TIMER[i];
        }

        EncoderAverage/= ((frame)*(float)codecFuncPtrBlk.frameLen/RATE8K);
        DecoderAverage/= ((frame)*(float)codecFuncPtrBlk.frameLen/RATE8K);
        EncoderPeak /=(float)codecFuncPtrBlk.frameLen/RATE8K;
        DecoderPeak /=(float)codecFuncPtrBlk.frameLen/RATE8K;
  
        printf("\n\nEncodeAverage CPU time: %.1f", EncoderAverage*100);
        printf("\nEncodePeak    CPU time: %.1f", EncoderPeak*100);
        printf("\nEncoder Peak Index %d", EncoderPeakIndex);

        printf("\n\nDecodeAverage CPU time: %.1f", DecoderAverage*100);
        printf("\nDecodePeak    CPU time: %.1f", DecoderPeak*100);
        printf("\nDecoder Peak Index %d\n", DecoderPeakIndex);
    }
   printf("\n");
#else
   printf("Finished!\n");
#endif

  return;
}


/** ************************************************************************ **/
/**                                                                          **/
/** FUNCTION: CommandLineParse()                                             **/
/** PURPOSE : Parses the command line                                        **/
/**                                                                          **/
/** ************************************************************************ **/
static unsigned short CommandLineParse(int argc, char **argv, int* pNumOfFrames, 
                                       CodecFuncPtrBlk* pCodecFuncPtrBlk)
{
   unsigned short i=0; 
   unsigned short wComLineParse=0;
   unsigned char wordSet;
   char *pParse;
   char *pVoxFileName=NULL;
   char *pCodecType="";
   unsigned char fileCount=0, codecSet=0;
   VCI_CODEC_INFO_BLOCK vciInfoBlk;

   while(++i<argc)
   {
      pParse=argv[i];
      if(*pParse!='-')
         continue;
      wordSet=0;
      while(*++pParse&&!wordSet)
      {
         switch(*pParse) 
         {
            case 'C':
            case 'c':
               if(!(*++pParse)) {
                  i++;
                  pParse=argv[i];
               }
               pCodecType=pParse;
               wordSet=1;
               break;

            case 'I':
            case 'i':
               wComLineParse|=ENCODE;
               if(!(*++pParse)) {
                  i++;
                  pParse=argv[i];
               }
               if(!(InFile = fopen(pParse, "rb"))) {
                  printf("Cannot open %s\n", pParse);
                  exit(1);
               } else 
                  fileCount++;
               wordSet=1;
               break;

            case 'O':
            case 'o':
               wComLineParse|=DECODE;
               if(!(*++pParse)) {
                  i++;
                  pParse=argv[i];
               }
               if(!(OutFile = fopen(pParse, "wb"))) {
                  printf("Cannot open %s\n", pParse);
                  exit(1);
               } else
                  fileCount++;
               wordSet=1;
               break;

            case 'V':
            case 'v':
               if(!(*++pParse)) {
                  i++;
                  pParse=argv[i];
               }
               pVoxFileName=pParse;
               fileCount++;
               wordSet=1;
               break;

            case 'F':
            case 'f':
               wComLineParse|=VOICE_FONTS;                       
               break;

            case 'P':
            case 'p':
               wComLineParse|=PRINT;
               break;

            case 'N':
            case 'n':
               wComLineParse|=NUMBERS;
               break;

            case 'W':
            case 'w':
               wComLineParse|=WARP;
               break;

            case 'B':
            case 'b':
               wComLineParse|=BYTE_SWAP;
               break;


            case 'T':
            case 't':
               wComLineParse|=BENCH_MARK;
               break;

            case 'H':
            case 'h':

               printf("\nUsage: %s <-iInfile> [-vVoxfile] <-oOutfile> [-Options]\n", argv[0]);
               printf("       %s <-vVoxfile> <-oOutfile> [-Options]\n", argv[0]);
               printf("       %s <-iInfile> <-vVoxfile> [-Options]\n\n",argv[0]);
               printf("Options:\n");

               printf("        -c      codec type (RT29/RT24/RT28/RT29LD/RT24LD/VR12/VR15) \n");
               printf("        -i      precedes input file (raw) \n");
               printf("        -o      precedes output file (raw) \n");
               printf("        -v      precedes raw vox file    \n\n");
               printf("        -f      voice fonts  \n");
               printf("        -#      number of frames to run \n");
               printf("        -p      print out codec operations \n");
               printf("        -n      print out frame numbers \n");
               printf("        -b      byte swap the data\n");
               printf("        -w      turn on the warping test \n");
               printf("        -t      bench mark test \n");

               if(InFile) 
                  fclose(InFile);
               if(OutFile)
                  fclose(OutFile);
               exit(1);
               break;

            default:
               if(isdigit(*pParse)) {
                  *pNumOfFrames=atoi(pParse);
                  wordSet=1;
               }
               else {
                  printf("\nUnrecognized command line argument: %c",*pParse);
                  printf("\nType -h for help\n");
                  exit(1);
               }
         }
      }
   }
   if(fileCount<2) {
      printf("\nNot enough files in command line");
      printf("\nRun program with -h for help");
      exit(1);
   }

   if(pVoxFileName)
   {
      if(ENCODE&wComLineParse) {
         if(!(VoxFileOut= fopen(pVoxFileName, "wb"))) {
            printf("Cannot open %s\n", pVoxFileName);
            exit(1);
         }
      }
      if(DECODE&wComLineParse) {
	if (VoxFileOut == NULL) {
	  /* Cannot read vox file for variable codec! */
         if(!(VoxFileIn = fopen(pVoxFileName, "rb+"))) {
            printf("Cannot open %s\n", pVoxFileName);
            exit(1);
         }
	}
      }
   }
#if(RT24==1) 
   if(!strcmp(pCodecType,"RT24")) {
      pCodecFuncPtrBlk->pVciDecode    = vciDecodeRT24;
      pCodecFuncPtrBlk->pVciEncode    = vciEncodeRT24;
      pCodecFuncPtrBlk->pVciInitEncode= vciInitEncodeRT24;
      pCodecFuncPtrBlk->pVciInitDecode= vciInitDecodeRT24;
      pCodecFuncPtrBlk->pVciFreeEncode= vciFreeEncodeRT24; 
      pCodecFuncPtrBlk->pVciFreeDecode= vciFreeDecodeRT24;
      vciGetInfoRT24(&vciInfoBlk);
      pCodecFuncPtrBlk->numOfBits     = (int)vciInfoBlk.dwMaxPacketBits;
      pCodecFuncPtrBlk->frameLen      = vciInfoBlk.wNumSamplesPerFrame;
      pCodecFuncPtrBlk->pVciSetWarpFactor=vciSetWarpFactorRT;
      pCodecFuncPtrBlk->pVciDecodeNeedsInput=vciDecodeNeedsInputRT;
      codecSet=1;
   }
#endif
#if(RT28==1)
   if(!strcmp(pCodecType,"RT28")) {
      pCodecFuncPtrBlk->pVciDecode    = vciDecodeRT28;
      pCodecFuncPtrBlk->pVciEncode    = vciEncodeRT28;
      pCodecFuncPtrBlk->pVciInitEncode= vciInitEncodeRT28;
      pCodecFuncPtrBlk->pVciInitDecode= vciInitDecodeRT28;
      pCodecFuncPtrBlk->pVciFreeEncode= vciFreeEncodeRT28;
      pCodecFuncPtrBlk->pVciFreeDecode= vciFreeDecodeRT28;
      vciGetInfoRT28(&vciInfoBlk);
      pCodecFuncPtrBlk->numOfBits     = (int)vciInfoBlk.dwMaxPacketBits;
      pCodecFuncPtrBlk->frameLen      = vciInfoBlk.wNumSamplesPerFrame;
      pCodecFuncPtrBlk->pVciSetWarpFactor=vciSetWarpFactorRT;
      pCodecFuncPtrBlk->pVciDecodeNeedsInput=vciDecodeNeedsInputRT;
      codecSet=1;
   }
#endif
#if(VR12==1)
   if(!strcmp(pCodecType,"VR12")) {
      pCodecFuncPtrBlk->pVciDecode    = vciDecodeVR12;
      pCodecFuncPtrBlk->pVciEncode    = vciEncodeVR12;
      pCodecFuncPtrBlk->pVciInitEncode= vciInitEncodeVR12;
      pCodecFuncPtrBlk->pVciInitDecode= vciInitDecodeVR12;
      pCodecFuncPtrBlk->pVciFreeEncode= vciFreeEncodeVR12;
      pCodecFuncPtrBlk->pVciFreeDecode= vciFreeDecodeVR12;
      vciGetInfoVR12(&vciInfoBlk);
        pCodecFuncPtrBlk->numOfBits     = (int)vciInfoBlk.dwMaxPacketBits;
      pCodecFuncPtrBlk->frameLen      = vciInfoBlk.wNumSamplesPerFrame;
      pCodecFuncPtrBlk->pVciSetWarpFactor=vciSetWarpFactorVR12;
      pCodecFuncPtrBlk->pVciDecodeNeedsInput=vciDecodeNeedsInputVR12;
      codecSet=1;
   }
#endif
#if(VR15==1)
   if(!strcmp(pCodecType,"VR15")) {
      pCodecFuncPtrBlk->pVciDecode    = vciDecodeVR15;
      pCodecFuncPtrBlk->pVciEncode    = vciEncodeVR15;
      pCodecFuncPtrBlk->pVciInitEncode= vciInitEncodeVR15;
      pCodecFuncPtrBlk->pVciInitDecode= vciInitDecodeVR15;
      pCodecFuncPtrBlk->pVciFreeEncode= vciFreeEncodeVR15;
      pCodecFuncPtrBlk->pVciFreeDecode= vciFreeDecodeVR15;
      vciGetInfoVR15(&vciInfoBlk);
        pCodecFuncPtrBlk->numOfBits     = (int)vciInfoBlk.dwMaxPacketBits;
      pCodecFuncPtrBlk->frameLen      = vciInfoBlk.wNumSamplesPerFrame;
      pCodecFuncPtrBlk->pVciSetWarpFactor=vciSetWarpFactorVR15;
      pCodecFuncPtrBlk->pVciDecodeNeedsInput=vciDecodeNeedsInputVR15;
      codecSet=1;
   }
#endif
#if(RT29==1) /* default codec is the RT29 */
   if(!codecSet) { 
      pCodecFuncPtrBlk->pVciDecode    = vciDecodeRT29;
      pCodecFuncPtrBlk->pVciEncode    = vciEncodeRT29;
      pCodecFuncPtrBlk->pVciInitEncode= vciInitEncodeRT29;
      pCodecFuncPtrBlk->pVciInitDecode= vciInitDecodeRT29;
      pCodecFuncPtrBlk->pVciFreeEncode= vciFreeEncodeRT29; 
      pCodecFuncPtrBlk->pVciFreeDecode= vciFreeDecodeRT29;
      vciGetInfoRT29(&vciInfoBlk);
      pCodecFuncPtrBlk->numOfBits     = (int)vciInfoBlk.dwMaxPacketBits;
      pCodecFuncPtrBlk->frameLen      = vciInfoBlk.wNumSamplesPerFrame;
      pCodecFuncPtrBlk->pVciSetWarpFactor=vciSetWarpFactorRT;
      pCodecFuncPtrBlk->pVciDecodeNeedsInput=vciDecodeNeedsInputRT;
      codecSet=1;
   }
#endif
   pCodecFuncPtrBlk->numOfBytes=pCodecFuncPtrBlk->numOfBits/8;
   if(pCodecFuncPtrBlk->numOfBits%8)
      pCodecFuncPtrBlk->numOfBytes++;

   if(!codecSet) {
      printf("\nCodec type not specified and RT29 (Default is not available)\n");
      exit(1);
   }
   return wComLineParse;
}

float setWarp(void)
{
   static float step = 0.0F;
   float warp=1.0F;
   float temp;
   float inc;

   temp = (float)(2.0*sin(step));
   if (temp > 1.5)
      inc = 0.007F;
   else if (temp > 0.5)
      inc = 0.005F;
   else if (temp > -0.5)
      inc = 0.001F;
   else if (temp > -1.5)
      inc = 0.002F;
   else
      inc = 0.007F;

   step+=inc;
   if (temp < 0.0F)
   {
      temp = -(temp-1.0F);
      temp = 1.0F/temp;
   }
   else
      temp+=1.0F;

   return(warp*=temp);
}

void byteSwap(short *sound_buffer, unsigned int bufferSize)
{
    unsigned int i;
    
    unsigned short thisSample;

    for(i = 0; i < bufferSize; i++)
    {
        thisSample = sound_buffer[i];
        thisSample = (((thisSample)>>8)|((thisSample)<<8));
        sound_buffer[i] = thisSample;

    }
}



static int bufToFile(unsigned char *pBuf, FILE *pFile, int bits, short last)
{ 
   static unsigned char wordToWrite  = 0;
   static int  bitsLeftOver = 0;
   int writeFail=0;

   for(bits+=bitsLeftOver;bits>8;bits-=8,pBuf++) {
      wordToWrite|=(*pBuf<<bitsLeftOver)&0xFF;
      if(!fwrite(&wordToWrite,sizeof(unsigned char),1,pFile)) {
         writeFail=1;
         break;
      }
      wordToWrite= (unsigned char)((*pBuf>>(8-bitsLeftOver))&0xFF);
   }

   if (bits > bitsLeftOver)
     wordToWrite|=(*pBuf<<bitsLeftOver)&0xFF;

   bitsLeftOver=bits;
   wordToWrite= (unsigned char) ((wordToWrite<<(8-bitsLeftOver))>>(8-bitsLeftOver));

   if (last == 1 && bitsLeftOver > 0) {
     if(!fwrite(&wordToWrite,sizeof(unsigned char),1,pFile))
       writeFail=1;
   }
   return writeFail;
}


/* file to circular buffer */
static int fileToCBuf(VCI_CODEC_IO_BLOCK *hvciIOBlk, FILE *pFile, int bytes)
{
   int endOfFile=0;
   int leftOverByte;
   unsigned char wordToRead;

   /* write bit offset should be always 0 */
   hvciIOBlk->chVoxBufferWriteBitOffset = 0; 

   /* find out how many bytes leftOver */
   if (hvciIOBlk->wVoxBufferWriteByteOffset >= hvciIOBlk->wVoxBufferReadByteOffset)
     leftOverByte = hvciIOBlk->wVoxBufferWriteByteOffset - hvciIOBlk->wVoxBufferReadByteOffset;
   else
     leftOverByte = hvciIOBlk->wVoxBufferWriteByteOffset + hvciIOBlk->wVoxBufferSize
       - hvciIOBlk->wVoxBufferReadByteOffset;

   if (hvciIOBlk->chVoxBufferReadBitOffset > 0)
     leftOverByte --;
   
   /* reset the bytes for reading */
   bytes -= leftOverByte;
   if (bytes >= hvciIOBlk->wVoxBufferSize)
     bytes = hvciIOBlk->wVoxBufferSize - 1;

   for (; bytes > 0; bytes--) {
      if(!fread(&wordToRead,sizeof(unsigned char), 1, pFile)) {
         endOfFile=1;
         break;
      }

      hvciIOBlk->pVoxBuffer[hvciIOBlk->wVoxBufferWriteByteOffset] = wordToRead;
      
      /* increase the index */
      hvciIOBlk->wVoxBufferWriteByteOffset 
         = (unsigned char)((hvciIOBlk->wVoxBufferWriteByteOffset+1)
	% hvciIOBlk->wVoxBufferSize);
   }

   return endOfFile;
}
