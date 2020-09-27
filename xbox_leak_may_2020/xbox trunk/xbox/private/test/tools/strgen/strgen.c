/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    strgen.c

Abstract:

    Routine to generate random string

Environment:

    Xbox

Revision History:

--*/


#include <xtl.h>
#include "strgen.h"


#define MAXRISKCHAR 128


//
// Risky characters: index by primary language ID
//
unsigned char SingleByteRiskChar[][MAXRISKCHAR+1] = {
                // Primary language
    {0},        // 00 - undefined
    {0},        // 01 - Arabic
    {0},        // 02 - Bulgarian
    {0},        // 03 - Catalan
    {0},        // 04 - Taiwan, Hong Kong (PRC and Singapore are 936)
    {0},        // 05 - Czech
    {0},        // 06 - Danish
    {0xE4,0xF6,0xFC,0xC4,0xD6,0xDC,0xDF,0x00 },     // 07 - German
    {0},        // 08 - Greek
    {0},        // 09 - English
    {0},        // 0a - Spanish
    {0},        // 0b - Finnish
    {0},        // 0c - French
    {0},        // 0d - Hebrew
    {0},        // 0e - Hungarian
    {0},        // 0f - Icelandic
    {0},        // 10 - Italian
    {0},        // 11 - Japan
    {0},        // 12 - Korea
    {0},        // 13 - Dutch
    {0},        // 14 - Norwegian
    {0},        // 15 - Polish
    {0},        // 16 - Portuguese
    {0},        // 17 - Rhaeto-Romanic
    {0},        // 18 - Romanian
    {0},        // 19 - Russian
    {0},        // 1a - Croatian
    {0},        // 1b - Slovak
    {0},        // 1c - Albanian
    {0},        // 1d - Swedish
    {0},        // 1e - Thai
    {0},        // 1f - Turkish
    {0},        // 20 - Urdu
    {0},        // 21 - Indonesian
    {0},        // 22 - Ukranian
    {0},        // 23 - Byelorussian
    {0},        // 24 - Slovenian
    {0},        // 25 - Estonia
    {0},        // 26 - Latvian
    {0},        // 27 - Lithuanian
    {0},        // 28 - undefined
    {0},        // 29 - Farsi
    {0},        // 2a - Vietnanese
    {0},        // 2b - undefined
    {0},        // 2c - undefined
    {0}         // 2d - Basque
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


//
// This is Japanese/FE risky chars, Need to add locale specfic code
//
unsigned char riskHi[20] = {
    0x81,0x82,0x9F,0xFC,0xE5,0x00

};
unsigned char riskLo[20] = {
    0x5B,0x5C,0x5D,0x7C,0xA1,0xAE,0xBF,0xDF,0xE0,0x40,0xFC,0x00
};

unsigned char InvalidChar[50] = {
    0x20,
    '\\',
    '/',
    ':',
    '<',
    '>',
    '|',
    '.',
    '_',
    '*',
    '+',
    ',',
    '?',
    '"',
    0x00
};


unsigned char OneAsscii(void);
unsigned char HalfDBSC(CPINFO ,char *,BOOL bIsLeadingByte);
BOOL          IsValid(unsigned char *,LCID);
void          EnsureValid(CPINFO,CPINFO,char *,char * ,BOOL , unsigned char *,LCID);
BOOL          IsMET(LCID);
int           StrGenMET(LCID ,int,int,int,unsigned int ,int,int,void **);
int           StrGenOther(LCID ,int,int,int,unsigned int ,int,int,void **);
BOOL          IsFE(LCID);
BOOL          IsValidAnsiString(char *, LCID );
int           THEXVAL(char);
int           StrtoInt(BYTE  , char * );
BOOL          IsValidAnsiString(char *, LCID );
void GenerateFEOneRiskChar(char *,WORD *,LCID,int);
void GerenareOneEuroRisk(char *,WORD *,LCID,int);
BOOL IsValidNETBiosChar(char);


int strgen(LCID lcid,           //Locale ID
           int nStyle,          //0 for ansi 
                                //1 for DBCS  or locale char eg in MET
                                //2 for Mixed
           int length,          //the lenght of string 
                                //0 for random MAX Limit is 255,
                                //-1  for random MAX Limit is 65535,
           int nFormat,         //0: ANSI 1:UNICODE
           unsigned int nSeed,
           int nPresudo,
           int nRisk,
           void **pStr
        )
{
    unsigned char  *achString;
    WCHAR           *awString;
    CPINFO          cpInfoLo,cpInfoHi;
    int i,nNumofChar=0;
    char   *pRiskHi,*pRiskLo;

    
    
        
    if (LOCALE_SYSTEM_DEFAULT == lcid)
        lcid = GetSystemDefaultLCID();
    else if (LOCALE_USER_DEFAULT == lcid)
        lcid = GetUserDefaultLCID();
    if (!IsValidLocale(lcid, LCID_INSTALLED))
    {
       assert(0 && " Not Correct Locale ");
       lcid = GetSystemDefaultLCID();
    }

    srand(nSeed);

    if (IsMET(lcid))
    {
        return StrGenMET(lcid,nStyle,length,nFormat,nSeed,nPresudo,nRisk,pStr);         
    }
    else
    if (!IsFE(lcid))
    {
        return StrGenOther(lcid,nStyle,length,nFormat,nSeed,nPresudo,nRisk,pStr);
    }
#ifdef NEVER
    if (lcid != GetSystemDefaultLCID())//we got the locale from LANG enviroment
    {//This usually means in US NT ,we generate Japanese string
        switch (lcid)
        {
            case 0x0411://Japanese
                cpInfoHi.LeadByte[0] =0x81;
                cpInfoHi.LeadByte[1] =0x9F;
                cpInfoHi.LeadByte[2] =0xE0;
                cpInfoHi.LeadByte[3] =0xFC;
                cpInfoHi.LeadByte[4] =0x00;
                cpInfoHi.LeadByte[5] =0x00;
                break;
            case 0x0804://Simplified  Chinese
                cpInfoHi.LeadByte[0] =0xA1;
                cpInfoHi.LeadByte[1] =0xFE;
                cpInfoHi.LeadByte[2] =0x00;
                cpInfoHi.LeadByte[3] =0x00;
                break;
            case 0x0412://Korea0x0404
                cpInfoHi.LeadByte[0] =0x84;
                cpInfoHi.LeadByte[1] =0xD3;
                cpInfoHi.LeadByte[2] =0xD8;
                cpInfoHi.LeadByte[3] =0xD8;
                cpInfoHi.LeadByte[4] =0xD9;
                cpInfoHi.LeadByte[5] =0xDE;
                cpInfoHi.LeadByte[6] =0xE0;
                cpInfoHi.LeadByte[7] =0xF9;
                cpInfoHi.LeadByte[8] =0x00;
                cpInfoHi.LeadByte[9] =0x00;
                break;
            case 0x0404://Traditional Chinese
                cpInfoHi.LeadByte[0] =0x81;
                cpInfoHi.LeadByte[1] =0xFE;
                cpInfoHi.LeadByte[2] =0x00;
                cpInfoHi.LeadByte[3] =0x00;
                break;
            default:
                assert(0 && "Invalid Enviroment LANG setting");
                nStyle = 0;//non FE NT Generate Ansi string
                break;
        }
        nFormat = 0;//We can only generate ANSI here , since in US NT, there is 
                    //no way to convert our internally ANSI string to Unicode string
    }
    else
#endif
    {
        GetCPInfo(ConvertLCIDtoCodePage(lcid),&cpInfoHi);//Get the Hi byte range    
    }

    if (nRisk)
    {
        pRiskHi = riskHi;
        pRiskLo = riskLo;
    }
    else
    {
        pRiskHi = NULL;
        pRiskLo = NULL;
    }

    //Set the Lobyte range
    //Copy from Developing International Software p65 except S Chinese using GBK range
    switch (lcid)
    {
        case 0x0411://Japanese
            cpInfoLo.LeadByte[0] =0x40;
            cpInfoLo.LeadByte[1] =0xFC;
            cpInfoLo.LeadByte[2] =0x00;
            cpInfoLo.LeadByte[3] =0x00;
            break;
        case 0x0804://Simplified  Chinese
            cpInfoLo.LeadByte[0] =0x40;
            cpInfoLo.LeadByte[1] =0xFE;
            cpInfoLo.LeadByte[2] =0x00;
            cpInfoLo.LeadByte[3] =0x00;
            break;
        case 0x0412://Korea0x0404
            cpInfoLo.LeadByte[0] =0x41;
            cpInfoLo.LeadByte[1] =0x5A;
            cpInfoLo.LeadByte[2] =0x61;
            cpInfoLo.LeadByte[3] =0x7A;
            cpInfoLo.LeadByte[4] =0x81;
            cpInfoLo.LeadByte[5] =0xFE;
            cpInfoLo.LeadByte[6] =0x00;
            cpInfoLo.LeadByte[7] =0x00;
            break;
        case 0x0404://Traditional Chinese
            cpInfoLo.LeadByte[0] =0x40;
            cpInfoLo.LeadByte[1] =0x7E;
            cpInfoLo.LeadByte[2] =0xA1;
            cpInfoLo.LeadByte[3] =0xFE;
            cpInfoLo.LeadByte[4] =0x00;
            cpInfoLo.LeadByte[5] =0x00;
            break;
        default:
            nStyle = 0;//non FE NT Generate Ansi string
            break;
    }
    if (-1 == length)
        length = rand() % 65535 +1;
    else if (0 == length)
        length = rand() % 255 +1 ;
    else if (length<0)
        length = length = rand() % 255 +1 ;
    
    
    achString = (char*) malloc(length+1);
    if (!achString)
        return 2;
    if ( (nStyle <0) || (nStyle >2) )
    {
        assert (0 && "Unknown Style");
        nStyle = 0;
    }


    

    if (0==nStyle)//Pure Asscii
    {
        for (i=0;i<length;i++)
        {
            achString[i]=OneAsscii();
        }
        achString[length]='\0';
        nNumofChar = length;
    }
    else if (1==nStyle)//Pure DBCS
    {
        if (1==length) length++;
        
        for (i=0;i<length;i+=2)
        {
            char achTmp[3];

            EnsureValid(cpInfoHi,cpInfoLo,riskHi,riskLo,!nPresudo, achTmp,lcid);
            
            achString[i]
                = achTmp[0];
            achString[i+1]
                = achTmp[1];
        }
        achString[(length/2) * 2 ]='\0';
        nNumofChar = length / 2;
    }
    else //DBCS and ASCII Mixed
    {
        i=0;
        if (1==length) length++;
        while (i<length)
        {
            nNumofChar ++;
            if ( (rand() %2) && (i+2) <=length) 
            {
                char achTmp[3];
                
                EnsureValid(cpInfoHi,cpInfoLo,riskHi,riskLo,!nPresudo, achTmp,lcid);
                
                achString[i]  = (unsigned char) achTmp[0];
                achString[i+1] = (unsigned char)achTmp[1];
                i+=2;
            }
            else
            {
                achString[i]=OneAsscii();
                i++;
            }
            
        }
        achString[length] = '\0';
    }
    

    //Now achString Contains the Result string in ANSI CodePage
    if ( (0 !=nFormat) && (1 !=nFormat) )
    {
        assert(0 && "unsupported format");
        nFormat = 0;
    }
    if (0 ==nFormat)
    {
        *pStr = achString;
    }
    else
    {
        awString = (WCHAR *) malloc((nNumofChar+1) * sizeof(WCHAR));
        MultiByteToWideChar(ConvertLCIDtoCodePage(lcid),
                        0,
                        achString,
                        -1,
                        awString,
                        nNumofChar+1);
        *pStr = awString;
        free(achString);
    }

    return 0;
}

unsigned char OneAsscii(void)
{
    unsigned char uc ;

    if  (0x0411 == GetSystemDefaultLCID())
    {
        uc = rand() % (0x7F+(0xDF-0xA1+1)-32) + 32;
        if (uc>0x7F)
            uc = uc +(0xa1-0x7f-1);
    }
    else uc =rand() % (128-32)+32; 
    return (uc);
}


unsigned char HalfDBSC(CPINFO cpInfo,char *pRisk,BOOL bIsLeadingByte)
{
    unsigned char uc;
    unsigned int nNum = 0;
    int i = 0;
    unsigned int nNumDue;

    
    if (pRisk)
    {
        if (rand() % 2)
        {
            int n = strlen(pRisk);
            return (pRisk[rand() % n]);
        }
    }//otherwise fall through

    //Calculate the Total Number of Possibilities for the current range .
    while (cpInfo.LeadByte[i])
    {
        nNum = nNum + (cpInfo.LeadByte[i+1] - cpInfo.LeadByte[i] +1);
        i+=2;
    }
    //Generate a random number with this range
    uc =  rand() % nNum;

    //Map the uc to real value
    i=0;
    nNumDue = cpInfo.LeadByte[1] - cpInfo.LeadByte[0];
    
    while (uc > nNumDue )
    {
        i+=2;
        nNumDue = nNumDue + cpInfo.LeadByte[i+1] - cpInfo.LeadByte[i];
    }
    if (!bIsLeadingByte)
    {
        if (0x7f == cpInfo.LeadByte[i]+uc-(nNumDue-(cpInfo.LeadByte[i+1]-cpInfo.LeadByte[i])))
        {
            return (0x7E);
        }
        
    }
    return (cpInfo.LeadByte[i]+uc-(nNumDue-(cpInfo.LeadByte[i+1]-cpInfo.LeadByte[i])));
}



void  EnsureValid(CPINFO cpInfoHi,CPINFO cpInfoLo,char *pRiskHi,char *pRiskLo,BOOL bEnsure, unsigned char *achResult,LCID lcid)
{
    BOOL bNotFinish=1;
    unsigned char achTmp[3];

    achTmp[0] = HalfDBSC(cpInfoHi,pRiskHi,1);
    achTmp[1] = HalfDBSC(cpInfoLo,pRiskLo,0);
    achTmp[2] = '\0';
    
    if (!bEnsure)
    {
        achResult[0] = achTmp[0];
        achResult[1] = achTmp[1];
        return ;
    }

    while (!IsValid(achTmp,lcid))
    {
        achTmp[0] = HalfDBSC(cpInfoHi,pRiskHi,1);
        achTmp[1] = HalfDBSC(cpInfoLo,pRiskLo,0);
        achTmp[2] ='\0';
    }

    achResult[0] = achTmp[0];
    achResult[1] = achTmp[1];
    return ;
}

BOOL IsValid(unsigned char *sour,LCID lcid)
{
    unsigned char achResult[16];
    WCHAR         wResult[16];
    int a;
    
    a = MultiByteToWideChar(ConvertLCIDtoCodePage(lcid),
                        0,
                        sour,
                        -1,
                        wResult,
                        16);
    if (!a)
        return (FALSE);

    a = WideCharToMultiByte(
        ConvertLCIDtoCodePage(lcid),
        0,
        wResult,
        -1,
        achResult,
        16,
        NULL,
        NULL );

    if (!a)
        return (FALSE);

    if ( strcmp(sour,achResult))
    {
        return (FALSE);

    }
    return (TRUE);
}

int StrGenMET(LCID lcid,           //Locale ID               
               int nStyle,          //0 for ansi 
                                    //1 for DBCS and 2 for Mixed
               int length,          //the lenght of string 
                                //0 for random MAX Limit is 255,
                                //-1  for random MAX Limit is 65535,
               int nFormat,         //0: ANSI 1:UNICODE
               unsigned int nSeed,
               int nPresudo,
               int nRisk,
               void **pStr
                )
{
    unsigned char  *achString;
    WCHAR           *awString;
    WORD UniocdeRangeUpper,UniocdeRangeLower;
    int i,nNumofChar=0;


    if (-1 == length)
        length = rand() % 65535 +1;
    else if (0 == length)
        length = rand() % 255 +1 ;
    else if (length<0)
        length = length = rand() % 255 +1 ;

    if ( (0 !=nFormat) && (1 !=nFormat) )
    {
        assert(0 && "unsupported format");
        nFormat = 0;
    }
    
    if (nFormat)
    {
        awString = (WCHAR *) malloc(sizeof(WCHAR)*(length+1));
        if (!awString)
            return 2;
        if  (LANG_ARABIC == LOBYTE(lcid)) 
        {
            UniocdeRangeUpper=0x06FF;
            UniocdeRangeLower=0x0600;
        }
        else if (0x040D == lcid)  //Hebrew
        {
            UniocdeRangeUpper=0x05FF;
            UniocdeRangeLower=0x0590;
        }
        else
        {
            UniocdeRangeUpper = 0x0E7F;
            UniocdeRangeLower = 0x0E00;
        }

    }
    else
    {
        achString = (char*) malloc(length+1);
        if (!achString)
            return 2;
    }
    
    if ( (nStyle <0) || (nStyle >2) )
    {
        assert (0 && "Unknown Style");
        nStyle = 0;
    }


    if (0==nStyle)//Pure Asscii
    {
        for (i=0;i<length;i++)
        {
            if (nFormat)
                awString[i] = rand() % (128-32)+32; 
            else
                achString[i] = rand() % (128-32)+32; 
        }
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';

        nNumofChar = length;
    }
    else if (1==nStyle)//Pure locale string
    {
        if (1==length) length++;
        
        for (i=0;i<length;i++)
        {
            if (nFormat)
                awString[i] = rand() % (UniocdeRangeUpper-UniocdeRangeLower+1)+UniocdeRangeLower; 
            else
                achString[i] = rand() % (0xFF-0x80 +1 )+0x80;            
        }
        
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';
        nNumofChar = length ;
    }
    else //locale char  and ASCII Mixed
    {
        i=0;
        if (1==length) length++;
        while (i<length)
        {
            if (rand() %2)
            {
                if (nFormat)
                    awString[i] = rand() % (128-32)+32; 
                else
                    achString[i] = rand() % (128-32)+32; 
            }
            else
            {
                if (nFormat)
                    awString[i] = rand() % (UniocdeRangeUpper-UniocdeRangeLower+1)+UniocdeRangeLower; 
                else
                    achString[i] = rand() % (0xFF-0x80 +1 )+0x80;            
            }
            i++;
        }
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';
        nNumofChar = length ;
    } 

    if (0 ==nFormat)
    {
        *pStr = achString;
    }
    else
    {       
        *pStr = awString;
    }

    return 0;
}

BOOL IsMET(LCID lcid)  //return TRUE if it is ARA HEB and THA
{
        if ( (LANG_ARABIC == LOBYTE(lcid)) //Arabic, since too many Arbic locale
            || (0x040D == lcid)  //Hebrew
            || (0x041E == lcid))  //Thai
            return TRUE;
        return FALSE;
            
}


BOOL IsFE(LCID lcid)  //return TRUE if it is CJK
{
    //CJK
        if ( (LANG_CHINESE == LOBYTE(lcid)) //Chinese
            || (LANG_JAPANESE == LOBYTE(lcid))  //japanese
            || (LANG_KOREAN == LOBYTE(lcid)))  //Korean
            return TRUE;
        return FALSE;
            
}




int StrGenOther(LCID lcid,           //Locale ID               
               int nStyle,          //0 for ansi 
                                    //1 for DBCS and 2 for Mixed
               int length,          //the lenght of string 
                                //0 for random MAX Limit is 255,
                                //-1  for random MAX Limit is 65535,
               int nFormat,         //0: ANSI 1:UNICODE
               unsigned int nSeed,
               int nPresudo,
               int nRisk,
               void **pStr
                )
{
    unsigned char  *achString;
    WCHAR           *awString;
    WORD UniocdeRangeUpper,UniocdeRangeLower;
    int i,nNumofChar=0;


    if (-1 == length)
        length = rand() % 65535 +1;
    else if (0 == length)
        length = rand() % 255 +1 ;
    else if (length<0)
        length = length = rand() % 255 +1 ;

    if ( (0 !=nFormat) && (1 !=nFormat) )
    {
        assert(0 && "unsupported format");
        nFormat = 0;
    }
    
    if (nFormat)
    {
        awString = (WCHAR *) malloc(sizeof(WCHAR)*(length+1));
        if (!awString)
            return 2;
        if  (LANG_GERMAN == LOBYTE(lcid)) 
        {
            UniocdeRangeUpper=0x0000;
            UniocdeRangeLower=0x00FF;
        }
        else
        {
            return 2;
        }

    }
    else
    {
        achString = (char*) malloc(length+1);
        if (!achString)
            return 2;
    }
    
    if ( (nStyle <0) || (nStyle >2) )
    {
        assert (0 && "Unknown Style");
        nStyle = 0;
    }


    if (0==nStyle)//Pure Asscii
    {
        for (i=0;i<length;i++)
        {
            if (nFormat)
                awString[i] = rand() % (128-32)+32; 
            else
                achString[i] = rand() % (128-32)+32; 
        }
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';

        nNumofChar = length;
    }
    else if (1==nStyle)//Pure locale string
    {
        if (1==length) length++;
        
        for (i=0;i<length;i++)
        {
            if (nFormat)
                awString[i] = rand() % (UniocdeRangeUpper-UniocdeRangeLower+1)+UniocdeRangeLower; 
            else
                achString[i] = rand() % (0xFF-0x80 +1 )+0x80;            
        }
        
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';
        nNumofChar = length ;
    }
    else //locale char  and ASCII Mixed
    {
        i=0;
        if (1==length) length++;
        while (i<length)
        {
            if (rand() %2)
            {
                if (nFormat)
                    awString[i] = rand() % (128-32)+32; 
                else
                    achString[i] = rand() % (128-32)+32; 
            }
            else
            {
                if (nFormat)
                    awString[i] = rand() % (UniocdeRangeUpper-UniocdeRangeLower+1)+UniocdeRangeLower; 
                else
                    achString[i] = rand() % (0xFF-0x80 +1 )+0x80;            
            }
            i++;
        }
        if (nFormat)
            awString[length]=0;
        else
            achString[length]='\0';
        nNumofChar = length ;
    } 

    if (0 ==nFormat)
    {
        *pStr = achString;
    }
    else
    {       
        *pStr = awString;
    }

    return 0;
}

int PassString(char *pString,int fUnicode,void **pStr,BOOL *pfValid,LCID lcid)
{
    int nLen;
    BYTE *pByte,HiByte,LoByte;
    char * pTmpString;
    int n,n1;
    char achOneChar[10];
    char ach;
    WORD *pWord;

    nLen = strlen(pString);
    pTmpString = malloc ( (nLen +4) * sizeof(BYTE) );
    if (!pTmpString)
        return FALSE;

    if (fUnicode)
    {
        //If it's Unicode, the only invalide case is odd byte lenth

        if (nLen % 4)
            *pfValid = 0;
        else
        {
            *pfValid = 1;        
        }
        n1=0;
        n = 0;
        pWord = (WORD*) pTmpString;

        while (n<nLen-nLen % 4)
        {
            strncpy(achOneChar,&(pString[n]),4);
            achOneChar[4]=0;
            pWord[n1] =  (WCHAR) StrtoInt(16 , achOneChar);
            n+=4;
            n1++;
        }
        // the last nLen %4 got clipped 
        // BUGBUG do we need to generate such invalid unciode string anyway ???
        pWord[n1]=0;
    }
    else// it's ANSI
    {
        if (!IsFE(lcid))
        {
            *pfValid = 1;            
        }
        else
        {
            *pfValid  = IsValidAnsiString(pString,lcid);
        }
        if (nLen % 2 )
        {
            nLen--;
        }

        n1=0;
        n=0;
        while (n<nLen)
        {
            strncpy(achOneChar,&(pString[n]),2);
            achOneChar[2]=0;            

            HiByte = (BYTE)StrtoInt(16 , achOneChar);
            pTmpString[n1] = HiByte;
            n1++;
            n+=2;
            if (IsDBCSLeadByteEx(ConvertLCIDtoCodePage(lcid),HiByte))
            {
                if (n+2>nLen)
                {
                    n1--;
                    pTmpString[n1] = 0;
                    break;
                }
                else
                {
                    strncpy(achOneChar,&(pString[n]),2);
                    achOneChar[2]=0;            
                    pTmpString[n1] =  (CHAR)StrtoInt(16,achOneChar);
                    
                    n1++;
                    n+=2;
                }
            }
        }
        pTmpString[n1] = 0;
    }
    
    *pStr = (void*)pTmpString;
    return TRUE;
}

BOOL IsValidAnsiString(char *pString, LCID lcid)
{
    int nLen,n;
    char achOneChar[3];
    char ach;

    if (!IsFE(lcid))
        return TRUE;

    nLen = strlen(pString);
    if (nLen % 2) 
        return FALSE;

    n=0;

    while (n < nLen )
    {
        strncpy(achOneChar,&(pString[n]),2);
        achOneChar[2]=0;
        ach =  (char)StrtoInt(16,achOneChar);

        if (IsDBCSLeadByteEx(ConvertLCIDtoCodePage(lcid),ach))
        {
            //BUGBUG Here do we need to take care trailing byte
            n+=4;
        }
        else
        {
            n+=2;
        }
    }
    if (n==nLen)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int   StrtoInt(BYTE bBase , char * pHexString)
{
    char *pStr = pHexString;
    int  nSum = 0;

    while (*pStr)
    {            
        nSum = nSum*bBase + THEXVAL(*pStr);
        pStr++;
    }
    return (nSum);
}

int THEXVAL(char tch)
{
    int val ;

    if  (tch>='a') 
        val = tch -'a' + 10;
    else if (tch>='A')
        val = tch -'A' + 10;
    else val = tch -'0';

    return (val);
}

void GenerateFEOneRiskChar(char *pchRisk,WORD *pwUnicode,LCID lcid,int nStyle)
{
    CPINFO cpInfoHi , cpInfoLo;
    WCHAR wTmp[10];
    int a;

    GetCPInfo(ConvertLCIDtoCodePage(lcid),&cpInfoHi);//Get the Hi byte range    

    cpInfoLo.LeadByte[0] =0x40;
    cpInfoLo.LeadByte[1] =0xFC;
    cpInfoLo.LeadByte[2] =0x00;
    cpInfoLo.LeadByte[3] =0x00;

    if (rand() %2 == 1 )
    {
        pchRisk[0] = riskHi[rand() % strlen(riskHi)];
        if (SG_STYLE_NETBIOS == nStyle)
        {
            while (!IsValidNETBiosChar(pchRisk[0]))
            {
                pchRisk[0] = riskHi[rand() % strlen(riskHi)];
            }
        }
        pchRisk[1] = HalfDBSC(cpInfoLo,NULL,0);
        if (SG_STYLE_NETBIOS == nStyle)
        {
            while (!IsValidNETBiosChar(pchRisk[1]))
            {
                pchRisk[1] = HalfDBSC(cpInfoLo,NULL,0);
            }
        }
    }
    else
    {
        pchRisk[0] = HalfDBSC(cpInfoHi,NULL,1);
        if (SG_STYLE_NETBIOS == nStyle)
        {
            while (!IsValidNETBiosChar(pchRisk[0]))
            {
                pchRisk[0] = HalfDBSC(cpInfoHi,NULL,0);
            }
        }
        pchRisk[1] = riskLo[rand() % strlen(riskLo)];
        if (SG_STYLE_NETBIOS == nStyle)
        {
            while (!IsValidNETBiosChar(pchRisk[1]))
            {
                pchRisk[1] = riskLo[rand() % strlen(riskLo)];
            }
        }
    }
    pchRisk[2]=0;

    *pwUnicode = 0xFFFF;

    
    a = MultiByteToWideChar(ConvertLCIDtoCodePage(lcid),
                            0,
                            pchRisk,
                            -1,
                            wTmp,
                            10);
    if (!a)
        return;

    *pwUnicode = (WORD) wTmp[0];
    return ;
}

void GerenareOneEuroRisk(char *pchRisk,WORD *pwUnicode,LCID lcid,int nStyle)
{
    WCHAR wTmp[10];
    int a;
    unsigned char achRisk[MAXRISKCHAR+1];

    
    UINT j;
    
    j = PRIMARYLANGID(lcid);            // j = primary language (PLID)

    if (SingleByteRiskChar[j][0])
    {
        strcpy(achRisk,SingleByteRiskChar[j]);
    }
    else
    {
        unsigned char  i;
        for (i=0;i<0x80;i++)
        {
            achRisk[i]=i+0x80;
        }
        achRisk[0x80]=0;

    }

    pchRisk[0] = achRisk[rand() % strlen(achRisk)];
    if (SG_STYLE_NETBIOS == nStyle)
    {
        while (!IsValidNETBiosChar(pchRisk[0]))
        {
            pchRisk[0] = achRisk[rand() % strlen(achRisk)];
        }
    }
    pchRisk[1]=0;

    *pwUnicode = 0xFFFF;
    
    
    a = MultiByteToWideChar(ConvertLCIDtoCodePage(lcid),
                            0,
                            pchRisk,
                            -1,
                            wTmp,
                            10);
    if (!a)
        return;

    *pwUnicode = (WORD) wTmp[0];
    return ;
}

void  GenerateFERiskString(char **pchRisk,WORD **pwUnicode,int nNumofChar,LCID lcid,int nStyle)
{
    int i;

    if (nNumofChar<=0)
        nNumofChar=1;

    *pchRisk = malloc((nNumofChar+1)*2*sizeof(char));
    if (!*pchRisk)
    {
        goto Error;
    }
    *pwUnicode = malloc((nNumofChar+1)*sizeof(WORD));
    if (!*pchRisk)
    {
        goto Error;
    }
    for (i=0;i<2*nNumofChar;i=i+2)
    {
        char achOne[3];
        WORD wUni;
        GenerateFEOneRiskChar(achOne,&wUni,lcid,nStyle);
        (*pchRisk)[i] = achOne[0];
        (*pchRisk)[i+1] = achOne[1];
        (*pwUnicode)[i / 2 ]=wUni;
    }
    (*pchRisk)[2*nNumofChar]=0;
    (*pwUnicode)[nNumofChar ]=0;
    return;

Error:
     if (!*pchRisk)
     {
         free( *pchRisk);
          *pchRisk = NULL;
     }
}

void GenerateEuroRiskString(char **pchRisk,WORD **pwUnicode,int nNumofChar,LCID lcid,int nStyle)
{
    int i;

    if (nNumofChar<=0)
        nNumofChar=1;

    *pchRisk = malloc((nNumofChar+1)*sizeof(char));
    if (!*pchRisk)
    {
        goto Error;
    }
    *pwUnicode = malloc((nNumofChar+1)*sizeof(WORD));
    if (!*pchRisk)
    {
        goto Error;
    }
    
    for (i=0;i<nNumofChar;i++)
    {
        char achOne[2];
        WORD wUni;
        GerenareOneEuroRisk(achOne,&wUni,lcid,nStyle);
        (*pchRisk)[i] = achOne[0];
        (*pwUnicode)[i]=wUni;
    }
    (*pchRisk)[nNumofChar]=0;
    (*pwUnicode)[nNumofChar]=0;
    return;

Error:
     if (!*pchRisk)
     {
         free( *pchRisk);
          *pchRisk = NULL;
     }
}


void GenerateMixRiskString(LCID *plcidArray,//LCID Array, Ended with 0x00
                          WCHAR **pWStr, //String generated , always in unciode
                          int nNumofChar,//number of char
                          int nStyle) //Whether exclude those not valid for netbios name 
{
    int i;
    int nNumofLCID;

    if (nNumofChar<=0)
        nNumofChar=1;

    nNumofLCID = 0;
    while (plcidArray[nNumofLCID]) nNumofLCID++;

    *pWStr = malloc((nNumofChar+1)*sizeof(WCHAR));
    if (!*pWStr)
    {
        return;
    }

    for (i=0;i<nNumofChar;i++)
    {
        char achOne[3];
        WCHAR wUni;

        if ( ISFELocale(plcidArray[i % nNumofLCID]) )
        {
            GenerateFEOneRiskChar(achOne,&wUni,plcidArray[i % nNumofLCID],nStyle);
        }
        else
        {
            GerenareOneEuroRisk(achOne,&wUni,plcidArray[i % nNumofLCID],nStyle);
        }
        (*pWStr)[i] = wUni;            
    }
    (*pWStr)[i] = 0;  
    return;

}


BOOL IsValidNETBiosChar(char ch)
{
    return (!strchr(InvalidChar,ch));
}


BOOL ISFELocale(LCID lcid)
{
    UINT j = PRIMARYLANGID(lcid);           // j = primary language (PLID)

    if ( (LANG_CHINESE == j)
          || (LANG_JAPANESE == j)
          || (LANG_KOREAN == j) )
          return TRUE ;
    else  return FALSE;
}