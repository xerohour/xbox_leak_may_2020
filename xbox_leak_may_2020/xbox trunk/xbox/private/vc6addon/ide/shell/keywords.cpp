#include "stdafx.h"
#include "keywords.h"

CKeywords::CKeywords(const char *pszFilename):m_pszFilename(0),m_nFileSize(0),m_pszIndexBuf(0),m_nIndexBufSize(0)
{
	// Set filename
	m_pszFilename = (char *)malloc(strlen(pszFilename)+1);
	strcpy(m_pszFilename,pszFilename);
	
	// Init index buffer
	m_pszIndexBuf = (char *)malloc(2);
	strcpy(m_pszIndexBuf,"\n");
	m_nIndexBufSize = 2;
	
	// Init header
	memset(m_nHeader,0,sizeof(m_nHeader));
	FILE *file = fopen(m_pszFilename,"rb");

	if(file)
	{
		if(fread(m_nHeader,sizeof(long),26,file)!=26)
		{
			strcpy(m_pszFilename,""); // force IsKeyword() to always return FALSE
		}
		else
		{
			fseek(file,0,SEEK_END);
			m_nFileSize = ftell(file);
		}
		
		fclose(file);
	}
	else
	{
		strcpy(m_pszFilename,""); // force IsKeyword() to always return FALSE
	}
}

CKeywords::~CKeywords()
{
	if(m_pszFilename)
	{
		free(m_pszFilename);
		m_pszFilename = 0;
	}
	if(m_pszIndexBuf)
	{
		free(m_pszIndexBuf);
		m_pszIndexBuf   = 0;
		m_nIndexBufSize = 0;
	}
}

int CKeywords::IsKeyword(const char *pszKeyword)
{
	int bFound = 0;
	
	// Valid filename?
	if(!m_pszFilename || strlen(m_pszFilename)<1) 
	{
		return 0;
	}
	
	// Valid keyword?
	if(!pszKeyword || strlen(pszKeyword)<1) 
	{
		return 0;
	}
	
	// prepend "\n" and make lowercase
	char *keyword = (char *)malloc(strlen(pszKeyword)+2);
	strcpy(keyword,"\n");
	strcat(keyword,pszKeyword);
	_strlwr(keyword);
	
	// do we need to load a new block of indices?
	if(m_nIndexBufSize<3 || keyword[1]!=m_pszIndexBuf[1])
	{
		// Assume number or symbol (anything that occurs before an 'a')
		long nBlockStart = sizeof(m_nHeader); 
		long nBlockSize  = m_nHeader[0] - sizeof(m_nHeader);
		
		if(keyword[1]>='a' && keyword[1]<'z') // 'a'-'y'
		{
			nBlockStart = m_nHeader[keyword[1]-'a']; 
			nBlockSize  = m_nHeader[keyword[1]-'a'+1] - nBlockStart;
		}
		else if(keyword[1]>='z') // 'z' or symbol that occurs after 'z'
		{
			nBlockStart = m_nHeader[25]; // 'z'
			nBlockSize  = m_nFileSize - nBlockStart;
		}
		
		// need to resize the buffer?
		if(nBlockSize>m_nIndexBufSize-2) // sub 1 for the '\n' and '\0'
		{
			m_pszIndexBuf = (char *)realloc(m_pszIndexBuf,nBlockSize+2);
			m_nIndexBufSize = nBlockSize+2;
			strcpy(m_pszIndexBuf,"\n");
		}
		
		// load index block
		FILE *file = fopen(m_pszFilename,"rb");
		fseek(file,nBlockStart,SEEK_SET);
		if(fread(&(m_pszIndexBuf[1]),1,nBlockSize,file)==(size_t)nBlockSize)
		{
			m_pszIndexBuf[m_nIndexBufSize-1]='\0';
			_strlwr(m_pszIndexBuf);
		}
		else
		{
			// something went wrong. Force IsKeyword() to return FASLE.
			strcpy(m_pszIndexBuf,"\n");
		}
		fclose(file);
	}

	// search
	if(strstr(m_pszIndexBuf,keyword))
	{
		bFound = 1;
	}
	
	return bFound;
}
