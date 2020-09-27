/////////////////////////////////////////////////////////////////////////////
// textlog.cpp
//
// email	date		change
// cflaat	11/01/94	created
//
// copyright 1994 Microsoft

// CTextLog member functions


#include "stdafx.h"

#define EXPORT_LOG	// Make sure implicit lib linkage doesn't occur.
#include "textlog.h"

#define new DEBUG_NEW

#undef __USE_RTTI  // to accommodate the MIPS folks

const int CTextLog::m_nIndent = 22;
                                    
CTextLog::CTextLog(LPCSTR szResultsFile /* = "results.log" */,
                   LPCSTR szSummaryFile /* = "summary.log" */,
                   LPCSTR szWorkDir /* = 0*/, 
                   HANDLE hOutput /* = 0*/,
                   BOOL bLogComments /* = FALSE*/ )
  : CLog(hOutput, bLogComments)
{
  ASSERT(szResultsFile && *szResultsFile && szSummaryFile && *szSummaryFile);

  m_nLastDetailBanner = -1;

  CString strLogDir(szWorkDir ? szWorkDir : ".");

  strLogDir += "\\";

  // The summary & results log names are configurable because TP3 is
  // dependent upon their exact name and form.

  VERIFY(m_sfHeader.  Open(strLogDir + "header.txt",  CFile::typeText | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite));
  VERIFY(m_sfDetails. Open(strLogDir + "details.txt", CFile::typeText | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite));
  VERIFY(m_sfSummary. Open(strLogDir + szSummaryFile, CFile::typeText | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite));
  VERIFY(m_sfResults. Open(strLogDir + szResultsFile, CFile::typeText | CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite));


  EmitHeaderInformation();
}


CTextLog::~CTextLog(void)
{
  //REVIEW(CFlaat): should the m_bTestRunning check be encapsulated up in CLog?
  if (m_bTestRunning)  // abnormal termination
  {
    EndTest();
    TRACE("Hey!  CTextLog destructed before EndTest was called!!!");
  }

  EmitSummaryInformation();
}

BOOL CTextLog::BeginSubSuite(LPCSTR szSubSuiteName, LPCSTR szOwner)
{
  // call the base class
  BOOL bRet = CLog::BeginSubSuite(szSubSuiteName, szOwner);

  if (bRet)
  {
    CString str;
    str.Format("SubSuite: %s - Owned by %s", (LPCSTR)m_strSubSuiteName, (LPCSTR)m_strSubSuiteOwner);
    EmitBanner(Header, str);

  }

  return bRet;
}

BOOL CTextLog::EndSubSuite(void)
{
  // call the base class
  return CLog::EndSubSuite();
}

BOOL CTextLog::BeginTest(LPCSTR szTestName, long cCompares /*= -1*/)
{
  // call the base class
  BOOL bRet = CLog::BeginTest(szTestName, cCompares);

  if (bRet)
  {
    CString str;
    str.Format("Test: %s", (LPCSTR)m_strTestName);
    EmitSeparator(str);  
  }

  return bRet;
}

BOOL CTextLog::EndTest(void)                         // this is called after each test case
{
  // call the base class
  CLog::EndTest();

  CString strV;

  strV.Format("%s - %s", m_cTestFailures? "FAIL": "PASS", (LPCSTR)m_strTestName);

  EmitDetails(Summary, strV);
  EmitLine(Summary, "");

	// this is so the step window doesn't display the name of a test after it is finished.
	m_strTestName.Empty();
  return TRUE;
}

BOOL CTextLog::RecordSuccess(PRINTF_ARGS)            // product passed a test
{
  // call the base class
  CLog::RecordSuccess(0);

  ParseArguments();

  EmitDetails(SuccessDetails, m_acArgBuf);

  return TRUE;
}


BOOL CTextLog::RecordFailure(PRINTF_ARGS)        // product failed a test
{
  // call the base class
  CLog::RecordFailure(0);

  ParseArguments();

  EmitDetails(FailureDetails, m_acArgBuf);

  return TRUE;
}

BOOL CTextLog::RecordTestKeys(PRINTF_ARGS)
{
  ParseArguments();
  
  EmitDetails(None, m_acArgBuf);

  return TRUE;
}

BOOL CTextLog::RecordInfo(PRINTF_ARGS)			      // recorded but ignored
{
  ParseArguments();
  
  EmitDetails(InfoDetails, m_acArgBuf);

  return TRUE;
}

BOOL CTextLog::RecordCompare(BOOL bSuccess, PRINTF_ARGS)		// called to record based on bSuccess
{
	ParseArguments();

	if (bSuccess) {
		RecordSuccess(m_acArgBuf);
	}
	else {
		RecordFailure(m_acArgBuf);
	}

	return bSuccess;
}


BOOL CTextLog::Comment(PRINTF_ARGS)              // chatty trace commentary that can be ignored
{
  ParseArguments();
  
  EmitDetails(Comments, m_acArgBuf);

  return TRUE;
}


BOOL CTextLog::RecordTextFile(LPCSTR szFile, PRINTF_ARGS)  // append a text file to the log
{
  CString str;
  
  ParseArguments();
  
  str.Format("Inserting file '%s': %s", szFile, m_acArgBuf);
  EmitLine(InfoDetails, str);

  try
  {
	CStdioFile *pTextFile = new CStdioFile(szFile, CFile::typeText | CFile::modeRead | CFile::shareDenyWrite);

	ConcatStdioFiles(m_sfDetails, *pTextFile);

	delete pTextFile;
  }
  catch (CFileException* fe)
  {
    RecordInfo("CTextLog::RecordTextFile(): Exception generated trying to read text file");
    fe->Delete();
  }

  return TRUE;
}


BOOL CTextLog::RecordCriticalError(PRINTF_ARGS)  // called for errors that force test termination
{
  EmitLine(FailureDetails, "*** CRITICAL ERROR ***");
  CString str;

  ParseArguments();

  str.Format("==>> %s", m_acArgBuf);
  EmitLine(FailureDetails, str);

  // TODO: find out if we need to throw an exception or some such thing

  // for now, just note that it's a failure

  CLog::RecordFailure(0);

  return TRUE;
}

// protected members

void CTextLog::EmitBanner( EntryType eType, LPCSTR pszBannerTitle)
{
  const LPCSTR BANNER_SEPARATOR = "[=============================================================================]";

  CString str;
  str.Format("[  %-73s  ]", pszBannerTitle);
  EmitLine(eType, BANNER_SEPARATOR);

  // Ignore all banner lines in the LogTree View .
 /* if (str == "Suite Summary")
	EmitLine(eType, str,SUMMARY);
		else
			if (str == "General Information")
				EmitLine(eType, str,HEADER);
				else 
					if (str.Find("Owned by") != -1)
						EmitLine(eType, str,HEADER);
					else 	  */
  EmitLine(eType, str);
  EmitLine(eType, BANNER_SEPARATOR);
  EmitLine(eType, "");
}

void CTextLog::EmitSeparator(LPCSTR pszSeparatorTitle)
{
  CString str;
  str.Format("[--------------------< %-33s >--------------------]", pszSeparatorTitle);

  EmitLine(Separator, str);
  EmitLine(Separator, "");
}

void CTextLog::EmitSummaryInformation( void )
{
  LPCSTR szValueFmt = "%4u";
  LPCSTR szFmt = "%4u  (%u%%)";
  CString str;
  unsigned uPctFailed, uPctPassed;
  double dPctFailed;

  ASSERT(m_cTestsFinished >= m_cSuiteFailures);

  if (m_cTestsFinished)
  {
#ifdef __USE_RTTI
    dPctFailed = 100.0 * static_cast<double>(m_cSuiteFailures) / static_cast<double>(m_cTestsFinished);
#else
    dPctFailed = 100.0 * (double)(m_cSuiteFailures) / (double)(m_cTestsFinished);
#endif

    // the following line rounds to the nearest integer
    //   (I couldn't find any obvious CRT function that rounds to the nearest integer)

#ifdef __USE_RTTI
    uPctFailed = static_cast<unsigned>((0.5) + (dPctFailed));
#else
    uPctFailed = (unsigned)((0.5) + (dPctFailed));
#endif
  
    if (m_cSuiteFailures && (!uPctFailed))  // if failing %age rounded away
      uPctFailed = 1;  // force it to a 1% minimum

    uPctPassed = 100 - uPctFailed;  // this will definitely make 'em add up to 100
  }
  else
  {
    uPctFailed = uPctPassed = 0;
  }

  EmitBanner(Results, "Suite Summary");
  // write total failure info
  str.Format(szValueFmt, m_cTotalFailures);
  EmitFieldAndValue(Results, "Total Failures", str);
  str.Format(szValueFmt, m_cTotalTestFailures);
  EmitFieldAndValue(Results, "Test Failures", str);
  str.Format(szValueFmt, m_cTotalFailures - m_cTotalTestFailures);
  EmitFieldAndValue(Results, "Non-test Failures", str);
  EmitLine(Results, "");
  // write test info
  str.Format("%4u", m_cTestsFinished);
  EmitFieldAndValue( Results, "Tests Run", str);
  str.Format(szFmt, m_cTestsFinished - m_cSuiteFailures, uPctPassed);
  EmitFieldAndValue( Results, "Tests PASSED", str);
  str.Format(szFmt, m_cSuiteFailures, uPctFailed);
  EmitFieldAndValue( Results, "Tests FAILED", str);
  m_sfResults.Flush();
  ConcatStdioFiles(m_sfResults, m_sfHeader);
  ConcatStdioFiles(m_sfResults, m_sfSummary);
  ConcatStdioFiles(m_sfResults, m_sfDetails);
}


void CTextLog::EmitHeaderInformation( void )
{
  CString str;

  EmitBanner(Header, "General Information");
	EmitFieldAndValue( Header, "Command Line", GetCommandLine() );
	EmitFieldAndValue( Header, "Date", CTime::GetCurrentTime().Format( "%A, %b %d %Y") );

  str = CTime::GetCurrentTime().Format("%#I:%M %p"); // get "AM/PM"
  str.MakeLower();   // make it "am/pm"
	EmitFieldAndValue( Header, "Time", str );

	EmitFieldAndValue( Header, "Operating System", GetOSDescription() );

	// Find and emit the computer name.
	DWORD len = MAX_COMPUTERNAME_LENGTH+1;
	LPTSTR lpszNetName = new char[len];
	if (!GetComputerName(lpszNetName, &len))
		strcpy(lpszNetName, "[Error calling GetComputerName()]");
	EmitFieldAndValue( Header, "Computer Name", lpszNetName);
	delete [] lpszNetName;

  CString strProcessor;

  SYSTEM_INFO si;
  GetSystemInfo(&si);
  switch(si.dwProcessorType)
  {
  case PROCESSOR_INTEL_386:
    strProcessor = "Intel 386"; 
    break;
  case PROCESSOR_INTEL_486:
    strProcessor = "Intel 486";
    break;
  case PROCESSOR_INTEL_PENTIUM:
    strProcessor = "Intel Pentium";
    break;


// WARNING:PROCESSOR_INTEL_860, PROCESSOR_MIPS_R2000, PROCESSOR_MIPS_R3000, PROCESSOR_PPC_601, 603, 604, 620
// no longer defined in WINNT.h

  case PROCESSOR_MIPS_R4000:
    strProcessor = "MIPS R4000";
    break;
  case PROCESSOR_ALPHA_21064:
    strProcessor = "Alpha 21064";
    break;

  default:
    strProcessor = "Undefined";
    break;
  }

	EmitFieldAndValue( Header, "Processor", strProcessor );

  const int ccBuf = 400;
  char acBuf[ccBuf];
  LPCSTR pcDrive = acBuf;
  DWORD dw;

  dw = GetLogicalDriveStrings(ccBuf, acBuf);

  BOOL bResult;
  DWORD dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters;
  unsigned uFree;
  double dFree;
  UINT uDriveType;

  CString strDrv, strSpc;

  while (pcDrive[0])
  {
    // see if it's a local hard drive

    uDriveType = GetDriveType(pcDrive);

    if (uDriveType == DRIVE_FIXED)  // if it's a local hard drive
    {
      bResult = GetDiskFreeSpace(
        pcDrive,
        &dwSectorsPerCluster,
        &dwBytesPerSector,
        &dwFreeClusters,
        &dw);

      uFree = dwBytesPerSector * dwSectorsPerCluster * dwFreeClusters;
      dFree = uFree / 1048576.0;  // express as megabytes
#ifdef __USE_RTTI
      uFree = static_cast<unsigned>(dFree + 0.5); // round to nearest MB
#else
      uFree = (unsigned)(dFree + 0.5); // round to nearest MB
#endif

      strDrv.Format("Space on Drive '%c'", toupper(pcDrive[0]));
      strSpc.Format("%uMB", uFree);

  	  EmitFieldAndValue( Header, strDrv, strSpc);
    }

    pcDrive += 1 + strlen(pcDrive);
  }


	// physical memory?
	// free memory?

  EmitLine(Header, "");
}


//	Header Text
//


//  EntryType definition from textlog.h:
//
//    enum EntryType { Header, Summary, Comments, Results, FailureDetails, InfoDetails, Separator };
//


void CTextLog::EmitFieldAndValue( EntryType eType, LPCSTR szField, LPCSTR szValue )
{
  CString str;

  str.Format("%*s: %s", m_nIndent, szField, szValue);  // -> CRASHES!  Olympus bug 2412.

  EmitLine(eType, str);

}


void CTextLog::EmitLine(EntryType eType, LPCSTR szEntry)
{
  ASSERT(szEntry);  // no null pointers
  CStdioFile *pSF;
   switch (eType)
  {
   case Header:
     pSF = &m_sfHeader;
     break;
   case Summary:
     pSF = &m_sfSummary;
     break;
  case InfoDetails:
  case FailureDetails:
  case Separator:
  case SuccessDetails:
  case  None:
     pSF = &m_sfDetails;
     break;
  case Results:
     pSF = &m_sfResults;
     break;
  case Comments:
     pSF = (m_bRecordComments ? &m_sfDetails : 0);
     break;
   default:
     ASSERT(FALSE); // a bad eType param
     pSF = 0;
     break;
  }

  CString str(szEntry);

  str += '\n';

  char acBuf[3] = {"\xff\0"};

  if (pSF)
  {
    pSF->WriteString(str);
    pSF->Flush();
  }

  if (m_pfOutput /* && (eType != Results)*/)
  {
#ifdef __USE_RTTI
    acBuf[1] = static_cast<char>(static_cast<char>(eType) + '0');  // just to make it printable
#else
    acBuf[1] = (char)((char)(eType) + '0');  // just to make it printable
#endif
    ASSERT(2 <= 1 + strlen(acBuf));
    m_pfOutput->Write(acBuf, 2);
    m_pfOutput->Write((LPCSTR)str, str.GetLength());
  }
}

void CTextLog::ConcatStdioFiles(CStdioFile &sfDest, CStdioFile &sfSrc)
{
  DWORD dwSrcLength;
  static const DWORD dwBufLen = 4096;  // 4KB

  static char acBuf[dwBufLen];

  dwSrcLength = sfSrc.GetLength();

  if (!dwSrcLength)
    return;

  sfSrc.SeekToBegin();

  UINT cbRead;

  do
  {
    cbRead = sfSrc.Read(acBuf, dwBufLen);
    sfDest.Write(acBuf, cbRead);
  }
  while ((sfSrc.GetPosition() < dwSrcLength) && (cbRead == dwBufLen));
}

void CTextLog::EmitDetails( EntryType eType, LPCSTR szText)
{
  CString str;

  LPCSTR pc;

  switch (eType)
  {
    case Comments:
      pc = "[Comment]";
      break;
    case InfoDetails:
      pc = "[Information]";
      break;
    case FailureDetails:
      pc = "[***FAILED***]";
      break;
    case Summary:
      pc = "[Result]";
      break;
    case SuccessDetails:
	  pc = "[Success]";
	  break;
	case None:
	  pc =" " ;
	  break;
    default:
      ASSERT(FALSE);  // shouldn't ever get here
      break;
  }

  str.Format("%-16s%s",pc, szText);
  EmitLine(eType, str);
}
