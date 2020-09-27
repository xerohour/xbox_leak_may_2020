
#include "stdafx.h"
#include "utils.h"
#include "AnsiBstr.h"

// input BStr from COM interface representing binary string, e.g.: 00 11 AB
// convert from BStr to char *
// remove any embedded space
// convert from chars to binary

int BstrToBinary(BSTR * pBstr, BYTE * pBuffer, int iBuffLen)
{
		// first convert from bstr to char 
	PCHAR pcTemp = CreateAnsiFromBstr(*pBstr);
	// next remove spaces
	char * pNewStr = (char *) malloc(strlen(pcTemp)+2);
	if(NULL == pNewStr)
	{
		return 0;
	}
	// remove any unwanted formatting spaces
	BOOL bResult = UnspaceString(pcTemp,pNewStr,strlen(pcTemp)+1);
	if(FALSE == bResult)
	{
		return 0;
	}
	LocalFree(pcTemp); // release string allocated by CreateAnsiFromBstr
	// finally convert from string to binary
	bResult = MakeBytesOfString(pNewStr,pBuffer,iBuffLen);
	if(FALSE == bResult)
	{
		return 0;
	}
	int iBytes = strlen(pNewStr)/2;
	free(pNewStr);
	return iBytes;
}

// convert an input byte, e.g. 0x1a to characters e.g. "1a"
USHORT MakeChars(BYTE bByte)
{
	USHORT usResult;
	BYTE bLNibble,bHNibble;

	bLNibble =  (UCHAR) (bByte & 0x0f);
	bHNibble = (UCHAR) (bByte >> 4);
	if(bHNibble < 10)
	{
		usResult = (USHORT) (bHNibble + 0x30);
	} else
	{
		usResult = (USHORT) (bHNibble + 0x41 - 0x0a);
	}
	usResult <<= 8;
	if(bLNibble < 10)
	{
		usResult = (USHORT) (usResult +  (bLNibble + 0x30));
	} else
	{
		usResult = (USHORT) (usResult +  (bLNibble + 0x41 - 0x0a));
	}
	return usResult;
}

// input buffer and len
// output pszWide String representation of Len bytes from the buffer, e.g. L"0012ab"

BOOL MakeWStringOfBytes(BYTE * pInBuff,int iInLen, WCHAR * pOutBuff, int iOutLen)
{
	USHORT	usByteChars;
	USHORT * pTemp = (USHORT *) pOutBuff; // going to move two chars at a time
	if(iOutLen < (iInLen * 4) + 2) return FALSE;
	for(int i = 0; i < iInLen; i++)
	{
		// get byte
		usByteChars = MakeChars(pInBuff[i]);

		*pTemp++ =(WCHAR) (usByteChars >> 8); // upper character
		*pTemp++ = (WCHAR) (usByteChars & 0xff); // lower character

	}
	pTemp[i] = 0x00; // terminate string
	return TRUE;
}


// input buffer and len
// output pszString representation of Len bytes from the buffer, e.g. "0012ab"
BOOL MakeStringOfBytes(BYTE * pInBuff,int iInLen, char * pOutBuff, int iOutLen)
{
	USHORT	usByteChars;
	char * pTemp =  pOutBuff; // going to move two chars at a time
	if(iOutLen < (iInLen * 4) + 2) return FALSE;
	for(int i = 0; i < iInLen; i++)
	{
		// get byte
		usByteChars = MakeChars(pInBuff[i]);

		*pTemp++ = (char) (usByteChars >> 8); // upper character
		*pTemp++ = (char) (usByteChars & 0xff); // lower character

	}
	*pTemp = 0x00; // terminate string
	return TRUE;
}

// remove spaces from string

BOOL UnspaceString(char * pszInString, char * pOutBuffer, int iOutLen)
{
	char * pInput = pszInString;
	char * pOutput = pOutBuffer;
	int iLen = strlen(pszInString);
	if(iOutLen < iLen+1)
	{
		return FALSE;
	}
	for(int i = 0; i < iLen; i++)
	{
		if(*pInput != 0x20)
		{
			*pOutput++ = *pInput;
		}
		pInput++;
	}
	*pOutput = 0x00;
	return TRUE;
}

// input pszString of binary hex, e.g. "0123ABCD"
// output binary hex, e.g. 0x0123abcd

BOOL MakeBytesOfString(char * pszString,BYTE * pOutBuff,int iOutLen)
{
	CHAR bTemp;
	BYTE bVal;
	BYTE bCurValue;
	BOOL fFlag = FALSE;
	int iLen = strlen(pszString);
	if(iOutLen < iLen/2 || iOutLen % 2 )
	{
		return 0;
	}

	for(int i = 0; i < iLen; i++)
	{
		bTemp = pszString[i];
		if(bTemp > 0x2f && bTemp < 0x3A)
		{   // decimal number
			bVal = bTemp - 0x30;
		} else
		{
			bTemp &= 0xDF; // fix any case problems
			if(bTemp > 0x40 && bTemp < 0x47) // a through f
			{
				bVal = bTemp - 0x37;
			} else
			{
				return 0; // not in numeric range
			}
		}
		if(FALSE == fFlag)
		{
			fFlag = TRUE;
			bCurValue = bVal << 4;
		} else
		{
			fFlag = FALSE;
			bCurValue |= bVal;
			pOutBuff[i/2] = bCurValue;
		}

	}
	return TRUE;
}


/// utility support for linked list of insertions and removal tracking






