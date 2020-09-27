/////////////////////////////////////////////////////////////////////////////
// textlog.h
//
// email	date		change
// cflaat	11/01/94	created
//
// copyright 1994 Microsoft

// CTextLog interface

#ifndef __TEXTLOG_H__
#define __TEXTLOG_H__

#pragma warning(disable: 4251)

#include "log.h"

/*
    CTextLog
    ========

    This file and design copyright 1993-94 by Microsoft Corp.
    Created by CFlaat, based on the CLogRecorder class by RickKr.
    
      This class is a particular implementation of the CLog interface.
    The constructor can take two strings.  The first is the name of the
    final results output file, and the second is the directory in which
    temporary files should be stored.

*/
			   
class AFX_EXT_CLASS CTextLog : public CLog
{
public:  // CLog overrides

  CTextLog(LPCSTR szResultsFile = "results.log",
           LPCSTR szSummaryFile = "summary.log",
           LPCSTR szWorkDir = 0,
           HANDLE hOutput = 0,
           BOOL bLogComments = FALSE);

  virtual ~CTextLog(void);

  virtual BOOL BeginSubSuite(LPCSTR, LPCSTR);
  virtual BOOL EndSubSuite(void);
  virtual BOOL BeginTest(LPCSTR szTestName, long cCompares = -1);
  virtual BOOL EndTest(void);      

  virtual BOOL RecordTestKeys(PRINTF_ARGS ); //Records test keystrokes.
  virtual BOOL RecordSuccess(PRINTF_ARGS);            // product passed a test
  virtual BOOL RecordFailure(PRINTF_ARGS);        // product failed a test
  virtual BOOL RecordInfo(PRINTF_ARGS);			      // recorded but ignored
  virtual BOOL RecordTextFile(LPCSTR szFile, PRINTF_ARGS);  // append a text file to the log
  virtual BOOL RecordCriticalError(PRINTF_ARGS);  // called for errors that force test termination
  virtual BOOL RecordCompare(BOOL bSuccess, PRINTF_ARGS);		// called to record based on bSuccess

  virtual BOOL Comment(PRINTF_ARGS);              // chatty trace commentary that can be ignored
                                                      // or dumped at the CAFE driver's discretion

protected:
  friend class CSuiteDoc;

  void EmitSummaryInformation( void );
  void EmitHeaderInformation( void );

  CStdioFile
	  m_sfHeader,
	  m_sfDetails,
	  m_sfSummary,
	  m_sfResults;

  enum EntryType { Header, Summary, Comments, Results, FailureDetails, InfoDetails, Separator, SuccessDetails, None };

  void EmitFieldAndValue( EntryType eType, LPCSTR szField, LPCSTR szValue );

  void EmitLine(EntryType, LPCSTR); 
  void ConcatStdioFiles(CStdioFile &dest, CStdioFile &src);
  void EmitBanner( EntryType eType, LPCSTR pszBannerTitle );
  void EmitSeparator(LPCSTR pszSeparatorTitle );

  void EmitDetails( EntryType eType, LPCSTR);

  static const int m_nIndent;  // # of spaces to indent from left

  int m_nLastDetailBanner;
};

#endif // __TEXTLOG_H__
