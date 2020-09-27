/*----------------------------------------------------------------------------
 *  Copyright © 1997 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 10/24/00
 *  Developer: Sean Wohlgemuth
 *----------------------------------------------------------------------------
 */

#ifdef _XBOX
#include <xtl.h>
extern "C" { 
	ULONG DebugPrint(PCHAR Format, ...);
}
#else
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "bldfiles.h"

/*	InitBuffer - Initializes the buffer used to create the data of in the files
		being generated.

	rgchBuffer - Buffer to hold data
	iSize - size of rgchBuffer in bytes
*/
HRESULT InitBuffer(char* rgchBuffer, int iSize){
	
	if (NULL==rgchBuffer)
		return E_INVALIDARG;
	if (0>=iSize)
		return E_INVALIDARG;

	bool bExtra=false;
	if ((iSize % 2)==1) bExtra=true;
	for (int i=0; i<iSize/2; i++){
		memcpy(&rgchBuffer[i*2],"01",2);
	}//endfor
	if (bExtra){
		rgchBuffer[iSize-1]='0';
	}//endif
	//memset(rgchBuffer,'A',iSize);

	return S_OK;

}//endmethod

/* See header */
HRESULT BuildFilesOfSize(unsigned __int64 hStart, DWORD dwCount, char* pszPath){

	const iBufferSize=1024;
	char rgchBuffer[iBufferSize];
	char pszFileName[13];

	FILE* pf=NULL;
	unsigned int uiBlocks=0;
	unsigned int uiRemainder=0;
	unsigned __int64 hcCount=0l;

	//Initialize buffer
	if FAILED(InitBuffer(rgchBuffer,iBufferSize)){
		return E_FAIL;
	}//endif

	//Create files
	for (hcCount=hStart; hcCount<=(dwCount+hStart-1); hcCount++){

		uiRemainder=uiBlocks=0;

		//Calculate blocks and remainder
		uiBlocks= (int)(hcCount / iBufferSize);
		uiRemainder=(int)(hcCount % iBufferSize);

		//Open file
		#ifdef _XBOX
			sprintf(pszFileName,"%s%08X.%03X",pszPath,uiBlocks,uiRemainder);
		#else
			sprintf(pszFileName,"%s%08X.%03X",pszPath,uiBlocks,uiRemainder);
		#endif
		if ((pf=fopen(pszFileName,"w"))==NULL)
			return E_FAIL;

		//Write blocks
		for (unsigned int uicBlocks=0; uicBlocks<uiBlocks; uicBlocks++){
			if (fwrite(rgchBuffer,sizeof(char),iBufferSize,pf)!=iBufferSize){
				return E_FAIL;
			}//endif
		}//endfor

		//Write remainder
		if (0<uiRemainder){
			if (fwrite(rgchBuffer,sizeof(char),uiRemainder,pf)!=uiRemainder){
				return E_FAIL;
			}//endif
		}//endif
		
		//Close file
		fflush(pf);

		if (NULL!=pf){
			fclose(pf);
			pf=NULL;
		}//endif

	}//endfor

	//Guaranteed cleanup
	if (NULL!=pf){
		fclose(pf);
		pf=NULL;
	}//endif

	return S_OK;

}//endmethod

#ifndef _XBOX
/*	main - Program Entry Point

	set dwStart and dwCount before execution
	see notes for Createfile parameters
*/

int __cdecl main(int argc, char *argv[ ]){

		unsigned __int64 hStart=0;
		DWORD dwCount=0;

		//Get arguments
		char rgchBuffer[255];
		if (3!=argc) goto usage;
		if (0==sscanf(argv[1],"%s",rgchBuffer)) goto usage;
		if (0==sscanf(argv[2],"%d",&dwCount)) goto usage;
		hStart=_atoi64(rgchBuffer);
		if ((0>hStart) || (0>=dwCount)) goto usage;

		//Create files
		BuildFilesOfSize(hStart, dwCount, ".\\");
		
		printf("File Creation Complete\n");

		return 0;

		//Usage
usage:
			printf("usage: %s start count\n",argv[0]);

		return -1;

}//endmethod
#endif