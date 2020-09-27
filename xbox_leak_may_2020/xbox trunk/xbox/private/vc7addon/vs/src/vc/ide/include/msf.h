// msf.h: see "The Multistream File API" for more information

#ifndef __MSF__H
#define __MSF__H

#ifndef TRUE

#define TRUE	1
#define FALSE	0
typedef int BOOL;

#ifdef _DEBUG
#define verify(x)	assert(x)
#else
#define verify(x) (x)
#endif

#endif

typedef unsigned short	SN;		// stream number
typedef long			CB;	

#ifdef MSF_PAGE_SIZE
#define cbPg	MSF_PAGE_SIZE
#else
#define cbPg	4096
#endif

#ifndef cbNil
#define cbNil	((CB)-1)
#endif
#define snNil	((SN)-1)

#if defined(__cplusplus)
extern "C" {
#endif

// MSFOpen		-- open MSF; return MSF* or NULL if error.  Create a temp msf if 
//			   supplied name is null.
// MSFGetCbStream	-- return size of stream or -1 if stream does not exist
// MSFReadStream	-- read stream into pvBuf; return TRUE if successful
// MSFWriteStream	-- overwrite stream with pvBuf; return TRUE if successful
// MSFCommit		-- commit all pending changes; return TRUE if successful
// MSFPack		-- pack MSF on disk; return TRUE if successful
// MSFClose		-- close MSF; return TRUE if successful
// MSFCreateCopy	-- create a new MSF with the same contents.
#define MSF_EXPORT 

class MSF;
MSF_EXPORT MSF*	MSFOpen(const char *name, BOOL fWrite);
MSF_EXPORT CB	MSFGetCbStream(MSF* pmsf, SN sn);
MSF_EXPORT BOOL	MSFReadStream(MSF* pmsf, SN sn, void* pvBuf, long cbBuf);
MSF_EXPORT BOOL	MSFWriteStream(MSF* pmsf, SN sn, void* pvBuf, long cbBuf);
MSF_EXPORT BOOL	MSFCommit(MSF* pmsf);
MSF_EXPORT BOOL	MSFPack(MSF* pmsf);
MSF_EXPORT BOOL	MSFClose(MSF* pmsf);
MSF_EXPORT MSF*	MSFCreateCopy (MSF* pmsf,const char *pCopyName);
#if defined(__cplusplus)
};
#endif

#endif // __MSF__H
