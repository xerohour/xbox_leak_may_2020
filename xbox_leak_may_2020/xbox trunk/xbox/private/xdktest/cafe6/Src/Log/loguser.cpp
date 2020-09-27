
#include "stdafx.h"
#include "log.h"


int main(void)
{
  CLog *pLog;
  CString strLogName("test.log");

  CFile *pfOutput = new CFile("output.txt", CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeBinary);


  pLog = new CTextLog(     // this will be done by the driver
    "Suite 7",  // suite name
    "This is a sample suite for CTextLog.",  // suite description
    strLogName,  // name of final output file
    "summary.log",  // name of summary file for TP3
    ".",  // work directory
    (HANDLE)pfOutput->m_hFile,    // output handle
    TRUE);  // record comments in log

  if (!pLog)
    return 1;


// first test

  pLog -> BeginTest("Dialog Box Check", "Debugger Sniff", "WaltCr");   // done by each subsuite

  pLog -> RecordFailure("My quickwatch dialog didn't appear!");
  pLog -> Comment("I was looking for caption 'QuickWatch'...");
  pLog -> RecordInfo("Our build took 32 seconds.");
  pLog -> RecordFailure("My breakpoints dialog didn't appear!");
  pLog -> Comment("BTW, the hWnd of the IDE was 0x000004f6");

  pLog -> EndTest();  // done by each subsuite




// second test

  pLog -> BeginTest("Build System Check", "System Sniff");   // done by each subsuite

  pLog -> Comment("We appear to have less than 10MB free, which is risky for this test.");

  pLog -> EndTest();  // done by each subsuite
  
  delete pLog;

  pfOutput->Flush();
  delete pfOutput;

  return 0;
}

