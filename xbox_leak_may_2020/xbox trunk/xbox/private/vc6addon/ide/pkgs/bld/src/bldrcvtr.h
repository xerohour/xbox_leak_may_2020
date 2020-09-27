// 
// CMak* (builder components) and CBuilderConverter
//
// Conversion of old VC++ 1.x builder files (.mak)
//
// [matthewt]
//

#ifndef _INCLUDE_BLDRCVTR_H
#define _INCLUDE_BLDRCVTR_H

#ifndef _SUSHI_PROJECT
// If you are including this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

// CMak* classes
// These classes represent the various syntactic elements of a
// builder makefile.
class BLD_IFACE CMakComment : public CObject
{
	DECLARE_DYNAMIC(CMakComment)

public:
	// constructor + destructor
	CMakComment(const TCHAR *);
	virtual ~CMakComment();

	CString		m_strText;	// text of comment

#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif
};

class BLD_IFACE CMakMacro : public CObject
{
	DECLARE_DYNAMIC(CMakMacro)

public:
  	// constructor + destructor
	CMakMacro(const CString&, const CString &);
	virtual ~CMakMacro();

 	CString		m_strName;		// macro name
	CString		m_strValue;		// macro value
	CString		m_strRawData;	// raw data

#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif
};

class BLD_IFACE CMakDescBlk : public CObject
{
	DECLARE_DYNAMIC(CMakDescBlk)

public:
  	// constructor + destructor
	// Note, the object is initially constructed with the targets
	// and dependencies strings.  Commands are added later
	// via AddCommand().
	CMakDescBlk(const CString &, const CString &);
	virtual ~CMakDescBlk();

	// Add a command to the list of commands.
	void AddCommand(const TCHAR *);

	CString		m_strTargets;	// string representing target(s) (left side of ':')
	CString		m_strDeps;		// string representing dependencies (right side of ':')
	CString		m_strTool;
	CStringList	m_listCommands;	// list of commands
	CString		m_strRawData;

#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif
};

class BLD_IFACE CMakDirective : public CObject
{
	DECLARE_DYNAMIC(CMakDirective)

public:
 	// local type
	enum DTYP
	{
		DTYP_CMDSWITCHES,
		DTYP_ERROR,
		DTYP_MESSAGE,
		DTYP_INCLUDE,
		DTYP_IFNDEF,
		DTYP_IFDEF,
		DTYP_IF,
		DTYP_ELSEIFNDEF,
		DTYP_ELSEIFDEF,
		DTYP_ELSEIF,
		DTYP_ELSE,
		DTYP_ENDIF,
		DTYP_UNDEF
	};

 	// constructor + destructor
	CMakDirective(DTYP, const CString &);
	virtual ~CMakDirective();

	DTYP		m_dtyp;			// directive type.
	CString		m_strRemOfLine;	// string containing remainder of line after directive

#ifdef _DEBUG
	virtual void Dump(CDumpContext & dc) const;
#endif
};

class BLD_IFACE CMakError : public CObject
{
	DECLARE_DYNAMIC(CMakError)

public:
  	// constructor + destructor
	CMakError();
	virtual ~CMakError();
};

class BLD_IFACE CMakEndOfFile : public CObject
{
	DECLARE_DYNAMIC(CMakEndOfFile)

public:
  	// constructor + destructor
	CMakEndOfFile();
	virtual ~CMakEndOfFile();
};

// Our CBuilderConverter class which is used to convert old VC++ 1.x builder files (.MAK).
// Each product variant should register one of these, e.g. the x86 C++ package and the x86 FORTRAN package.
// information block for the conversion of old Caviar/Cuda/Sanchovy projects

class CBuilderConverter;
typedef struct tagCNV_INFO
{
	const CPath * pPath;	// path of the builder we are attempting to convert
	CProject * pProject;	// builder we are creating
	
	int nStatus;			// wants to attempt (1) or abort (-1) this conversion?
	BOOL fSilent;			// silent conversion? used for converting samples without prompting
	BOOL f32Bit;			// is this a 32-bit project? (read-only)
	int nUseMFC;			// project using MFC == (read/write)
							// NoUseMFC
							// UseMFCInLibrary 
							// UseMFCInDll (UseMFCDefault)

	CBuilderConverter *	pbldrcnvtr;	// the builder converter that's performing the conversion
} CNV_INFO;

class BLD_IFACE CBuilderConverter : public CBldSysCmp, public COptHdlrMapper
{
	DECLARE_DYNAMIC(CBuilderConverter)

public:
	// contructor + destructor
	CBuilderConverter(const TCHAR * szPkg, WORD id);
	virtual ~CBuilderConverter();
	
	// Initialise the converter prior to builder conversion.
	virtual BOOL FInitialise();

	// Can this builder converter do the conversion?
	// Return FALSE if cannot, return TRUE if still deciding and set
	// 'pcnvinfo->fAttempt' to TRUE if would like to attempt.
	virtual BOOL CanConvert(CObject * pMakObj, UINT nCurrLine, CNV_INFO * pcnvinfo) = 0;

	// Do the conversion...
	// Return FALSE if cannot, return TRUE if successful.
	virtual BOOL DoConvert(CObject * pMakObj, UINT nCurrLine, CNV_INFO * pcnvinfo) = 0;

	// Finish the conversion
	virtual BOOL FinishConvert(CNV_INFO * pcnvinfo) = 0;

private:
};

#endif // _INCLUDE_BLDRCVTR_H
