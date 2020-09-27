/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    StepsLog.h

Abstract:
    This object can be used to log test steps and variations to a file
    so that cases can be run manually

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-10-2000  Created

Notes:

*/

#ifndef _STEPSLOG_H_
#define _STEPSLOG_H_

#include <xtl.h>
#include <stdio.h>

#define DEFAULT_LOG_NAME  L"steps"
#define DEFAULT_DRIVE     L"t:\\"
#define HTML_FILE_EXT     L".htm"
#define TEXT_FILE_EXT     L".txt"
#define USE_HTML_DEFAULT  true

// The Header/Footer is placed at the very beginning and end of the HTML file
#define HTML_HEADER       L"<HTML>\n  <HEAD>\n    <TITLE>Test Steps</TITLE>\n  </HEAD>\n  <SCRIPT language=\"JScript\">\n    function ToggleVariations(DivTag)\n    {\n      if (DivTag.style.display == \"block\")\n      {\n        DivTag.style.display = \"none\";\n      }\n      else\n      {\n        DivTag.style.display = \"block\";\n      }\n    }\n  </SCRIPT>\n  <BODY>\n    <TABLE border=\"1\" width=\"100%%\">\n"
#define HTML_FOOTER       L"    </TABLE>\n  </BODY>\n</HTML>\n"

// The Header/Footer surrounds all of the steps for a single test case
#define HTML_STEP_HEADER  L"          <OL>\n"
#define HTML_STEP_FOOTER  L"          </OL>\n"
// The begin/end surrounds each individual step of a test case
#define HTML_STEP_BEGIN   L"            <LI>"
#define HTML_STEP_END     L"\n"

// The Header/Footer surrounds all of the variations for a single test case
#define HTML_VAR_HEADER   L"          <INPUT type=\"button\" value=\"Toggle Variations\" onclick=\"ToggleVariations(VAR%d)\"><br><br>\n          <DIV id=\"VAR%d\" STYLE=\"display:block\">\n"
#define HTML_VAR_FOOTER   L"          </DIV>\n"
// The begin/end surrounds each individual variation of a test case
#define HTML_VAR_BEGIN    L"            <STRONG>&nbsp;&nbsp;"
#define HTML_VAR_END      L"</STRONG><br>\n"

#define MAX_STEPS         1024
#define STEP_SIZE         1024

#define MAX_VARIATIONS    2048
#define VARIATION_SIZE    1024

extern "C"
{
        ULONG DebugPrint(PCHAR Format, ...);
}

class CStepsLog
{
public:

    // Constructors and Destructors
    CStepsLog();
    ~CStepsLog();

    // Log Specific Methods
    void OpenLog      ( char* lpszLogFileName /*=NULL*/ );
    void LogTestCase  ();
    void LogCaseName  ();
    void LogVariations();
    void LogSteps     ();
    void CloseLog     ();

    // Case Specific Methods
    void SetCaseName ( const WCHAR* caseName );
    void AddStep     ( const WCHAR* lpwszFormat, ... );
    void AddVariation( const WCHAR* lpwszFormat, ... );
    
    // These functions will remove any user defined information
    // ClearTestCase will clear ALL information
    void ClearTestCase     ();
    void ClearTestCaseName ();
    void ClearSteps        ();
    void ClearVariations   ();
    
    // Get a pointer to a specific step or Variation
    WCHAR* GetStep     ( const size_t index );
    WCHAR* GetVariation( const size_t index );

    // Accessors
    void SetHTMLLogFormat( BOOL bUseHTML );

private:
    // Properties
    FILE* m_LogFile;
    BOOL m_bHTMLLogFormat;

    BOOL m_bCaseLogged;
    size_t m_numTestCases;

    WCHAR* m_currentCaseName;
    
    size_t m_maxSteps;
    size_t m_currentStep;
    WCHAR** m_steps;

    size_t m_maxVariations;
    size_t m_currentVariation;
    WCHAR** m_variations;
};

#endif //_CSTEPSLOG_H_