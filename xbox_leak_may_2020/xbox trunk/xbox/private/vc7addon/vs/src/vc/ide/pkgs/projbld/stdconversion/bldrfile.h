// CBuildFile
//
// Build file reading and writing.
//
// [matthewt]
//
				   
#ifndef _BUILD_FILE_H_
#define _BUILD_FILE_H_


#include "makread.h"

extern const UINT nVersionNumber;
extern const UINT nLastCompatibleMakefile;
extern const UINT nLastCompatibleOPTFile;

// Computed value do not alter
extern const UINT nVersionMajor;
extern const UINT nVersionMinor;
extern const UINT nLastCompatibleMakefileMinor;

class CNameMunger;

class CBuildFile 
{
public:
	CBuildFile();
	virtual ~CBuildFile();

	//
	// Top-level read/write
	//

	// read in the VC++ builder file
	BOOL ReadBuildFile(const CProject * pBuilder);

	// 'NMake syntax' section read/write
	BOOL ReadSectionForNMake(const TCHAR * pchname);

	CMakFileReader * m_pmr;

private:
	// current builder we are reading or writing
	CProject * m_pBuilder;

	// list of tools which were included in the read/write
	CVCPtrList m_lstToolsUsed;
};

// our *single* instance of the build file reader and writer
extern  CBuildFile g_buildfile;

#endif // _BUILD_FILE_H_
