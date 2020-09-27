// 
// CMak* (builder components)
//
// Conversion of old VC++ 1.x builder files (.mak)
//
// [matthewt]
//

#ifndef _INCLUDE_BLDRCVTR_H
#define _INCLUDE_BLDRCVTR_H

// CMak* classes
// These classes represent the various syntactic elements of a
// builder makefile.
class  CMakComment : public CObject
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

class  CMakMacro : public CObject
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

class  CMakDescBlk : public CObject
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

class  CMakDirective : public CObject
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

class  CMakError : public CObject
{
	DECLARE_DYNAMIC(CMakError)

public:
  	// constructor + destructor
	CMakError();
	virtual ~CMakError();
};

class  CMakEndOfFile : public CObject
{
	DECLARE_DYNAMIC(CMakEndOfFile)

public:
  	// constructor + destructor
	CMakEndOfFile();
	virtual ~CMakEndOfFile();
};

#endif // _INCLUDE_BLDRCVTR_H
