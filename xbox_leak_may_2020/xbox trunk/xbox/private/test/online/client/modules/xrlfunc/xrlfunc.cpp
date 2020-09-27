/*----------------------------------------------------------------------------
 *  Copyright © 1997 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 11/01/00
 *  Developer: Sean Wohlgemuth
 *----------------------------------------------------------------------------
 */

//BUGBUG: NULL PARAMETERS CHECK COMMENTED OUT; WAITING FOR RIP DECTECTION LOGIC

#include <stdio.h>
#include <stdlib.h>

#include <xlog.h> //xlog includes
#include <xtestlib.h> //harness includes

#include "xrl.h" //component under test includes
#include "xonlinep.h"
#include "svrsim.h" //IIS simulator includes

//**********
//********** global defines
//**********
#define TRACEDEBUG
#ifdef TRACEDEBUG
#ifdef _XBOX
extern "C" {ULONG DebugPrint(PCHAR Format, ...);}
#else
#define DebugPrint printf
#endif
#else
#define DebugPrint(a)
#endif

#define BREAK_INTO_DEBUGGER __asm int 3
#define MAX_SERVER_RESPONSE 4096
#define DEFAULT_URL "content:/default.html"
#define DOWNLOAD_URL "content:/read/00000001.000"
#define DOWNLOAD_URL2 "content:/read/00000400.000"
#define DEFAULT_FILE "T:\\xrldata.dat"
#define BASIC_RESPONSE "HTTP/1.1 200 OK\r\nDate: Mon, 26 Nov 2000 14:36:10 GMT\r\nLast-Modified: Mon, 26 Nov 2000 14:36:10 GMT\r\nContent-Length: 10\r\n\r\n0123456789"
#define TICKET_FILE "T:\\SERVMAP.XBA"

//**********
//********** global variables
//**********
HANDLE g_hLog = INVALID_HANDLE_VALUE; //xlog handle
svrsim g_oSimulator; //IIS simulator object
HANDLE g_hThread; //IIS simulator thread handle

//**********
//********** enumeration list for possible IIS responses
//**********

//string, valid/invalid pair
typedef struct value_pair_struct{
	char* pszValue;
	bool fValid;
}value_pair;

//HTTP version field
const unsigned int MAX_HTTP_Version = 13;
value_pair g_rgHTTP_Version[MAX_HTTP_Version]={
	{"HTTP/1.1",true}, //valid
	{"HTTP/2.2",true}, //valid
	{"HTTP/0.0",true}, //valid
	{"HTTP/9.9",true}, //valid
	{"HTTP/10.10",false}, //invalid version
	{"HTTP/X.X",false}, //invalid version
	{"        ",false}, //missing required components
	{"XXXXXXXX",false}, //missing required components
	{"HXXXXXXX",false}, //1st char valid; rest chars invalid
	{" ",false}, //space
	{"H",false}, //1st valid char
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual; invalid chars
};

//SP (space) field
#define MAX_SP 5
const unsigned int MAX_SP0 = MAX_SP;
const unsigned int MAX_SP1= MAX_SP;
value_pair g_rgSP[MAX_SP]={
	{" ",true}, //valid
	{"X",false}, //invalid; valid length
	{"  ",false}, //valid + invalid chars
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual; invalid chars
};

//Status code field
const unsigned int MAX_Status_Code = 7;
value_pair g_rgStatus_Code[MAX_Status_Code]={
	{"200",true}, //valid
	{"100",true}, //valid
	{"   ",false}, //invalid chars; valid length
	{"XXX",false}, //invalid chars; valid length
	{"999",false}, //not success code
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual
};

//Reason field
const unsigned int MAX_Reason_Phrase = 5;
value_pair g_rgReason_Phrase[MAX_Reason_Phrase]={
	{"OK",true}, //valid reason
	{"XX",true}, //valid reason
	{"  ",true}, //valid reason
	{"",true}, //valid empty
	{"01234567890123456789",true} //valid larger than just OK
};

//CRLF (Carriage Return/Line Feed) field
#define MAX_CRLF 7
const unsigned int MAX_CRLF0 = MAX_CRLF;
const unsigned int MAX_CRLF1 = MAX_CRLF;
const unsigned int MAX_CRLF2 = MAX_CRLF;
const unsigned int MAX_CRLF3 = MAX_CRLF;
const unsigned int MAX_CRLF4 = MAX_CRLF;
value_pair g_rgCRLF[MAX_CRLF]={
	{"\r\n",true}, //valid
	{"\r",false}, //half order of parameters
	{"\n",false}, //half order of parameters
	{"",false}, //empty
	{"XX",false}, //invalid chars; valid length
	{"  ",false}, //invalid chars; valid length
	{"01234567890123456789",false} //larger than usual
};

//Header date name field
const unsigned int MAX_DateName = 9;
value_pair g_rgDateName[MAX_DateName]={
	{"Date: ",true}, //valid
	{"Date:",true}, //valid
	{"DateDate:",false}, //invalid
	{"XXXX:",false}, //invalid chars; valid length
	{"XXXX: ",false}, //invalid chars; valid length
	{":",false}, //no field name
	{" ",false}, //invalid space
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual
};

//Header last-modified name field
const unsigned int MAX_LastModifiedName = 9;
value_pair g_rgLastModifiedName[MAX_LastModifiedName]={
	{"Last-Modified: ",true}, //valid
	{"Last-Modified:",true}, //valid
	{"Last-ModifiedLast-Modified:",false}, //invalid
	{"XXXXXXXXXXXXX:",false}, //invalid chars; valid length
	{"XXXXXXXXXXXXX: ",false}, //invalid chars; valid length
	{":",false}, //no field name
	{" ",false}, //invalid space
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual
};

//Header content-length name field
const unsigned int MAX_ContentLengthName = 9;
value_pair g_rgContentLengthName[MAX_ContentLengthName]={
	{"Content-Length: ",true}, //valid
	{"Content-Length:",true}, //valid
	{"Content-LengthContent-Length:",false}, //invalid
	{"XXXXXXXXXXXXXX:",false}, //invalid chars; valid length
	{"XXXXXXXXXXXXXX: ",false}, //invalid chars; valid length
	{":",false}, //no field name
	{" ",false}, //invalid space
	{"",false}, //empty
	{"01234567890123456789",false} //larger than usual
};

//Header extension name field
const unsigned int MAX_ExtensionName = 7;
value_pair g_rgExtensionName[MAX_ExtensionName]={
	{"XXXXXX: ",true}, //valid
	{"XXXXXX:",true}, //valid
	{":",false}, //no field name
	{" ",false}, //invalid space
	{"",true}, //empty (not required field)
	{"012345678901234567890123456789012345678901234567890123456789",false}, //larger than usual
	{"012345678901234567890123456789012345678901234567890123456789:",true} //larger than usual
};

//Header date value field
#define MAX_DateValue 41
const unsigned int MAX_DateValue0 = MAX_DateValue;
const unsigned int MAX_DateValue1 = MAX_DateValue;
value_pair g_rgDateValue[MAX_DateValue]={
	{"Mon, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Tue, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Wed, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Thu, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Fri, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Sat, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"Sun, 26 Nov 2000 14:36:10 GMT",true}, //valid
	{"XXX, 26 Nov 2000 14:36:10 GMT",false}, //invalid day
	{"Mon, 1 Nov 2000 14:36:10 GMT",true}, //valid
	{"Mon, 30 Nov 2000 14:36:10 GMT",true}, //valid
	{"Mon, 31 Nov 2000 14:36:10 GMT",false}, //there is no nov 31st
	{"Mon, 99 Nov 2000 14:36:10 GMT",false}, //there is no nov 99th
	{"Mon, -1 Nov 2000 14:36:10 GMT",false}, //there is no nov -1th
	{"Mon, 1 Nov 4050 14:36:10 GMT",true}, //valid
	{"Mon, 1 Jan 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Feb 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Mar 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Apr 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 May 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Jun 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Jul 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Aug 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Sep 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Oct 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Nov 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 Dec 2000 14:36:10 GMT",true}, //valid
	{"Mon, 1 XXX 2000 14:36:10 GMT",false}, //invalid month
	{"Mon, 1 Nov 10000000 14:36:10 GMT",false}, //invalid year
	{"Mon, 1 Nov -1 14:36:10 GMT",false}, //invalid year
	{"Mon, 1 Nov 2000 -1:36:10 GMT",false}, //invalid hour
	{"Mon, 1 Nov 2000 00:00:00 GMT",true}, //valid midnight
	{"Mon, 1 Nov 2000 23:59:59 GMT",true}, //valid one minute before midnight
	{"Mon, 1 Nov 2000 24:00:00 GMT",false}, //invalid rep of midnight
	{"Mon, 1 Nov 2000 14:-1:10 GMT",false}, //invalid minute
	{"Mon, 1 Nov 2000 14:60:10 GMT",false}, //invalid minute
	{"Mon, 1 Nov 2000 14:36:-1 GMT",false}, //invalid second
	{"Mon, 1 Nov 2000 14:36:60 GMT",false}, //invalid second
	{"Mon, 1 Nov 2000 14:36:10 XMT",false}, //invalid timestamp
	{"Mon, 1 Nov 2000 14:36:10.50 GMT",false}, //invalid milliseconds
	{"Mon,X1XNovX2000X14:36:10XGMT",false}, //invalid required whitespace
	{"MonXX1XNovX2000X14X36X10XGMT",false} //invalid required whitespace and special chars
};

//Header content-length value field
const unsigned int MAX_ContentLength = 5;
value_pair g_rgContentLength[MAX_ContentLength]={
	{"10",true}, //valid length and content
	{"0",true}, //valid length and content
	{"-1",false}, //invalid length for content
	{"9",false}, //length and content mismatch
	{"11",false} //length and content mismatch
};

//content field (parallel array to g_rgContentLength array)
value_pair g_rgContent[MAX_ContentLength]={
	{"0123456789",true}, //parallel
	{"",true}, //parallel
	{"0123456789",true}, //parallel
	{"0123456789",true}, //parallel
	{"0123456789",true} //parallel
};

//second dimension to response enumeration
typedef struct response_struct{
	const char* pcszName; //field identifier
	value_pair* rgValuePair; //possibilities
	DWORD dwcValuePair; //# of possibilities
}response;//endtypedef

const unsigned int ResponseComponents=17;
const unsigned int ContentLengthIndex=13;
const unsigned int ContentIndex=16;
response g_response[ResponseComponents]={
	{"HTTP Version",g_rgHTTP_Version,MAX_HTTP_Version},
	{"SP 0",g_rgSP,MAX_SP},
	{"Status Code",g_rgStatus_Code,MAX_Status_Code},
	{"SP 1",g_rgSP,MAX_SP},
	{"Reason Phrase",g_rgReason_Phrase,MAX_Reason_Phrase},
	{"CRLF 0",g_rgCRLF,MAX_CRLF},
	{"Date Name",g_rgDateName,MAX_DateName},
	{"Date Value 0",g_rgDateValue,MAX_DateValue},
	{"CRLF 1",g_rgCRLF,MAX_CRLF},
	{"Last Modified Name",g_rgLastModifiedName,MAX_LastModifiedName},
	{"Date Value 1",g_rgDateValue,MAX_DateValue},
	{"CRLF 2",g_rgCRLF,MAX_CRLF},
	{"Content Length Name",g_rgContentLengthName,MAX_ContentLengthName},
	{"Content Length",g_rgContentLength,MAX_ContentLength},
	{"CRLF 3",g_rgCRLF,MAX_CRLF},
	{"CRLF 4",g_rgCRLF,MAX_CRLF},
	{"Content",g_rgContent,MAX_ContentLength}
};

const MAX_FileNames=19;
value_pair g_rgFilenames[MAX_FileNames]={
	{"T:\\download.txt",true},					//valid filename
	{"T:\\temp\\download.txt",true},			//valid filename
	{"T:\\temp\\temp\\download.txt",true},		//valid filename
	{"T:/download.txt",false},					//invalid filename
	{"T:/temp/download.txt",false},			//invalid filename
	{"download.txt",false},					//no drive
	{"\\download.txt",false},					//no drive
	{"T:\\01234567.012",true},					//valid filename
	{"T:\\012345678.012",false},				//breaks the 8 in 8.3
	{"T:\\01234567.0123",false},				//breaks the 3 in 8.3
	{"T:\\012345678.0123",false},				//breaks the 8 & 3 in 8.3
	{"T:\\\0",false},							//invalid char
	{"T:\\\n",false},							//invalid char
	{"T:\\\r",false},							//invalid char
	{"T:\\\n\r",false},						//invalid char
	{"T:\\\r\n",false},						//invalid char
	{"T:\\\\\\",false},						//invalid char
	{"T:\\~!@#$%^&*()_+:\"{}[]|<>?,.;`~",false},//invalid char
	{"xxxx:\\download.txt",false}				//bad drive
};

//**********
//********** webserver_simulator thread
//**********
DWORD WINAPI webserver_simulator(LPVOID pvoid){
	svrsim* pSimulator; //simulator pointer

	//valid parameter
	if (NULL==pvoid)
		return -1;

	//cast function pointer
	pSimulator=(svrsim*)pvoid;

	//start simulator
	pSimulator->Listen();

	//finished
	return 0;

}//endmethod

//**********
//********** xrlfunc_InitializeNet
//**********
HRESULT xrlfunc_InitializeNet(){

	//Initialize XNET
	int err=XnetInitialize(NULL,true);
	if (0!=err)
		return E_FAIL;

	//Initialize winsock
	WSADATA wsadata;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup(wVersionRequested, &wsadata);
	if (0!=err){
		XnetCleanup();
		return E_FAIL;
	}//endif

	return S_OK;

}//endmethod

//**********
//********** xrlfunc_CleanupNet
//**********
HRESULT xrlfunc_CleanupNet(){

	//Cleanup winsock
	WSACleanup();

	//Cleanup XNET
	XnetCleanup();

	return S_OK;

}//endmethod

//**********
//********** Enumerate_IIS_Responses
//**********
HRESULT Enumerate_IIS_Responses(const char* rgchServerResponseBuffer, DWORD dwcServerResponseBuffer, DWORD dwcValidContent, bool fExpected, bool fFile){
	HRESULT hr=S_OK;
	const XRL pcszURL=DEFAULT_URL;
	const CHAR* pcwszFile=DEFAULT_FILE;

	//configure server simulator to reply with test response
	g_oSimulator.SetResponse(rgchServerResponseBuffer,dwcServerResponseBuffer);

	//create valid XRL_KNOWN_HEADERS struct
	XRL_KNOWN_HEADERS* pXKH = new XRL_KNOWN_HEADERS;
	pXKH->pDate=new FILETIME;
	pXKH->pLastModifiedTime=new FILETIME;
	pXKH->pContentLength=new ULARGE_INTEGER;

	//Allocate space for return repsonse
	unsigned char rgchLocalResponseBuffer[MAX_SERVER_RESPONSE];
	DWORD dwcLocalResponseBuffer=MAX_SERVER_RESPONSE;

	//execute the API
	SYSTEMTIME st;
	st.wYear=1900;
	st.wMonth=1;
	st.wDayOfWeek=0;
	st.wDay=1;
	st.wHour=12;
	st.wMinute=0;
	st.wSecond=0;
	st.wMilliseconds=0;
	FILETIME ft;
	SystemTimeToFileTime(&st,&ft);

	HRESULT lhr=S_OK;
	__try{
		if (fFile){
			//Delete any existing file
			DeleteFile(pcwszFile);

			//Perform API
			lhr=XRL_DownloadFile(pcszURL,dwcLocalResponseBuffer,rgchLocalResponseBuffer,&ft,pcwszFile,pXKH);

			//If success read file into buffer
			if (S_OK==lhr){
				FILE* fp=fopen(pcwszFile,"rb");
				if (NULL==fp){
					xLog(g_hLog, XLL_FAIL, "File Missing");
					return E_FAIL;
				}//endif
				ZeroMemory(rgchLocalResponseBuffer,sizeof(rgchLocalResponseBuffer));
				dwcLocalResponseBuffer=fread(rgchLocalResponseBuffer,sizeof(char),sizeof(rgchLocalResponseBuffer),fp);
				fclose(fp);
			}//endif
		}else{
			//Perform API
			lhr=XRL_DownloadToMemory(pcszURL,&dwcLocalResponseBuffer,rgchLocalResponseBuffer,pXKH);
		}//endelse
	}__except(1){
		xLog(g_hLog, XLL_EXCEPTION, "Exception Raised");
		return E_FAIL;
	};//endexcept

	//check return code
	hr=S_OK;
	
	switch (lhr){

	case ERROR_INVALID_PARAMETER:
		if (false==fExpected){
			xLog(g_hLog, XLL_PASS, "");
		}else{
			xLog(g_hLog, XLL_FAIL, "ERROR_INVALID_PARAMETER Not Expected");
			hr=E_FAIL;
		}//endelse
		break;

	case (HRESULT_FROM_WIN32(ERROR_BAD_NET_RESP)):
		if (false==fExpected){
			xLog(g_hLog, XLL_PASS, "");
		}else{
			xLog(g_hLog, XLL_FAIL, "ERROR_BAD_NET_RESP Not Expected");
			hr=E_FAIL;
		}//endelse
		break;

	case ERROR_INSUFFICIENT_BUFFER:
		if (false==fExpected){
			xLog(g_hLog, XLL_PASS, "");
		}else{
			xLog(g_hLog, XLL_FAIL, "ERROR_INSUFFICIENT_BUFFER Not Expected");
			hr=E_FAIL;
		}//endelse
		break;

	case S_OK:
		if (true!=fExpected){
			xLog(g_hLog, XLL_INFO, "Possible Recovery Case; Validating Content");
			hr=E_FAIL;
		}//endif
		if (dwcValidContent!=dwcLocalResponseBuffer){
			//INVALID CONTENT SIZE
			xLog(g_hLog, XLL_FAIL, "Content Size Mismatch");
			hr=E_FAIL;
		}else{
			//Content valid?
			if (0!=memcmp(rgchLocalResponseBuffer,rgchServerResponseBuffer+dwcServerResponseBuffer-dwcValidContent,dwcValidContent)){
				//CONTENT INVALID
				xLog(g_hLog, XLL_FAIL, "Corrupted Content");
				hr=E_FAIL;
			}else{
				//VALID!
				xLog(g_hLog, XLL_PASS, "");
			}//endelse
		}//endelse
		break;

	case S_FALSE:

		//Content size match?
		if (dwcValidContent==dwcLocalResponseBuffer){
			//INVALID CONTENT SIZE
			xLog(g_hLog, XLL_FAIL, "Content Size Match Not Expected");
			hr=E_FAIL;
		}else{
			xLog(g_hLog, XLL_PASS, "");
		}//endelse
		break;

	default:	
		char pszErrorResult[20];
		sprintf(pszErrorResult,"Unexpected Return Value 0x%08x",lhr);
		xLog(g_hLog, XLL_FAIL, pszErrorResult);
		hr=E_FAIL;
	}//endswitch

	//destroy the XRL_KNOWN_HEADERS
	delete pXKH->pContentLength;
	delete pXKH->pLastModifiedTime;
	delete pXKH->pDate;
	delete pXKH;
	pXKH=NULL;

	//return
	return hr;
}//endmethod

//**********
//********** xrlfunc_Enumerate_IIS_Responses
//**********

HRESULT xrlfunc_Enumerate_IIS_Responses(){

	char rgchBuffer[MAX_SERVER_RESPONSE];//response to send back
	char pszVariation[256];	//variation name
	DWORD dwcValidContentLength=0l; //the cases Content-Length member
	bool fExpected; //expect to pass?
	HRESULT hr=S_OK;

	//Loop through the number of server components
	for (unsigned int uiA=0; uiA<ResponseComponents; uiA++){
		//Don't enumerate content field; that is done through content length field
		if (uiA==ContentIndex){
			continue;
		}//endif
		//Loop through the number of variations for the current server component
		for (unsigned int uiB=0; uiB<g_response[uiA].dwcValuePair; uiB++){
			//Initialize the server string and expected outcome
			rgchBuffer[0]='\0';
			fExpected=true;
			dwcValidContentLength=atol(g_response[ContentLengthIndex].rgValuePair[0].pszValue);
			//Constructin this variation
			for (unsigned int uiC=0; uiC<ResponseComponents; uiC++){
				//if we are on the variation field get the variation
				if (uiA==uiC){
					strcat(rgchBuffer,g_response[uiC].rgValuePair[uiB].pszValue);
					fExpected&=g_response[uiC].rgValuePair[uiB].fValid;
					//if we are varring content store the length
					if (uiC==ContentLengthIndex){
						dwcValidContentLength=atol(g_response[uiC].rgValuePair[uiB].pszValue);
					}//endif
				}else{
					//Special check: are we varrying content length field and are we about to write the content field?
					if ((uiC==ContentIndex)&&(uiA==ContentLengthIndex)){
						//special case use special content for content length field variation
						strcat(rgchBuffer,g_response[uiC].rgValuePair[uiB].pszValue);
						fExpected&=g_response[uiC].rgValuePair[uiB].fValid;
					}else{
						//Not a varation field
						strcat(rgchBuffer,g_response[uiC].rgValuePair[0].pszValue);
						fExpected&=g_response[uiC].rgValuePair[0].fValid;
					}//endelse
				}//endelse
			}//endfor
			//Execute the variation
			
			//Run once for XRL_DownloadToMemory
			HRESULT l_hr=S_OK;

			//SET COMPONENT
			xSetComponent(g_hLog, "Online", "XRL_DownloadToMemory");

			//SET FUNCTION
			xSetFunctionName(g_hLog, "IIS Response Tests");

			sprintf(pszVariation,"%s %d",g_response[uiA].pcszName,uiB);
			xStartVariation(g_hLog, pszVariation);
			l_hr=Enumerate_IIS_Responses(rgchBuffer,strlen(rgchBuffer),dwcValidContentLength,fExpected,false);
			if FAILED(l_hr) hr=l_hr;
			xEndVariation(g_hLog);

			//Run once for XRL_DownloadFile

			//SET COMPONENT
			xSetComponent(g_hLog, "Online", "XRL_DownloadFile");

			//SET FUNCTION
			xSetFunctionName(g_hLog, "IIS Response Tests");

			sprintf(pszVariation,"%s %d",g_response[uiA].pcszName,uiB);
			xStartVariation(g_hLog, pszVariation);
			l_hr=Enumerate_IIS_Responses(rgchBuffer,strlen(rgchBuffer),dwcValidContentLength,fExpected,true);
			if FAILED(l_hr) hr=l_hr;
			xEndVariation(g_hLog);

		}//endfor	
	}//endfor

	return hr;
}//endmethod

//**********
//********** TEST CASE MACROS
//**********

//NOTE: hr must init to S_OK
#define BEGINCASE(hr, hLog, desc)\
{\
	HANDLE l_hLog=(hLog);\
	HRESULT* phr=&hr;\
	xStartVariation(l_hLog,(desc));\
	__try{\
		HRESULT lhr=S_OK;

#define TRYCASE(func) lhr=(func);

//Top level HR must be defined
#define ENDCASE(hresult)\
		if (lhr!=(hresult)){\
			char pszErrorResult[20];\
			sprintf(pszErrorResult,"Unexpected Return Value 0x%08x",lhr);\
			xLog(l_hLog, XLL_FAIL, pszErrorResult);\
			*phr=E_FAIL;\
		}else{\
			xLog(l_hLog, XLL_PASS, "");\
		}\
	}__except(1){\
		xLog(l_hLog, XLL_EXCEPTION, "Exception Raised");\
		*phr=E_FAIL;\
	}\
	xEndVariation(l_hLog);\
}

//**********
//********** xrlfunc_DownloadFile
//**********
HRESULT xrlfunc_DownloadFile(){

	//Setup simulator
	const char pcszResponse[]=BASIC_RESPONSE;
	if FAILED(g_oSimulator.SetResponse(pcszResponse,strlen(pcszResponse))) return E_FAIL;

	//Valid parameter #1
	const XRL ValidParam1=DEFAULT_URL;

	//Valid parameter #2
	const DWORD ValidParam2=1024;

	//Valid parameter #3
	BYTE ValidParam3[ValidParam2];

	//Valid parameter #4
	FILETIME* ValidParam4=NULL;

	//Valid parameter #5
	const CHAR* ValidParam5=DEFAULT_FILE;

	//Valid parameter #6
	FILETIME ValidSubParam6Date;
	FILETIME ValidSubParam6LastModifiedTime;
	ULARGE_INTEGER ValidSubParam6ContentLength;
	XRL_KNOWN_HEADERS ValidParam6;
	ValidParam6.pDate=&ValidSubParam6Date;
	ValidParam6.pLastModifiedTime=&ValidSubParam6LastModifiedTime;
	ValidParam6.pContentLength=&ValidSubParam6ContentLength;

	//SET COMPONENT
	xSetComponent(g_hLog, "Online", "XRL_DownloadFile");
	
	//SET FUNCTION
	xSetFunctionName(g_hLog, "Parameter Checks");
	HRESULT hr=S_OK;

	BEGINCASE(hr,g_hLog,"Param #1-#6 VALID")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,ValidParam2,ValidParam3,ValidParam4,ValidParam5,&ValidParam6))
	ENDCASE(S_OK)

/*	BEGINCASE(hr,g_hLog,"Param #1-#6 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(NULL,NULL,NULL,NULL,NULL,NULL))
	ENDCASE(ERROR_INVALID_PARAMETER)

	BEGINCASE(hr,g_hLog,"Param #1 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(NULL,ValidParam2,ValidParam3,ValidParam4,ValidParam5,&ValidParam6))
	ENDCASE(ERROR_INVALID_PARAMETER)

	BEGINCASE(hr,g_hLog,"Param #2 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,NULL,ValidParam3,ValidParam4,ValidParam5,&ValidParam6))
	ENDCASE(ERROR_INVALID_PARAMETER)

	BEGINCASE(hr,g_hLog,"Param #3 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,ValidParam2,NULL,ValidParam4,ValidParam5,&ValidParam6))
	ENDCASE(ERROR_INVALID_PARAMETER)
*/
	BEGINCASE(hr,g_hLog,"Param #4 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,ValidParam2,ValidParam3,NULL,ValidParam5,&ValidParam6))
	ENDCASE(S_OK)

/*	BEGINCASE(hr,g_hLog,"Param #5 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,ValidParam2,ValidParam3,ValidParam4,NULL,&ValidParam6))
	ENDCASE(ERROR_INVALID_PARAMETER)

	BEGINCASE(hr,g_hLog,"Param #6 NULL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,ValidParam2,ValidParam3,ValidParam4,ValidParam5,NULL))
	ENDCASE(S_OK)
*/
	//SET FUNCTION
	xSetFunctionName(g_hLog, "Buffer Size");

	BEGINCASE(hr,g_hLog,"Param #2 TOO SMALL")
		DeleteFile(ValidParam5);
		TRYCASE(XRL_DownloadFile(ValidParam1,1,ValidParam3,ValidParam4,ValidParam5,NULL))
	ENDCASE(ERROR_INSUFFICIENT_BUFFER)

	//SET FUNCTION
	xSetFunctionName(g_hLog, "Filenames");

	CreateDirectory("T:\\temp",NULL);
	CreateDirectory("T:\\temp\\temp",NULL);

	char pszVariation[255];
	for (int i=0; i<MAX_FileNames; i++){
		HRESULT lhr=S_OK;
		sprintf(pszVariation,"Variation %d",i);
		xStartVariation(g_hLog,pszVariation);
		__try{
			lhr=XRL_DownloadFile(ValidParam1,ValidParam2,ValidParam3,NULL,g_rgFilenames[i].pszValue,NULL);
			if (g_rgFilenames[i].fValid==true){
				if (lhr==S_OK){
					if (0!=DeleteFile(g_rgFilenames[i].pszValue)){
						xLog(g_hLog, XLL_PASS, "");
					}else{
						xLog(g_hLog, XLL_FAIL, "File Not Created");
					}//endelse
				}else{
					char pszErrorResult[20];
					sprintf(pszErrorResult,"Unexpected Return Value 0x%08x",lhr);
					xLog(g_hLog, XLL_FAIL, pszErrorResult);
					hr=E_FAIL;
				}//endelse
			}else{
				if (0==DeleteFile(g_rgFilenames[i].pszValue)){
					xLog(g_hLog, XLL_PASS, "");
				}else{
					xLog(g_hLog, XLL_FAIL, "File Was Not Supposed To Be Created");
				}//endelse
			}//endelse
		}__except(1){
			xLog(g_hLog, XLL_EXCEPTION, "Exception Raised");
			hr=E_FAIL;
		}//endexcept
		xEndVariation(g_hLog);
	}//endfor

	return hr;

}//endmethod

//**********
//********** xrlfunc_DownloadToMemory
//**********
HRESULT xrlfunc_DownloadToMemory(){

	//Setup simulator
	const char pcszResponse[]=BASIC_RESPONSE;
	if FAILED(g_oSimulator.SetResponse(pcszResponse,strlen(pcszResponse))) return E_FAIL;

	//Valid parameter #1
	const XRL ValidParam1=DEFAULT_URL;

	//Valid parameter #2
	const DWORD cValidParam2=1024;
	DWORD ValidParam2=cValidParam2;

	//Valid parameter #3
	BYTE ValidParam3[cValidParam2];

	//Valid parameter #4
	FILETIME ValidSubParam4Date;
	FILETIME ValidSubParam4LastModifiedTime;
	ULARGE_INTEGER ValidSubParam4ContentLength;
	XRL_KNOWN_HEADERS ValidParam4;
	ValidParam4.pDate=&ValidSubParam4Date;
	ValidParam4.pLastModifiedTime=&ValidSubParam4LastModifiedTime;
	ValidParam4.pContentLength=&ValidSubParam4ContentLength;

	XRL_KNOWN_HEADERS CrazyParam4;

	//SET COMPONENT
	xSetComponent(g_hLog, "Online", "XRL_DownloadToMemory");

	//SET FUNCTION
	xSetFunctionName(g_hLog, "Parameter Checks");
	HRESULT hr=S_OK;

	BEGINCASE(hr,g_hLog,"Param #1-#4 VALID");
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&ValidParam4))
	ENDCASE(S_OK);

/*	BEGINCASE(hr,g_hLog,"Param #1-#4 NULL");
		TRYCASE(XRL_DownloadToMemory(NULL,NULL,NULL,NULL))
	ENDCASE(ERROR_INVALID_PARAMETER);

	BEGINCASE(hr,g_hLog,"Param #1 NULL");
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(NULL,&ValidParam2,ValidParam3,&ValidParam4))
	ENDCASE(ERROR_INVALID_PARAMETER);

	BEGINCASE(hr,g_hLog,"Param #2 NULL");
		TRYCASE(XRL_DownloadToMemory(ValidParam1,NULL,ValidParam3,&ValidParam4))
	ENDCASE(ERROR_INVALID_PARAMETER);

	BEGINCASE(hr,g_hLog,"Param #3 NULL");
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,NULL,&ValidParam4))
	ENDCASE(ERROR_INVALID_PARAMETER);

*/	BEGINCASE(hr,g_hLog,"Param #4 NULL");
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,NULL))
	ENDCASE(S_OK);

	BEGINCASE(hr,g_hLog,"Param #4 SubParam ALL NULL");
		CrazyParam4.pDate=&ValidSubParam4Date;
		CrazyParam4.pLastModifiedTime=&ValidSubParam4LastModifiedTime;
		CrazyParam4.pContentLength=NULL;
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&CrazyParam4))
	ENDCASE(S_OK);

	BEGINCASE(hr,g_hLog,"Param #4 SubParam pDate NULL");
		CrazyParam4.pDate=NULL;
		CrazyParam4.pLastModifiedTime=&ValidSubParam4LastModifiedTime;
		CrazyParam4.pContentLength=&ValidSubParam4ContentLength;
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&CrazyParam4))
	ENDCASE(S_OK);

	BEGINCASE(hr,g_hLog,"Param #4 SubParam pLastModified NULL");
		CrazyParam4.pDate=&ValidSubParam4Date;
		CrazyParam4.pLastModifiedTime=NULL;
		CrazyParam4.pContentLength=&ValidSubParam4ContentLength;
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&CrazyParam4))
	ENDCASE(S_OK);

	BEGINCASE(hr,g_hLog,"Param #4 SubParam pContentLength NULL");
		CrazyParam4.pDate=&ValidSubParam4Date;
		CrazyParam4.pLastModifiedTime=&ValidSubParam4LastModifiedTime;
		CrazyParam4.pContentLength=NULL;
		ValidParam2=cValidParam2;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&CrazyParam4))
	ENDCASE(S_OK);

	//SET FUNCTION
	xSetFunctionName(g_hLog, "Buffer Size");
	BEGINCASE(hr,g_hLog,"Param #2 TOO SMALL");
		ValidParam2=1;
		TRYCASE(XRL_DownloadToMemory(ValidParam1,&ValidParam2,ValidParam3,&ValidParam4))
	ENDCASE(ERROR_INSUFFICIENT_BUFFER);

	return hr;
}//endmethod

//**********
//********** xrlfunc_UploadFile
//**********
HRESULT xrlfunc_UploadFile(){
	return E_NOTIMPL;
}//endmethod
	
//**********
//********** xrlfunc_Performance
//**********
HRESULT xrlfunc_Performance(){
	HRESULT hr=S_OK;

	const DWORD dwcSize=1024;
	BYTE pBuffer[dwcSize];
	DWORD dwSize=dwcSize;

	DWORD dwBeginTime;
	DWORD dwEndTime;
	char pszTiming[1024];

	//BUGBUG: TEMP CODE
	XonlineChangeAllIPs( 0x0B56A8C0 );

	//SETCOMPONENT
	xSetComponent(g_hLog, "Online", "XRL_DownloadToMemory");

	//SETFUNCTION
	xSetFunctionName(g_hLog, "Performance");

	xStartVariation(g_hLog,"1K");
	dwBeginTime=GetTickCount();
	hr=XRL_DownloadToMemory(DOWNLOAD_URL,&dwSize,pBuffer,NULL);
	dwEndTime=GetTickCount();
	if SUCCEEDED(hr){
		sprintf(pszTiming,"%d ms\n",dwEndTime-dwBeginTime);
		xLog(g_hLog, XLL_PASS, pszTiming);
	}else{
		xLog(g_hLog, XLL_FAIL, "Execution Failed");
	}//endelse
	xEndVariation(g_hLog);

	//SETCOMPONENT
	xSetComponent(g_hLog, "Online", "XRL_DownloadFile");

	//SETFUNCTION
	xSetFunctionName(g_hLog, "Performance");

	xStartVariation(g_hLog,"1K");
	DeleteFile(DEFAULT_FILE);
	dwBeginTime=GetTickCount();
	hr=XRL_DownloadFile(DOWNLOAD_URL,dwcSize,pBuffer,NULL,DEFAULT_FILE,NULL);
	dwEndTime=GetTickCount();
	if SUCCEEDED(hr){
		if (0==DeleteFile(DEFAULT_FILE)){
			xLog(g_hLog, XLL_FAIL, "File Was Not Created");
		}else{
			sprintf(pszTiming,"%d ms",dwEndTime-dwBeginTime);
			xLog(g_hLog, XLL_PASS, pszTiming);
		}//endelse
	}else{
		xLog(g_hLog, XLL_FAIL, "Execution Failed");
	}//endelse
	xEndVariation(g_hLog);

	//SETCOMPONENT
	xSetComponent(g_hLog, "Online", "XRL_DownloadFile");

	//SET FUNCTION
	xSetFunctionName(g_hLog, "Performance");

	xStartVariation(g_hLog,"1M");
	DeleteFile(DEFAULT_FILE);
	dwBeginTime=GetTickCount();
	hr=XRL_DownloadFile(DOWNLOAD_URL2,dwcSize,pBuffer,NULL,DEFAULT_FILE,NULL);
	dwEndTime=GetTickCount();
	if SUCCEEDED(hr){
		if (0==DeleteFile(DEFAULT_FILE)){
			xLog(g_hLog, XLL_FAIL, "File Was Not Created");
		}else{
			sprintf(pszTiming,"%d ms",dwEndTime-dwBeginTime);
			xLog(g_hLog, XLL_PASS, pszTiming);
		}//endelse
	}else{
		xLog(g_hLog, XLL_FAIL, "Execution Failed");
	}//endelse
	xEndVariation(g_hLog);

	//UPLOAD HERE

	return hr;
}//endmethod


//**********
//********** xrlfunc_RunTests
//**********
HRESULT xrlfunc_RunTests(){

	HRESULT hr=S_OK;

	//Initialize Networking
	if FAILED(xrlfunc_InitializeNet()){
		return E_FAIL;
	}//endif

	//Run XRL_SERVER_RESPONSE Tests (for both XRL_DownloadFile and XRL_DownloadToMemory)
	hr=xrlfunc_Enumerate_IIS_Responses();
	DebugPrint("xrlfunc_Enumerate_IIS_Responses() 0x%08x\n",hr);
	
	//XRL_DownloadFile Specific Tests
	hr=xrlfunc_DownloadFile();
	DebugPrint("xrlfunc_DownloadFile() 0x%08x\n",hr);

	//XRL_DownloadToMemory Specific Tests
	hr=xrlfunc_DownloadToMemory();
	DebugPrint("xrlfunc_DownloadToMemory() 0x%08x\n",hr);

	//XRL_DownloadToMemory Specific Tests
	hr=xrlfunc_UploadFile();
	DebugPrint("xrlfunc_UploadFile() 0x%08x\n",hr);

	//Run Performance Tests
	hr=xrlfunc_Performance();
	DebugPrint("xrlfunc_Performance() 0x%08x\n",hr);

	//Cleanup Networking
	xrlfunc_CleanupNet();

	return S_OK;

}//endmethod

static XONLINE_USER testUsers[1] =
{
	{ "User1", 0,
		{'1','2','3','4','5','6','7','8','1','2','3','4','5','6','7','8','1','2','3','4','5','6','7','8','1','2','3','4','5','6','7','8'} }
};

//**********
//********** xrlfunc_StartTest
//**********
VOID WINAPI xrlfunc_StartTest(HANDLE hLog){

	BREAK_INTO_DEBUGGER;

	g_hLog = hLog;

	//start server simulator
	DWORD dwThreadid=0L;
	g_hThread=CreateThread(NULL, 0, &webserver_simulator, &g_oSimulator, 0, &dwThreadid);

	//BUGBUG: TEMP CODE
	HRESULT hr=S_OK;
	XonlineResetState();
	hr = XonlineCreateMachineAccount();
	hr = XonlineCreateUserAccount(&testUsers[0], 0, NULL, NULL);
    hr = XonlineLogon("xmatch,content,xstats", testUsers, 1);
	XonlineChangeAllIPs( 0x0100007F );

	//give simulator a second to start up
	Sleep(1000);

	//run tests
	xrlfunc_RunTests();

}//endmethod

//**********
//********** xrlfunc_EndTest
//**********
VOID WINAPI xrlfunc_EndTest(VOID){

	//stop server simulator
	g_oSimulator.Stop();

	//delete temporary ticket file
	DeleteFile(TICKET_FILE);

	//give simulator a second to shutdown
	Sleep(1000);

	//If it did not shutdown kill it
	DWORD dwExitCode=0;
	if (0!=GetExitCodeThread(g_hThread,&dwExitCode)){
		//TerminateThread(g_hThread,-1); //unsupported at this time
	}//endif

	CloseHandle(g_hThread);

}//endmethod

//**********
//********** Export function pointers of StartTest and EndTest
//**********
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xrlfunc )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xrlfunc )
    EXPORT_TABLE_ENTRY( "StartTest", xrlfunc_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", xrlfunc_EndTest )
END_EXPORT_TABLE( xrlfunc )