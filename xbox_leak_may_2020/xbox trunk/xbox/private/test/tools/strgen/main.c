#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>

#include <assert.h>
#include <locale.h>

#include <tchar.h>

#include "strgen.h"

//
// CONSTANTS
//

#define  chDash      '-'
#define  chSlash     '/'
#define  chColon     ':'

#define  chFormat    'F'
#define  chformat    'f'

#define  chseed      's'
#define  chSeed      'S'

#define  chstyle     't'
#define  chStyle     'T'

#define  chlength    'l'
#define  chLength    'L'

#define  chOutput    'o'
#define  choutput    'O'

#define  chRisk    'R'
#define  chrisk    'r'

#define  chPresudo    'P'
#define  chpresudo    'p'

#define  chANSIInput    'A'
#define  chansinput    'a'

#define  chUnicodeInput    'U'
#define  chunciodeinput    'u'

#define  chUnicodeInput    'U'
#define  chunciodeinput    'u'


#define  chLANG            'G'
#define  chlang            'g'

#define  chNETBIOS            'N'
#define  chnetbios            'n'



void  Usage(char **);
int  CommandLineParse(	int,char**,int*,unsigned int*,int*,int*,int*,int*,char *,char**);
int  ConvertLCIDtoCodePage(LCID);
LCID LanguageHelper(char *);
BOOL SendDataToClipBoard(void);

const char *pstrUsage[] =
{
    "\n",
//    "strgen [-L:length] [-s:seed] [-f:format] [-t:style] [-o:OutFileName] [-p:Presudo]",
    //"strgen [-l:length] [-s:seed] [-f:format] [-g:style] ] [-n:nValidNetBiosName] [-r:nRiskString]",
    "strgen [-l:length] [-s:seed] [-f:format] [-g:style] ] [-n:nValidNetBiosName] ",
    "\n",
    "        -l  specifies the length of string in byte	\n",
	"             0 means random length with max_length being 255,default is 0  \n",
    "            -1 means random length with max_length being 32767 \n",
    "        -s  specifies the seed ,default seed is generated from time\n",
    //"        -t  specifies string style 0:ANSI 1:Pure loc string 2:MIXED,default is 2\n",
    //"        -o  output file name \n",
    "        -f  a :the strgen generated in Clipboard is in ANSI \n",
    "            u :the strgen generated in Clipboard is in UNICODE  default is  u\n",
    //"        -r  0 means ,default is try to generate risky chars ,specify 0 will generate random chars\n",
    "        -g  specify language(3-letter,eg JPN/GER/ARA/HEB/CHS/CHT/KOR...)\n"
    "        -a  input ANSI code String \n"
    "        -u  input Unicode  code String \n"
//    "            0 means not necessary contains risk string ,default is 0\n",
//    "        -p  1 means generate char despite wether it really exists, \n",
//    "            0 means generate char  only  it really exists \n ", 
//    "              default is 0  \n",
	"\n",
    "  Note: Generated string is always put in the Clipboard. \n",
    "  Note: set clipboard=off before running if you do not want the string \n",
    "  Note: be put in clipboard\n",
    "\n",    
    NULL
};

const char *pstrJapanese[] =
{
    "\n",
    "In Japanese leading byte is 0x81,0x40,0x82,0x9F,0xFC,0xE5 \n",
    "or trailing byte is 0x5B,0x5C,0x5D,0x7C,0xA1,0xAE,0xBF,0xDF,0xE0,0x40,0xFC\n",
    "will mostly exposed code dealing with DBCS problem ",
    "\n",
    NULL
};

const char *pstrGerman[] =
{
    "\n",
    "In German using  extended chars (above 0x80) will have larger chance to find \n",
    "coding dealing with code page problem, since the same coed point above 0x80  \n",
    "has different meaning between German's Wndows code page 1252 and DOC code page 850 \n",
    "The  most common used chars are 0xE4,0xF6,0xFC,0xC4,0xD6,0xDC,0xDF in 1252 \n",
    "\n",
    NULL
};
    

LCID  g_lcID[MAX_PATH] ;
int   g_nLang = 0;


int _cdecl  main(int argc, char *argv[ ])
{
    int nFormat ,nStyle, nLength,nPresudo,nRisk;
    unsigned int  uSeed;
    char achOutputFileName[MAX_PATH];
    
    void  *pVoid;
    HANDLE hFile = NULL;
    int nWritten;
    int  n;
    char achLang[128];
    HWND hWnd;
    HANDLE hMem;
    DWORD dw;
    char *pInputString;
    BOOL fValid;
    char achCodePage[10],achCodePageTmp[10];
    WORD *pwRiskUnicode = NULL;
    char *pRiskAnsi = NULL;
    BYTE i,j;
    BOOL bNotValidMixedLocale=FALSE;
    char *achTmp;
    


    //setlocale(LC_ALL,"");
    if ( ! CommandLineParse(argc,
                              argv,
                  	      &nFormat,
                              &uSeed,
                              &nStyle,
			                  &nLength,
                              &nPresudo,
                              &nRisk,
                              achOutputFileName,
                              &pInputString))
    {
            Usage(pstrUsage);
            return -1;
    }
    if (!g_nLang)
    {
       g_lcID[0]=GetSystemDefaultLCID();
       g_lcID[1]=0;
    }  

    for (i=0;i<g_nLang;i++)
    {
        if (!IsValidLocale(g_lcID[i], LCID_INSTALLED))
        {
            //assert(0 && " Not Correct Locale ");
	        //g_lcID = GetSystemDefaultLCID();
            printf("LCID %x not valid, most probally the langpack for that locale not installed !\n",g_lcID[i]);
            exit(1);
        }
    }

    strcpy(achCodePage,".");

    _itoa(ConvertLCIDtoCodePage(g_lcID[0]),achCodePageTmp,10);
    strcat(achCodePage,achCodePageTmp);

    setlocale(LC_ALL,achCodePage);
    //setlocale(LC_ALL,"");

    if (pInputString)
    {
       nRisk = 0;
       PassString(pInputString,nFormat,&pVoid,&fValid,g_lcID[0]);    
    }
    else
    if (g_nLang >1 )
    {
        if (SG_MAXLENGTH_256==nLength)
        {
           nLength = 1;           
        }        
        nFormat = 1;
        srand(uSeed);
        GenerateMixRiskString(g_lcID,(WCHAR**)&pVoid,nLength  ,nStyle);
    }
    else
    {
        if (nRisk)
        {
            if (SG_MAXLENGTH_256==nLength)
            {
                nLength = 1;
            }                
            srand(uSeed);
            if ( ISFELocale(g_lcID[0]))
            {
                GenerateFERiskString(&pRiskAnsi,&pwRiskUnicode,nLength,g_lcID[0],nStyle);
            }
            else
            {
               GenerateEuroRiskString(&pRiskAnsi,&pwRiskUnicode,nLength,g_lcID[0],nStyle);
            }
        }        
        else  
        {
            //BUGBUG Normal risk string
        }
    }

    if (!pVoid)
    {
       printf("Internal Error,Not Enough Memory\n");
       exit(1);
    }

    if (nRisk && (g_nLang<2) )
    {
        printf("\n The code generated is :\n");

    }

    if (g_nLang >1)
    {
        printf("The String generated is in clipboard and str.txt in current directory\n");
    }
    else
    {

        if (nFormat)
        {
            if (pwRiskUnicode)
                pVoid = pwRiskUnicode;
            wprintf(L"%s",(WCHAR*)pVoid);
        }
        else   
        {
            if (pRiskAnsi)
                pVoid = pRiskAnsi;
            printf("%s",(char*)pVoid);
        }
    }

    if ( (!pInputString) && (g_nLang<2))
    {
    if (nRisk)
    {
        
        if (ISFELocale(g_lcID[0]))
        {
            int k,n;
            n = strlen((char*)pVoid);

            if (1!=nLength) 
            {
                printf("\n");
            }
            printf("ANSI: ");
            for (k=0;k<n;k=k+2)
            {
                i = pRiskAnsi[k];
                j = pRiskAnsi[k+1];
                printf("%X%X ",i,j);
            }            
            if (1!=nLength) 
            {
                printf("\n");
            }
            printf("Unicode:");
            n = wcslen(pwRiskUnicode);
            for (k=0;k<n  ;k++)
            {
                printf("%X ",pwRiskUnicode[k]);
            }
        }
        else
        {
            
            int k,n;
            n = strlen((char*)pVoid);

            if (1!=nLength) 
            {
                printf("\n");
            }
            printf("ANSI: ");
            for (k=0;k<n;k++)
            {
                BYTE  a = 0;
                a = pRiskAnsi[k];
                printf("%X",a);
            }            
            if (1!=nLength) 
            {
                printf("\n");
            }
            printf("Unicode:");
            n = wcslen(pwRiskUnicode);
            for (k=0;k<n    ;k++)
            {
                printf("%X ",pwRiskUnicode[k]);
            }
        }

    }
    }
    
    printf("\n");
    //first try user input file name
    hFile = INVALID_HANDLE_VALUE;

    if (*achOutputFileName)
    {
        hFile =  CreateFile(
				achOutputFileName,
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
    }

    //If fail or user did not give file name,use default name
    if (INVALID_HANDLE_VALUE == hFile)
    {
		hFile =  CreateFile(
				"str.txt",
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
    }
    if (INVALID_HANDLE_VALUE == hFile) 
    {   char tmpPath[MAX_PATH];

        GetTempPath(MAX_PATH,tmpPath);
        strcat(tmpPath,"\\str.txt");

        hFile =  CreateFile(
				tmpPath,
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
    }
    if (INVALID_HANDLE_VALUE == hFile) 
    {
        //BUGBUG need to contine , do not quit 
        //printf("file write error !");
        //return 1;
        hFile = NULL;
    }
    hWnd = GetDesktopWindow();
	assert(hWnd&&"Invalid Desktop Win handle");

    
    if (nFormat)
    {
        unsigned char ucTag;
        ucTag=0xFF;
        if (hFile)
		    WriteFile(hFile,
					&ucTag,
					sizeof(unsigned char),
					&nWritten,
					NULL);
        ucTag=0xFE;
        if (hFile)
            WriteFile(hFile,
					&ucTag,
					sizeof(unsigned char),
					&nWritten,
					NULL);
        if (hFile)
		    WriteFile(hFile,
					(WCHAR*)pVoid,
					wcslen((WCHAR*)pVoid) * sizeof(WCHAR),
					&nWritten,
					NULL);  
        if (SendDataToClipBoard())
        {
            if (OpenClipboard(NULL)) 
		    {
			    WCHAR *pWStr;
			    hMem = GlobalAlloc(
						    GMEM_MOVEABLE | GMEM_DDESHARE,
						    (wcslen((WCHAR*)pVoid)+1) * sizeof(WCHAR));
			    if (!hMem) 
				    goto CleanUp;
			
			    pWStr = GlobalLock(hMem);

			    if (!pWStr) 
			    {	
				    GlobalFree(hMem);
				    goto CleanUp;
			    }

			    wcscpy(pWStr,(WCHAR*)pVoid);
			    GlobalUnlock(hMem);
                EmptyClipboard();
			    SetClipboardData(CF_UNICODETEXT,hMem);
                dw = GetLastError();
			    CloseClipboard();
		    }
        }
                    
    }
    else
    {
        if (hFile)
		    WriteFile(hFile,
					(char*)pVoid,
					strlen((char*)pVoid) ,
					&nWritten,
					NULL);  

        if (SendDataToClipBoard())
        {
            if (OpenClipboard(NULL)) 
		    {
			    char *pStr;
                int n;

                n = (strlen((char*)pVoid)+1) * sizeof(char);
			    hMem = GlobalAlloc(
				    		GMEM_MOVEABLE | GMEM_DDESHARE,
					    	(strlen((char*)pVoid)+1) * sizeof(char));
			    if (!hMem) 
				    goto CleanUp;
			
			    pStr = GlobalLock(hMem);

			    if (!pStr) 
			    {	
				    GlobalFree(hMem);
				    goto CleanUp;
			    }

			    strcpy(pStr,(char*)pVoid);
			    GlobalUnlock(hMem);
                EmptyClipboard();
			    SetClipboardData(CF_TEXT,hMem);
                dw = GetLastError();
			    CloseClipboard();
		    }
        }
    }
 
	CleanUp:
    if ( (INVALID_HANDLE_VALUE!=hFile) && (hFile!=NULL) )
        CloseHandle(hFile);

	free(pVoid);
	return 1;

}


void  Usage(char **achStr)
{
    register i = 0;
    while (achStr[i] != NULL)
    {
        fprintf(stderr, achStr[i++]);
    }
    fflush(stderr);
}


int  CommandLineParse(	int           argc,
                        char          *argv[],
			int           *pFormat,
			unsigned int  *pSeed,
			int	      *pStyle,
			int           *pLength,
                        int           *pPresudo,
                        int           *pRisk,
			char	      *pOutputFileName,
                        char          **pInputString
                        )
{
    int  nParaType;

    // Cycle through all the command line arguments.
    //
#define PARAMETER_TYPE_FORMAT		0
#define PARAMETER_TYPE_STYLE		1
#define PARAMETER_TYPE_SEED		2
#define PARAMETER_TYPE_LENGTH		3
#define PARAMETER_TYPE_OUTPUT		4
#define PARAMETER_TYPE_PRESUDO		5
#define PARAMETER_TYPE_RISK 		6
#define PARAMETER_TYPE_ANSIINPUT	7
#define PARAMETER_TYPE_UNICODEINPUT	8
#define PARAMETER_TYPE_LANG             9

    register i = 1;

    *pFormat = SG_FORMAT_UNICODE;
    *pLength = SG_MAXLENGTH_256;
    *pStyle  = SG_STYLE_NORMAL;
    *pSeed   = (unsigned)time( NULL );
    *pPresudo = SG_PRESUDO_NO;
    *pRisk    = SG_RISK_YES;

    *pInputString = NULL;

    strcpy(pOutputFileName,"");

    if (1 == argc)//no parameter using default
    {
        return TRUE;
    }
    if (_stricmp(argv[1], "-?") == 0 ||
                    _stricmp(argv[1], "/?") == 0 ||
                    _stricmp(argv[1], "?") == 0 ||
                    _stricmp(argv[1], "help") == 0)
    {
        return FALSE;//return false and print help meaasge
    }

    while (i < argc)
    {
        char  *ptchArg = argv[i++];

        // Check that what has been typed is meant to be an argument.
        //
        if ((*ptchArg == chDash) || (*ptchArg == chSlash))
        {
            ptchArg++;

            // Determine which argument is being set.
            //
            switch (*ptchArg)
            {
            case chFormat:
            case chformat:
                   nParaType = PARAMETER_TYPE_FORMAT;
		   break;

            case chstyle:
            case chStyle:
                   nParaType = PARAMETER_TYPE_STYLE;
                 break;

            case chseed:
            case chSeed:
                   nParaType = PARAMETER_TYPE_SEED;
                break;
            case chlength:
            case chLength:
		nParaType = PARAMETER_TYPE_LENGTH;
                break;

            case choutput:
            case chOutput:
		nParaType = PARAMETER_TYPE_OUTPUT;
                break;
            case chPresudo:
            case chpresudo:
                nParaType = PARAMETER_TYPE_PRESUDO;
                break;
            case chNETBIOS:
            case chnetbios:
                nParaType = PARAMETER_TYPE_STYLE;
                break;
            case chRisk:
            case chrisk:
                nParaType = PARAMETER_TYPE_RISK;
                break;

            case chANSIInput:
            case chansinput :
                nParaType = PARAMETER_TYPE_ANSIINPUT;
                break;
            case chUnicodeInput:
            case chunciodeinput:
                nParaType = PARAMETER_TYPE_UNICODEINPUT;
                break;
            case  chLANG:
            case  chlang:
                nParaType = PARAMETER_TYPE_LANG;
                break;
            default:
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }

        // Only the first character is used to determine the option
        // being set.  After it should be a colon followed by the
        // argument itself.  If this is not the case, then it is an
        // error.
        //
        ptchArg++;
        if ((*ptchArg++ == chColon) && (*ptchArg != (char)NULL))
        {
            switch (nParaType)
	    {
		case PARAMETER_TYPE_FORMAT:
		   if (*ptchArg=='A' || *ptchArg=='a')
        		*pFormat = SG_FORMAT_ASCII;
		break;
		case PARAMETER_TYPE_LENGTH:
		if (-1==atoi(ptchArg))
			*pLength=SG_MAXLENGTH_32768;
		else if (atoi(ptchArg) > 0)
	    	        *pLength=atoi(ptchArg);
		break;

			case PARAMETER_TYPE_SEED:
				*pSeed = atoi(ptchArg);
                break;
            case PARAMETER_TYPE_RISK:
                //if (1==atoi(ptchArg))
					*pRisk=SG_RISK_NO;
				 break;
			case PARAMETER_TYPE_STYLE:
				if (SG_STYLE_NETBIOS == atoi(ptchArg))
					*pStyle = SG_STYLE_NETBIOS;
				break;
			case PARAMETER_TYPE_OUTPUT:
				strcpy(pOutputFileName,ptchArg);
				break;
            case PARAMETER_TYPE_PRESUDO:
				if (atoi(ptchArg)==0)
                    *pPresudo = SG_PRESUDO_NO;
                else  if (atoi(ptchArg)==1)
                    *pPresudo = SG_PRESUDO_YES;
                break;
            case PARAMETER_TYPE_ANSIINPUT:
                *pInputString = malloc((strlen(ptchArg)+1)*sizeof(BYTE));
                strcpy(*pInputString,ptchArg);
                *pFormat = 0;
                i = argc + 1;
                break;
            case PARAMETER_TYPE_UNICODEINPUT:
                *pInputString = malloc((strlen(ptchArg)+1)*sizeof(BYTE));
                strcpy(*pInputString,ptchArg);
                *pFormat = 1;
                i = argc + 1;
                break;
            case PARAMETER_TYPE_LANG:
                g_lcID[g_nLang] = LanguageHelper(ptchArg);
                g_nLang++;
                break;

			default:
				assert(0&&"Undefined Parameter Type!");
				break;

			}
            
        }
        else
        {
            return FALSE;
        }
    }

    // If we got to here, then we made it.
    //
    return TRUE;
}

LCID LanguageHelper(char *achLang)
{
    int n;
    LCID lcid;

    n=0;
    while(achLang[n]) 
    {
        achLang[n] = _totupper(achLang[n]);
        n++;
    }

    if (0==_tcscmp(achLang,_T("JPN")))
    {
        lcid = 0x0411;
    }
    else if (0==_tcscmp(achLang,_T("CHT")))
    {
        lcid = 0x0404;
    }
    else if (0==_tcscmp(achLang,_T("CHS")))
    {
       lcid = 0x0804;
    }
    else if (0==_tcscmp(achLang,_T("KOR")))
    {
        lcid = 0x0412;
    }
    else if (0==_tcscmp(achLang,_T("ARA")))
    {
        lcid = 0x0401;
    }
    else if (0==_tcscmp(achLang,_T("HEB")))
    {
        lcid = 0x040D;
    }
    else if (0==_tcscmp(achLang,_T("THA")))
    {
        lcid = 0x041E;
    }
    else if (0==_tcscmp(achLang,_T("GER")))
    {
        lcid = 0x0407;
    }
    else
    {
        assert(0&&"Invalid LANG Enviroment Setting!");
        //lcid = LOCALE_SYSTEM_DEFAULT;
        lcid = GetSystemDefaultLCID();
    }
    return (lcid);
}


//**********Locale _code Page conversion
#define lidSerbianCyrillic 0xc1a

WORD CodePageTable[] = {
/* CodePage		  PLID	primary language
   ------------------------------------- */
	   0,		// 00 -	undefined
	1256,		// 01 - Arabic
	1251,		// 02 - Bulgarian
	1252,		// 03 - Catalan
	 950,		// 04 - Taiwan, Hong Kong (PRC and Singapore are 936)
	1250,		// 05 - Czech
	1252,		// 06 - Danish
	1252,		// 07 - German
	1253,		// 08 - Greek
	1252,		// 09 - English
	1252,		// 0a - Spanish
	1252,		// 0b - Finnish
	1252,		// 0c - French
	1255,		// 0d - Hebrew
	1250,		// 0e - Hungarian
	1252,		// 0f - Icelandic
	1252,		// 10 - Italian
	 932,		// 11 - Japan
	 949,		// 12 - Korea
	1252,		// 13 - Dutch
	1252,		// 14 - Norwegian
	1250,		// 15 - Polish
	1252,		// 16 - Portuguese
	   0,		// 17 -	Rhaeto-Romanic
	1250,		// 18 - Romanian
	1251,		// 19 - Russian
	1250,		// 1a -	Croatian
	1250,		// 1b - Slovak
	1250,		// 1c -	Albanian
	1252,		// 1d - Swedish
	 874,		// 1e - Thai
	1254,		// 1f - Turkish
	   0,		// 20 -	Urdu
	1252,		// 21 - Indonesian
	1251,		// 22 - Ukranian
	1251,		// 23 - Byelorussian
	1250,		// 24 -	Slovenian
	1257,		// 25 - Estonia
	1257,		// 26 - Latvian
	1257,		// 27 - Lithuanian
	   0,		// 28 -	undefined
	1256,		// 29 - Farsi
	   0,		// 2a -	Vietnanese
	   0,		// 2b -	undefined
	   0,		// 2c -	undefined
	1252		// 2d - Basque
				// 2e - Sorbian
				// 2f - Macedonian
				// 30 - Sutu
				// 31 - Tsonga
				// 32 - Tswana
				// 33 - Venda
				// 34 - Xhosa
				// 35 - Zulu
				// 36 - Africaans (uses 1252)
				// 38 - Faerose
				// 39 - Hindi
				// 3a - Maltese
				// 3b - Sami
				// 3c - Gaelic
				// 3e - Malaysian
};

#define nCodePageTable	(sizeof(CodePageTable)/sizeof(CodePageTable[0]))

int  ConvertLCIDtoCodePage(LCID  lcid)	
{
	UINT j = PRIMARYLANGID(lcid);			// j = primary language (PLID)

	if(j >= LANG_CROATIAN)					// PLID = 0x1a
	{
		if(lcid == lidSerbianCyrillic)		// Special case for LID = 0xc1a
			return 1251;					// Use Cyrillic code page

		if(j >= nCodePageTable)				// Africans PLID = 0x36, which
			return j == 0x36 ? 1252 : CP_ACP;	//  is outside table
	}

	j = CodePageTable[j];					// Translate PLID to code page

	if(j != 950 || (lcid & 0x400))			// All but Singapore and PRC
		return j;

	return 936;								// Singapore and PRC
}


BOOL SendDataToClipBoard()
{
    int n;
    char achValue[127];

    n = GetEnvironmentVariable("ClipBoard",achValue,128);
    if (!n)
        return TRUE;
    if (!_stricmp(achValue,"off"))
        return FALSE;
    return TRUE;
}