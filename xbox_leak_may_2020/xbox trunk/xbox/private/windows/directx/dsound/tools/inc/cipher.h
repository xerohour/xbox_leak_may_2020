#define KEY_SIZE 8
#define PRIVATE_KEY_SEED ((ULONGLONG) 0x7fa49bca49de12ba)

int XAudiopUtility_Encode(
						  PUCHAR pKey,
						  PUCHAR pSrc, 
						  DWORD dwSize, 
						  PUCHAR pDst,
						  BOOL fEmbeddedKey);

int XAudiopUtility_Decode(PUCHAR pKey,
						  PUCHAR pSrc, 
						  DWORD dwSize, 
						  PUCHAR pDst,
						  BOOL fEmbeddedKey);

void XAudiopUtility_GenerateKey(PUCHAR pKey);


