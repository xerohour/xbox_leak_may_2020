

USHORT MakeChars(BYTE bByte);
BOOL MakeWStringOfBytes(BYTE * pInBuff,int iInLen, WCHAR * pOutBuff, int iOutLen);
BOOL MakeStringOfBytes(BYTE * pInBuff,int iInLen, char * pOutBuff, int iOutLen);
BOOL MakeBytesOfString(char * pszString,BYTE * pOutBuff,int iOutLen);
BOOL UnspaceString(char * pszInString, char * pOutBuffer, int iOutLen);
int BstrToBinary(BSTR * pBstr, BYTE * pBuffer, int iBuffLen);
