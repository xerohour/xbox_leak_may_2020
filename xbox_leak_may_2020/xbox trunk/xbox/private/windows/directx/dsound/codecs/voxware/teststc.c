/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
*
* Filename:       TestSTC.c
*
* Purpose:        Test program for scalable codecs
*
* Functions:      main()
*
* Author/Date:    Bob
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/testcode/TestSTC.c_v   1.4   18 May 1998 10:37:42   weiwang  $
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
 
#include "VoxMem.h"
#include "vci.h"

#include "zargs.h"

#include "vciSC6.h"
#include "vciSC3.h"
#include "vciPlus.h"
#include "vciPlsSC.h"

#include "codec.h"

#define VCI_VAD 0

#if(VCI_VAD==1)
#include "vciVad.h"
FILE *vadFile=NULL, *agcFile=NULL;

static void setVadAgc(void *hCodecMemBlk1);
static void getVadAgc(void *hCodecMemBlk1, unsigned short framelen);
#endif

static void resetArgTable(void);
static float setWarp(void);

#define ALIGNMENT_SAMPLES    ENC_DELAY_SIZE 
#define MAX_FRAME_LENGTH 160

#ifdef WIN32
#include "NspTimer.h"
#endif

#ifdef SPARCSTATION
#include "sys/time.h"
#endif

#if (defined(WIN32)==1 || defined(SPARCSTATION)==1)
#define TIMER_NO   1
#define TEST_TIMES 1
#define MAX_FRAME_NO 4000
static double ENCODER_TIMER[MAX_FRAME_NO];
static double DECODER_TIMER[MAX_FRAME_NO];
#endif

#define SC_RATE_3K 0x01
#define SC_RATE_6K 0x03
/*--------------------------------------------------------------
  VOX_BUFFER_SIZE should be at least 15 for 6.0 kBits and
    at least  8 for 3.2 kBits
--------------------------------------------------------------*/
#define VOX_BUFFER_SIZE  20


static int  iWriteParams=0;
static FILE *pFWPFile;

int   iPrnFrmNum = 0;     /* print each frame number during processing       */
int   iFirstFrame = 0;    /* first frame to process                          */
int   iSCX = 0;           /* flag to switch between 3 and 6 kBits            */
int   iSC6 = 0;           /* flag to run at 6 kBits                          */
int   iParamCopy = 0;     /* flag to call ParamCopy() for uncoded parameters */
char  *cWPFileName = "";  /* output file for writing parameters into         */
char  *cVoxFile  ="";     /* vox file for half duplex                        */
int   iEncode=0;          /* half duplex mode, encode                        */
int   iDecode=0;          /* half duplex mode, decode                        */
float fWarp = 1.0F;       /* time scale warping factor                       */
int   fWarpTest = 0;      /* puts the warp on a sin wave a la RT             */
int   iAlign = 0;         /* time align the output and input                 */
int   iVad =0;            /* switch to turn Vad/Agc on/off                   */

int STCmain(int argc, char** argv)
{
   int  iFrameCount=0;
   FILE    *fp=NULL, *fp2=NULL;
   void    *pMemBlkE, *pMemBlkD;
   int      iTotalSamples,iTableSize;
   int      iContFlag=0;
   unsigned long iToggle=SC_RATE_3K,iToggle2=SC_RATE_3K;
   unsigned short wBitRate;
   int     i;
   VCI_RETCODE wError;
   unsigned char wNeedsInputFlag=0;
#if (defined(SPARCSTATION) == 1)
   struct timeval tp0, tp1, tpNull;
#endif
#if (defined(WIN32)==1 || defined(SPARCSTATION)==1)
   double tempClock=0.0;
   unsigned int uiTestCount=0;
   double DecoderPeak=0.0, EncoderPeak=0.0;
   double DecoderAverage=0.0, EncoderAverage=0.0;
   int EncoderPeakIndex=0, DecoderPeakIndex=0;
#else
   clock_t cTime;
   float   fSeconds,fPercent;
#endif
   int        iAlignmentSamples = 0;
   short int *psPCM=NULL;
   int        iSamples;

   VCI_CODEC_IO_BLOCK hvciEncodeIOBlk;     /* Encoder interface structure.     */
   VCI_CODEC_IO_BLOCK hvciDecodeIOBlk;     /* Decoder interface structure.     */

   static ARG argtab[]= {
      {"iSCX", BOOLEAN, &iSCX, "switch between  3 and 6 kbits"},
      {"iSC6", BOOLEAN, &iSC6, "run at 6 kbits"},
      {"iParamCopy", BOOLEAN, &iParamCopy, "call ParamCopy()"},
      {"iPrnFrmNum", BOOLEAN, &iPrnFrmNum, "print frame numbers"},
      {"iFirstFrame", INT, &iFirstFrame, "first frame to process"},
      {"cWPFileName", STRING, (int *)&cWPFileName, "parameter output file"},
      {"iEncode", BOOLEAN, &iEncode, "half duplex (encode)"},
      {"iDecode", BOOLEAN, &iDecode, "half duplex (decode)"}, 
      {"cVoxFile", STRING, (int *)&cVoxFile, "vox file"},
      {"fWarp", FLOAT, (int *)&fWarp, "time warping factor"},
      {"fWarpTest", BOOLEAN, &fWarpTest, "warp test"},
      {"iAlign", BOOLEAN, &iAlign, "time align the output and input"},
      {"iVad", BOOLEAN, &iVad, "Vad/Agc test"}
   };
   
   /* we need to reset each argument value in case we call the STCmain again */
   resetArgTable();

   /* Get the command-line arguments */
   iTableSize=sizeof(argtab)/sizeof(ARG);
   argc = zargs(argc, argv, argtab, iTableSize);
 
   if (iAlign) 
      iAlignmentSamples = ALIGNMENT_SAMPLES;

   (iSC6)?printf("\nRunning SC6 "):printf("\nRunning SC3 ");

   if (iParamCopy) printf("(calling ParamCopy())");

   if(iEncode) {
      if(iDecode) {
         printf("Half Duplex!!!! Get it... like duhhhh");
         exit(1);
      } 
      printf("(Encode->Vox)\n");

      if (argc <= 2) {
         fprintf(stderr, "Usage: %s <Input Speech File> <Output Raw Vox File>\n\n",
                 argv[0]);
         fprintf(stderr, "example: %s INPUT.pcm OUTPUT.vox \n", argv[0]);
         exit(1);
      }

   }
   else if(iDecode) {
      printf("(Vox->Decode)\n");

      if (argc <= 2) {
         fprintf(stderr, "Usage: %s <Input Raw Vox File> <Output Speech File>\n\n",
                 argv[0]);
         fprintf(stderr, "example: %s INPUT.vox OUTPUT.out \n", argv[0]);
         exit(1);
      }


   } else { /* Full duplex */
      printf("(Full Duplex)\n");
      if (argc <= 2) {
         fprintf(stderr, "Usage: %s <Input Speech File> <Output Speech File>\n\n",
                 argv[0]);
         fprintf(stderr, "example: %s INPUT.pcm OUTPUT.out \n", argv[0]);
         exit(1);
      }
   }

   /*** Specify the input speech file ***/
   if ((fp = fopen(argv[1], "rb")) == NULL) {
      fprintf(stderr, "\nCannot open %s\n", argv[1]);
      exit(1);
   } else  
      printf("\nInput File:  \"%s\"\n", argv[1]); 

   /*** Specify the output speech file ***/
   if ((fp2 = fopen(argv[2], "wb")) == NULL) {
      fprintf(stderr, "\nCannot create %s\n", argv[2]);
      exit(1);
   } else 
      printf("Output File: \"%s\"\n", argv[2]); 

   if(!iDecode) {
      if(!(hvciEncodeIOBlk.pPCMBuffer=calloc(FRAME_LENGTH_SC,sizeof(short)))) {
         printf("\nFailed to allocated memory! .. exiting");
         exit(1);
      }

      if(!(hvciEncodeIOBlk.pVoxBuffer=calloc(VOX_BUFFER_SIZE,sizeof(unsigned char)))) {
         printf("\nFailed to allocated memory! .. exiting");
         exit(1);
      }

      hvciEncodeIOBlk.wVoxBufferSize = VOX_BUFFER_SIZE;
      hvciEncodeIOBlk.wVoxBufferReadByteOffset = 0;
      hvciEncodeIOBlk.chVoxBufferReadBitOffset = 0;
      hvciEncodeIOBlk.wVoxBufferWriteByteOffset = 0;
      hvciEncodeIOBlk.chVoxBufferWriteBitOffset = 0;

   }
   if(!iEncode) {
      if(!(hvciDecodeIOBlk.pPCMBuffer=calloc(FRAME_LENGTH_SC,sizeof(short)))) {
         printf("\nFailed to allocated memory! .. exiting");
         exit(1);
      }

      if(!(hvciDecodeIOBlk.pVoxBuffer=calloc(VOX_BUFFER_SIZE,sizeof(unsigned char)))) {
         printf("\nFailed to allocated memory! .. exiting");
         exit(1);
      }

      hvciDecodeIOBlk.wVoxBufferSize = VOX_BUFFER_SIZE;
      hvciDecodeIOBlk.wVoxBufferReadByteOffset = 0;
      hvciDecodeIOBlk.chVoxBufferReadBitOffset = 0;
      hvciDecodeIOBlk.wVoxBufferWriteByteOffset = 0;
      hvciDecodeIOBlk.chVoxBufferWriteBitOffset = 0;
   }

   /* Initialize all neccesary structures */
   if (iSC6 || iSCX) {
      if(!iDecode)
         if(vciInitEncodeSC6(&pMemBlkE)) {
            printf("\nFailed in Encoder 6 initialization! ... exiting");
            exit(1);
         }
      if(!iEncode)
         if(vciInitDecodeSC6(&pMemBlkD)) {
            printf("\nFailed in Decoder 6 initialization! ... exiting");
            exit(1);
         }
   } else {
      if(!iDecode) 
         if(vciInitEncodeSC3(&pMemBlkE)) {
            printf("\nFailed in Encoder 3 initialization! ... exiting");
            exit(1);
         }
      if(!iEncode)
         if(vciInitDecodeSC3(&pMemBlkD)) {
            printf("\nFailed in Decoder 3 initialization! ... exiting");
            exit(1);
         }
   }

#if VCI_VAD==1
   if(!iDecode&&iVad) {
      if (!(vadFile = fopen("vad.out", "wb"))) {
         printf("Cannot open vad.out\n");
         exit(1);
      }
      if (!(agcFile = fopen("agc.out", "wb"))) {
         printf("Cannot open agc.out\n");
         exit(1);
      }
      if(vciEnableVAD(pMemBlkE)) {
         printf("Memory Allocation error for Vad/Agc \n");
         exit(1); 
      }
      if(vciEnableAGC(pMemBlkE)) {
         printf("Memory Allocation error for Vad/Agc \n");
         exit(1);
      }
   }  
#endif

   if (cWPFileName[0]!='\0') {
      iWriteParams = 1;
      pFWPFile = fopen( cWPFileName, "w" );
      if (!pFWPFile) {
         printf("ERROR: could not open file: \'%s\'\n",cWPFileName);
         exit(0);
      }
   }

   if (!iEncode) {
      vciSetWarpFactorSC(pMemBlkD, fWarp);
      vciGetWarpFactorSC(pMemBlkD, &fWarp);
      printf("Warp set to %.3f\n",fWarp);
   }

   iTotalSamples = 0;

#if (defined(WIN32) == 0 && defined(SPARCSTATION) == 0)
   cTime = clock();
#else
   do {

   fseek(fp,0,0); /* resetting for bench mark */
#endif


   if(iFirstFrame) 
      fseek(fp,iFirstFrame*(FRAME_LENGTH_SC),0);
   iFrameCount=iFirstFrame;

   /* Processing loop */
   do  {
      iTotalSamples += FRAME_LENGTH_SC;
     
         if (iSCX) {
            if ((iFrameCount>=0)&&!(iFrameCount%20)) {
               iToggle=(iToggle==SC_RATE_3K)?SC_RATE_3K|SC_RATE_6K:SC_RATE_3K;
               vciSetEncoderRateSC(pMemBlkE, iToggle);
               vciGetEncoderRateSC(pMemBlkE, &iToggle2);
               vciCalculateBitRateSC(pMemBlkE, iToggle, &wBitRate);
               assert(iToggle==iToggle2);
               vciCalculateRateSC(pMemBlkE, wBitRate, &iToggle2);
               assert(iToggle==iToggle2);
               printf("\nRate now set at %d", wBitRate);
            }
         }

         if (iPrnFrmNum)
            printf("\nFrame: %d",iFrameCount);

         if(!iDecode) {
#if(VCI_VAD==1)
            if(iVad)
               setVadAgc(pMemBlkE);
#endif

            iContFlag=fread(hvciEncodeIOBlk.pPCMBuffer, sizeof(short),FRAME_LENGTH_SC, fp);
            if(iContFlag<FRAME_LENGTH_SC) {
               for(i=iContFlag;i<(FRAME_LENGTH_SC);i++)
                  hvciEncodeIOBlk.pPCMBuffer[i]=(short)0;
               iContFlag=0;
            }

            /*--------------------------------------------
              clear bit stream and reset offsets to zero
            --------------------------------------------*/
            hvciEncodeIOBlk.wVoxBufferWriteByteOffset = 0;
            hvciEncodeIOBlk.chVoxBufferWriteBitOffset = 0;

            /*--------------------------------------------
              Encode the speech
            --------------------------------------------*/
#ifdef WIN32
            tstTimerStart(TIMER_NO);
#elif (defined(SPARCSTATION) == 1)
            gettimeofday(&tp0, &tpNull);
#endif
            wError=(unsigned short)((iSC6||iSCX)?vciEncodeSC6(pMemBlkE, &hvciEncodeIOBlk):
               vciEncodeSC3(pMemBlkE, &hvciEncodeIOBlk));
#ifdef WIN32
            tstTimerStop(TIMER_NO);
            tempClock = tstTimerSec(TIMER_NO);
#elif (defined(SPARCSTATION)== 1)
            gettimeofday(&tp1, &tpNull);
            tempClock = (tp1.tv_sec-tp0.tv_sec) + (tp1.tv_usec-tp0.tv_usec)*1.0e-6F;
#endif

#if (defined(WIN32) == 1 || defined(SPARCSTATION) == 1)
            if(iFrameCount<MAX_FRAME_NO)
               if(tempClock < ENCODER_TIMER[iFrameCount] || uiTestCount==0)
                  ENCODER_TIMER[iFrameCount] = tempClock;
#endif

            if(wError) {
               printf("\nError %d in Encoder... exiting", wError);
               exit(1);
            }
#if(VCI_VAD==1)
            if(iVad)
               getVadAgc(pMemBlkE, (unsigned short)FRAME_LENGTH_SC);
#endif
         }

         if(iEncode) {
           /* write out the frame boundary */
           fwrite(hvciEncodeIOBlk.pVoxBuffer, sizeof(unsigned char), 
              hvciEncodeIOBlk.wVoxBufferWriteByteOffset, fp2); 
          }
         else if(iDecode) {
            /* seek back the frame boundary */
            fseek(fp, hvciDecodeIOBlk.wVoxBufferReadByteOffset
              -hvciDecodeIOBlk.wVoxBufferWriteByteOffset,SEEK_CUR); 
            /* read the data */
            iContFlag=(fread(hvciDecodeIOBlk.pVoxBuffer,sizeof(unsigned char), 
               hvciDecodeIOBlk.wVoxBufferSize-1, fp)==
               (unsigned int)(hvciDecodeIOBlk.wVoxBufferSize-1))?1:0;

            /* set the write byt offset */
            hvciDecodeIOBlk.wVoxBufferWriteByteOffset = 
            (unsigned short)((hvciDecodeIOBlk.wVoxBufferSize-1) % hvciDecodeIOBlk.wVoxBufferSize);
         }
         else { /* Full Duplex */
         /*--------------------------------------------
           Copy bit stream for decoder.

           NOTE: A bitstream must ALWAYS be passed
                   to the decoder, even when using
                   ParamCopy() to transfer the parameters.
                   Without a bitstream the VCI
                   cannot tell which decoder to 
                   invoke!
         --------------------------------------------*/

            for (i=0; i<hvciEncodeIOBlk.wVoxBufferSize; i++)
               hvciDecodeIOBlk.pVoxBuffer[i] = hvciEncodeIOBlk.pVoxBuffer[i];
         /*--------------------------------------------
              copy unquantized parameters
         --------------------------------------------*/

         /*--------------------------------------------------
           copy the write flags 
           --------------------------------------------------*/
          hvciDecodeIOBlk.wVoxBufferWriteByteOffset = hvciEncodeIOBlk.wVoxBufferWriteByteOffset;
          hvciDecodeIOBlk.chVoxBufferWriteBitOffset = hvciEncodeIOBlk.chVoxBufferWriteBitOffset;
         }

         if(!iEncode) {
            /*--------------------------------------------
              copy bit stream and reset offsets to zero
            --------------------------------------------*/
            hvciDecodeIOBlk.wVoxBufferReadByteOffset = 0;
            hvciDecodeIOBlk.chVoxBufferReadBitOffset = 0;

            if(fWarpTest)
               vciSetWarpFactorSC(pMemBlkD, setWarp());

            do {
               /*--------------------------------------------
                 Decode the speech
               --------------------------------------------*/
#ifdef WIN32
               tstTimerStart(TIMER_NO);
#elif (defined(SPARCSTATION) == 1)
               gettimeofday(&tp0, &tpNull);
#endif
               wError=(unsigned short)((iSC6||iSCX)?vciDecodeSC6(pMemBlkD, &hvciDecodeIOBlk):
                  vciDecodeSC3(pMemBlkD, &hvciDecodeIOBlk));
#ifdef WIN32
            tstTimerStop(TIMER_NO);
            tempClock = tstTimerSec(TIMER_NO);
#elif (defined(SPARCSTATION)== 1)
            gettimeofday(&tp1, &tpNull);
            tempClock = (tp1.tv_sec-tp0.tv_sec) + (tp1.tv_usec-tp0.tv_usec)*1.0e-6F;
#endif

#if (defined(WIN32) == 1 || defined(SPARCSTATION) == 1)
               if(iFrameCount<MAX_FRAME_NO)
                  if(tempClock < DECODER_TIMER[iFrameCount] || uiTestCount==0)
                     DECODER_TIMER[iFrameCount] = tempClock; 
#endif
               if(wError) {
                  printf("\nError %d in Decoder... exiting", wError);
                  exit(1);
               }
               /*--------------------------------------------
                 write decoded speech to a file
               --------------------------------------------*/
               if (hvciDecodeIOBlk.wSamplesInPCMBuffer) {
                  /*------------------------------------------------------
                    possibly time align the output by skipping the
                      first 146 output samples.....
                  ------------------------------------------------------*/
                  if (iAlignmentSamples) {
                     if (iAlignmentSamples>=hvciDecodeIOBlk.wSamplesInPCMBuffer) {   
                        iSamples = 0;
                        iAlignmentSamples-=hvciDecodeIOBlk.wSamplesInPCMBuffer;
                     } else {
                        iSamples = hvciDecodeIOBlk.wSamplesInPCMBuffer -
                                   iAlignmentSamples;
                        psPCM = hvciDecodeIOBlk.pPCMBuffer + iAlignmentSamples;
                        iAlignmentSamples = 0;
                     }
                  } else {
                     psPCM = hvciDecodeIOBlk.pPCMBuffer;
                     iSamples = hvciDecodeIOBlk.wSamplesInPCMBuffer;
                  }

                  /*----------------------------------------------------
                    Write the output speech to disk
                  ----------------------------------------------------*/
                  if (iSamples)
                     fwrite((char*)psPCM, sizeof(short), iSamples, fp2);
               }
   
               vciDecodeNeedsInputSC(pMemBlkD,&wNeedsInputFlag);
            } while (!wNeedsInputFlag);
         }
         iFrameCount++;

#if 0
        if (iFrameCount > MAX_FRAME_NO) break;
#endif

   } while (iContFlag);

    printf ("Job Completed... \n");

#if (defined(WIN32) == 0 && defined(SPARCSTATION) == 0)
   cTime = clock() - cTime;
   fSeconds = (float)cTime/(float)CLOCKS_PER_SEC;
   fPercent = fSeconds/(float)iTotalSamples*8000.0F*100.0F;

   printf("\nElapsed Time: %.3f seconds (%.1f%% of real time)\n",
          fSeconds, fPercent);
   /*printf("Clock resolution is %e seconds\n",1.0F/(float)CLOCKS_PER_SEC);*/
#else
   } while(++uiTestCount<TEST_TIMES);
   iFrameCount=(iFrameCount>MAX_FRAME_NO)?MAX_FRAME_NO:iFrameCount;
   for(i=0;i<iFrameCount;i++) {
      if(ENCODER_TIMER[i]>EncoderPeak) {
         EncoderPeak = ENCODER_TIMER[i];
         EncoderPeakIndex = i;
      }
      if(DECODER_TIMER[i]>DecoderPeak) {
         DecoderPeak = DECODER_TIMER[i];
         DecoderPeakIndex = i;
      }
      EncoderAverage += ENCODER_TIMER[i];
      DecoderAverage += DECODER_TIMER[i];
   }
   
   EncoderAverage/= (iFrameCount*0.02);
   DecoderAverage/= (iFrameCount*0.02);
   EncoderPeak /=(0.02);
   DecoderPeak /=(0.02);
  
   if(!iDecode) {
      printf("\n\nEncodeAverage CPU time: %.1f", EncoderAverage*100);
      printf("\nEncodePeak    CPU time: %.1f", EncoderPeak*100);
      printf("\nEncoder Peak Index %d", EncoderPeakIndex);
   }

   if(!iEncode) {
      printf("\n\nDecodeAverage CPU time: %.1f", DecoderAverage*100);
      printf("\nDecodePeak    CPU time: %.1f", DecoderPeak*100);
      printf("\nDecoder Peak Index %d\n", DecoderPeakIndex);
   }
#endif
#if VCI_VAD==1
      if(iVad) {
         printf("\nFreeing: Vad/Agc. \n");
         vciDisableAGC(pMemBlkE);
         vciDisableVAD(pMemBlkE);

         if(vadFile)
            fclose(vadFile);
         if(agcFile)
            fclose(agcFile);
      }
#endif

   if (iSC6 || iSCX) {
      if(!iDecode)
         vciFreeEncodeSC6(&pMemBlkE);
      if(!iEncode)
         vciFreeDecodeSC6(&pMemBlkD);
   } else  {
      if(!iDecode)
         vciFreeEncodeSC3(&pMemBlkE);
      if(!iEncode)
         vciFreeDecodeSC3(&pMemBlkD);
   }

   if(!iDecode) {
      free(hvciEncodeIOBlk.pPCMBuffer);
      free(hvciEncodeIOBlk.pVoxBuffer);
   }

   if(!iEncode) {
      free(hvciDecodeIOBlk.pPCMBuffer);
      free(hvciDecodeIOBlk.pVoxBuffer);
   }

   fclose(fp);
   fclose(fp2);

   return(0);
} /* main() */

static void resetArgTable(void)
{
   iPrnFrmNum = 0;     /* print each frame number during processing       */
   iFirstFrame = 0;    /* first frame to process                          */
   iSCX = 0;           /* flag to switch between 3 and 6 kBits            */
   iSC6 = 0;           /* flag to run at 6 kBits                          */
   iParamCopy = 0;     /* flag to call ParamCopy() for uncoded parameters */
   cWPFileName = "";  /* output file for writing parameters into         */
   cVoxFile  ="";     /* vox file for half duplex                        */
   iEncode=0;          /* half duplex mode, encode                        */
   iDecode=0;          /* half duplex mode, decode                        */
   fWarp = 1.0F;       /* time scale warping factor                       */
   fWarpTest = 0;      /* puts the warp on a sin wave a la RT             */
   iAlign = 0;         /* time align the output and input                 */
   iVad =0;            /* switch to turn Vad/Agc on/off                   */
}

static float setWarp(void)
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
   if (temp < 0.0F) {
      temp = -(temp-1.0F);
      temp = 1.0F/temp;
   }
   else
      temp+=1.0F;

   return(warp*=temp);
}

#if VCI_VAD==1
static void setVadAgc(void *hCodecMemBlk1)
{
  unsigned short wSilenceThresh = 450;     /* 450ms (5 packets) */
  unsigned short wEnergyTrigLevel = 150;   /* energy level is 150 */
  short wTargetGain = 180;   /* set target gain to 180 */
  
  /* set silence threshold */
  vciSetVADSilenceThresh(hCodecMemBlk1, wSilenceThresh);

  /* set user triggle level */
  vciSetVADEnergyTrigLevel(hCodecMemBlk1, wEnergyTrigLevel);

  /* set AGC control value to 0 */
  vciSetAGCControlValue(hCodecMemBlk1, 0);

  /* set AGC target gain */
  vciSetAGCTargetGain(hCodecMemBlk1, wTargetGain);
}

static void getVadAgc(void *hCodecMemBlk1, unsigned short framelen)
{
  unsigned short VadValue;
  short AGCcontrolValue;
  short outBuf[MAX_FRAME_LENGTH];
  int i;

  /* get VAD */
  vciGetVADVoiceActivity(hCodecMemBlk1, &VadValue);
  
  /* get AGC control value */
  vciGetAGCControlValue(hCodecMemBlk1, &AGCcontrolValue);

  /* write to the buffer */
  for (i = 0; i < framelen; i++)
    outBuf[i] = VadValue;
  fwrite(outBuf, sizeof(short), framelen, vadFile);

  for (i = 0; i < framelen; i++)
    outBuf[i] = AGCcontrolValue;
  fwrite(outBuf, sizeof(short), framelen, agcFile);
}
#endif 
