
#ifndef _XBOX
#include "stdafx.h"
#endif

#include "..\inc\cipher.h"


void lfsr(ULONGLONG seed, PVOID Contents, ULONG Length)
{
	static ULONGLONG ShiftReg;

	if (seed != 0 ) {
		ShiftReg = seed;
	}

	ShiftReg = ((((ShiftReg >> 63) 
		^ (ShiftReg >> 3)
		^ (ShiftReg >> 2)
		^ (ShiftReg)) & 0x1) << 63) | (ShiftReg >> 1);

	if (Contents) {

		memcpy(Contents, &ShiftReg,Length);
	}


}

void rotr(unsigned long *value,ULONG nRotBits,UCHAR valueSize)
{
	union {
		USHORT shortV;
		ULONG longV;
		UCHAR charV;
	} u;

	u.longV = *value;

	switch (valueSize) {
	case 1:
		u.charV = (u.charV << nRotBits) | (u.charV >> (8-nRotBits));
		break;
	case 2:
		u.shortV = (u.shortV << nRotBits) | (u.shortV >> (16-nRotBits));
		break;
	case 4:
		u.longV = (u.longV << nRotBits) | (u.longV >> (32-nRotBits));
		break;
	}


	*value = u.longV;

	
}

void rotl(unsigned long * value,ULONG nRotBits,UCHAR valueSize)
{
	union {
		USHORT shortV;
		ULONG longV;
		UCHAR charV;
	} u;

	u.longV = *value;

	switch (valueSize) {
	case 1:
		u.charV = (u.charV >> nRotBits) | (u.charV << (8-nRotBits));
		break;
	case 2:
		u.shortV = (u.shortV >> nRotBits) | (u.shortV << (16-nRotBits));
		break;
	case 4:
		u.longV = (u.longV >> nRotBits) | (u.longV << (32-nRotBits));
		break;
	
	}


	*value = u.longV;
	
}

void XAudiopUtility_GenerateKey(PUCHAR pKey)
{

	ULONGLONG seed = PRIVATE_KEY_SEED;

	//
	// seed LFSR
	//

	lfsr(seed,NULL,0);

	//
	// shift a few times
	//

	for (ULONG i=0;i<KEY_SIZE*10;i++) {
		lfsr(0,pKey,KEY_SIZE);
	}

}

int XAudiopUtility_Encode(PUCHAR pKey,
						  PUCHAR pSrc, 
						  DWORD dwSize, 
						  PUCHAR pDst,
						  BOOL fIncludeKey)
{
	ULONGLONG seed;
	UCHAR szPrivateKey[KEY_SIZE];
	ULONG i;
	int err = ERROR_SUCCESS;
	UCHAR temp,temp2;

	XAudiopUtility_GenerateKey(szPrivateKey);

	for (i=0;i<KEY_SIZE;i++) {

		//
      	// store our key with their key in the first N bytes of destination
	    //

		temp = szPrivateKey[i] ^ pKey[i];
		if (fIncludeKey) {

			*(pDst+i) = temp;

		}

	}

    //
	// seed the LFSR
	//

	memcpy(&seed,szPrivateKey,sizeof(seed));
	lfsr(seed,NULL,0);

	//
	// encode the data, byte at a time..
	// we do a XOR, rght shift, left shift
	//

	if (fIncludeKey) {
		pDst += KEY_SIZE;
	}

	temp = 0;

	for (i=0;i<dwSize;i++) {

		lfsr(0,&temp2,sizeof(UCHAR));
		temp = temp2 ^ *(pSrc+i) ^ szPrivateKey[i%KEY_SIZE];

		temp += szPrivateKey[i%KEY_SIZE]*pKey[i%KEY_SIZE];
		*(pDst+i) = temp;

	}


#if _DEBUG

	//
	// DEBUG ONLY: Verify encryption/decryption
	//

	PUCHAR pDst2 = new UCHAR[dwSize];
	memset(pDst2,0,dwSize);

	if (fIncludeKey) {
		XAudiopUtility_Decode(pKey, pDst-KEY_SIZE,dwSize+KEY_SIZE,pDst2,TRUE);
	} else {
		XAudiopUtility_Decode(pKey, pDst,dwSize,pDst2, FALSE);
	}

	if (pSrc != pDst) {
		err = memcmp(pDst2,pSrc,dwSize);
		if (err) {
			err = -1;
		}
	}


	delete [] pDst2;

#endif

	return err;

}


//
// NOTE: if fIsKeyEmbedded is TRUE, pSrc != pDst
//
	
int XAudiopUtility_Decode(PUCHAR pKey,
						  PUCHAR pSrc, 
						  DWORD  dwSize, 
						  PUCHAR pDst,
						  BOOL   fIsKeyEmbedded)
{
	ULONGLONG seed;

	UCHAR szPrivateKey[KEY_SIZE],szDevKey[KEY_SIZE];
	ULONG i;
	int err = ERROR_SUCCESS;
	UCHAR temp,temp2;

	//
	// regenerate our key
	//
	
	XAudiopUtility_GenerateKey(szPrivateKey);

	if (fIsKeyEmbedded) {

		//
		// recover user key from first N bytes of src
		//
		
		for (i=0;i<KEY_SIZE;i++) {
			
			szDevKey[i] = szPrivateKey[i] ^ *(pSrc+i);
			
		}

		if (pKey) {
			memcpy(pKey,szDevKey,KEY_SIZE);
		}

	} else {

		if (pKey == NULL) {
			return -1;
		}

		memcpy(szDevKey,pKey,KEY_SIZE);

	}

    //
	// seed the LFSR
	//

	memcpy(&seed,szPrivateKey,sizeof(seed));
	lfsr(seed,NULL,0);

	//
	// decode the data, byte at a time..
	// we do the inverse of the encode algorithm
	//

	if (fIsKeyEmbedded) {
		pSrc += KEY_SIZE;
		dwSize -= KEY_SIZE;
	}

	temp = 0;

	for (i=0;i<dwSize;i++) {

		temp = *(pSrc+i);
		temp -= szPrivateKey[i%KEY_SIZE]*szDevKey[i%KEY_SIZE];	
		lfsr(0,&temp2,sizeof(UCHAR));
		*(pDst+i) = temp2 ^ temp ^ szPrivateKey[i%KEY_SIZE];
	}


	return err;

}
