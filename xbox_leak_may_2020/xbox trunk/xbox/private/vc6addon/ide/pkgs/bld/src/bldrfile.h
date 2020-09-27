// CBuildFile
//
// Build file reading and writing.
//
// [matthewt]
//
				   
#ifndef _BUILD_FILE_H_
#define _BUILD_FILE_H_


extern const UINT nVersionNumber;
extern const UINT nLastCompatibleMakefile;
extern const UINT nLastCompatibleOPTFile;

// Computed value do not alter
extern const UINT nVersionMajor;
extern const UINT nVersionMinor;
extern const UINT nLastCompatibleMakefileMinor;


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

	// write out the VC++ builder file
	BOOL WriteBuildFile(const CProject * pBuilder);

	// 'NMake syntax' section read/write
	BOOL ReadSectionForNMake(const TCHAR * pchname);
	BOOL WriteSectionForNMake(CBldrSection * pbldsect);

	// 
	// 'NMake syntax' tools write
	// Write for 'pItem' in configs. 'pCfgArray' (default is use builder's) using the
	// 'nm' name mangler.
	//
	BOOL WriteToolForNMake(CProjItem * pItem, CNameMunger * nm, CPtrArray * pCfgArray = (CPtrArray *)NULL);

	// Current build tool mark to use
	static WORD m_wToolMarker;

	// FUTURE: move these two back to private: when all CProject read/write is moved here
	// current reader
	CMakFileReader * m_pmr;

	// current project write
	CMakFileWriter * m_ppw;

	// current makefile write
	CMakFileWriter * m_pmw;

	// current depfile write
	CMakFileWriter * m_pdw;

private:
	// current builder we are reading or writing
	CProject * m_pBuilder;

	// list of tools which were included in the read/write
	CPtrList m_lstToolsUsed;

	// Store the refs we add into the dep graph for project dependencies in this list
	// so we can remove them after writing the makefile
	CPtrList m_lstProjDepRefs;

	void AddProjDepsToGraph();
	void RemoveProjDepsFromGraph();
};

// our *single* instance of the build file reader and writer
extern BLD_IFACE CBuildFile g_buildfile;

#endif // _BUILD_FILE_H_
