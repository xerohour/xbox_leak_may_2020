class CMemStat
{
public:
	HFILE m_hfile;
	unsigned cbTotalAlloc;
	unsigned cbTotalFree;
	CMemStat();
	CMapStringToPtr m_mapAlloc, m_mapFree;
	BOOL PrintMap( void );
	void Analize();
	void ReportInExcel();
};

// ****** WARNING *******
// The following is copied from CRT headers and may become out of date.
// (However this currently seems to be the only way to enumerate the free
// blocks in the heap.)
#define nNoMansLandSize 4
typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
        size_t                      nDataSize;
        int                         nBlockUse;
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

