
#include <xtl.h>
#include <stdio.h>
#include <tchar.h>
//#include <afx.h>
//#include <afxdisp.h>
#include "testcrt.h"
#include "CRunTime.h"
//#include "CTime.h"
//#include "COleDT.h"

//CStdioFile f1;
FILE * f1;

DTMINFO tDates[] = {
	{1996,2,28,4,12,0,0},
	{1996,2,29,5,12,0,0},
	{1996,3,1,6,12,0,0},
	{1997,2,28,6,12,0,0},
	{1997,3,1,7,12,0,0},
	{1998,2,28,7,12,0,0},
	{1998,3,1,1,12,0,0},
	{1998,4,3,6,12,0,0},
	{1998,4,4,7,12,0,0},
	{1998,4,5,1,1,59,58},
	{1998,4,5,1,3,0,0},
	{1998,4,6,2,12,0,0},
	{1998,10,23,6,12,0,0},
	{1998,10,24,7,12,0,0},
	{1998,10,25,1,1,59,58},
	{1998,10,25,1,2,0,0},
	{1998,10,26,2,12,0,0},
	{1999,2,28,1,12,0,0},
	{1999,3,1,2,12,0,0},
	{1999,4,2,6,12,0,0},
	{1999,4,3,7,12,0,0},
	{1999,4,4,1,1,59,58},
	{1999,4,4,1,3,0,0},
	{1999,4,5,2,12,0,0},
	{1999,10,29,6,12,0,0},
	{1999,10,30,7,12,0,0},
	{1999,10,31,1,1,59,58},
	{1999,10,31,1,2,0,0},
	{1999,11,1,2,12,0,0},
	{1999,12,31,6,12,0,0},
	{2000,1,1,7,12,0,0},
	{2000,2,28,2,12,0,0},
	{2000,2,29,3,12,0,0},
	{2000,3,1,4,12,0,0},
	{2000,3,31,6,12,0,0},
	{2000,4,1,7,12,0,0},
	{2000,4,2,1,1,59,58},
	{2000,4,2,1,3,0,0},
	{2000,4,3,2,12,0,0},
	{2000,10,27,6,12,0,0},
	{2000,10,28,7,12,0,0},
	{2000,10,29,1,1,59,58},
	{2000,10,29,1,2,0,0},
	{2000,10,30,2,12,0,0},
	{2000,12,31,1,12,0,0},
	{2001,1,1,2,12,0,0},
	{2001,2,28,4,12,0,0},
	{2001,3,1,5,12,0,0},
	{2001,3,30,6,12,0,0},
	{2001,3,31,7,12,0,0},
	{2001,4,1,1,1,59,58},
	{2001,4,1,1,3,0,0},
	{2001,4,2,2,12,0,0},
	{2001,4,6,6,12,0,0},
	{2001,4,7,7,12,0,0},
	{2001,4,8,1,1,59,58},
	{2001,4,8,1,3,0,0},
	{2001,4,9,2,12,0,0},
	{2001,10,26,6,12,0,0},
	{2001,10,27,7,12,0,0},
	{2001,10,28,1,1,59,58},
	{2001,10,28,1,2,0,0},
	{2001,10,29,2,12,0,0},
	{0,0,0,0,0,0,0}
};

LangName LangNameTable[] = {
		//*0x01,0x00,	_T("Arabic"), 
		//*0x01,0x01,	_T("Arabic (Saudi Arabia)"),
		//*0x01,0x02,	_T("Arabic (Iraq)"),
		//*0x01,0x03,	_T("Arabic (Egypt)"),
		//*0x01,0x04,	_T("Arabic (Libya)"),
		0x01,0x05,	_T("Arabic (Algeria)"),
		0x01,0x06,	_T("Arabic (Morocco)"),
		0x01,0x07,	_T("Arabic (Tunisia)"),
		//*0x01,0x08,	_T("Arabic (Oman)"),
		//*0x01,0x09,	_T("Arabic (Yemen)"),
		//*0x01,0x0A,	_T("Arabic (Syria)"),
		//*0x01,0x0B,	_T("Arabic (Jordan)"),
		//*0x01,0x0C,	_T("Arabic (Lebanon)"),
		//*0x01,0x0D,	_T("Arabic (Kuwait)"),
		//*0x01,0x0E,	_T("Arabic (U.A.E)"),
		//*0x01,0x0F,	_T("Arabic (Bahrain)"),
		//*0x01,0x10,	_T("Arabic (Qatar)"),
		//*0x02,0x01,	_T("Bulgarian"),
		0x03,0x01,	_T("Catalan"),				
		//*0x04,0x00,	_T("Chinese"),
		//*0x04,0x01,	_T("Chinese (Taiwan)"),
		0x04,0x02,	_T("Chinese (PRC)"),
		0x04,0x03,	_T("Chinese (Hong Kong)"),
		0x04,0x04,	_T("Chinese (Singapore)"),
		0x05,0x01,	_T("Czech"),
		0x06,0x01,	_T("Danish"),			
		0x07,0x01,	_T("German (Standard)"),			
		0x07,0x02,	_T("German (Swiss)"),
		0x07,0x03,	_T("German (Austrian)"),
		0x07,0x04,	_T("German (Luxembourg)"),
		0x07,0x05,	_T("German (Liechtenstein)"),
		//*0x08,0x01,	_T("Greek"),
		0x09,0x00,	_T("English"),
		0x09,0x01,	_T("English (United States)"),
		0x09,0x02,	_T("English (British)"),
		0x09,0x03,	_T("English (Australian)"),
		0x09,0x04,	_T("English (Canadian)"),
		0x09,0x05,	_T("English (New Zealand)"),
		0x09,0x06,	_T("English (Ireland)"),
		0x09,0x07,	_T("English (South Africa)"),
		0x09,0x08,	_T("English (Jamaica)"),
		0x09,0x0A,	_T("English (Belise)"),
		0x09,0x0B,	_T("English (Trinidad)"),
		0x0A,0x01,	_T("Spanish"),
		0x0A,0x02,	_T("Spanish (Mexican)"),
		0x0A,0x03,	_T("Spanish"),
		0x0A,0x04,	_T("Spanish (Guatemala)"),
		0x0A,0x05,	_T("Spanish (Costa Rica)"),
		0x0A,0x06,	_T("Spanish (Panama)"),
		0x0A,0x07,	_T("Spanish"),
		0x0A,0x08,	_T("Spanish (Venezuela)"),
		0x0A,0x09,	_T("Spanish (Columbia)"),
		0x0A,0x0A,	_T("Spanish (Peru)"),
		0x0A,0x0B,	_T("Spanish (Argentina)"),
		0x0A,0x0C,	_T("Spanish (Ecuador)"),
		0x0A,0x0D,	_T("Spanish (Chile)"),
		0x0A,0x0E,	_T("Spanish (Uruguay)"),
		0x0A,0x0F,	_T("Spanish (Paraguay)"),
		0x0A,0x10,	_T("Spanish (Bolivia)"),
		0x0A,0x11,	_T("Spanish (El Salvador)"),
		0x0A,0x12,	_T("Spanish (Honduras)"),
		0x0A,0x13,	_T("Spanish (Nicaragua)"),
		0x0A,0x14,	_T("Spanish (Puerto Rico)"),
		0x0B,0x01,	_T("Finnish"),	
		0x0C,0x01,	_T("French (Standard)"),			
		0x0C,0x02,	_T("French (Belgian)"),
		0x0C,0x03,	_T("French (Canadian)"),
		0x0C,0x04,	_T("French (Swiss)"),
		0x0C,0x05,	_T("French (Luxembourg)"),
		0x0D,0x01,	_T("Hebrew"),			
		0x0E,0x01,	_T("Hungarian"),
		0x0F,0x01,	_T("Icelandic"),		
		0x10,0x01,	_T("Italian (Standard)"),			
		0x10,0x02,	_T("Italian (Swiss)"),
		0x11,0x01,	_T("Japanese"),		
		//*0x12,0x01,	_T("Korean"),		
		0x13,0x01,	_T("Dutch (Standard)"),		
		0x13,0x02,	_T("Dutch (Belgian)"),
		0x14,0x01,	_T("Norwegian (Bokmal)"),
		0x14,0x02,	_T("Norwegian (Nynorsk)"),	
		0x15,0x01,	_T("Polish"),
		0x16,0x01,	_T("Portuguese (Brazilian)"),
		0x16,0x02,	_T("Portuguese (Standard)"),
		//0x17,0x01,	_T("Rhaeto-Romanic"),
		0x18,0x01,	_T("Romanian"),
		//0x18,0x02,	_T("Romanian (Moldavia)"),
		0x19,0x01,	_T("Russian"),
		//0x19,0x02,	_T("Russian (Moldavia)"),
		0x1A,0x01,	_T("Croatian"),
		0x1A,0x02,	_T("Serbian (Latin)"),
		0x1A,0x03,	_T("Serbian (Cyrillic)"),
		0x1B,0x01,	_T("Slovak"),
		0x1C,0x01,	_T("Albanian"),
		0x1D,0x01,	_T("Swedish"),
		0x1D,0x02,	_T("Swedish (Finland)"),
		0x1E,0x01,	_T("Thai"),
		0x1F,0x01,	_T("Turkish"),
		//0x20,0x01,	_T("Urdu"),
		0x21,0x01,	_T("Indonesian"),
		0x22,0x01,	_T("Ukranian"),
		0x23,0x01,	_T("Belarusian"),
		0x24,0x01,	_T("Slovenian"),
		0x25,0x01,	_T("Estonian"),
		0x26,0x01,	_T("Latvian"),
		0x27,0x01,	_T("Lithuanian"),
		//*0x29,0x01,	_T("Farsi"),
		0x2A,0x01,	_T("Vietnamese"),
		0x2D,0x01,	_T("Basque"),
		//0x2E,0x01,	_T("Sorbian"),
		//0x2F,0x01,	_T("FYROM"),
		//0x30,0x01,	_T("Sutu"),
		//0x31,0x01,	_T("Tsonga"),
		//0x32,0x01,	_T("Tswana"),
		//0x34,0x01,	_T("Xhosa"),
		//0x35,0x01,	_T("Zulu"),
		0x36,0x01,	_T("Afrikaans"),
		0x38,0x01,	_T("Faeroese"),
		//0x39,0x01,	_T("Hindi"),
		//0x3A,0x01,	_T("Maltese"),
		//0x3C,0x01,	_T("Gaelic"),
		//0x3D,0x01,	_T("Yiddish"),
		//0x3E,0x01,	_T("Malaysian"),
		0x00,0x00,  _T("Language neutral")
};

int testcrtEntry() {
	BOOL bRetVal;

	f1= fopen("Results.log","wt");
	fputs ("Testing CRunTime\n", f1);
	bRetVal = TestCRunTime();
	fclose (f1);

	// bRetVal == TRUE if test passed
	if (bRetVal)
	{
		return 0;
	}
	else
	{
		puts ("FAIL\n");
		return 1;
	}
}

BOOL compare(_TCHAR *sText, int x, int y) {
//	CString sOut(' ', 128);
	if (x != y) {
//		sOut.Format("\t%s: %4d not equal to %4d\n", sText, x, y);
//		f1.WriteString(sOut);
		fprintf (f1, "\t%s: %4d not equal to %4d\n", sText, x, y);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void PrintPassFail(BOOL bFailed) {
//	CString sOut(' ',128);
	if (bFailed) 
		fputs ("Test Failed!\n\n", f1);
//		sOut = "Test Failed!\n\n";
	else
		fputs ("Test Passed!\n\n", f1);
//		sOut = "Test Passed!\n\n";
//	f1.WriteString(sOut);
}
  
