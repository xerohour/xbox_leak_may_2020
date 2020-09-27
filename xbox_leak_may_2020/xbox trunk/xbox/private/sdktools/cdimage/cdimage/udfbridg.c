#include "precomp.h"
#include <assert.h>
#pragma hdrstop

// From UDF 1.50 2.2.7.2
#define LVINFO_LVINFO_LENGTH    36
#define LVINFO_LVIDENT_LENGTH   128
#define LVIFNO_IMPUSE_LENGTH    128

typedef struct tagLVInformation
{
    CHARSPEC LVICharset;
    UCHAR LogicalVolumeIdentifier[LVINFO_LVIDENT_LENGTH];
    UCHAR LVInfo1[LVINFO_LVINFO_LENGTH];
    UCHAR LVInfo2[LVINFO_LVINFO_LENGTH];
    UCHAR LVInfo3[LVINFO_LVINFO_LENGTH];
    REGID ImplementationID;
    UCHAR ImplementationUse[LVIFNO_IMPUSE_LENGTH];
} LVINFORMATION, *PLVINFORMATION;

DWORD NumberOfStartingZeros(BYTE* pBuffer, DWORD dwBufferLength);

void AddNewShortExtent(PICBFILE pIcb, SHORTAD* pShortAD, LPDWORD pdwNextFreeBlock);
DWORD GetNumberOfShortExtents(PICBFILE pIcbFile);
VOID GetShortExtent(PICBFILE pIcbFile, DWORD dwExtentNum, PSHORTAD pShortExt);
VOID PutShortExtent(PICBFILE pIcbFile, DWORD dwExtentNum, PSHORTAD pShortExt);
BOOL IsShortExtentSparse(PSHORTAD pShortExt);
void AddNewShortAdToExtent(DWORD dwExtentBlock, SHORTAD* pShortAd, LPDWORD pdwNextFreeBlock);

void AddNewLongExtent(PICBFILE pIcb, LONGAD* pLongAD, LPDWORD pdwNextFreeBlock);
DWORD GetNumberOfLongExtents(PICBFILE pIcbFile);
VOID GetLongExtent(PICBFILE pIcbFile, DWORD dwExtentNum, PLONGAD pLongExt);
VOID PutLongExtent(PICBFILE pIcbFile, DWORD dwExtentNum, PLONGAD pLongExt);
BOOL IsLongExtentSparse(PLONGAD pLongExt);
void AddNewLongAdToExtent(DWORD dwExtentBlock, LONGAD* pLongAd, LPDWORD pdwNextFreeBlock);

BOOL BuildSparseFileExtent(DWORD* pdwNextFreeBlock, HANDLE hReadContext, PICBFILE pIcb, PHASH_AND_LENGTH pHashAndLength, DWORDLONG dwlFileLength);
BOOL MakeAllocExtent(PICBFILE pIcb, BYTE** Buffer, DWORD* pdwBytesRead, HANDLE hReadContext, DWORD* pdwNextFreeBlock, BOOL bSparse, PHASH_AND_LENGTH pHashAndLength, DWORDLONG* pdwlFileLengthLeft);
BOOL MakeSparseExtent(PICBFILE pIcb, BYTE** Buffer, DWORD* pdwBytesRead, HANDLE hReadContext, DWORD* pdwNextFreeBlock, PHASH_AND_LENGTH pHashAndLength, DWORDLONG* pdwlFileLengthLeft);

VOID InitalizeAllocationExtentDescriptor(PNSR_ALLOC pAllocDesc, DWORD dwBlock, DWORD dwPrevBlock);
void SetRandomExtentLength(void);
void WriteICBFileEntry( PDIRENTRY pFile, DWORDLONG UniqueID );
void WriteICBExtendedFileEntry( PDIRENTRY pFile, DWORDLONG UniqueID );

#define CopyStruct(dest, src) memcpy((&(dest)), (&(src)), (sizeof (dest)))

// Use a random number as partition number to discover incorrect implementations
// which confuse partition number and partition reference number
// NOTE: Changing PARTITION_NUMBER must also change the value in WriteLvolDescriptor()
#define PARTITION_NUMBER            0x0BAD
#define VIRTUAL_PARTITION_NUMBER    0x0BAD

// each directory has one PARENTFID (..) entry
// which is 38 + 0 (".." file name is not recorded)
// bytes long, rounding up to next ULONG ==> 40
#define PARENTFID_LEN           40

//
// The maximum single extent length for UDF is 0x3FFFFFFF.
// This is because the upper two bits of the length are reserved.
// We are going to round down one sector, and use multiple extents
// if the file is >= 1GB.
//
#define UDF_MAXEXTENT           (0x40000000 - CD_SECTORSIZE)

TIMESTAMP GlobalNsrTimeStamp;    // exported via extern in cdimage.h
short     NsrLocalTimeBias;      // exported via extern in cdimage.h
DWORDLONG UniqueID = 0;
DWORD     g_dwIcbBlockCount;

DWORD   gdwSpaceSaved;
WCHAR   wchStaticUnicodeBuffer[ MAX_NAME_LENGTH ];

ULONG   ulNextBlockToUse;
ULONG   ulGapBlock = 0;
ULONG   ulMainVDSExtentStart;
ULONG   ulMainVDSExtentBlocks;
ULONG   ulLVISExtentStart;
ULONG   ulLVISExtentBlocks;
ULONG   ulReserveVDSExtentStart;
ULONG   ulReserveVDSExtentBlocks;

ULONG   ulFSDExtentBlocks;
ULONG   ulFSDExtentStart;
ULONG   ulPartitionStart;
ULONG   ulPartitionLength;              // Length of Partion in logical sectors
ULONG   ulVATICBLocation;
ULONG   ulVolumeDescSeqNum;             // Volume descriptor sequence number

BOOL    bZeroLengthIcbSaved=FALSE;
DWORD   dwZeroLengthIcb;

extern ULONG   ulAnchorSectorNumber;
extern ULONG   ulAnchorSlackSector;

UCHAR   OSTABuffer[ MAX_NAME_LENGTH * 2 ];  // Common out buffer for OSTA Compressed Unicode conversion
UCHAR   VolName[ MAX_NAME_LENGTH * 2 ];     // Common out buffer for OSTA Compressed Unicode conversion
UCHAR   dstrBuffer[ 256 ];                  // Common out buffer for dstring

#define MAX_AD_EXTENT               1000        // big enough that the extent runs out of space before the limit is hit
#define MAX_RANDOM_AD_EXTENT        10      // want this to be small enough to have extents
//#define MAX_RANDOM_AD_EXTENT        2      // want this to be small enough to have extents

ULONG g_ulMaxAdsInExtent=MAX_AD_EXTENT;

 // Do we want lots of debug output?
#ifndef UDF_DBG
BOOL bVerbose   = FALSE;
#else
BOOL bVerbose   = FALSE;
#endif

/***    UDF Data Definitions
 *
 */
/************************************************************************/
static USHORT  g_usCurrentUDFVersion = 0x00;

void SetCurrentUDFVersion(USHORT nVer)
{
	g_usCurrentUDFVersion = nVer;

	ASSERT( (   g_usCurrentUDFVersion == 0x102
		     || g_usCurrentUDFVersion == 0x150
			 || g_usCurrentUDFVersion == 0x200));

}

USHORT GetCurrentUDFVersion()
{
	ASSERT( (   g_usCurrentUDFVersion == 0x102
		     || g_usCurrentUDFVersion == 0x150
			 || g_usCurrentUDFVersion == 0x200));

	return g_usCurrentUDFVersion;
}


/***********************************************************************/

static  REGID   const   UDF_DomainID102 = {      // REGID: "*OSTA UDF Compliant"
    0x00,{0x2A, 0x4F, 0x53, 0x54, 0x41, 0x20, 0x55,
    0x44, 0x46, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x6C,
    0x69, 0x61, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00},
   {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // Domain Suffix UDF 1.50
};

static  REGID   const   UDF_DomainID150 = {      // REGID: "*OSTA UDF Compliant"
    0x00,{0x2A, 0x4F, 0x53, 0x54, 0x41, 0x20, 0x55,
    0x44, 0x46, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x6C,
    0x69, 0x61, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00},
   {0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // Domain Suffix UDF 1.50
};

static REGID   const   UDF_DomainID200 = {      // REGID: "*OSTA UDF Compliant"
    0x00,{0x2A, 0x4F, 0x53, 0x54, 0x41, 0x20, 0x55,
    0x44, 0x46, 0x20, 0x43, 0x6F, 0x6D, 0x70, 0x6C,
    0x69, 0x61, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // Domain Suffix UDF 2.00
};
const REGID*  GetUDFDomainID()
{
	switch (GetCurrentUDFVersion())
	{
		case 0x102 : return &UDF_DomainID102;
		case 0x150 : return &UDF_DomainID150;
		case 0x200 : return &UDF_DomainID200;
		default: ASSERT(FALSE); // Unknown Version
		
	}
	return &UDF_DomainID102;
}

/***********************************************************************/


static REGID   const   CDImageID = {   // REGID: "*Microsoft CDIMAGE UDF"
    0x00,{0x2A, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73,
    0x6F, 0x66, 0x74, 0x20, 0x43, 0x44, 0x49, 0x4D,
    0x41, 0x47, 0x45, 0x20, 0x55, 0x44, 0x46, 0x00},
   {0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // Domain Suffix: Win NT
};

const REGID*  GetImplementationID()
{
	return &CDImageID;
}

/***********************************************************************/

typedef UCHAR REGID_SUFFIX[8];

static REGID_SUFFIX const Suffix102 = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // WinNT UDF 1.50
static REGID_SUFFIX const Suffix150 = {0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // WinNT UDF 1.50
static REGID_SUFFIX const Suffix200 = {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // WinNT UDF 2.00

const REGID_SUFFIX* GetRegIDSuffix()
{
	switch (GetCurrentUDFVersion())
	{
	case 0x102 : return &Suffix102;
	case 0x150 : return &Suffix150;
	case 0x200 : return &Suffix200;
	}
	return &Suffix102;
}
/***********************************************************************/

/***    UDF Character Set Marker (By Agreement)
 *
 *      This character set marker identifies the OSTA Compressed Unicode
 *      character set.  Code points are stored as 8-bit, 16-bit, or 32-bit
 *      quantities.  Their zero-extended values map to unicode code points.
 *
 */

CHARSPEC    const   OSTAUnicode = {
    CHARSPEC_T_CS0, {                                       // CS0 (Agreement)
              0x4f, 0x53, 0x54, 0x41, 0x20, 0x43, 0x6f,     //  'OSTA Co'
        0x6d, 0x70, 0x72, 0x65, 0x73, 0x73, 0x65, 0x64,     // 'mpressed'
        0x20, 0x55, 0x6e, 0x69, 0x63, 0x6f, 0x64, 0x65,     // ' Unicode'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // ' 0000000'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // '00000000'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // '00000000'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // '00000000'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      // '00000000'
    }
};

/**************************************************************************/

static REGID   const   NSR01ID = { // REGID: "+NSR01"
    0x00,{0x2B, 0x4E, 0x53, 0x52, 0x30, 0x31, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}      // Undefined Suffix
};

static REGID   const   NSR02ID = { // REGID: "+NSR02"
    0x00,{0x2B, 0x4E, 0x53, 0x52, 0x30, 0x32, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}      // Undefined Suffix
};

static REGID   const   NSR03ID = { // REGID: "+NSR03"
    0x00,{0x2B, 0x4E, 0x53, 0x52, 0x30, 0x33, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}      // Undefined Suffix
};

const REGID*  GetPartitionContentsID()
{
	switch (GetCurrentUDFVersion())
	{
		case 0x102 : //fall through
		case 0x150 : return &NSR02ID;
		case 0x200 : return &NSR03ID;
		default    : ASSERT(FALSE);
	};
	return &NSR02ID;
}

/****************************************************************************/
USHORT  GetDesTagVersion()
{
	switch (GetCurrentUDFVersion())
	{
		case 0x102 : // fallthrough
		case 0x150 : return 2;
		case 0x200 : return 3;
		default:	ASSERT(FALSE);
	}
	return 2;
}
/****************************************************************************/
UCHAR __inline ComputeChecksum( PVOID Buffer, ULONG Bytes ) {

    PUCHAR p = Buffer;
    UCHAR checksum = 0;

    while ( Bytes-- )
        checksum = (UCHAR)( checksum + *p++ );

    return checksum;
}


PUCHAR dstring(PUCHAR pSrc, ULONG cbLen, PUCHAR pBuffer)
{
    PUCHAR pb = pBuffer;
    PUSHORT pw, pwSrc;
    ULONG cbCopied, j;

    if (cbLen < 2 || cbLen > 0xff)
        return NULL;

    // compensate for 0 based index
    cbLen--;

    // copy the first byte, OSTA Compressed Unicode char width
    switch (*pb++ = *pSrc++) {
        case 8:
            for (cbCopied = 1; cbCopied < cbLen && *pSrc; cbCopied++)
                *pb++ = *pSrc++;

            // zero fill remaing bytes if any
            for (j = cbCopied; j < cbLen; j++)
                *pb++ = '\0';

            break;

        case 16:
            for (cbCopied = 1, pw = (PUSHORT)pb, pwSrc = (PUSHORT)pSrc;
               cbCopied < cbLen-1 && *pwSrc; cbCopied += 2)
                    // cbLen-1 because need at least 2 bytes to copy a 16 bit char
                *pw++ = *pwSrc++;

            // zero fill remaing bytes if any
            for (j = cbCopied, pb = (PUCHAR)pw; j < cbLen; j++)
                *pb++ = '\0';

            break;

        default:
            return NULL;
    }

    // strlen is encoded as the last byte of the field
    *pb = (UCHAR) cbCopied;

    return pBuffer;
}



#define OSTA16BitUnicodeLen( n )     ( n ) + 1

PUCHAR OSTA16BitUnicode(LPWSTR pszUnicodeName, ULONG ulUnicodeLen, PUCHAR pBuffer)
{
    PUCHAR pName = (PUCHAR)pszUnicodeName, pBuf = pBuffer;
    ULONG  ulCount = 0;

    // Reverse the byte ordering for each Unicode character.
    for(*(pBuf++) = 16;ulCount < ulUnicodeLen;ulCount += 2)
    {
      *(pBuf++) = *(pName+ulCount+1);
      *(pBuf++) = *(pName+ulCount);
    }

    return pBuffer;
}

#define OSTA8BitUnicodeLen( n )     ( n >> 1 ) + 1

PUCHAR OSTA8BitUnicode(LPWSTR pszUnicodeName, ULONG ulUnicodeLen, PUCHAR pBuffer)
{
    PUCHAR pName = (PUCHAR)pszUnicodeName, pBuf = pBuffer;
    ULONG  ulCount = 0;

    // Reverse the byte ordering for each Unicode character.
    for(*(pBuf++) = 8;ulCount < ulUnicodeLen;ulCount += 2)
    {
      *(pBuf++) = *(pName+ulCount);
    }

    return pBuffer;
}

void MakeNsrTimeStampFromFileTime( PTIMESTAMP pTimeStamp, FILETIME ftFileTime )
{
    SYSTEMTIME st;

    ASSERT( *(UNALIGNED DWORDLONG*)&(ftFileTime) != 0 );

    ZeroMemory( &st, sizeof( st ));      // in case FileTimeToSystemTime fails.

    *(UNALIGNED DWORDLONG*)&ftFileTime += LocalTimeBiasInFileTimeUnits;

    FileTimeToSystemTime( &ftFileTime, &st );

    pTimeStamp->timestamp_Type        = TIMESTAMP_T_LOCAL; // UDF 2.1.3.1
    pTimeStamp->timestamp_Zone        = NsrLocalTimeBias;
    pTimeStamp->timestamp_Year        = st.wYear;
    pTimeStamp->timestamp_Month       = (UCHAR) st.wMonth;
    pTimeStamp->timestamp_Day         = (UCHAR) st.wDay;
    pTimeStamp->timestamp_Hour        = (UCHAR) st.wHour;
    pTimeStamp->timestamp_Minute      = (UCHAR) st.wMinute;
    pTimeStamp->timestamp_Second      = (UCHAR) st.wSecond;
    pTimeStamp->timestamp_centiSecond = (UCHAR) (  st.wMilliseconds / 10 );
    pTimeStamp->timestamp_usec100     = (UCHAR) (( st.wMilliseconds % 10 ) * 10 );
    pTimeStamp->timestamp_usec        = 0;
}

/***    Allocate CD Blocks
 *
 *      Sector 256 is the UDF anchor sector.
 *              Other UDF structures and ISO 9660 meta data
 *              has to skip over this "road block".
 *
 */

#ifdef DONTCOMPILE  // tommcg -- Now use AllocateMetaBlocks in cdimage.c

 ULONG AllocateCdBlocks( ULONG n ) {

        ULONG ulNextBlock = ulNextBlockToUse;

        if ( (!bUDFBridge) || (ulNextBlock > ANCHOR_SECTOR) )   // if already after anchor point
                ulNextBlockToUse += n;
        else if ( (ulNextBlock + n) > ANCHOR_SECTOR ) {         // if the currently requested allocation crosses the anchor point

                ASSERT( ulGapBlock == 0 );
                if ( ulNextBlock < ANCHOR_SECTOR )              // this should only eliminate the == case
                        ulGapBlock = ulNextBlock;
                else
                        ASSERT( ulNextBlock == ANCHOR_SECTOR );

                ulNextBlock = ANCHOR_SECTOR + 1;
                ulNextBlockToUse = ulNextBlock + n;
                }
        else {
                ulNextBlockToUse += n;
                }


        return ulNextBlock;

        }

#endif // DONTCOMPILE


/***    Volume Recognition Sequence
 *
 *      VSR (including CD-ROM Volume Descriptor Set) shall begin at
 *      byte number 32768 of the volume space.
 *
 */

void WriteBEA01Descriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PVSD_BEA01 pBEA01;

    ASSERT( SectorNumber >= 16 );       // this descriptor shall start at or after 32K

    pBuffer =  AllocateBuffer( CD_SECTORSIZE, TRUE );

    pBEA01 = (PVSD_BEA01) pBuffer;
    pBEA01->vsd_bea01_Type = 0x00;
    memcpy( pBEA01->vsd_bea01_Ident, VSD_IDENT_BEA01, 5 );
    pBEA01->vsd_bea01_Version = 0x01;

    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }

void WriteNSRDescriptor(ULONG SectorNumber)
{
	PUCHAR pBuffer;
    PVSD_NSR02 pNSR;

    ASSERT( SectorNumber >= 16 );       // this descriptor shall start at or after 32K

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pNSR = (PVSD_NSR02) pBuffer;
    pNSR->vsd_nsr02_Type = 0x00;
	switch (GetCurrentUDFVersion())
	{
		case 0x102:		memcpy( pNSR->vsd_nsr02_Ident, VSD_IDENT_NSR02, 5 ); break;
		case 0x150:		memcpy( pNSR->vsd_nsr02_Ident, VSD_IDENT_NSR02, 5 ); break;
		case 0x200:		memcpy( pNSR->vsd_nsr02_Ident, VSD_IDENT_NSR03, 5 ); break;
		default: ASSERT(FALSE);
	}
    pNSR->vsd_nsr02_Version = 0x01;

    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );
}




void WriteTEA01Descriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PVSD_TEA01 pTEA01;

    // this descriptor shall start at or after byte 32K
    ASSERT( ( SectorNumber * CD_SECTORSIZE ) >= 32768 );

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pTEA01 = (PVSD_TEA01) pBuffer;
    pTEA01->vsd_tea01_Type = 0x00;
    memcpy( pTEA01->vsd_tea01_Ident, VSD_IDENT_TEA01, 5 );
    pTEA01->vsd_tea01_Version = 0x01;

    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


/***    Anchor Volume Descriptor Pointer
 *
 *      Anchor Volume Descriptor Pointer shall be recorded at sectors 256
 *      and N, where N is the last addressable sector of a volume.
 *
 */

void WriteAnchorDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_ANCHOR pAnchor;

    // Anchor Pointer must be in sector 256
    ASSERT ( SectorNumber == ANCHOR_SECTOR || SectorNumber == ulAnchorSectorNumber );

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pAnchor = (PNSR_ANCHOR) pBuffer;

    pAnchor->nsr_anchor_Main.extentad_Len = ulMainVDSExtentBlocks * CD_SECTORSIZE;
    pAnchor->nsr_anchor_Main.extentad_lsn = ulMainVDSExtentStart;
    pAnchor->nsr_anchor_Reserve.extentad_Len = ulReserveVDSExtentBlocks * CD_SECTORSIZE;
    pAnchor->nsr_anchor_Reserve.extentad_lsn = ulReserveVDSExtentStart;

    // Fill in Descriptor Tag
    pAnchor->nsr_anchor_destag.destag_Ident = DESTAG_ID_NSR_ANCHOR;
    pAnchor->nsr_anchor_destag.destag_Version = GetDesTagVersion();
    pAnchor->nsr_anchor_destag.destag_CRCLen = (USHORT)( sizeof( NSR_ANCHOR ) - sizeof( DESTAG ));
    pAnchor->nsr_anchor_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pAnchor+sizeof(DESTAG), pAnchor->nsr_anchor_destag.destag_CRCLen );
    pAnchor->nsr_anchor_destag.destag_lsn = SectorNumber;

    pAnchor->nsr_anchor_destag.destag_Checksum=0;
       // Fill in checksum last
    pAnchor->nsr_anchor_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pAnchor->nsr_anchor_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }



/***    Volume Descriptor Sequence
 *
 *      Both the main and reserve VDS extents shall each have a
 *      minimum length of 16 logical sectors.
 *
 */


void WritePvdDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_PVD pPvd;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pPvd = (PNSR_PVD) pBuffer;

    ulVolumeDescSeqNum = 0;
    pPvd->nsr_pvd_VolDescSeqNum = ulVolumeDescSeqNum++;
    ASSERT( pPvd->nsr_pvd_Number == 0 );        // shall be the only PVD
    pPvd->nsr_pvd_VolSetSeq = pPvd->nsr_pvd_VolSetSeqMax = 1;   // 3/8.6
    if (dstring(OSTA16BitUnicode(wchVolumeLabel, 32, OSTABuffer), 32, dstrBuffer))
        memcpy( pPvd->nsr_pvd_VolumeID, dstrBuffer, 32 );
    pPvd->nsr_pvd_Level = 2;                    // UDF 2.2.2.1
    pPvd->nsr_pvd_LevelMax = 3;                 // UDF 2.2.2.2
    pPvd->nsr_pvd_CharSetList = 0x00000001;     // UDF 2.1.2 CS0
    pPvd->nsr_pvd_CharSetListMax = 0x00000001;  // UDF 2.1.2 CS0

    //    First 16 characters of VolSetID field shall be set to a unique value.
    //    First 8 characters should come from a CS0 hex representation
    //    of a 32-bit time value. The next 8 characters are free for
    //    implementation use.

    OSTABuffer[0] = 8;                          // Use 8 bit character
    _ultoa(ftGlobalFileTime.dwLowDateTime, (char *)&OSTABuffer[1], 16);
    strcpy((char *)&OSTABuffer[9], "MS UDFBridge");
    ZeroMemory(VolName,LVINFO_LVIDENT_LENGTH);
    if (dstring(OSTABuffer, 128, dstrBuffer))
    {
        memcpy( pPvd->nsr_pvd_VolSetID, dstrBuffer, 128 );
        memcpy(VolName,dstrBuffer,LVINFO_LVIDENT_LENGTH);
    }

    CopyStruct( pPvd->nsr_pvd_charsetDesc, OSTAUnicode );
    CopyStruct( pPvd->nsr_pvd_charsetExplan, OSTAUnicode );
    CopyStruct( pPvd->nsr_pvd_ImpUseID, *GetImplementationID());
    CopyStruct( pPvd->nsr_pvd_RecordTime, GlobalNsrTimeStamp );

    ASSERT( pPvd->nsr_pvd_Predecessor == 0 );   // this shall be the sole PVD

    // Fill in Descriptor Tag
    pPvd->nsr_pvd_destag.destag_Ident = DESTAG_ID_NSR_PVD;
    pPvd->nsr_pvd_destag.destag_Version = GetDesTagVersion();
    pPvd->nsr_pvd_destag.destag_CRCLen = (USHORT)( sizeof( NSR_PVD ) - sizeof( DESTAG ));
    pPvd->nsr_pvd_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pPvd+sizeof(DESTAG), pPvd->nsr_pvd_destag.destag_CRCLen );
    pPvd->nsr_pvd_destag.destag_lsn = SectorNumber;

    pPvd->nsr_pvd_destag.destag_Checksum = 0;
    // Fill in checksum last
    pPvd->nsr_pvd_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pPvd->nsr_pvd_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }

void WriteImpUseVolDescriptor( ULONG SectorNumber )
{

    PUCHAR pBuffer;
    PNSR_IMPUSE pImpUse;
    PREGID pRegId;
    PLVINFORMATION pLVInfo;
    PDESTAG pDestag;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pImpUse = (PNSR_IMPUSE) pBuffer;

    pImpUse->nsr_impuse_VolDescSeqNum = ulVolumeDescSeqNum++;

    // Fill in the REG ID
    pRegId=&(pImpUse->nsr_impuse_ImpUseID);
    strcpy((char*)pRegId->regid_Identifier,"*UDF LV Info");
    memcpy(pRegId->regid_Suffix,*GetRegIDSuffix(),REGID_LENGTH_SUFFIX);

    // Fill in ImpUse
    pLVInfo=(PLVINFORMATION) pImpUse->nsr_impuse_ImpUse;
    memcpy(&(pLVInfo->LVICharset),&OSTAUnicode,sizeof(CHARSPEC));
    memcpy(pLVInfo->LogicalVolumeIdentifier,VolName,LVINFO_LVIDENT_LENGTH);
    pLVInfo->LVInfo1[0]=8;
    pLVInfo->LVInfo2[0]=8;
    pLVInfo->LVInfo3[0]=8;
    strcpy((char*)&(pLVInfo->LVInfo1[1]),"Microsoft");
    strcpy((char*)&(pLVInfo->LVInfo2[1]),"Sample UDF 1.50 media");
    strcpy((char*)&(pLVInfo->LVInfo3[1]),"Contact danlo or nathann");
    pRegId=&(pLVInfo->ImplementationID);
    memcpy(pRegId,GetImplementationID(),sizeof(REGID));

    // Fill in Descriptor Tag
    pDestag=&(pImpUse->nsr_impuse_destag);
    pDestag->destag_Ident = DESTAG_ID_NSR_IMPUSE;
    pDestag->destag_Version = GetDesTagVersion();
    pDestag->destag_CRCLen = (USHORT)( sizeof( NSR_IMPUSE ) - sizeof( DESTAG ));
    pDestag->destag_CRC =
        Crc16( 0, (PUCHAR)pImpUse+sizeof(DESTAG), pDestag->destag_CRCLen );
    pDestag->destag_lsn = SectorNumber;
    pDestag->destag_Checksum = 0;
    pDestag->destag_Checksum =
        ComputeChecksum( (PUCHAR)pDestag, sizeof (DESTAG) );

    // Save it
    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );
}

void WriteLvolDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_LVOL pLvol;
    ULONG ulMapTableLength = 0;
    ULONG ulMapTableCount = 0;   // shall have exactly one partition and one partition map


    PARTMAP   partMap;
    //PARTMAPVIRT partVirt;

    partMap.partmap_Type = 1;
    partMap.partmap_Length = 6;
    partMap.partmap_VolSetSeq = 1;
    partMap.partmap_Partition =PARTITION_NUMBER;



	
	ulMapTableCount = 1;
    ulMapTableLength = partMap.partmap_Length;
	


    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pLvol = (PNSR_LVOL) pBuffer;

    pLvol->nsr_lvol_VolDescSeqNum = ulVolumeDescSeqNum++;
    CopyStruct( pLvol->nsr_lvol_charspec, OSTAUnicode );
    if (dstring(OSTA16BitUnicode(wchVolumeLabel, 32, OSTABuffer), 128, dstrBuffer))
        memcpy( pLvol->nsr_lvol_VolumeID, dstrBuffer, 128);
    pLvol->nsr_lvol_BlockSize = CD_SECTORSIZE;           // shall be 2048 bytes
    CopyStruct( pLvol->nsr_lvol_DomainID, *GetUDFDomainID() );
    pLvol->nsr_lvol_FSD.longad_Length = ulFSDExtentBlocks * CD_SECTORSIZE;
    pLvol->nsr_lvol_FSD.longad_Start.nsr_lba_lbn = ulFSDExtentStart;
    pLvol->nsr_lvol_FSD.longad_Start.nsr_lba_PartRef = 0;
    pLvol->nsr_lvol_MapTableLength = ulMapTableLength;
    pLvol->nsr_lvol_MapTableCount = ulMapTableCount;

    CopyStruct(pLvol->nsr_lvol_ImpUseID, *GetImplementationID());

    pLvol->nsr_lvol_Integrity.extentad_Len = ulLVISExtentBlocks * CD_SECTORSIZE;
    pLvol->nsr_lvol_Integrity.extentad_lsn = ulLVISExtentStart;

    memcpy( pLvol->nsr_lvol_MapTable, &partMap, sizeof(PARTMAP) );

    // Fill in Descriptor Tag
    pLvol->nsr_lvol_destag.destag_Ident = DESTAG_ID_NSR_LVOL;
    pLvol->nsr_lvol_destag.destag_Version = GetDesTagVersion();
    pLvol->nsr_lvol_destag.destag_CRCLen = (USHORT)( sizeof( NSR_LVOL ) - sizeof( DESTAG )) + (USHORT)ulMapTableLength;
    pLvol->nsr_lvol_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pLvol+sizeof(DESTAG), pLvol->nsr_lvol_destag.destag_CRCLen );
    pLvol->nsr_lvol_destag.destag_lsn = SectorNumber;

   pLvol->nsr_lvol_destag.destag_Checksum = 0;

    // Fill in checksum last
    pLvol->nsr_lvol_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pLvol->nsr_lvol_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }

void WriteLvIntegDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_INTEG pInteg;
    PNSR_LVHD pLvhd;
    PUDF_LVID_IMPUSE pImpUse;

    ASSERT( dwlTotalNumberOfFiles       < 0x100000000 );
    ASSERT( dwlTotalNumberOfDirectories < 0x100000000 );

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pInteg = (PNSR_INTEG) pBuffer;

    CopyStruct( pInteg->nsr_integ_Time, GlobalNsrTimeStamp );
    pInteg->nsr_integ_Type = NSR_INTEG_T_CLOSE;
	
    ASSERT( pInteg->nsr_integ_Next.extentad_Len == 0 );

    pLvhd = &pInteg->nsr_integ_LVHD;
    *(UNALIGNED DWORDLONG*)&(pLvhd->nsr_lvhd_UniqueID) = 15 + dwlTotalNumberOfFiles + dwlTotalNumberOfDirectories;

    ASSERT(sizeof(UDF_LVID_IMPUSE) >= 46);      // this needs to be at least 46 bytes
	
	pInteg->nsr_integ_PartitionCount = 1;
	
    pInteg->nsr_integ_ImpUseLength = sizeof (UDF_LVID_IMPUSE);

    ASSERT( ulPartitionLength != 0 );

	{
		DWORD i;
		DWORD dwPartitions = pInteg->nsr_integ_PartitionCount;
		for( i = 0; i < dwPartitions;i++)
		{
			pInteg->nsr_integ_Free[i] = 0;
			pInteg->nsr_integ_Free[dwPartitions+i] = ulPartitionLength;
		}
	}

    pImpUse = ( PUDF_LVID_IMPUSE ) ( pInteg->nsr_integ_Free + (pInteg->nsr_integ_PartitionCount*2) );
    CopyStruct( pImpUse->riImplementID, *GetImplementationID() );
    pImpUse->ulNumberOfFiles	= (DWORD)dwlTotalNumberOfFiles;
    pImpUse->ulNumberOfDirs		= (DWORD)dwlTotalNumberOfDirectories;
    pImpUse->usMinUDFReadRev	=  GetCurrentUDFVersion();
    pImpUse->usMinUDFWriteRev	=  GetCurrentUDFVersion();
    pImpUse->usMaxUDFWriteRev	=  GetCurrentUDFVersion();


    // Fill in Descriptor Tag
    pInteg->nsr_integ_destag.destag_Ident = DESTAG_ID_NSR_LVINTEG;
    pInteg->nsr_integ_destag.destag_Version = GetDesTagVersion();
    pInteg->nsr_integ_destag.destag_CRCLen = (USHORT)( sizeof( NSR_INTEG ) - sizeof( DESTAG )) + (USHORT)pInteg->nsr_integ_ImpUseLength + (USHORT)((pInteg->nsr_integ_PartitionCount-1)*8); // 8bytes for each extra partition
    pInteg->nsr_integ_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pInteg+sizeof(DESTAG), pInteg->nsr_integ_destag.destag_CRCLen );
    pInteg->nsr_integ_destag.destag_lsn = SectorNumber;

   pInteg->nsr_integ_destag.destag_Checksum = 0;

    // Fill in checksum last
    pInteg->nsr_integ_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pInteg->nsr_integ_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }

void WritePartDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_PART pPart;
    PNSR_PART_H pPartHeader;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pPart = (PNSR_PART) pBuffer;

    pPart->nsr_part_VolDescSeqNum = ulVolumeDescSeqNum++;
    pPart->nsr_part_Flags = NSR_PART_F_ALLOCATION;      // volume space allocated
    pPart->nsr_part_Number = PARTITION_NUMBER;

	
	CopyStruct( pPart->nsr_part_ContentsID, *GetPartitionContentsID() );  // "+NSR02"

    pPartHeader = (PNSR_PART_H) pPart->nsr_part_ContentsUse;
    pPartHeader->nsr_part_h_UASTable.shortad_Length = 0;
    pPartHeader->nsr_part_h_UASBitmap.shortad_Length = 0;
    pPartHeader->nsr_part_h_FreedTable.shortad_Length = 0;
    pPartHeader->nsr_part_h_FreedBitmap.shortad_Length = 0;

    pPart->nsr_part_AccessType = NSR_PART_ACCESS_RO;    // Read Only Access

    ASSERT( ulPartitionLength != 0 );

    pPart->nsr_part_Start = ulPartitionStart;
    pPart->nsr_part_Length = ulPartitionLength ;

    ASSERT( ISALIGN2( pPart->nsr_part_Start, 16 ) );

    CopyStruct(pPart->nsr_part_ImpUseID, *GetImplementationID());


    // Fill in Descriptor Tag
    pPart->nsr_part_destag.destag_Ident = DESTAG_ID_NSR_PART;
    pPart->nsr_part_destag.destag_Version = GetDesTagVersion();
    pPart->nsr_part_destag.destag_CRCLen = (USHORT)( sizeof( NSR_PART ) - sizeof( DESTAG ));
    pPart->nsr_part_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pPart+sizeof(DESTAG), pPart->nsr_part_destag.destag_CRCLen );
    pPart->nsr_part_destag.destag_lsn = SectorNumber;

   pPart->nsr_part_destag.destag_Checksum = 0;

    // Fill in checksum last
    pPart->nsr_part_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pPart->nsr_part_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteUasdDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_UASD pUasd;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pUasd = (PNSR_UASD) pBuffer;

    pUasd->nsr_uasd_VolDescSeqNum = ulVolumeDescSeqNum++;
    pUasd->nsr_uasd_ExtentCount = 0;            // No unallocated space

    // Fill in Descriptor Tag
    pUasd->nsr_uasd_destag.destag_Ident = DESTAG_ID_NSR_UASD;
    pUasd->nsr_uasd_destag.destag_Version = GetDesTagVersion();
    pUasd->nsr_uasd_destag.destag_CRCLen = (USHORT)( sizeof( NSR_UASD ) - sizeof( DESTAG ));
    pUasd->nsr_uasd_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pUasd+sizeof(DESTAG), pUasd->nsr_uasd_destag.destag_CRCLen );
    pUasd->nsr_uasd_destag.destag_lsn = SectorNumber;

   pUasd->nsr_uasd_destag.destag_Checksum = 0;

    // Fill in checksum last
    pUasd->nsr_uasd_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pUasd->nsr_uasd_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteTermDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    PNSR_TERM pTerm;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pTerm = (PNSR_TERM) pBuffer;

    // Fill in Descriptor Tag
    pTerm->nsr_term_destag.destag_Ident = DESTAG_ID_NSR_TERM;
    pTerm->nsr_term_destag.destag_Version = GetDesTagVersion();
    pTerm->nsr_term_destag.destag_CRCLen = (USHORT)( sizeof( NSR_TERM ) - sizeof( DESTAG ));
    pTerm->nsr_term_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pTerm+sizeof(DESTAG), pTerm->nsr_term_destag.destag_CRCLen );
    pTerm->nsr_term_destag.destag_lsn = SectorNumber;

   pTerm->nsr_term_destag.destag_Checksum = 0;
    // Fill in checksum last
    pTerm->nsr_term_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pTerm->nsr_term_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteFsdDescriptor( ULONG BlockNumber ) {

    PUCHAR pBuffer;
    PNSR_FSD pFsd;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pFsd = (PNSR_FSD) pBuffer;

    ASSERT( BlockNumber == 0 );         // write at the beginning of partition

    CopyStruct( pFsd->nsr_fsd_Time, GlobalNsrTimeStamp );
    pFsd->nsr_fsd_Level = 3;                    // UDF 2.3.2.1
    pFsd->nsr_fsd_LevelMax = 3;                 // UDF 2.3.2.2
    pFsd->nsr_fsd_CharSetList = 0x00000001;     // UDF 2.3.2.3
    pFsd->nsr_fsd_CharSetListMax = 0x00000001;  // UDF 2.3.2.3
    pFsd->nsr_fsd_FileSet = 1;                  // ??
    ASSERT( pFsd->nsr_fsd_FileSetDesc == 0 );
    CopyStruct( pFsd->nsr_fsd_charspecVolID, OSTAUnicode );
    if (dstring(OSTA16BitUnicode(wchVolumeLabel, 32, OSTABuffer), 128, dstrBuffer))
        memcpy( pFsd->nsr_fsd_VolID, dstrBuffer, 128);
    CopyStruct( pFsd->nsr_fsd_charspecFileSet, OSTAUnicode );
    if (dstring(OSTABuffer, 32, dstrBuffer))
        memcpy( pFsd->nsr_fsd_FileSetID, dstrBuffer, 32);
    pFsd->nsr_fsd_icbRoot.longad_Length = CD_SECTORSIZE;         // 1 direct ICB
    pFsd->nsr_fsd_icbRoot.longad_Start.nsr_lba_lbn =
        ulFSDExtentStart + ulFSDExtentBlocks;         // root ICB immediately follows FSD extent

    pFsd->nsr_fsd_icbRoot.longad_Start.nsr_lba_PartRef = 0;
    CopyStruct( pFsd->nsr_fsd_DomainID, *GetUDFDomainID() );
    ASSERT( pFsd->nsr_fsd_NextExtent.longad_Length == 0 );      // shall be the only FSD


    // Fill in Descriptor Tag
    pFsd->nsr_fsd_destag.destag_Ident = DESTAG_ID_NSR_FSD;
    pFsd->nsr_fsd_destag.destag_Version = GetDesTagVersion();
    pFsd->nsr_fsd_destag.destag_CRCLen = (USHORT)( sizeof( NSR_FSD ) - sizeof( DESTAG ));
    pFsd->nsr_fsd_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pFsd+sizeof(DESTAG), pFsd->nsr_fsd_destag.destag_CRCLen );
    pFsd->nsr_fsd_destag.destag_lsn = BlockNumber;

   pFsd->nsr_fsd_destag.destag_Checksum= 0;
    // Fill in checksum last
    pFsd->nsr_fsd_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pFsd->nsr_fsd_destag, sizeof (DESTAG) );


    HeaderWrite((DWORDLONG)(ulPartitionStart + BlockNumber) * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteFsdTerminator( ULONG BlockNumber ) {

    PUCHAR pBuffer;
    PNSR_TERM pTerm;

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pTerm = (PNSR_TERM) pBuffer;

    ASSERT( BlockNumber == 1 );         // shall be immediately following FSD

    // Fill in Descriptor Tag
    pTerm->nsr_term_destag.destag_Ident = DESTAG_ID_NSR_TERM;
    pTerm->nsr_term_destag.destag_Version = GetDesTagVersion();
    pTerm->nsr_term_destag.destag_CRCLen = (USHORT)( sizeof( NSR_TERM ) - sizeof( DESTAG ));
    pTerm->nsr_term_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pTerm+sizeof(DESTAG), pTerm->nsr_term_destag.destag_CRCLen );
    pTerm->nsr_term_destag.destag_lsn = BlockNumber;

   pTerm->nsr_term_destag.destag_Checksum = 0;
    // Fill in checksum last
    pTerm->nsr_term_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pTerm->nsr_term_destag, sizeof (DESTAG) );

    HeaderWrite((DWORDLONG)(ulPartitionStart + BlockNumber) * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteUDFAnchorAndVDS( void ) {

    ULONG SectorNumber = ANCHOR_SECTOR;         // Write anchor at sector 256

	// fix up partition length to take into account sparseness, hard links and chained extents
    ulPartitionLength = (dwTotalImageBlocks - ulPartitionStart); // + 1;

    WriteAnchorDescriptor( SectorNumber++ );
	SectorNumber++;
    // Write Main VDS
    SectorNumber = ulMainVDSExtentStart;
    WritePvdDescriptor( SectorNumber++ );
    WriteLvolDescriptor( SectorNumber++ );
    WritePartDescriptor( SectorNumber++ );
    WriteUasdDescriptor( SectorNumber++ );
    WriteImpUseVolDescriptor( SectorNumber++);
    WriteTermDescriptor( SectorNumber++ );

    if ( ( ulMainVDSExtentStart + ulMainVDSExtentBlocks ) > SectorNumber ) {

        ULONG ZeroSectors = ulMainVDSExtentStart
                          + ulMainVDSExtentBlocks
                          - SectorNumber;

        WriteZeroedSectors( SectorNumber, ZeroSectors );
        SectorNumber = ulMainVDSExtentStart + ulMainVDSExtentBlocks;
        }

    // Write Logical Volume Integrity Sequence
    ASSERT( SectorNumber == ulLVISExtentStart );
    WriteLvIntegDescriptor( SectorNumber++ );
    WriteTermDescriptor( SectorNumber++ );

    if ( ( ulLVISExtentStart + ulLVISExtentBlocks ) > SectorNumber ) {

        ULONG ZeroSectors = ulLVISExtentStart
                          + ulLVISExtentBlocks
                          - SectorNumber;

        WriteZeroedSectors( SectorNumber, ZeroSectors );
        SectorNumber = ulLVISExtentStart + ulLVISExtentBlocks;
        }

    // Write Reserve VDS
    ASSERT( SectorNumber == ulReserveVDSExtentStart );
    WritePvdDescriptor( SectorNumber++ );
    WriteLvolDescriptor( SectorNumber++ );
    WritePartDescriptor( SectorNumber++ );
	WriteUasdDescriptor( SectorNumber++ );
    WriteImpUseVolDescriptor( SectorNumber++);
    WriteTermDescriptor( SectorNumber++ );

    if ( ( ulReserveVDSExtentStart + ulReserveVDSExtentBlocks ) > SectorNumber ) {

        ULONG ZeroSectors = ulReserveVDSExtentStart
                          + ulReserveVDSExtentBlocks
                          - SectorNumber;

        WriteZeroedSectors( SectorNumber, ZeroSectors );
        SectorNumber = ulReserveVDSExtentStart + ulReserveVDSExtentBlocks;
        }

    if ( SectorNumber < ulPartitionStart ) {
        WriteZeroedSectors( SectorNumber, ulPartitionStart - SectorNumber );
        SectorNumber = ulPartitionStart;
        }


    // now partition should start here
    ASSERT( ulPartitionStart == SectorNumber);

    // partition start should be 16 sector (32K byte) aligned
    ASSERT( ISALIGN2( ulPartitionStart, 16 ) );

    ASSERT( ulFSDExtentStart == 0 );    // sector offset zero within partition

    SectorNumber = ulFSDExtentStart;
    WriteFsdDescriptor( SectorNumber++ );
    WriteFsdTerminator( SectorNumber++ );

    ASSERT( SectorNumber == ulFSDExtentStart + ulFSDExtentBlocks );

    }

/***
 *      Returns pointer to byte position to write the next FID
 *
 */
PUCHAR MakeFID(PUCHAR pBuffer, PDIRENTRY pFile, ULONG FidBlockNumber) {

    PNSR_FID pFid = (PNSR_FID) pBuffer;
    size_t cbStruct;
    ULONG dwNameLength;
    LPWSTR pszUnicodeName;

    if(!(pFile->dwFlags & IS_DIRECTORY) && (pFile->dwlFileSize == 0))
    {
        BYTE* pBuffer;
        PICBFILE pIcb;
        DWORDLONG dwlOffset;

#ifdef UDF_DBG
        printf("Zero-Length file [%S] found\r\n",pFile->pszUnicodeName);
#endif

        // zero length files didn't go through the file transfer code
        pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );
        pIcb = (PICBFILE) pBuffer;


        // if optimization is on and another 0 length file exists, lets link'em
        if(bZeroLengthIcbSaved && bOptimizeStorage)
        {
            pFile->pUdfInfo->ulICBBlockNumber = dwZeroLengthIcb;
            dwlOffset = (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;

            // already there, just update ICB link count
            GenericRead(dwlOffset,CD_SECTORSIZE,pBuffer);
            pIcb->icbfile_LinkCount++;
            pFile->pUdfInfo->ulLinkCount=pIcb->icbfile_LinkCount;

            dwlTotalDupFileBytes=CD_SECTORSIZE;
            gdwSpaceSaved+=CD_SECTORSIZE;
            dwlTotalDupFiles++;

			if(bShowDuplicates)
                printf("\r\nZero-Length Duplicate file [%S] found",pFile->pszUnicodeName);
        }
        else
        {
            pIcb->icbfile_icbtag.icbtag_Flags = ICBTAG_F_ALLOC_IMMEDIATE;
            pIcb->icbfile_AllocLen = 0;
            pIcb->icbfile_BlocksRecorded = 0;
            pIcb->icbfile_LinkCount=1;

            // what is the next available sector?
            pFile->pUdfInfo->ulICBBlockNumber=dwTotalImageBlocks++;
            pFile->pUdfInfo->ulLinkCount=1;

            bZeroLengthIcbSaved=TRUE;
            dwZeroLengthIcb=pFile->pUdfInfo->ulICBBlockNumber;

            dwlOffset = (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;
        }

        ASSERT( pIcb->icbfile_LinkCount > 0);
        HeaderWrite(dwlOffset,pIcb,CD_SECTORSIZE);

        // this should finish up the ICBfile struct with the right times, CRC and everything

        // BUGBUG - Might have to come back to this
        WriteICBFileEntry(pFile,(DWORDLONG) -1);

        // I need to restore the block number because WriteICBFileEntry messes with it
        pFile->pUdfInfo->ulICBBlockNumber = dwZeroLengthIcb;
    }

    pFid->nsr_fid_Version = 1;          // UDF 2.3.4.1

    if ( pFile->dwFlags & IS_DIRECTORY )
        pFid->nsr_fid_Flags |= NSR_FID_F_DIRECTORY;

    if ( pFile->dwFlags & IS_HIDDEN )
        pFid->nsr_fid_Flags |= NSR_FID_F_HIDDEN;

    pszUnicodeName = pFile->pszUnicodeName;

    if ( pszUnicodeName ) {
        dwNameLength = pFile->wUnicodeNameLength * 2;
        }
    else {
        pszUnicodeName = AnsiToUnicode( pFile->pszFileName, wchStaticUnicodeBuffer );
        dwNameLength = pFile->wFileNameLength * 2;
        }

    ASSERT( pszUnicodeName != NULL );
    ASSERT( dwNameLength != 0 );
	ASSERT( (dwNameLength % 2) == 0);

//	DEBUGCODE( wprintf(L"File name = %s ",pszUnicodeName) );

    if (bUdf8BitFileNames) {
        pFid->nsr_fid_FileIDLen = (UCHAR)( OSTA8BitUnicodeLen( dwNameLength ));
    } else {
        pFid->nsr_fid_FileIDLen = (UCHAR)( OSTA16BitUnicodeLen( dwNameLength ));
    }

//	ASSERT((pFid->nsr_fid_FileIDLen % 2) == 0);

    pFid->nsr_fid_icb.longad_Length = CD_SECTORSIZE;
    if(pFile->dwFlags & IS_DIRECTORY )
        pFid->nsr_fid_icb.longad_Start.nsr_lba_lbn = pFile->pUdfInfo->ulICBBlockNumber;
    else
        pFid->nsr_fid_icb.longad_Start.nsr_lba_lbn = pFile->pUdfInfo->ulICBBlockNumber - ulPartitionStart;

	pFid->nsr_fid_icb.longad_Start.nsr_lba_PartRef = 0;

    ASSERT( pFid->nsr_fid_ImpUseLen == 0 );
    if (bUdf8BitFileNames) {
        memcpy( pBuffer+38, OSTA8BitUnicode( pszUnicodeName, dwNameLength, OSTABuffer ), pFid->nsr_fid_FileIDLen );
    } else {
        memcpy( pBuffer+38, OSTA16BitUnicode( pszUnicodeName, dwNameLength, OSTABuffer ), pFid->nsr_fid_FileIDLen );
    }

//	DEBUGCODE( wprintf(L"Recorded file name = %s", ((PBYTE) pFid->nsr_fid_ImpUse) + pFid->nsr_fid_ImpUseLen) );


    // Fill in Descriptor Tag
    pFid->nsr_fid_destag.destag_Ident = DESTAG_ID_NSR_FID;
    pFid->nsr_fid_destag.destag_Version = GetDesTagVersion();
    pFid->nsr_fid_destag.destag_CRCLen =
        (USHORT)(( cbStruct = ( ( 38 + pFid->nsr_fid_FileIDLen + 3 ) & ~3 ) ) - sizeof( DESTAG ));
    pFid->nsr_fid_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pFid+sizeof(DESTAG), pFid->nsr_fid_destag.destag_CRCLen );
    pFid->nsr_fid_destag.destag_lsn = FidBlockNumber;

   pFid->nsr_fid_destag.destag_Checksum = 0;
    // Fill in checksum last
    pFid->nsr_fid_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pFid->nsr_fid_destag, sizeof (DESTAG) );

    return ( pBuffer + cbStruct );

    }


/***
 *      Returns pointer to byte position to write the next FID
 *
 */

PUCHAR MakeParentFID(PUCHAR pBuffer, PDIRENTRY pDir, ULONG FidBlockNumber) {

    PNSR_FID pFid = (PNSR_FID) pBuffer;

    ASSERT( pDir->dwFlags & IS_DIRECTORY );
    ASSERT( pDir->pParentDir->dwFlags & IS_DIRECTORY );

    pFid->nsr_fid_Version = 1;          // UDF 2.3.4.1
    pFid->nsr_fid_Flags = NSR_FID_F_PARENT | NSR_FID_F_DIRECTORY;
    pFid->nsr_fid_FileIDLen = 0;        // 4/14.4.4: Parent has IDLen of 0

    pFid->nsr_fid_icb.longad_Length = CD_SECTORSIZE;
    pFid->nsr_fid_icb.longad_Start.nsr_lba_lbn = pDir->pParentDir->pUdfInfo->ulICBBlockNumber;

    pFid->nsr_fid_icb.longad_Start.nsr_lba_PartRef = 0;

    ASSERT( pFid->nsr_fid_ImpUseLen == 0 );

    // Fill in Descriptor Tag
    pFid->nsr_fid_destag.destag_Ident = DESTAG_ID_NSR_FID;
    pFid->nsr_fid_destag.destag_Version = GetDesTagVersion();
    pFid->nsr_fid_destag.destag_CRCLen = (USHORT)( PARENTFID_LEN - sizeof( DESTAG ));
    pFid->nsr_fid_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pFid+sizeof(DESTAG), pFid->nsr_fid_destag.destag_CRCLen );
    pFid->nsr_fid_destag.destag_lsn = FidBlockNumber;

   pFid->nsr_fid_destag.destag_Checksum = 0;
    // Fill in checksum last
    pFid->nsr_fid_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pFid->nsr_fid_destag, sizeof (DESTAG) );

    return ( pBuffer + PARENTFID_LEN );

    }

void GetFullPath (PDIRENTRY pFile, LPWSTR PathName)
{
   if (!pFile || pFile->pParentDir == pFile)
   {
      AnsiToUnicode(pszSourceTree, PathName);
      return;
   }

   GetFullPath(pFile->pParentDir, PathName);
   lstrcatW(PathName, L"\\");
   lstrcatW(PathName, pFile->pszUnicodeName);
}

void WriteICBFileEntry( PDIRENTRY pFile, DWORDLONG UniqueID )
{
    PUCHAR pBuffer, pMem, pStartFID;
    PDIRENTRY pSubFile;
    PICBFILE pIcb;
    SHORTAD ShortFileExtent;
    USHORT  NumExtents = 1;
    PNSR_XAH pXah;
    PNSR_XA_FILETIMES pXaTime;
    PTIMESTAMP pTimeStamp;
    ULONG      FidBlockNumber;

	if (GetCurrentUDFVersion() >= 0x200)
	{
		WriteICBExtendedFileEntry(pFile,UniqueID);
		return;
	};

    if(pFile->pUdfInfo->ulLinkCount == 0)
    {
        // ICB already written by other linked file
        return;
    }

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );
    pIcb = (PICBFILE) pBuffer;

    // load ICB from image if it's a file's
    if(!(pFile->dwFlags & IS_DIRECTORY))
    {
        DWORDLONG dwlOffset;

        dwlOffset= (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;

        //DEBUGCODE( printf("Block # is %u\n",pFile->pUdfInfo->ulICBBlockNumber) );

        pFile->pUdfInfo->ulICBBlockNumber-=ulPartitionStart;    // adjust the ICB to fit the partition
        GenericRead(dwlOffset,CD_SECTORSIZE,pBuffer);
        ASSERT( pIcb->icbfile_LinkCount > 0);
    }

    // Fill in ICB Tag
    pIcb->icbfile_icbtag.icbtag_PriorDirectCount = 0;
    pIcb->icbfile_icbtag.icbtag_StratType  = ICBTAG_STRAT_DIRECT;
    pIcb->icbfile_icbtag.icbtag_StratParm  = 0;
    pIcb->icbfile_icbtag.icbtag_MaxEntries = 1;
    pIcb->icbfile_icbtag.icbtag_FileType = (UCHAR)((pFile->dwFlags & IS_DIRECTORY) ?
                                    ICBTAG_FILE_T_DIRECTORY : ICBTAG_FILE_T_FILE );
    ASSERT( pIcb->icbfile_icbtag.icbtag_icbParent.nsr_lba_lbn == 0 );
    ASSERT( (pIcb->icbfile_icbtag.icbtag_Flags == ICBTAG_F_ALLOC_LONG) ||
            (pIcb->icbfile_icbtag.icbtag_Flags == ICBTAG_F_ALLOC_SHORT) ||
            (pIcb->icbfile_icbtag.icbtag_Flags & ICBTAG_F_ALLOC_IMMEDIATE));

    // Fill File fields
    pIcb->icbfile_UID = pIcb->icbfile_GID = (ULONG) ( ( 1 << 32 ) - 1 );    // UDF 3.3.3.1 and .2
    pIcb->icbfile_Permissions =
      ICBFILE_PERM_OTH_X | ICBFILE_PERM_OTH_R | ICBFILE_PERM_OTH_A |
      ICBFILE_PERM_GRP_X | ICBFILE_PERM_GRP_R | ICBFILE_PERM_GRP_A |
      ICBFILE_PERM_OWN_X | ICBFILE_PERM_OWN_R | ICBFILE_PERM_OWN_A;


    ASSERT( pIcb->icbfile_RecordFormat  == 0 );          // UDF 2.3.6.1
    ASSERT( pIcb->icbfile_RecordDisplay == 0 );         // UDF 2.3.6.2
    ASSERT( pIcb->icbfile_RecordLength  == 0 );          // UDF 2.3.6.3

    *(UNALIGNED DWORDLONG*)&(pIcb->icbfile_InfoLength) = (pFile->dwFlags & IS_DIRECTORY) ?
                                                pFile->pUdfInfo->ulFIDLength :
                                                pFile->dwlFileSize;
	// file already set
	if(pFile->dwFlags & IS_DIRECTORY)
    {
        pIcb->icbfile_LinkCount = (USHORT)pFile->pUdfInfo->ulLinkCount;
		pIcb->icbfile_BlocksRecorded = CD_BLOCKS_FROM_SIZE (*(UNALIGNED DWORDLONG*)&(pIcb->icbfile_InfoLength));
    }

    //DEBUGCODE( printf("Link count = %u\n", (USHORT) pFile->pUdfInfo->ulLinkCount) );
    ASSERT( pIcb->icbfile_LinkCount > 0);


    if ( bUseGlobalTime )
	{
        CopyStruct( pIcb->icbfile_AccessTime, GlobalNsrTimeStamp );
        CopyStruct( pIcb->icbfile_ModifyTime, GlobalNsrTimeStamp );
        CopyStruct( pIcb->icbfile_AttributeTime, GlobalNsrTimeStamp );
    }
    else
	{
        ASSERT( *(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastWriteTime) != 0 );
        MakeNsrTimeStampFromFileTime( &(pIcb->icbfile_ModifyTime), pFile->pFileTimeInfo->ftLastWriteTime );

        // LastAccessTime may be 0 (e.g. on CDFS)
        // we don't want to encode 0 for UDF's LastAccessTime
        if ( *(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastAccessTime) != 0 )
            MakeNsrTimeStampFromFileTime( &(pIcb->icbfile_AccessTime), pFile->pFileTimeInfo->ftLastAccessTime );
        else
            CopyStruct( pIcb->icbfile_AccessTime, pIcb->icbfile_ModifyTime );

        CopyStruct( pIcb->icbfile_AttributeTime, pIcb->icbfile_ModifyTime );
    }

    pIcb->icbfile_Checkpoint = 1;   // ISO 4/14.0.15

    CopyStruct(pIcb->icbfile_ImpUseID, *GetImplementationID());

    if(UniqueID != (DWORDLONG) -1)
    {
        *(UNALIGNED DWORDLONG*)&(pIcb->icbfile_UniqueID) = UniqueID;
    }

    pIcb->icbfile_XALength = XA_LENGTH;

    // fill in File Creation Time in XA File Times field
    pXaTime = (PNSR_XA_FILETIMES) ( pIcb->icbfile_XAs + 24 );
    pXaTime->nsr_xa_filetimes_XAType     = 5;   // 4/14/10.5
    pXaTime->nsr_xa_filetimes_XASubType  = 1;
    pXaTime->nsr_xa_filetimes_XALength   = 32;  // length of this EA:
                                                // 20 (File Time EA format) + 12 (one timestamp)
    pXaTime->nsr_xa_filetimes_DataLength = 12;  // one timestamp
    pXaTime->nsr_xa_filetimes_Existence  = XA_FILETIMES_E_CREATION;

    pTimeStamp = (PTIMESTAMP) &(pXaTime->nsr_xa_filetimes_XAData);
    if ( bUseGlobalTime )
    {
       CopyStruct( *pTimeStamp,     GlobalNsrTimeStamp );   // Create Date
    }
    else
	{
        // ISO13346 4/14.9.12 & 4/14.9.13 stipulate that LastAccessTime
        // and LastWriteTime not be earlier than CreationTime
        FILETIME ftCreationTime = pFile->pFileTimeInfo->ftCreationTime;

        if (*(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastAccessTime) < *(UNALIGNED DWORDLONG*)&ftCreationTime)
            ftCreationTime = pFile->pFileTimeInfo->ftLastAccessTime;
        if (*(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastWriteTime) < *(UNALIGNED DWORDLONG*)&ftCreationTime)
            ftCreationTime = pFile->pFileTimeInfo->ftLastWriteTime;
        if (*(UNALIGNED DWORDLONG*)&(ftCreationTime) == 0)    // don't want to encode a 0 timestamp
            ftCreationTime = pFile->pFileTimeInfo->ftLastWriteTime;

        MakeNsrTimeStampFromFileTime( pTimeStamp,   ftCreationTime );   // Create Date
	}

    pXah = (PNSR_XAH) &(pIcb->icbfile_XAs);
    if (GetCurrentUDFVersion() < 0x200)
	{
		pXah->nsr_xah_XAImp = XA_LENGTH;            // point to the end of XA field
		pXah->nsr_xah_XAApp = XA_LENGTH;            // see UDF 3.3.4.1
    }
	else
	{
	    pXah->nsr_xah_XAImp = 0xFFFFFFFF;
		pXah->nsr_xah_XAApp = 0xFFFFFFFF;          //UDF 2.00+ 3.3.4.1
	}

    pXah->nsr_xah_destag.destag_Ident   = DESTAG_ID_NSR_XA;
    pXah->nsr_xah_destag.destag_Version = GetDesTagVersion();
    pXah->nsr_xah_destag.destag_CRCLen  = 8;
    pXah->nsr_xah_destag.destag_CRC     =
        Crc16( 0, (PUCHAR)pXah+sizeof(DESTAG), pXah->nsr_xah_destag.destag_CRCLen );
    pXah->nsr_xah_destag.destag_lsn = pFile->pUdfInfo->ulICBBlockNumber;

    pXah->nsr_xah_destag.destag_Checksum = 0;
    // Fill in checksum last
    pXah->nsr_xah_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pXah->nsr_xah_destag, sizeof (DESTAG) );

    if ( pFile->dwFlags & IS_DIRECTORY )
    {
      if (bVerbose)
         printf("FIDLength: [0x%lx]\r\n", pFile->pUdfInfo->ulFIDLength);

      if (bUdfEmbedFID && (pFile->pUdfInfo->ulFIDLength <= (CD_SECTORSIZE - (176+XA_LENGTH))))
      {
         FidBlockNumber = pFile->pUdfInfo->ulICBBlockNumber;

         if (bVerbose)
            printf("FIDBlockNumber: [0x%lx]\r\n", FidBlockNumber);

         pStartFID = pBuffer + 176 + XA_LENGTH;

         pMem = MakeParentFID( pStartFID, pFile, FidBlockNumber );

         for ( pSubFile = pFile->pDirectoryInfo->pFirstTargetFile; pSubFile; pSubFile = pSubFile->pNextTargetFile )
         {
             pMem = MakeFID( pMem, pSubFile, FidBlockNumber);
         }

         // We found a directory where we can embed the FIDs!
         pIcb->icbfile_icbtag.icbtag_Flags |= ICBTAG_F_ALLOC_IMMEDIATE;
         pIcb->icbfile_AllocLen = pMem - pStartFID;
         ASSERT ( ( 176 + XA_LENGTH + (USHORT)pIcb->icbfile_AllocLen ) <= CD_SECTORSIZE );

         if (bVerbose)
            printf("Embedded FIDs for: [%ls]!\r\n", pFile->pszUnicodeName);

         pIcb->icbfile_BlocksRecorded = 0;
         gdwSpaceSaved+=CD_SECTORSIZE;
      }
      else
      {
         // Directory ICB points to FID extent
         // FID extent starts the next block following the ICB
         ShortFileExtent.shortad_Length = pFile->pUdfInfo->ulFIDLength;
         ShortFileExtent.shortad_Start  = pFile->pUdfInfo->ulICBBlockNumber + 1;
         pIcb->icbfile_AllocLen = NumExtents * sizeof (SHORTAD);
         memcpy(pBuffer+176+XA_LENGTH, &ShortFileExtent, pIcb->icbfile_AllocLen);
      }
    }

    // Fill in Descriptor Tag
    pIcb->icbfile_destag.destag_Ident    = DESTAG_ID_NSR_FILE;
    pIcb->icbfile_destag.destag_Version  = GetDesTagVersion();
    pIcb->icbfile_destag.destag_CRCLen   = (USHORT)(( 176 + XA_LENGTH + (USHORT)pIcb->icbfile_AllocLen ) - sizeof( DESTAG ));
    pIcb->icbfile_destag.destag_CRC =
        Crc16( 0, (PUCHAR)pIcb+sizeof(DESTAG), pIcb->icbfile_destag.destag_CRCLen );
    pIcb->icbfile_destag.destag_lsn = pFile->pUdfInfo->ulICBBlockNumber;

    pIcb->icbfile_destag.destag_Checksum = 0;
    // Fill in checksum last
    pIcb->icbfile_destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pIcb->icbfile_destag, sizeof (DESTAG) );

    ASSERT ( ( 176 + XA_LENGTH + (USHORT)pIcb->icbfile_AllocLen ) <= CD_SECTORSIZE );

    {
        DWORDLONG dwlICBOffset;
        dwlICBOffset = ((DWORDLONG)ulPartitionStart + (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber) * (DWORDLONG) CD_SECTORSIZE;

		if(bVerbose)
			printf("Offset is %I64u\r\n",dwlICBOffset);

        HeaderWrite(dwlICBOffset, pBuffer, CD_SECTORSIZE );
    }

}

void WriteICBExtendedFileEntry( PDIRENTRY pFile, DWORDLONG UniqueID )
{
    PUCHAR pBuffer, pMem, pStartFID;
    PDIRENTRY pSubFile;
    PICBEXTFILE pIcb;
    SHORTAD ShortFileExtent;
    USHORT  NumExtents = 1;

    ULONG      FidBlockNumber;

	ASSERT(GetCurrentUDFVersion() >= 0x200);

    if(pFile->pUdfInfo->ulLinkCount == 0)
    {
        // ICB already written by other linked file
        return;
    }

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );
    pIcb = (PICBEXTFILE) pBuffer;

    // load ICB from image if it's a file's
    if(!(pFile->dwFlags & IS_DIRECTORY))
    {
        DWORDLONG dwlOffset;
		PICBFILE  pICBBuffer;

		pICBBuffer =  AllocateBuffer( CD_SECTORSIZE, TRUE );
        dwlOffset= (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;

        //DEBUGCODE( printf("Block # is %u\n",pFile->pUdfInfo->ulICBBlockNumber) );

        pFile->pUdfInfo->ulICBBlockNumber-=ulPartitionStart;    // adjust the ICB to fit the partition
        GenericRead(dwlOffset,CD_SECTORSIZE,pICBBuffer);
		
		// This has to be done because, part of the ICB (allocation info)
		// is written by  the thread which writes file data.
		// And that info is written as a File Entry.
		// So, we have to change it to an extended
		// Entry and rewrite that information

		CopyStruct(pIcb->Destag,pICBBuffer->icbfile_destag);
		CopyStruct(pIcb->Icbtag,pICBBuffer->icbfile_icbtag);
		pIcb->UID = pICBBuffer->icbfile_UID;
		pIcb->GID = pICBBuffer->icbfile_GID;
		pIcb->Permissions = pICBBuffer->icbfile_Permissions;
		pIcb->LinkCount = pICBBuffer->icbfile_LinkCount;
		pIcb->Permissions = pICBBuffer->icbfile_Permissions;
		// Record info should be zero so not copying
		pIcb->InfoLength = pICBBuffer->icbfile_InfoLength;
		pIcb->BlocksRecorded = pICBBuffer->icbfile_BlocksRecorded;
		//Access time etc are set later
		CopyStruct(pIcb->IcbEA,pICBBuffer->icbfile_icbXA);
		pIcb->AllocLength =  pICBBuffer->icbfile_AllocLen;
		//Implementation ID and Unique ID are set later
		memcpy(pIcb->EAs
			,pICBBuffer->icbfile_XAs+XA_LENGTH
			,pICBBuffer->icbfile_AllocLen);
		// Whew! Quite a kludge! but, has to be there till CDImage is redesigned!

		ReleaseBuffer(pICBBuffer);

        ASSERT( pIcb->LinkCount > 0);
    }

    // Fill in ICB Tag
    pIcb->Icbtag.icbtag_PriorDirectCount = 0;
    pIcb->Icbtag.icbtag_StratType  = ICBTAG_STRAT_DIRECT;
    pIcb->Icbtag.icbtag_StratParm  = 0;
    pIcb->Icbtag.icbtag_MaxEntries = 1;
    pIcb->Icbtag.icbtag_FileType = (UCHAR)((pFile->dwFlags & IS_DIRECTORY) ?
                                    ICBTAG_FILE_T_DIRECTORY : ICBTAG_FILE_T_FILE );

    ASSERT( pIcb->Icbtag.icbtag_icbParent.nsr_lba_lbn == 0 );
    ASSERT( (pIcb->Icbtag.icbtag_Flags == ICBTAG_F_ALLOC_LONG) ||
            (pIcb->Icbtag.icbtag_Flags == ICBTAG_F_ALLOC_SHORT) ||
            (pIcb->Icbtag.icbtag_Flags & ICBTAG_F_ALLOC_IMMEDIATE));

    // Fill File fields
    pIcb->UID = pIcb->GID = (ULONG) ( ( 1 << 32 ) - 1 );    // UDF 3.3.3.1 and .2
    pIcb->Permissions =
      ICBFILE_PERM_OTH_X | ICBFILE_PERM_OTH_R | ICBFILE_PERM_OTH_A |
      ICBFILE_PERM_GRP_X | ICBFILE_PERM_GRP_R | ICBFILE_PERM_GRP_A |
      ICBFILE_PERM_OWN_X | ICBFILE_PERM_OWN_R | ICBFILE_PERM_OWN_A;

    // file already set
    if(pFile->dwFlags & IS_DIRECTORY)
    {
        pIcb->LinkCount         = (USHORT)pFile->pUdfInfo->ulLinkCount;
        pIcb->BlocksRecorded    = CD_BLOCKS_FROM_SIZE (pFile->dwlFileSize);
    }

    //DEBUGCODE( printf("Link count = %u\n", (USHORT) pFile->pUdfInfo->ulLinkCount) );
    ASSERT( pIcb->LinkCount > 0);

    ASSERT( pIcb->RecordFormat  == 0 );          // UDF 2.3.6.1
    ASSERT( pIcb->RecordDisplay == 0 );         // UDF 2.3.6.2
    ASSERT( pIcb->RecordLength  == 0 );          // UDF 2.3.6.3

    *(UNALIGNED DWORDLONG*)&(pIcb->InfoLength) = (pFile->dwFlags & IS_DIRECTORY) ?
                                                pFile->pUdfInfo->ulFIDLength :
                                                pFile->dwlFileSize;
    *(UNALIGNED DWORDLONG*)&pIcb->ObjectSize = *(UNALIGNED DWORDLONG*)&(pIcb->InfoLength);

    if ( bUseGlobalTime )
	{
        CopyStruct( pIcb->AccessTime, GlobalNsrTimeStamp );
        CopyStruct( pIcb->ModifyTime, GlobalNsrTimeStamp );
        CopyStruct( pIcb->CreationTime, GlobalNsrTimeStamp );
        CopyStruct( pIcb->AttributeTime, GlobalNsrTimeStamp );
    }
    else
	{
		FILETIME ftCreationTime;
        ASSERT( *(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastWriteTime) != 0 );
        MakeNsrTimeStampFromFileTime( &(pIcb->ModifyTime), pFile->pFileTimeInfo->ftLastWriteTime );

        // LastAccessTime may be 0 (e.g. on CDFS)
        // we don't want to encode 0 for UDF's LastAccessTime
        if ( *(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastAccessTime) != 0 )
            MakeNsrTimeStampFromFileTime( &(pIcb->AccessTime), pFile->pFileTimeInfo->ftLastAccessTime );
        else
            CopyStruct( pIcb->AccessTime, pIcb->ModifyTime );

        CopyStruct( pIcb->AttributeTime, pIcb->ModifyTime );
		
		// ISO13346 4/14.9.12 & 4/14.9.13 stipulate that LastAccessTime
        // and LastWriteTime not be earlier than CreationTime
        ftCreationTime = pFile->pFileTimeInfo->ftCreationTime;

        if (*(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastAccessTime) < *(UNALIGNED DWORDLONG*)&ftCreationTime)
            ftCreationTime = pFile->pFileTimeInfo->ftLastAccessTime;
        if (*(UNALIGNED DWORDLONG*)&(pFile->pFileTimeInfo->ftLastWriteTime) < *(UNALIGNED DWORDLONG*)&ftCreationTime)
            ftCreationTime = pFile->pFileTimeInfo->ftLastWriteTime;
        if (*(UNALIGNED DWORDLONG*)&(ftCreationTime) == 0)    // don't want to encode a 0 timestamp
            ftCreationTime = pFile->pFileTimeInfo->ftLastWriteTime;

        MakeNsrTimeStampFromFileTime( &(pIcb->CreationTime),   ftCreationTime );
    }

    pIcb->Checkpoint = 1;   // ISO 4/14.0.15

    CopyStruct(pIcb->ImpUseID, *GetImplementationID());

    if(UniqueID != (DWORDLONG) -1)
    {
        *(UNALIGNED DWORDLONG*)&(pIcb->UniqueID) = UniqueID;
    }

    pIcb->EALength = 0;


    if ( pFile->dwFlags & IS_DIRECTORY )
    {
      if (bVerbose)
         printf("FIDLength: [0x%lx]\r\n", pFile->pUdfInfo->ulFIDLength);

      if (bUdfEmbedFID && (pFile->pUdfInfo->ulFIDLength <= (CD_SECTORSIZE - (216))))
      {
         FidBlockNumber = pFile->pUdfInfo->ulICBBlockNumber;

         if (bVerbose)
            printf("FIDBlockNumber: [0x%lx]\r\n", FidBlockNumber);

         pStartFID = pBuffer + 216 + 0;//XA_LENGTH;

         pMem = MakeParentFID( pStartFID, pFile, FidBlockNumber );

         for ( pSubFile = pFile->pDirectoryInfo->pFirstTargetFile; pSubFile; pSubFile = pSubFile->pNextTargetFile )
         {
             pMem = MakeFID( pMem, pSubFile, FidBlockNumber);
         }

         // We found a directory where we can embed the FIDs!
         pIcb->Icbtag.icbtag_Flags |= ICBTAG_F_ALLOC_IMMEDIATE;
         pIcb->AllocLength = pMem - pStartFID;
         ASSERT ( ( 216 + (USHORT)pIcb->AllocLength ) <= CD_SECTORSIZE );

         if (bVerbose)
            printf("Embedded FIDs for: [%ls]!\r\n", pFile->pszUnicodeName);

         pIcb->BlocksRecorded = 0;
         gdwSpaceSaved+=CD_SECTORSIZE;
      }
      else
      {
         // Directory ICB points to FID extent
         // FID extent starts the next block following the ICB
         ShortFileExtent.shortad_Length = pFile->pUdfInfo->ulFIDLength;
         ShortFileExtent.shortad_Start  = pFile->pUdfInfo->ulICBBlockNumber + 1;
         pIcb->AllocLength = NumExtents * sizeof (SHORTAD);
         memcpy(pBuffer+216, &ShortFileExtent, pIcb->AllocLength);
      }
    }

    // Fill in Descriptor Tag
    pIcb->Destag.destag_Ident    = DESTAG_ID_NSR_EXTFILE;
    pIcb->Destag.destag_Version  = GetDesTagVersion();
    pIcb->Destag.destag_CRCLen   = (USHORT)(( 216 + (USHORT)pIcb->AllocLength ) - sizeof( DESTAG ));
    pIcb->Destag.destag_CRC =
        Crc16( 0, (PUCHAR)pIcb+sizeof(DESTAG), pIcb->Destag.destag_CRCLen );
    pIcb->Destag.destag_lsn = pFile->pUdfInfo->ulICBBlockNumber;

    pIcb->Destag.destag_Checksum = 0;
    // Fill in checksum last
    pIcb->Destag.destag_Checksum =
        ComputeChecksum( (PUCHAR)&pIcb->Destag, sizeof (DESTAG) );

    ASSERT ( ( 216 +  (USHORT)pIcb->AllocLength ) <= CD_SECTORSIZE );

    {
        DWORDLONG dwlICBOffset;
        dwlICBOffset = ((DWORDLONG)ulPartitionStart + (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber) * (DWORDLONG) CD_SECTORSIZE;

		if(bVerbose)
			printf("Offset is %I64u\r\n",dwlICBOffset);

        HeaderWrite(dwlICBOffset, pBuffer, CD_SECTORSIZE );
    }

}


void ComputeUDFDirectorySizes( void ) {

    PDIRENTRY pDir, pFile;
    ULONG ulFIDLength;
    DWORD dwNameLength;
    UINT uLevel;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {

        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            // at least 1 ICB per directory
            g_dwIcbBlockCount++;

            dwNameLength = pDir->wUnicodeNameLength ?
                           pDir->wUnicodeNameLength :
                           pDir->wFileNameLength;

            ASSERT( dwNameLength != 0 );

            // each directory has one PARENTFID (..) entry
            ulFIDLength = PARENTFID_LEN;

            if ( pDir->dwFlags & IS_DIRECTORY )
                pDir->pUdfInfo->ulLinkCount = 1;        // Start from 1 for all directories

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                dwNameLength = pFile->wUnicodeNameLength ?
                               pFile->wUnicodeNameLength :
                               pFile->wFileNameLength;

                ASSERT( dwNameLength != 0 );

                // each file/subdir has a FID entry
                // each FID entry is 38+Length_of_FileID bytes long
                // rounded up to the next ULONG boundary

                if (bUdf8BitFileNames) {
                    ulFIDLength += ( 38 + OSTA8BitUnicodeLen(dwNameLength * 2) + 3 ) & ~3;
                } else {
                    ulFIDLength += ( 38 + OSTA16BitUnicodeLen(dwNameLength * 2) + 3 ) & ~3;
                }

                if ( pFile->dwFlags & IS_DIRECTORY )
                {
                    pDir->pUdfInfo->ulLinkCount++;  // Increment for each subdirectories contained
                }
                else
                {
                    // at least 1 ICB per file
                    g_dwIcbBlockCount++;

                }
            }

            pDir->pUdfInfo->ulFIDLength = ulFIDLength;

            }
        }

    }

void AssignUDFDirectoryBlockNumbers( void )
{

    DWORD     dwNextBlock;
    PDIRENTRY pDir;
    UINT      uLevel;

    ASSERT( dwNextMetaBlockNumber > ANCHOR_SECTOR );

    //
    //  Implementation note:  no longer necessary to call AllocateMetaBlocks
    //  since we're guaranteed to be past the anchor sector now, so we'll
    //  use and modify the dwNextMetaBlockNumber value directly.
    //

    // Main VDS starts at next available meta block.

    ulMainVDSExtentStart  = dwNextMetaBlockNumber;
    ulMainVDSExtentBlocks = 16;         // UDF 2.2.3.1

    // Logical Volume Integrity Sequence follows Main VDS
    ulLVISExtentStart = ulMainVDSExtentStart + ulMainVDSExtentBlocks;
    ulLVISExtentBlocks = 2;            // LVID and Terminator

    // Reserve VDS starts immediately after Main VDS and LVID
    ulReserveVDSExtentStart =  ulLVISExtentStart + ulLVISExtentBlocks;
    ulReserveVDSExtentBlocks = 16;

    // FSD Extent starts at logical block 0 (within partition)
    ulFSDExtentStart = 0;
    ulFSDExtentBlocks = 2;              // FSD and FSDTerminator

    // partition starts immediately after Reserve VDS
    // partition start shall be 16 sector (32K byte) aligned
    // leaving empty sectors that will be zero-filled during WriteUDFAnchorAndVDS()
    ulPartitionStart = ulReserveVDSExtentStart + ulReserveVDSExtentBlocks;
    ulPartitionStart = ROUNDUP2( ulPartitionStart, 16 );

    // directory structures immediately follow FSD extent
    dwNextBlock = ulFSDExtentStart + ulFSDExtentBlocks;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ )
    {
       for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir )
       {
          ASSERT( pDir->pDirectoryInfo != NULL );
          ASSERT( pDir->pUdfInfo != NULL );
          ASSERT( pDir->pUdfInfo->ulFIDLength != 0 );

          // each dir has 1 ICB and FID entries
          if ( pDir->dwFlags & IS_DIRECTORY )
          {
            pDir->pUdfInfo->ulICBBlockNumber = dwNextBlock++;
            if (!bUdfEmbedFID || (pDir->pUdfInfo->ulFIDLength > CD_SECTORSIZE-(176+XA_LENGTH)))
                dwNextBlock += CD_BLOCKS_FROM_SIZE (pDir->pUdfInfo->ulFIDLength);
          }
          else
          {
              // this is where the file icb's used to be allocated.
              ASSERT(FALSE);
          }
       }
    }


    dwNextMetaBlockNumber = ulPartitionStart + dwNextBlock;
}

void WriteSecondAnchorSector(void)
{
    ASSERT(ulAnchorSectorNumber != 0);
#ifdef UDF_DBG
    printf("Second Anchor sector = %d\r\n",ulAnchorSectorNumber);
#endif
    WriteAnchorDescriptor(ulAnchorSectorNumber);
}

void ComputeUDFPartitionLength( void )
{
    ASSERT( ulPartitionLength == 0 );
    ASSERT( ulPartitionStart  != 0 );

    ulPartitionLength = dwTotalImageBlocks - ulPartitionStart;

    // Take into account padding for second Anchor Sector at 32K boundary
    ulPartitionLength -= ulAnchorSectorNumber - ulAnchorSlackSector + 1;


    if (bVerbose)
    {
      printf("ulPartitionStart:     %d\r\n", ulPartitionStart);
      printf("dwTotalImageBlocks:   %d\r\n", dwTotalImageBlocks);
      printf("ulAnchorSectorNumber: %d\r\n", ulAnchorSectorNumber);
      printf("ulAnchdorSlackSector: %d\r\n", ulAnchorSlackSector);
      printf("ulPartitionLength:    %d\r\n", ulPartitionLength);
    }
}


void WriteUDFDirectories( void )
{

    PDIRENTRY pDir, pFile;
    PUCHAR pBuffer, pMem;
    UINT uLevel;
    DWORD dwFIDAllocation;
    DWORDLONG dwlOffset;
    ULONG FidBlockNumber;

    ASSERT(UniqueID == 0);

    dwlOffset = (DWORDLONG)( ulPartitionStart + ulFSDExtentStart + ulFSDExtentBlocks ) * CD_SECTORSIZE;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ )
    {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir )
        {
            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->pUdfInfo != NULL );
            ASSERT( pDir->pUdfInfo->ulFIDLength != 0 );
            //ASSERT( dwlOffset == (DWORDLONG)( ulPartitionStart + pDir->pUdfInfo->ulICBBlockNumber ) * CD_SECTORSIZE );

            // each directory has one ICB, which points to
            // the FID Extent for the directory

            if (bVerbose)
              printf("Directory ICBBlockNumber: [0x%lx]\r\n", pDir->pUdfInfo->ulICBBlockNumber);


			 WriteICBFileEntry(pDir,UniqueID++);
			
            if ( UniqueID == 1 )        // root dir must have 0
                UniqueID = 16;          // 1-15 reserved for Macintosh

            if (!bUdfEmbedFID || (pDir->pUdfInfo->ulFIDLength > CD_SECTORSIZE-176-XA_LENGTH))
            {
               dwFIDAllocation = ROUNDUP2( pDir->pUdfInfo->ulFIDLength, CD_SECTORSIZE );
               pBuffer = AllocateBuffer( dwFIDAllocation, TRUE );
               FidBlockNumber = pDir->pUdfInfo->ulICBBlockNumber + 1;
               pMem = MakeParentFID( pBuffer, pDir, FidBlockNumber );
               for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile )
               {
                   pMem = MakeFID( pMem, pFile, FidBlockNumber + (DWORD)(pMem-pBuffer)/CD_SECTORSIZE);
               }
               if (bVerbose)
                  printf("FIDBlockNumber: [0x%lx]\r\n", FidBlockNumber);
               dwlOffset = (DWORDLONG)( ulPartitionStart + FidBlockNumber ) * CD_SECTORSIZE;
               HeaderWrite( dwlOffset, pBuffer, dwFIDAllocation );
               DEBUGCODE( dwlOffset += dwFIDAllocation );
            } else {
                   FidBlockNumber = pDir->pUdfInfo->ulICBBlockNumber;
                   dwlOffset = (DWORDLONG) (ulPartitionStart + FidBlockNumber + 1) * CD_SECTORSIZE;
            }

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile )
            {
               if ( !( pFile->dwFlags & IS_DIRECTORY ) )
               {

                  if (bVerbose)
                    printf("File ICBBlockNumber: [0x%lx]\r\n", pFile->pUdfInfo->ulICBBlockNumber);

                  WriteICBFileEntry( pFile, UniqueID++ );
				
                  DEBUGCODE( dwlOffset += CD_SECTORSIZE );
               }
            }
        }
    }



    // UniqueID offset by 16, but root dir has Unique ID of 0
    // hence 15

    DEBUGCODE( printf("Num Dir = %I64u Num Files = %I64u Id = %I64u\n",
                      dwlTotalNumberOfDirectories,
                      dwlTotalNumberOfFiles,
                      UniqueID) );

    ASSERT( ( 15 + dwlTotalNumberOfDirectories + dwlTotalNumberOfFiles  ) == UniqueID);

    }

DWORD NumberOfStartingZeros(BYTE* pBuffer, DWORD dwBufferLength)
{
	DWORD dwCurrentPosition=0;

	if(dwBufferLength == 0)
		return 0;

	while(0 == pBuffer[dwCurrentPosition] && (dwCurrentPosition < dwBufferLength))
	{
		dwCurrentPosition++;
	}

	return dwCurrentPosition;
}

BOOL MakeFileExtent(PDIRENTRY pFile, DWORD* pdwNextFreeBlock, DWORD dwIcbBlock, HANDLE hReadContext)
{
    PICBFILE pIcb;
    DWORD dwStartingBlock;
    BOOL bResult=TRUE;
    HASH_AND_LENGTH HashAndLength;
    PSHORTAD pShortAd;

#ifdef UDF_DBG
    printf( "Make file extent - [%S]\r\n",pFile->pszUnicodeName );
#endif

	SetRandomExtentLength();

    // Max file size
    ASSERT( pFile->dwlFileSize < 0xFFFFFFFF );

    // for duplicate checking
    if(bOptimizeStorage)
    {
        if(bUdfVideoZone)
        {
            ErrorExit(0, "ERROR: Video Zone can't have symbolic links\r\n");
        }

        InitMD5( &HashAndLength.Hash );
        HashAndLength.Length = pFile->dwlFileSize;
    }

    pIcb=(PICBFILE) AllocateBuffer(CD_SECTORSIZE, TRUE);
//    printf("Got ICB memory = %p - %u\r\n",pIcb,GetCurrentThreadId());

    dwStartingBlock=*pdwNextFreeBlock;

	pIcb->icbfile_destag.destag_lsn=dwIcbBlock - ulPartitionStart;
    pFile->pUdfInfo->ulICBBlockNumber=dwIcbBlock;

    //DEBUGCODE( printf("ICB block = %u\n",dwIcbBlock) );

    if (bUdfEmbedData && (pFile->dwlFileSize <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)))
    {
        BYTE* pSrcBuffer;
        BYTE* pDestBuffer;
        DWORD dwBytesRead;

        pDestBuffer =(BYTE*) pIcb;
        pDestBuffer+=ICBFILE_EXTENT_OFFSET;

        WaitForRead(hReadContext,&pSrcBuffer,&dwBytesRead);
        ASSERT(dwBytesRead <= CD_SECTORSIZE);

        CopyMemory(pDestBuffer,pSrcBuffer,dwBytesRead);

        // update hash
        if(bOptimizeStorage)
        {
            DWORD dwBytesToHash;

            // must be 64 byte chunks
            dwBytesToHash=ROUNDUP2( dwBytesRead, 64 );
            // add 0s at end
            if(dwBytesToHash != dwBytesRead)
                ZeroMemory(pSrcBuffer + dwBytesRead, dwBytesToHash - dwBytesRead);

            // make the hash
            UpdateMD5_Multiple64ByteChunks( &HashAndLength.Hash, pSrcBuffer, dwBytesToHash );
        }
        ReleaseBuffer(pSrcBuffer);
        if(dwBytesRead == pFile->dwlFileSize)
        {
            pIcb->icbfile_icbtag.icbtag_Flags = ICBTAG_F_ALLOC_IMMEDIATE;
            pIcb->icbfile_AllocLen = (DWORD)pFile->dwlFileSize;
            pIcb->icbfile_BlocksRecorded = 0;

            gdwSpaceSaved+=CD_SECTORSIZE;

            if (bVerbose)
                printf("Embedded data for: [%ls]!\r\n", pFile->pszUnicodeName);
        }
        else
        {
            ErrorExit( GETLASTERROR, "MakeFileExtent, amount read (%d) did not match file size (%I64d)\r\n", dwBytesRead,pFile->dwlFileSize);
            return FALSE;
        }
    }
    else
    {
        if(bUdfUseLongADs)
        {
            if(bUdfVideoZone)
            {
                ErrorExit(0, "ERROR: Video Zone can't have long extents\r\n");
            }

            pIcb->icbfile_icbtag.icbtag_Flags = ICBTAG_F_ALLOC_LONG;
        }
        else
        {
            pIcb->icbfile_icbtag.icbtag_Flags = ICBTAG_F_ALLOC_SHORT;
        }

        pIcb->icbfile_BlocksRecorded = 0;
        pIcb->icbfile_AllocLen=0;
        ASSERT(pIcb->icbfile_AllocLen == 0);
	    bResult=BuildSparseFileExtent(pdwNextFreeBlock,hReadContext,pIcb,&HashAndLength,pFile->dwlFileSize);
        ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
    }

    if(bResult)
    {
        DWORDLONG dwlOffset;
        PBNODE pBnode = NULL;
        BOOL bIsDuplicate=FALSE;

        if(bOptimizeStorage)
        {
            // check if it's a dup
            bIsDuplicate = LookupOrAddBnode(&pFirstBlockHashTree,
                                            &HashAndLength,
                                            pFile,
                                            &pBnode);
        }

        if(!bIsDuplicate)
        {
            // not a duplicate

            dwlOffset= (DWORDLONG) pFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;
            pFile->pUdfInfo->ulLinkCount=1;
            pIcb->icbfile_LinkCount=1;
            ASSERT(pIcb->icbfile_LinkCount > 0);

            ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
            HeaderWrite(dwlOffset,pIcb,CD_SECTORSIZE);
        }
        else
        {
            PDIRENTRY pLinkedFile;
            DWORD dwBytesSaved;

            // context is the file pointer

            pLinkedFile=(PDIRENTRY) pBnode->pContext;

            pLinkedFile->pUdfInfo->ulLinkCount++;
            pFile->pUdfInfo->ulICBBlockNumber=pLinkedFile->pUdfInfo->ulICBBlockNumber;
            pFile->pUdfInfo->ulLinkCount=0;
			
            // read icb from disk, increment link and save it out
            dwlOffset= (DWORDLONG) pLinkedFile->pUdfInfo->ulICBBlockNumber * (DWORDLONG) CD_SECTORSIZE;
            // generic read doesn't allocate memory, overwrite ICB
            GenericRead(dwlOffset,CD_SECTORSIZE,pIcb);
            ASSERT(pIcb->icbfile_LinkCount > 0);
            pIcb->icbfile_LinkCount++;

            dwBytesSaved=(DWORD)(pIcb->icbfile_BlocksRecorded + 1) * CD_SECTORSIZE;
            dwlTotalDupFileBytes+=dwBytesSaved;
            gdwSpaceSaved+=dwBytesSaved;
            dwlTotalDupFiles++;

			if(bShowDuplicates)
				printf("\r\nDuplicate file [%S] found, saved %u bytes",pFile->pszUnicodeName,(pIcb->icbfile_BlocksRecorded + 1) * CD_SECTORSIZE);

            // reset image size
            *pdwNextFreeBlock=dwStartingBlock;

            // write last so that memory will be valid
            ASSERT( pIcb->icbfile_LinkCount > 0);
            HeaderWrite(dwlOffset,pIcb,CD_SECTORSIZE);
        }
    }

    // for bridge media
    if(bEncodeUdf && !bEncodeOnlyUdf)
    {
        ASSERT((pIcb->icbfile_icbtag.icbtag_Flags & ICBTAG_F_ALLOC_MASK)== ICBTAG_F_ALLOC_SHORT);

        if((pIcb->icbfile_icbtag.icbtag_Flags & ICBTAG_F_ALLOC_MASK) != ICBTAG_F_ALLOC_SHORT)
        {
            ErrorExit(0, "Bridge media can only support short allocation descriptors\r\n");
        }

        pShortAd = (PSHORTAD) ((PBYTE) pIcb + ICBFILE_EXTENT_OFFSET);

        // BUGBUG
        pFile->dwStartingBlock = pShortAd->shortad_Start + ulPartitionStart;

		ASSERT(pFile->dwStartingBlock != 0);
    }

#ifdef UDF_DBG
	printf"Last free block = %u - partition start -%u\r\n",*pdwNextFreeBlock,ulPartitionStart);
#endif

	return bResult;
}


BOOL MakeSparseExtent(PICBFILE pIcb, BYTE** pBuffer, DWORD* pdwBytesRead, HANDLE hReadContext, DWORD* pdwNextFreeBlock, PHASH_AND_LENGTH pHashAndLength, DWORDLONG* pdwlFileLengthLeft)
{
	DWORD dwNumZeros;
	DWORD dwCurrentLength=0;
    SHORTAD ShortAd;
    LONGAD LongAd;

#ifdef UDF_DBG
	printf("Starting sparse extent\r\n");
#endif
    ASSERT(*pdwlFileLengthLeft >= *pdwBytesRead);
    dwNumZeros=NumberOfStartingZeros(*pBuffer,*pdwBytesRead);
    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
	do
	{
        // update hash
        if(bOptimizeStorage)
        {
            DWORD dwBytesToHash;

            // must be 64 byte chunks
            dwBytesToHash=ROUNDUP2( *pdwBytesRead, 64 );
            // add 0s at end
            if(dwBytesToHash != *pdwBytesRead)
                ZeroMemory(*pBuffer + *pdwBytesRead, dwBytesToHash - *pdwBytesRead);

            // make the hash
            UpdateMD5_Multiple64ByteChunks( &(pHashAndLength->Hash), *pBuffer, dwBytesToHash );
        }

        dwCurrentLength+=dwNumZeros;
        (*pdwlFileLengthLeft)-=dwNumZeros;

        ReleaseBuffer(*pBuffer);
        if(*pdwlFileLengthLeft > 0)
        {
            DWORD dwStatus;

            ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
            dwStatus=WaitForRead(hReadContext,pBuffer,pdwBytesRead);
            ASSERT(((*pdwBytesRead < CD_SECTORSIZE) && (dwStatus != 0)) || (*pdwBytesRead == CD_SECTORSIZE));
            ASSERT(*pdwlFileLengthLeft >= *pdwBytesRead);
            ASSERT((dwStatus == 0) || ((dwStatus != 0) && (*pdwlFileLengthLeft == *pdwBytesRead)));
            dwNumZeros=NumberOfStartingZeros(*pBuffer,*pdwBytesRead);
        }
        ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
    }while(((dwCurrentLength + *pdwBytesRead) <= EXTENTAD_LEN_MASK) && (dwNumZeros == *pdwBytesRead) && (*pdwlFileLengthLeft > 0));

	gdwSpaceSaved+=dwCurrentLength;

    if(pIcb->icbfile_icbtag.icbtag_Flags & ICBTAG_F_ALLOC_LONG)
    {
        LongAd.longad_Start.nsr_lba_lbn=0;
        LongAd.longad_Start.nsr_lba_PartRef=0;      // 0 is the first partition
        LongAd.longad_Length=EXTENTAD_ALLOC_NRNA | (dwCurrentLength & EXTENTAD_LEN_MASK);
        AddNewLongExtent(pIcb,&LongAd,pdwNextFreeBlock);
    }
    else
    {
        ShortAd.shortad_Start=0;
	    ShortAd.shortad_Length=EXTENTAD_ALLOC_NRNA | (dwCurrentLength & EXTENTAD_LEN_MASK);
        AddNewShortExtent(pIcb,&ShortAd,pdwNextFreeBlock);
    }

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

#ifdef UDF_DBG
	printf("Done with Sparse extent\r\n");
    //printf("length = %08x Start=%u\r\n",pShortExt->shortad_Length,pShortExt->shortad_Start);
    if(*pdwBytesRead == CD_SECTORSIZE)
    {
        ASSERT((((ShortAd.shortad_Length & EXTENTAD_LEN_MASK) % CD_SECTORSIZE) == 0) ||
               (((LongAd.longad_Length & EXTENTAD_LEN_MASK) % CD_SECTORSIZE) == 0));
    }
#endif

	return TRUE;
}

BOOL MakeAllocExtent(PICBFILE pIcb, BYTE** pBuffer, DWORD* pdwBytesRead, HANDLE hReadContext, DWORD* pdwNextFreeBlock, BOOL bSparse, PHASH_AND_LENGTH pHashAndLength, DWORDLONG* pdwlFileLengthLeft)
{
	DWORD dwNumZeros=0;
	DWORD dwCurrentLength=0;
	DWORDLONG dwlOffset;
    SHORTAD ShortAd;
    LONGAD LongAd;
    ULONG ulLBN;

#ifdef UDF_DBG
	printf("Starting Alloc extent\r\n");
#endif

    ASSERT(*pdwlFileLengthLeft >= *pdwBytesRead);
    ulLBN=*pdwNextFreeBlock;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
    do
    {
        // update hash
        if(bOptimizeStorage)
        {
            DWORD dwBytesToHash;

            // must be 64 byte chunks
            dwBytesToHash=ROUNDUP2( *pdwBytesRead, 64 );
            // add 0s at end
            if(dwBytesToHash != *pdwBytesRead)
                ZeroMemory(*pBuffer + *pdwBytesRead, dwBytesToHash - *pdwBytesRead);

            // make the hash
            UpdateMD5_Multiple64ByteChunks( &(pHashAndLength->Hash), *pBuffer, dwBytesToHash );
        }

        dwCurrentLength+=*pdwBytesRead;
		dwlOffset=(DWORDLONG) *pdwNextFreeBlock * (DWORDLONG) CD_SECTORSIZE;
		(*pdwNextFreeBlock)++;
        (*pdwlFileLengthLeft)-=*pdwBytesRead;

        PaddedFileDataWrite(dwlOffset,*pBuffer,*pdwBytesRead);

        if(*pdwlFileLengthLeft > 0)
        {
            DWORD dwStatus;

            ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
            dwStatus=WaitForRead(hReadContext,pBuffer,pdwBytesRead);

            ASSERT(((*pdwBytesRead < CD_SECTORSIZE) && (dwStatus != 0)) || (*pdwBytesRead == CD_SECTORSIZE));
            ASSERT(*pdwlFileLengthLeft >= *pdwBytesRead);
            ASSERT((dwStatus == 0) || ((dwStatus != 0) && (*pdwlFileLengthLeft == *pdwBytesRead)));

            dwNumZeros=NumberOfStartingZeros(*pBuffer,*pdwBytesRead);
//            printf("CurrentLength =%u, BytesRead = %u NumZeros =%u FileLengthLeft = %I64u\r\n",
//                    dwCurrentLength,*pdwBytesRead,dwNumZeros,*pdwlFileLengthLeft);
        }
        ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
    }while(((dwCurrentLength + *pdwBytesRead) <= EXTENTAD_LEN_MASK) && !((dwNumZeros == *pdwBytesRead) && bSparse) && (*pdwlFileLengthLeft > 0));

//    printf("LEAVING CurrentLength =%u BytesRead = %u NumZeros =%u FileLengthLeft = %I64u\r\n",
//            dwCurrentLength,*pdwBytesRead,dwNumZeros,*pdwlFileLengthLeft);

    // bugbug will this work?
    ASSERT(ulLBN >= ulPartitionStart);
    ulLBN-=ulPartitionStart;

    if(pIcb->icbfile_icbtag.icbtag_Flags & ICBTAG_F_ALLOC_LONG)
    {
        LongAd.longad_Start.nsr_lba_lbn=ulLBN;
        LongAd.longad_Start.nsr_lba_PartRef=0;  // 0 is the first partition
        LongAd.longad_Length=EXTENTAD_ALLOC__R_A | (dwCurrentLength & EXTENTAD_LEN_MASK);
        AddNewLongExtent(pIcb,&LongAd,pdwNextFreeBlock);
    }
    else
    {
        ShortAd.shortad_Start=ulLBN;
	    ShortAd.shortad_Length=EXTENTAD_ALLOC__R_A | (dwCurrentLength & EXTENTAD_LEN_MASK);
        AddNewShortExtent(pIcb,&ShortAd,pdwNextFreeBlock);
    }

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

#ifdef UDF_DBG
	printf("Done with Alloc extent\r\n");
    if(*pdwBytesRead == CD_SECTORSIZE)
    {
        ASSERT( ( ( (ShortAd.shortad_Length & EXTENTAD_LEN_MASK) % CD_SECTORSIZE) == 0) ||
                ( ( (LongAd.longad_Length & EXTENTAD_LEN_MASK) % CD_SECTORSIZE) == 0));
    }
#endif

	return TRUE;
}

BOOL BuildSparseFileExtent(DWORD* pdwNextFreeBlock, HANDLE hReadContext, PICBFILE pIcb, PHASH_AND_LENGTH pHashAndLength, DWORDLONG dwlFileLength)
{
	DWORD dwBytesRead;
	BOOL bResult;
	BYTE* pBuffer;
	DWORD dwNumZeros;
    DWORD dwStartBlock;
    DWORDLONG dwlFileLengthLeft;
    DWORD dwStatus;

    dwStartBlock=*pdwNextFreeBlock;
    dwlFileLengthLeft=dwlFileLength;

    dwStatus=WaitForRead(hReadContext,&pBuffer,&dwBytesRead);
    ASSERT(((dwBytesRead < CD_SECTORSIZE) && (dwStatus != 0)) || (dwBytesRead == CD_SECTORSIZE));
    ASSERT(pIcb->icbfile_AllocLen == 0);
	do
	{

		dwNumZeros=NumberOfStartingZeros(pBuffer,dwBytesRead);
		if((dwNumZeros == dwBytesRead) && bUdfSparse)
        {
            if(bUdfVideoZone)
            {
                ErrorExit(0, "ERROR: Sparse files can't not be present in Video Zone images\r\n");
            }

			bResult=MakeSparseExtent(pIcb,&pBuffer,&dwBytesRead,hReadContext,pdwNextFreeBlock,pHashAndLength,&dwlFileLengthLeft);
        }
		else
        {
			bResult=MakeAllocExtent(pIcb,&pBuffer,&dwBytesRead,hReadContext,pdwNextFreeBlock,bUdfSparse,pHashAndLength,&dwlFileLengthLeft);
        }

		if(!bResult)
		{
            ErrorExit( GETLASTERROR, "BuildSparseFileExtent could not make extent\r\n" );
			return FALSE;
		}
        ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));
	}while(dwlFileLengthLeft > 0);

    pIcb->icbfile_BlocksRecorded = *pdwNextFreeBlock - dwStartBlock;

	return TRUE;
}

/************************* SHORTAD functions **********************************/

VOID AddNewShortExtent(PICBFILE pIcb, SHORTAD* pShortAD, LPDWORD pdwNextFreeBlock)
{
    DWORD dwExtentBlock;
    BYTE* pBuffer;
    DWORD dwOffset;
	SHORTAD NextAd;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*) pIcb;
    dwOffset=ICBFILE_EXTENT_OFFSET + pIcb->icbfile_AllocLen;

    if(bUdfVideoZone && (0 != pIcb->icbfile_AllocLen))
    {
        ErrorExit(0, "ERROR: Each file can only have one extent for Video Zone\r\n");
    }

    ZeroMemory(&NextAd,sizeof(SHORTAD));

	if(GetNumberOfShortExtents(pIcb) == 0)
	{
		NextAd.shortad_Length=0;
	}
	else
	{
        GetShortExtent(pIcb,GetNumberOfShortExtents(pIcb) - 1, &NextAd);
	}

	if((NextAd.shortad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NEXT)
	{
        if(bUdfVideoZone)
        {
            ErrorExit(0, "ERROR: Each file can only have one extent for Video Zone\r\n");
        }

		// continue extent

        dwExtentBlock=NextAd.shortad_Start + ulPartitionStart;
        AddNewShortAdToExtent(dwExtentBlock,pShortAD,pdwNextFreeBlock);
	}
	else if(((dwOffset + (sizeof(SHORTAD) * 2)) > CD_SECTORSIZE) ||
			(g_ulMaxAdsInExtent <= GetNumberOfShortExtents(pIcb)))
    {
        DWORDLONG dwlOffset;
        BYTE* pExtentBuffer;

		// new extent

        if(bUdfVideoZone)
        {
            ErrorExit(0, "ERROR: Each file can only have one extent for Video Zone\r\n");
        }

        pExtentBuffer =  AllocateBuffer( CD_SECTORSIZE, TRUE );
        dwExtentBlock=*pdwNextFreeBlock;
        (*pdwNextFreeBlock)++;

        InitalizeAllocationExtentDescriptor((PNSR_ALLOC) pExtentBuffer,dwExtentBlock - ulPartitionStart,pIcb->icbfile_destag.destag_lsn);

		NextAd.shortad_Length=(CD_SECTORSIZE & EXTENTAD_LEN_MASK) | EXTENTAD_ALLOC_NEXT;
        NextAd.shortad_Start=dwExtentBlock - ulPartitionStart;

        dwlOffset=(DWORDLONG) dwExtentBlock * (DWORDLONG) CD_SECTORSIZE;

        memcpy(pBuffer+dwOffset,&NextAd,sizeof(SHORTAD));
	    pIcb->icbfile_AllocLen+=sizeof(SHORTAD);

        HeaderWrite(dwlOffset, pExtentBuffer, CD_SECTORSIZE );

        AddNewShortAdToExtent(dwExtentBlock,pShortAD,pdwNextFreeBlock);
    }
    else
	{
 #ifdef UDF_DBG
		printf("Offset = %u, pIcb = %p\r\n",dwOffset,pIcb);
		printf("Length = %u\r\n",pShortAD->shortad_Length & EXTENTAD_LEN_MASK);
		printf("Start  = %u\r\n",pShortAD->shortad_Start);
#endif
		memcpy(pBuffer+dwOffset,pShortAD,sizeof(SHORTAD));
		pIcb->icbfile_AllocLen+=sizeof(SHORTAD);
	}
}

DWORD GetNumberOfShortExtents(PICBFILE pIcbFile)
{
    return (DWORD)((DWORD)pIcbFile->icbfile_AllocLen / (DWORD)sizeof(SHORTAD));
}

VOID GetShortExtent(PICBFILE pIcb, DWORD dwExtentNum, PSHORTAD pShortExt)
{
    BYTE* pBuffer;
    PSHORTAD pExtArray;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*)pIcb;
    pBuffer+=ICBFILE_EXTENT_OFFSET;
    pExtArray=(PSHORTAD)pBuffer;
    memcpy(pShortExt,&pExtArray[dwExtentNum],sizeof(SHORTAD));

#ifdef UDF_DBG
    if(bVerbose)
    {
        DWORD dwLength;
        dwLength=pShortExt->shortad_Length & EXTENTAD_LEN_MASK;
        IsShortExtentSparse(pShortExt);
	    printf("Extent %d: Length = %u [0x%x]\r\n", dwExtentNum+1, dwLength, dwLength);
	    printf("Extent %d: Start  = %u [0x%x]\r\n", dwExtentNum+1, pShortExt->shortad_Start, pShortExt->shortad_Start);
    }
#endif
}

VOID PutShortExtent(PICBFILE pIcb, DWORD dwExtentNum, PSHORTAD pShortExt)
{
    BYTE* pBuffer;
    PSHORTAD pExtArray;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*)pIcb;
    pBuffer+=ICBFILE_EXTENT_OFFSET;
    pExtArray=(PSHORTAD)pBuffer;
    memcpy(&pExtArray[dwExtentNum],pShortExt,sizeof(SHORTAD));

#ifdef UDF_DBG
    if(bVerbose)
    {
        DWORD dwLength;

        IsShortExtentSparse(pShortExt);
        dwLength=pShortExt->shortad_Length & EXTENTAD_LEN_MASK;
	    printf("Extent %d: Length = %u [0x%x]\r\n", dwExtentNum+1, dwLength, dwLength);
	    printf("Extent %d: Start  = %u [0x%x]\r\n", dwExtentNum+1, pShortExt->shortad_Start, pShortExt->shortad_Start);
    }
#endif
}

BOOL IsShortExtentSparse(PSHORTAD pShortExt)
{
    if((pShortExt->shortad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NRNA)
    {

        if(bVerbose)
            printf("Sparse Extent\r\n");

        return TRUE;
    }
    else
    {

        if(bVerbose)
            printf("Regular Extent\r\n");

        return FALSE;
    }
}

void AddNewShortAdToExtent(DWORD dwExtentBlock, SHORTAD* pShortAd, LPDWORD pdwNextFreeBlock)
{
    BYTE* pBuffer;
    DWORDLONG dwlOffset;
    PNSR_ALLOC pAllocDesc;
    BYTE* pShortAds;
    PSHORTAD pNextAd;
    DWORD dwNextBlock;

    pBuffer=AllocateBuffer(CD_SECTORSIZE, FALSE);

    dwlOffset=(DWORDLONG) dwExtentBlock * (DWORDLONG) CD_SECTORSIZE;

    GenericRead(dwlOffset,(DWORD) CD_SECTORSIZE, (PVOID) pBuffer);

    pAllocDesc=(PNSR_ALLOC) pBuffer;
    pShortAds=pBuffer + sizeof(NSR_ALLOC);

    pNextAd=(PSHORTAD) (pShortAds + pAllocDesc->nsr_alloc_AllocLen - sizeof(SHORTAD));

    if((pNextAd->shortad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NEXT)
	{
        dwNextBlock=pNextAd->shortad_Start + ulPartitionStart;
        ReleaseBuffer(pBuffer);

        AddNewShortAdToExtent(dwNextBlock,pShortAd,pdwNextFreeBlock);
	}
	else if(((pAllocDesc->nsr_alloc_AllocLen + sizeof(NSR_ALLOC) + (sizeof(SHORTAD) * 2)) > CD_SECTORSIZE) ||
             (g_ulMaxAdsInExtent <= (pAllocDesc->nsr_alloc_AllocLen / sizeof(SHORTAD))))
    {
        // it's full

        dwNextBlock=*pdwNextFreeBlock;
        (*pdwNextFreeBlock)++;

        pNextAd=(PSHORTAD) (pShortAds + pAllocDesc->nsr_alloc_AllocLen);
        pAllocDesc->nsr_alloc_AllocLen+=sizeof(SHORTAD);

        pNextAd->shortad_Length=(CD_SECTORSIZE & EXTENTAD_LEN_MASK) | EXTENTAD_ALLOC_NEXT;
        pNextAd->shortad_Start=dwNextBlock - ulPartitionStart;
        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);

        pBuffer=AllocateBuffer(CD_SECTORSIZE, TRUE);
        InitalizeAllocationExtentDescriptor((PNSR_ALLOC) pBuffer,dwNextBlock-ulPartitionStart,dwExtentBlock-ulPartitionStart);

        dwlOffset=(DWORDLONG) dwNextBlock * (DWORDLONG) CD_SECTORSIZE;
        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);

        AddNewShortAdToExtent(dwNextBlock,pShortAd,pdwNextFreeBlock);
    }
    else
    {
        CopyMemory(pShortAds + pAllocDesc->nsr_alloc_AllocLen, pShortAd, sizeof(SHORTAD));
        pAllocDesc->nsr_alloc_AllocLen+=sizeof(SHORTAD);

        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);
    }
}


/************************* LONGAD functions **********************************/

VOID AddNewLongExtent(PICBFILE pIcb,LONGAD* pLongAd, LPDWORD pdwNextFreeBlock)
{
    BYTE* pBuffer;
    DWORD dwOffset;
    DWORD dwExtentBlock;
    LONGAD NextAd;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*) pIcb;
    dwOffset=ICBFILE_EXTENT_OFFSET + pIcb->icbfile_AllocLen;

    ZeroMemory(&NextAd,sizeof(LONGAD));

	if(GetNumberOfLongExtents(pIcb) == 0)
	{
		NextAd.longad_Length=0;
	}
	else
	{
	    GetLongExtent(pIcb,GetNumberOfLongExtents(pIcb) - 1, &NextAd);
	}

    if((NextAd.longad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NEXT)
	{
        dwExtentBlock=NextAd.longad_Start.nsr_lba_lbn + ulPartitionStart;

        AddNewLongAdToExtent(dwExtentBlock,pLongAd,pdwNextFreeBlock);
	}
	else if(((dwOffset + (sizeof(LONGAD) * 2)) > CD_SECTORSIZE) ||
             (g_ulMaxAdsInExtent <= GetNumberOfLongExtents(pIcb)))
    {
        DWORDLONG dwlOffset;
        BYTE* pExtentBuffer;

        dwExtentBlock=*pdwNextFreeBlock;
        (*pdwNextFreeBlock)++;

		pExtentBuffer =  AllocateBuffer( CD_SECTORSIZE, TRUE );

		InitalizeAllocationExtentDescriptor((PNSR_ALLOC) pExtentBuffer,dwExtentBlock - ulPartitionStart,pIcb->icbfile_destag.destag_lsn);

		NextAd.longad_Length=(CD_SECTORSIZE & EXTENTAD_LEN_MASK) | EXTENTAD_ALLOC_NEXT;
		NextAd.longad_Start.nsr_lba_PartRef=0; // 0 is the first partition
		NextAd.longad_Start.nsr_lba_lbn=dwExtentBlock - ulPartitionStart;

		dwlOffset=(DWORDLONG) dwExtentBlock * (DWORDLONG) CD_SECTORSIZE;

		CopyMemory(pBuffer+dwOffset,&NextAd,sizeof(LONGAD));
		pIcb->icbfile_AllocLen+=sizeof(LONGAD);

        HeaderWrite(dwlOffset, pExtentBuffer, CD_SECTORSIZE );

        AddNewLongAdToExtent(dwExtentBlock,pLongAd,pdwNextFreeBlock);
    }
    else
    {
#ifdef UDF_DBG
        printf("Offset = %u, pIcb = %p\r\n",dwOffset,pIcb);
	    printf("Length = %u\r\n",pLongAd->longad_Length & EXTENTAD_LEN_MASK);
	    printf("Start  = %u\r\n",pLongAd->longad_Start.nsr_lba_lbn);
#endif
	    memcpy(pBuffer+dwOffset,pLongAd,sizeof(LONGAD));
	    pIcb->icbfile_AllocLen+=sizeof(LONGAD);
    }
}

DWORD GetNumberOfLongExtents(PICBFILE pIcbFile)
{
    return (DWORD)((DWORD)pIcbFile->icbfile_AllocLen / (DWORD)sizeof(LONGAD));
}

VOID GetLongExtent(PICBFILE pIcb, DWORD dwExtentNum, PLONGAD pLongExt)
{
    BYTE* pBuffer;
    PLONGAD pExtArray;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*)pIcb;
    pBuffer+=ICBFILE_EXTENT_OFFSET;
    pExtArray=(PLONGAD)pBuffer;
    memcpy(pLongExt,&pExtArray[dwExtentNum],sizeof(LONGAD));

#ifdef UDF_DBG
    if(bVerbose)
    {
        DWORD dwLength;
        dwLength=pLongExt->longad_Length & EXTENTAD_LEN_MASK;
        IsLongExtentSparse(pLongExt);
	    printf("Extent %d: Length = %u [0x%x]\r\n", dwExtentNum+1, dwLength, dwLength);
	    printf("Extent %d: Start  = %u [0x%x]\r\n", dwExtentNum+1, pLongExt->longad_Start.nsr_lba_lbn, pLongExt->longad_Start.nsr_lba_lbn);
    }
#endif
}

VOID PutLongExtent(PICBFILE pIcb, DWORD dwExtentNum, PLONGAD pLongExt)
{
    BYTE* pBuffer;
    PLONGAD pExtArray;

    ASSERT((pIcb->icbfile_AllocLen >= 0) && (pIcb->icbfile_AllocLen <= (CD_SECTORSIZE - ICBFILE_EXTENT_OFFSET)));

    pBuffer=(BYTE*)pIcb;
    pBuffer+=ICBFILE_EXTENT_OFFSET;
    pExtArray=(PLONGAD)pBuffer;
    memcpy(&pExtArray[dwExtentNum],pLongExt,sizeof(LONGAD));

#ifdef UDF_DBG
    if(bVerbose)
    {
        DWORD dwLength;

        IsLongExtentSparse(pLongExt);
        dwLength=pLongExt->longad_Length & EXTENTAD_LEN_MASK;
	    printf("Extent %d: Length = %u [0x%x]\r\n", dwExtentNum+1, dwLength, dwLength);
	    printf("Extent %d: Start  = %u [0x%x]\r\n", dwExtentNum+1, pLongExt->longad_Start.nsr_lba_lbn, pLongExt->longad_Start.nsr_lba_lbn);
    }
#endif
}

BOOL IsLongExtentSparse(PLONGAD pLongExt)
{
    if((pLongExt->longad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NRNA)
    {

        if(bVerbose)
            printf("Sparse Extent\r\n");

        return TRUE;
    }
    else
    {

        if(bVerbose)
            printf("Regular Extent\r\n");

        return FALSE;
    }
}

VOID InitalizeAllocationExtentDescriptor(PNSR_ALLOC pAllocDesc, DWORD dwBlock, DWORD dwPrevBlock)
{
    ZeroMemory(pAllocDesc,sizeof(NSR_ALLOC));

    pAllocDesc->nsr_alloc_Prev=dwPrevBlock;
    pAllocDesc->nsr_alloc_destag.destag_Ident=DESTAG_ID_NSR_ALLOC;
    pAllocDesc->nsr_alloc_destag.destag_Version=GetDesTagVersion();
    pAllocDesc->nsr_alloc_destag.destag_lsn=dwBlock;
    pAllocDesc->nsr_alloc_destag.destag_CRCLen=sizeof(ULONG);
    pAllocDesc->nsr_alloc_destag.destag_CRC=
            Crc16(0,&(pAllocDesc->nsr_alloc_Prev),pAllocDesc->nsr_alloc_destag.destag_CRCLen);
    pAllocDesc->nsr_alloc_destag.destag_Checksum = 0;
       pAllocDesc->nsr_alloc_destag.destag_Checksum=
        ComputeChecksum( (PUCHAR)&(pAllocDesc->nsr_alloc_destag), sizeof (DESTAG) );

    SetRandomExtentLength();
}

void AddNewLongAdToExtent(DWORD dwExtentBlock, LONGAD* pLongAd, LPDWORD pdwNextFreeBlock)
{
    BYTE* pBuffer;
    DWORDLONG dwlOffset;
    PNSR_ALLOC pAllocDesc;
    BYTE* pLongAds;
    DWORD dwNextBlock;
    PLONGAD pNextAd;

#ifdef UDF_DBG
    printf("Getting extent at %u Next free = %u (long)\r\n",dwExtentBlock,*pdwNextFreeBlock);
#endif

    pBuffer=AllocateBuffer(CD_SECTORSIZE, FALSE);

    dwlOffset=(DWORDLONG) dwExtentBlock * (DWORDLONG) CD_SECTORSIZE;

    GenericRead(dwlOffset,(DWORD) CD_SECTORSIZE, (PVOID) pBuffer);

    pAllocDesc=(PNSR_ALLOC) pBuffer;
    pLongAds=pBuffer + sizeof(NSR_ALLOC);
    pNextAd=(PLONGAD) (pLongAds + (pAllocDesc->nsr_alloc_AllocLen - sizeof(LONGAD)));

	if((pNextAd->longad_Length & EXTENTAD_ALLOC_MASK) == EXTENTAD_ALLOC_NEXT)
	{
        dwNextBlock=pNextAd->longad_Start.nsr_lba_lbn + ulPartitionStart;
        ReleaseBuffer(pBuffer);

        ASSERT(dwNextBlock > ulPartitionStart);

        AddNewLongAdToExtent(dwNextBlock,pLongAd,pdwNextFreeBlock);
	}
	else if(((pAllocDesc->nsr_alloc_AllocLen + sizeof(NSR_ALLOC) + (sizeof(LONGAD) * 2)) > CD_SECTORSIZE) ||
             (g_ulMaxAdsInExtent <= (pAllocDesc->nsr_alloc_AllocLen / sizeof(LONGAD))))
    {
        // it's full
        ASSERT(pAllocDesc->nsr_alloc_AllocLen != 0);

        // need a new extent
        dwNextBlock=*pdwNextFreeBlock;
        (*pdwNextFreeBlock)++;

        pNextAd=(PLONGAD) (pLongAds + pAllocDesc->nsr_alloc_AllocLen);
        pAllocDesc->nsr_alloc_AllocLen+=sizeof(LONGAD);

        pNextAd->longad_Length=(CD_SECTORSIZE & EXTENTAD_LEN_MASK) | EXTENTAD_ALLOC_NEXT;
        pNextAd->longad_Start.nsr_lba_lbn=dwNextBlock - ulPartitionStart;
        pNextAd->longad_Start.nsr_lba_PartRef=0; // 0 is the first partition
        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);

        pBuffer=AllocateBuffer(CD_SECTORSIZE, TRUE);
        InitalizeAllocationExtentDescriptor((PNSR_ALLOC) pBuffer,dwNextBlock-ulPartitionStart,dwExtentBlock-ulPartitionStart);

        dwlOffset=(DWORDLONG) dwNextBlock * (DWORDLONG) CD_SECTORSIZE;
        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);

        ASSERT(dwNextBlock > ulPartitionStart);

        AddNewLongAdToExtent(dwNextBlock,pLongAd,pdwNextFreeBlock);
    }
    else
    {
#ifdef UDF_DBG
        printf("Extent = %u, pIcb = %p\r\n",dwExtentBlock);
	    printf("Length = %u\r\n",pLongAd->longad_Length & EXTENTAD_LEN_MASK);
	    printf("Start  = %u\r\n",pLongAd->longad_Start.nsr_lba_lbn);
#endif

        CopyMemory(pLongAds + pAllocDesc->nsr_alloc_AllocLen, pLongAd, sizeof(LONGAD));
        pAllocDesc->nsr_alloc_AllocLen+=sizeof(LONGAD);

        HeaderWrite(dwlOffset,pBuffer,CD_SECTORSIZE);
    }
}

void SetRandomExtentLength(void)
{
    if(bUdfUseRandomExtents)
    {
		// 0 is allowed, so we can have an extent that only points to another extent
        g_ulMaxAdsInExtent=(rand() % MAX_RANDOM_AD_EXTENT);
#ifdef UDF_DBG
        printf("Max ads = %d\r\n",g_ulMaxAdsInExtent);
#endif
    }
}

DWORD GetIcbBlockCount()
{
    return g_dwIcbBlockCount;
}
