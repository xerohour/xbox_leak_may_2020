
typedef struct  UDF_LVID_IMPUSE {
    REGID       riImplementID;          // Implementation ID
    ULONG       ulNumberOfFiles;
    ULONG       ulNumberOfDirs;
    USHORT      usMinUDFReadRev;
    USHORT      usMinUDFWriteRev;
    USHORT      usMaxUDFWriteRev;
} UDF_LVID_IMPUSE, *PUDF_LVID_IMPUSE;       
// this needs to be at least 46 bytes

extern TIMESTAMP GlobalNsrTimeStamp;
extern short     NsrLocalTimeBias;

VOID
MakeNsrTimeStampFromFileTime(
    OUT PTIMESTAMP pTimeStamp,
    IN  FILETIME   ftFileTime
    );

extern BOOL bEncodeUdf;
extern BOOL bEncodeOnlyUdf;
extern BOOL bUdfEmbedData;
extern BOOL bUdfSparse;
extern BOOL bUdfEmbedFID;
extern BOOL bUdfUseLongADs;
extern BOOL bUdfUseRandomExtents;
extern BOOL bUdfVideoZone;
extern BOOL bUdf8BitFileNames;

extern DWORD gdwSpaceSaved;

#define XA_LENGTH       (56)              // == 24 + 20 + 12 (XA Header Descriptor + File Times XA format + one timestamp)
#define ICBFILE_EXTENT_OFFSET   (176+(XA_LENGTH))   //176+56 = 232

void SetCurrentUDFVersion(USHORT nVer);

VOID ComputeUDFDirectorySizes( VOID );
VOID AssignUDFDirectoryBlockNumbers( VOID );
VOID ComputeUDFPartitionLength( VOID );
VOID WriteUDFAnchorAndVDS( VOID );
VOID WriteUDFDirectories( VOID );
VOID WriteBEA01Descriptor( ULONG SectorNumber );
VOID WriteTEA01Descriptor( ULONG SectorNumber );
VOID WriteNSRDescriptor( ULONG SectorNumber );


BOOL MakeFileExtent(PDIRENTRY pFile, DWORD* pdwNextFreeBlock, DWORD dwIcbBlock, HANDLE hReadContext);
DWORD GetIcbBlockCount();

