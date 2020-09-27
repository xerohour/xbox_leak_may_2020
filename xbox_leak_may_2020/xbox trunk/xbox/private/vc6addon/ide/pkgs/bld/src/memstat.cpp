#include "stdafx.h"

#ifdef DEBUG

#include <afx.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include <fstream.h>

#include "memstat.h"


class COriginInfo
{
public:
	ULONG m_cBlocks;
	ULONG m_cbTotal;
	CString m_strOrigin;
	COriginInfo() { m_cBlocks = m_cbTotal = 0; }
};

int compOrigin( const void *p1, const void * p2 ){
	COriginInfo *o1 = *(COriginInfo **)p1;
	COriginInfo *o2 = *(COriginInfo **)p2;
	if( o1->m_cbTotal < o2->m_cbTotal ) return 1;
	if( o1->m_cbTotal == o2->m_cbTotal ) return 0;
	return -1;
}

BOOL CMemStat::PrintMap( void )
{

	COriginInfo **array = new COriginInfo*[m_mapAlloc.GetCount()];

	int count=0;
	int i=0;
	COriginInfo *pinfo;

	for (POSITION pos = m_mapAlloc.GetStartPosition();
		 pos != NULL;
		 )
	{
		CString strOrigin;
   	    m_mapAlloc.GetNextAssoc(pos, strOrigin, (void *&)array[count]);
		count++;
	}
	qsort(array,count,sizeof(COriginInfo *),compOrigin);

	m_hfile = (HFILE)::CreateFile("\\heap.xls", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hfile == (HFILE)INVALID_HANDLE_VALUE)
		return FALSE;

	static char szBuf[500];
	DWORD cbWritten;
	sprintf(szBuf, "Total=%u\t#new\tavg size\tlocation\tclass\n", cbTotalAlloc);
	WriteFile((HANDLE)m_hfile, szBuf, strlen(szBuf), &cbWritten, NULL);
	for (i=0;i<count;i++)
	{
   	    pinfo = array[i];
		sprintf(szBuf, "%u\t%u\t%u\t%s\n",
			   pinfo->m_cbTotal,
			   pinfo->m_cBlocks,
			   pinfo->m_cbTotal / pinfo->m_cBlocks,
			   LPCTSTR(pinfo->m_strOrigin));
		delete pinfo;
		WriteFile((HANDLE)m_hfile, szBuf, strlen(szBuf), &cbWritten, NULL);
	}
	::CloseHandle((HANDLE)m_hfile);
	delete array;
	return FALSE;
}


void CMemStat::Analize()
{

	ifstream ifs("\\heap.out");

	while (!ifs.eof())
	{
		char szOrigin[100];
		strcpy(szOrigin, "[unknown]");

		unsigned cb = 0;
		BOOL fAlloc = TRUE;

		char szLine[500];
		ifs.getline(szLine, 500);


		char szSeq[100];
		char szSubtype[100];
		char szAddr[100];
		char szBlockType[100];
		if (strncmp(szLine, "Detected", 8) == 0 ||
			strncmp(szLine, "Dumping", 7) == 0 ||
			strncmp(szLine, "Object dump", 11) == 0)
		{
			continue;	// ignore random header line
		}
		else if (szLine[0] == '\0')
		{
			continue;	// blank line
		}
		else if (szLine[0] == '{' &&	// balance: }
				 sscanf(szLine, "%s %s block at 0x%s %u bytes",
						szSeq, szBlockType, szAddr, &cb) == 4)
		{
		}
		else if (szLine[0] == '{' &&	// balance: }
				 sscanf(szLine, "%s %s block at 0x%s subtype %s %u bytes",
						szSeq, szBlockType, szAddr, szSubtype, &cb) == 5)
		{
		}
		else if (sscanf(szLine, "%s : %s %s block at %s %u bytes",
						szOrigin, szSeq, szBlockType, szAddr, &cb) == 5)
		{
		}
		else if (sscanf(szLine, "%s : %s %s block at %s subtype %s %u bytes",
						szOrigin, szSeq, szBlockType, szAddr, szSubtype, &cb) == 6)
		{
		}
		else if (sscanf(szLine, "*free* %s -- %u bytes long", szOrigin, &cb))
		{
			fAlloc = FALSE;
		}
		else
		{
			printf("unrecognized: \"%s\"\n", szLine);
			continue;
		}
	
		*(fAlloc ? &cbTotalAlloc : &cbTotalFree) += cb;
	
		// read the 2nd part of each block dump
		if (fAlloc)		// free lines have no 2nd part
		{
			ifs.getline(szLine, 500);
			if (strncmp(szLine, "a ", 2) == 0)
			{
				// got a name for the object type
				strcat(szOrigin, "\t");
				sscanf(szLine + 2, "%s", szOrigin + strlen(szOrigin));
			}
		}
	
		CMapStringToPtr &map = fAlloc ? m_mapAlloc : m_mapFree;
		COriginInfo *pinfo;
		if (!map.Lookup(szOrigin, (void *&)pinfo))
		{
			pinfo = new COriginInfo;
			map[szOrigin] = pinfo;
			pinfo->m_strOrigin = szOrigin;
		}
		pinfo->m_cBlocks += 1;
		pinfo->m_cbTotal += cb;
	}
}


CMemStat::CMemStat() {

	cbTotalAlloc = 0;
	cbTotalFree = 0;

}

void CMemStat::ReportInExcel()
{
	HFILE hfile = (HFILE)::CreateFile("\\heap.out", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == (HFILE)INVALID_HANDLE_VALUE)
		return;

	{
		_HFILE hfilePrev = ::_CrtSetReportFile(_CRT_WARN, (_HFILE)hfile);
		int modePrev = ::_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);

		::_CrtDumpMemoryLeaks();

		::_CrtSetReportFile(_CRT_WARN, hfilePrev);
		::_CrtSetReportMode(_CRT_WARN, modePrev);
	}

	// Dump all known freed blocks on the heap.
	// (There can only be known freed blocks if someone has set the _CRTDBG_DELAY_FREE_MEM_DF
	// flag.)
	_CrtMemState s;
	_CrtMemCheckpoint(&s);
	_CrtMemBlockHeader *poldblk;
	_CrtMemBlockHeader *pblk = poldblk = s.pBlockHeader;

	for ( pblk = s.pBlockHeader; pblk != NULL; pblk = pblk->pBlockHeaderNext)
	{
		if (pblk->nBlockUse != _FREE_BLOCK)
		{
			continue;	// ignore everything but free blocks
		}

		static char szBuf[300];
		if (pblk->szFileName != NULL)
		{
			sprintf(szBuf, "*free* %hs(%d) -- %u bytes long\n", pblk->szFileName, pblk->nLine,
					pblk->nDataSize);
		}
		else
		{
			sprintf(szBuf, "*free* [unknown] -- %u bytes long\n", pblk->nDataSize);
		}
		DWORD cbWritten;
		WriteFile((HANDLE)hfile, szBuf, strlen(szBuf), &cbWritten, NULL);
	}
	::CloseHandle((HANDLE)hfile);

	{
		Analize();
		PrintMap();
	}

	// Dump all CString.

	hfile = (HFILE)::CreateFile("\\strings.xls", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == (HFILE)INVALID_HANDLE_VALUE)
		return;

	// (There can only be known freed blocks if someone has set the _CRTDBG_DELAY_FREE_MEM_DF
	// flag.)
	DWORD cbWritten;
	WriteFile((HANDLE)hfile, "String\tRefs\tSize\n", 18, &cbWritten, NULL);
	for ( pblk = poldblk; pblk != NULL; pblk = pblk->pBlockHeaderNext)
	{
		if (pblk->nBlockUse != _NORMAL_BLOCK)
		{
			continue;	// ignore everything but normal blocks
		}

		static char szBuf[1000]; // dont take any chances
		static char szBuf2[800]; // dont take any chances
		if ( pblk->szFileName != NULL && !strncmp( pblk->szFileName, "strcore", 7) )
		{
			CStringData *data = (CStringData *)(pblk+1);
			if( data->nAllocLength < 800 )
			{
				strcpy(szBuf2,data->data());
				int i=0;
				while( (szBuf2[i] != '\0') && (i<800) ){
					if(szBuf2[i]=='\t' || szBuf2[i]=='\n')
						szBuf2[i] = '~';
					i++;
				}
				sprintf(szBuf, "%s\t%d\t%d\n", szBuf2, data->nRefs, data->nAllocLength );
			} else {
				sprintf(szBuf, "%s\t%d\t%d\n", "string too long", data->nRefs, data->nAllocLength );
			}

			WriteFile((HANDLE)hfile, szBuf, strlen(szBuf), &cbWritten, NULL);
		}
	}
	::CloseHandle((HANDLE)hfile);
	ShellExecute(NULL,NULL,"\\strings.xls",NULL,NULL,0);
	ShellExecute(NULL,NULL,"\\heap.xls",NULL,NULL,0);

	return;
}

#endif
