/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	rvbparam.cpp

Abstract:

	Defines the entry point for the console application

Author:

	George Chrysanthakopolous

Revision History:

	21-Jun-2001 robheit
		Added support for long filenames and fixed a few bugs

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"

#define MODE_24K 24
#define MODE_48K 48


#define REVERB_EFFECT_BUFFERREVERBIONSIZE_OFFSET	17

char REVERB_EFFECT_PARAMS_FILTER[94] = {

	0,//DWORD dwMainDelayLineBase;
	1,//DWORD dwMainDelayLineSize;

	0,//DWORD dwMainDelayLine1Base;
	1,//DWORD dwMainDelayLine1Size;

	0,//DWORD dwMainDelayLine2Base;
	1,//DWORD dwMainDelayLine2Size;

	0,//DWORD dwMainDelayLine3Base;
	1,//DWORD dwMainDelayLine3Size;

	0,//DWORD dwBufferReflection0Base;
    1,//DWORD dwBufferReflection0Size;

	0,//DWORD dwBufferReflection1Base;
    1,//DWORD dwBufferReflection1Size;

	0,//DWORD dwBufferReflection2Base;
    1,//DWORD dwBufferReflection2Size;

	0,//DWORD dwBufferReflection3Base;
    1,//DWORD dwBufferReflection3Size;

	0,//DWORD dwBufferShortReverb0Base;
    1,//DWORD dwBufferShortReverb0Size;

	0,//DWORD dwBufferShortReverb1Base;
    1,//DWORD dwBufferShortReverb1Size;

	0,//DWORD dwBufferShortReverb2Base;
    1,//DWORD dwBufferShortReverb2Size;

	0,//DWORD dwBufferShortReverb3Base;
    1,//DWORD dwBufferShortReverb3Size;

	0,//DWORD dwBufferLongReverb0Base;
    1,//DWORD dwBufferLongReverb0Size;

	0,//DWORD dwBufferLongReverb1Base;
    1,//DWORD dwBufferLongReverb1Size;

	2,//DWORD dwReflectionsInputDelay0;
	2,//DWORD dwReflectionsInputDelay1;
	2,//DWORD dwReflectionsInputDelay2;
	2,//DWORD dwReflectionsInputDelay3;
	2,//DWORD dwReflectionsInputDelay4;

	2,//DWORD dwShortReverbInputDelay;

	2,//DWORD dwLongReverbInputDelay0a;
	2,//DWORD dwLongReverbInputDelay0b;

	2,//DWORD dwLongReverbInputDelay1a;
	2,//DWORD dwLongReverbInputDelay1b;

	2,//DWORD dwLongReverbInputDelay2a;
	2,//DWORD dwLongReverbInputDelay2b;

	2,//DWORD dwLongReverbInputDelay3a;
	2,//DWORD dwLongReverbInputDelay3b;

	2,//DWORD dwReflectionDelay0;
	2,//DWORD dwReflectionDelay1;
	2,//DWORD dwReflectionDelay2;
	2,//DWORD dwReflectionDelay3;

	2,//DWORD dwLongReverbDelay;

	0,//DWORD dwShortReverbInputGain_Channel0a;
	0,//DWORD dwShortReverbInputGain_Channel0b;

	0,//DWORD dwShortReverbInputGain_Channel1a;
	0,//DWORD dwShortReverbInputGain_Channel1b;

	0,//DWORD dwShortReverbInputGain_Channel2a;
	0,//DWORD dwShortReverbInputGain_Channel2b;

	0,//DWORD dwShortReverbInputGain_Channel3a;
	0,//DWORD dwShortReverbInputGain_Channel3b;

	0,//DWORD dwLongReverbInputGain;

	0,//DWORD dwLongReverbCrossfeedGain;

	0,//DWORD dwNewReflectionOutputGain_Channel0;
	0,//DWORD dwNewReflectionOutputGain_Channel1;
	0,//DWORD dwNewReflectionOutputGain_Channel2;
	0,//DWORD dwNewReflectionOutputGain_Channel3;

	0,//DWORD dwNewShortReverbOutputGain_Channel;
	0,//DWORD dwNewLongReverbOutputGain_Channel;

    0,//DWORD dwNumOutputChannels;
	0,//DWORD IIR_Input_Delay;
	0,//DWORD IIR_InputCoefficientA;
	0,//DWORD IIR_InputCoefficientB;

	0,//DWORD IIR_LongReverb_MainDelay0_Delay;
	0,//DWORD IIR_LongReverb_MainDelay0_CoefficientA;
	0,//DWORD IIR_LongReverb_MainDelay0_CoefficientB;

	0,//DWORD IIR_LongReverb_MainDelay1_Delay;
	0,//DWORD IIR_LongReverb_MainDelay1_CoefficientA;
	0,//DWORD IIR_LongReverb_MainDelay1_CoefficientB;

	0,//DWORD IIR_LongReverb_MainDelay2_Delay;
	0,//DWORD IIR_LongReverb_MainDelay2_CoefficientA;
	0,//DWORD IIR_LongReverb_MainDelay2_CoefficientB;

	0,//DWORD IIR_ShortReverb_Channel0_Delay;
	0,//DWORD IIR_ShortReverb_Channel0_CoefficientA;
	0,//DWORD IIR_ShortReverb_Channel0_CoefficientB;

	0,//DWORD IIR_ShortReverb_Channel1_Delay;
	0,//DWORD IIR_ShortReverb_Channel1_CoefficientA;
	0,//DWORD IIR_ShortReverb_Channel1_CoefficientB;

	0,//DWORD IIR_ShortReverb_Channel2_Delay;
	0,//DWORD IIR_ShortReverb_Channel2_CoefficientA;
	0,//DWORD IIR_ShortReverb_Channel2_CoefficientB;

	0,//DWORD IIR_ShortReverb_Channel3_Delay;
	0,//DWORD IIR_ShortReverb_Channel3_CoefficientA;
	0,//DWORD IIR_ShortReverb_Channel3_CoefficientB;

	0,//DWORD IIR_LongReverb_Channel0_Delay;
	0,//DWORD IIR_LongReverb_Channel0_CoefficientA;
	0,//DWORD IIR_LongReverb_Channel0_CoefficientB;

	0,//DWORD IIR_LongReverb_Channel1_Delay;
	0,//DWORD IIR_LongReverb_Channel1_CoefficientA;
	0,//DWORD IIR_LongReverb_Channel1_CoefficientB;

};



#define NUM_HEADER_STRINGS 8

CHAR *iniHeader[NUM_HEADER_STRINGS]= {

	    "[FXSTATE]",
		"FX_SCRATCHLENGTH=580000",
		"FX_NUMINPUTS=2",
		"FX_NUMOUTPUTS=35",
		"FX_YMEMSIZE=0",
		"FX_DSPCYCLES=31000",
		"FX_NUMPARAMS=512",
		" "
};

CHAR *iniHeader24k[NUM_HEADER_STRINGS]= {

	    "[FXSTATE]",
		"FX_SCRATCHLENGTH=380000",
		"FX_NUMINPUTS=2",
		"FX_NUMOUTPUTS=35",
		"FX_YMEMSIZE=0",
		"FX_DSPCYCLES=31000",
		"FX_NUMPARAMS=512",
		" "
};

#define NUM_TRAILER_STRINGS 44

CHAR *iniTrailer[NUM_TRAILER_STRINGS]= {
        ";Constants",
	    "FX_PARAM94=12582912",
		"FX_PARAM95=12582912",
		"FX_PARAM96=12582912",
		"FX_PARAM97=4194304",
		"FX_PARAM98=12582912",
		"FX_PARAM99=12582912",
		"FX_PARAM100=12582912",
		" ",
		";ReflectionsFeedbackGain",
		" ",
		"FX_PARAM101=13421773",
		"FX_PARAM102=3355443",
		"FX_PARAM103=13421773",
		"FX_PARAM104=13421773",
		" ",
		";ShortReverbInputDelayList",
		" ",
		"FX_PARAM105=0",
		"FX_PARAM106=464",
		"FX_PARAM107=1072",
		"FX_PARAM108=1856",
		"FX_PARAM109=0",
		"FX_PARAM110=464",
		"FX_PARAM111=1072",
		"FX_PARAM112=1856",
		" ",
		";ShortReverbDelayList",
		" ",
		"FX_PARAM113=11136",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM114=13488",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM115=16352",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM116=19824",				// This values is equal to a buffer length and should be read from the IO file
		" ",
		"; DSPBuilder settings",
		"FX_NAME=I3DL2 BATHROOM Reverb",
        "FX_CONFIG_NAME=Arena",
		"FX_DSPCODE=reverb.scr",
		"FX_INPUT0_NAME=Left",
		"FX_INPUT1_NAME=Right",
		"FX_OUTPUT0_NAME=Front Left",
		"FX_OUTPUT1_NAME=Front Right",
		"FX_OUTPUT2_NAME=Back Left",
		"FX_OUTPUT3_NAME=Back Right"
};

CHAR *iniTrailer24k[NUM_TRAILER_STRINGS]= {
        ";Constants",
	    "FX_PARAM94=12582912",
		"FX_PARAM95=12582912",
		"FX_PARAM96=12582912",
		"FX_PARAM97=4194304",
		"FX_PARAM98=12582912",
		"FX_PARAM99=12582912",
		"FX_PARAM100=12582912",
		" ",
		";ReflectionsFeedbackGain",
		" ",
		"FX_PARAM101=13421773",
		"FX_PARAM102=3355443",
		"FX_PARAM103=13421773",
		"FX_PARAM104=13421773",
		" ",
		";ShortReverbInputDelayList",
		" ",
		"FX_PARAM105=0",
		"FX_PARAM106=232",
		"FX_PARAM107=536",
		"FX_PARAM108=928",
		"FX_PARAM109=0",
		"FX_PARAM110=232",
		"FX_PARAM111=536",
		"FX_PARAM112=928",
		" ",
		";ShortReverbDelayList",
		" ",
		"FX_PARAM113=5568",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM114=6736",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM115=8176",				// This values is equal to a buffer length and should be read from the IO file
		"FX_PARAM116=9904",				// This values is equal to a buffer length and should be read from the IO file
		" ",
		"; DSPBuilder settings",
		"FX_NAME=Bathroom Reverb",
        "FX_CONFIG_NAME=Arena",
		"FX_DSPCODE=reverb.scr",
		"FX_INPUT0_NAME=Left",
		"FX_INPUT1_NAME=Right",
		"FX_OUTPUT0_NAME=Front Left",
		"FX_OUTPUT1_NAME=Front Right",
		"FX_OUTPUT2_NAME=Back Left",
		"FX_OUTPUT3_NAME=Back Right"
};

#define FXNAME_OFFSET 35

#define FX_SHORTREVERBINPUTDELAYLIST_ID 105
#define FX_SHORTREVERBINPUTDELAYLIST_COUNT 8

#define FX_SHORTREVERBDELAYLIST_ID 113
#define FX_SHORTREVERBDELAYLIST_COUNT 4

DWORD ioParams[94];


int _cdecl main(int argc, char* argv[])
{
	DWORD dwValue;
	DWORD i=0,j=0;
	CHAR szTemp[256],szParam[256];
	CHAR szInFile[MAX_PATH], szOutFile[MAX_PATH];
    FILE *inFile  = NULL;	
    FILE *outFile = NULL;	
	CHAR szFriendlyName[256];
	DWORD dwConversionFactor = 4;
    BOOL bHalfSampleRate=FALSE;					// RDA : bHalfSampleRate indicates if the system is to generate 
												//       a state file for a half sample rate system.


	REVERB_EFFECT_PARAMS rvbParams;
	PDWORD pBuffer;

	memset(&rvbParams,0,sizeof(rvbParams));

	pBuffer = (PDWORD) &rvbParams;

	//
	// convert the input file
	//

	if (argc < 4) {
        printf("\n Usage: rvbparam <friendly name> <input *.IO file> <output state INI file> [HALFSAMPLERATE]\n");
		return -1;
	}

	strcpy(szFriendlyName, argv[1]);
	strcpy(szInFile, argv[2]);
    strcpy(szOutFile, argv[3]);

	//memcpy(szFriendlyName,argv[1],sizeof(szInFile));
	//memcpy(szInFile,argv[2],sizeof(szInFile));
    //memcpy(szOutFile2,argv[3],sizeof(szOutFile2));

	// Replace all ' ' in the filenames with '_'
	for(i=0; i<strlen(szOutFile); ++i)
	{
		if(szOutFile[i] == ' ')
			szOutFile[i] = '_';
	}

	if(argc==5 && strcmp(argv[4],"HALFSAMPLERATE")==0)
		bHalfSampleRate=TRUE;


	//
	// read the parameters words, one at a time
	//

	CHAR t=0;
	DWORD dwOffset = 0;

	if (( outFile = fopen( szOutFile, "w" ) ) == NULL )
	{
		printf( "cannot open output file %s", szOutFile );
		return -1;
	}

	if (( inFile = fopen( szInFile, "r" ) ) == NULL )
	{
		printf( "cannot open input file %s", szInFile );
		return -1;
	}

    //
    // write the state file
    //

	j = 0;
	t = 0;
	dwOffset = 0;
	i = 0;

	memset(&rvbParams,0,sizeof(rvbParams));
	pBuffer = (PDWORD) &rvbParams;

	DWORD count = 0;

	//
	// write out the header
	//

    if (bHalfSampleRate == TRUE) {

        for (i=0;i<NUM_HEADER_STRINGS;i++) {
    
            sprintf(szParam,"%s\n",iniHeader24k[i]);
            fwrite((void*)szParam,strlen(szParam),1,outFile);
    
        }

    } else {

        for (i=0;i<NUM_HEADER_STRINGS;i++) {
    
            sprintf(szParam,"%s\n",iniHeader[i]);
            fwrite((void*)szParam,strlen(szParam),1,outFile);
    
        }

    }


	while (j<(sizeof(rvbParams)/sizeof(DWORD))) {

		if (fread((void*)&t,sizeof(CHAR),1,inFile) == 0) {
			if (fread((void*)&t,sizeof(CHAR),1,inFile) == 0)
				break;
		}

		

		if ((t == 0x20) || (t == 0xa)) {

			szTemp[i]=0;

			if (i>0) {

				i=0;
				dwValue = atoi(szTemp);

				//
				// filter some values. the need to be conveted
				//

				if (REVERB_EFFECT_PARAMS_FILTER[count] == 1){

					//
					// convert this value to bytes(its in dwords)
					//

					if (count <=27) {
						dwValue ++;
						dwValue *= dwConversionFactor;
						dwValue --;
					}

				} else if (count <27) {
					dwValue = 0;
				}

				if (REVERB_EFFECT_PARAMS_FILTER[count] == 2){

					//
					// convert this value to bytes(its in samples)
					//

					dwValue *= dwConversionFactor;

				}


				// Store the converted parameter
				ioParams[j]=dwValue;


                //
                // set number of outputs to 4. IO files have it to 2
                //

                if (count==63) {
					dwValue = 4;
				}



				//
				// when we reach the IIR coefficients portion of the *.IO
				// input file, we need to start writing a zero every 3 DWORD
				// since the actual DSP param layout has a delay element word
				// infront of each coeffient pair (a,b) but the sensuar data file dont
				// account for that. Their reverb used to have translation code to get around that
				// (baaaa)
				//

				if (pBuffer == &rvbParams.IIR_Input_Delay){

					dwOffset = j;			

				}


				if (dwOffset && ((j-dwOffset)%2 == 0)){
					DWORD dwTemp = 0;				
					sprintf(szParam,"FX_PARAM%d=%d\n",count,dwTemp);
					fwrite((void*)szParam,strlen(szParam),1,outFile);
					*pBuffer = dwTemp;
					pBuffer++;					
					count++;


				}

				if (dwValue < -1) {
					dwValue &=0x00FFFFFF;
				}

				*pBuffer = dwValue;
				pBuffer++;				


				sprintf(szParam,"FX_PARAM%d=%d\n",count,dwValue);
				fwrite((void*)szParam,strlen(szParam),1,outFile);

				count++;
				j++;


			}

			continue;
		}

		szTemp[i++] = t;
		
	}

	//
	// write out the trailer
	//
	int iParamValue=(sizeof(rvbParams)/sizeof(DWORD));
	BOOL bIsParam;
	for (i=0;i<NUM_TRAILER_STRINGS;i++) {

		// Check to see if this string defines a DSP parameter
		if(strncmp(iniTrailer[i],"FX_PARAM",8)==0)
			bIsParam=TRUE;
		else
			bIsParam=FALSE;

		// Write the default string from the array
		// This string is overwritten if we are writing a 'special case'
		if(!bHalfSampleRate)
			sprintf(szParam,"%s\n",iniTrailer[i]);			// Running at full sample rate.....
		else
			sprintf(szParam,"%s\n",iniTrailer24k[i]);		// Running at half sample rate.....

		// Put the proper name in the DSP builder settings
		if(i == FXNAME_OFFSET) 
		{
            if(bHalfSampleRate)
                sprintf(szParam,"FX_NAME=I3DL2 24K Reverb\n");
            else
                sprintf(szParam,"FX_NAME=I3DL2 Reverb\n");
		}

		// Put the proper name in the DSP builder settings
		else if(i == FXNAME_OFFSET+1) 
            sprintf(szParam,"FX_CONFIG_NAME=%s\n",szFriendlyName);

		// Write the correct reverb '.scr' file 
		else if(i == FXNAME_OFFSET+2)
		{
            if(bHalfSampleRate)
                sprintf(szParam,"FX_DSPCODE=reverb24k.scr\n");
            else
                sprintf(szParam,"FX_DSPCODE=reverb.scr\n");
		}

		// If processing a DSP parameter & it is a short reverb delay .......
		if(bIsParam && iParamValue>=FX_SHORTREVERBDELAYLIST_ID && iParamValue<FX_SHORTREVERBDELAYLIST_ID+FX_SHORTREVERBDELAYLIST_COUNT)
		{
			// Get the number of the short reverb (0-3)
			int iReverbID=iParamValue-FX_SHORTREVERBDELAYLIST_ID;
			// Get the length of the Short reverb buffer from the IO file
			int iBufferSize=1+ioParams[REVERB_EFFECT_BUFFERREVERBIONSIZE_OFFSET+2*iReverbID];
			// Output the modified parameter value
			sprintf(szParam,"FX_PARAM%i=%i\n",iParamValue,iBufferSize );
		}


		// Write the string to the IO file
		fwrite((void*)szParam,strlen(szParam),1,outFile);

		// If we have just processed a parameter then increment the parameter count
		if(bIsParam) iParamValue++;
	}


	fclose( inFile );
	fclose( outFile );

	return 0;
	

}