/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    StepsLog.cpp

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

#include "StepsLog.h"
#include <stdio.h>

// Constructor
CStepsLog::CStepsLog()
{
    DebugPrint("CStepsLog::CStepsLog()\r\n");

    m_LogFile          = NULL;
    m_bHTMLLogFormat   = USE_HTML_DEFAULT;

    m_bCaseLogged      = false;
    m_numTestCases     = 0;
    
    m_currentCaseName  = NULL;
    
    m_maxSteps         = MAX_STEPS;
    m_currentStep      = 0;
    m_steps            = new WCHAR*[MAX_STEPS];

    m_maxVariations    = MAX_VARIATIONS;
    m_currentVariation = 0;
    m_variations       = new WCHAR*[MAX_VARIATIONS];

    // Initialize all our step pointers to NULL
    for( size_t i=0; i < MAX_STEPS; ++i )
    {
        m_steps[i] = NULL;
    }

    // Initial all our variation pointers to NULL
    for( i=0; i < MAX_VARIATIONS; ++i )
    {
        m_variations[i] = NULL;
    }
}

// Destructor
CStepsLog::~CStepsLog()
{
    DebugPrint("CStepsLog::~CStepsLog()\r\n");

    // If our log file is opened, then lets close it
    if ( m_LogFile )
    {
        CloseLog();
    }

    // Clean up our arrays -- This will delete memory allocated for the test case name,
    // and memory for each WCHAR array within m_steps and m_variations
    ClearTestCase();

    // Clear out the space we allocated for our test steps
    if( m_steps )
    {
        delete[] m_steps;
        m_steps = NULL;
    }

    // Clear out the space we allocated for our test variations
    if( m_variations )
    {
        delete[] m_variations;
        m_variations = NULL;
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let clear out the memory used by the current test case.
  That includes zeroing out the test case name memory, and deleting all memory
  set by the m_steps and m_variations arrays.

  NOTE: This should be called between each test case!

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::ClearTestCase()
{
    ClearTestCaseName();
    ClearSteps();
    ClearVariations();
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let zero out the memory used by the current test case name.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::ClearTestCaseName()
{
    // Reset our flag indicating whether or not this case was logged
    m_bCaseLogged = false;

    // Clear out the memory space we have allocated for our test case name
    if( m_currentCaseName )
    {
        delete[] m_currentCaseName;
        m_currentCaseName = NULL;
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let clear out the memory used by the current set of steps so
  that a new set of steps can be created.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::ClearSteps()
{
    // Set our current step to 0
    m_currentStep = 0;

    // Destroy any memory we have allocated
    if( m_steps )
    {
        for( size_t i=0; i < MAX_STEPS; ++i )
        {
            if( m_steps[i] )
            {
                delete[] m_steps[i];
                m_steps[i] = NULL;
            }
        }
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let clear out the memory used by the current set of
  variations so that a new set of steps can be created.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::ClearVariations()
{
    // Set our current step to 0
    m_currentVariation = 0;

    // Destroy any memory we have allocated
    if( m_variations )
    {
        for( size_t i=0; i < MAX_VARIATIONS; ++i )
        {
            if( m_variations[i] )
            {
                delete[] m_variations[i];
                m_variations[i] = NULL;
            }
        }
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let set the name of the current Test Case

Arguments:

  caseName - An array of wide characters that is the "test case name" the user
             wishes to use for the current steps and variations.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::SetCaseName( const WCHAR* caseName )
{
    if( !caseName )
    {
        DebugPrint( "CStepsLog::SetTestCaseName():caseName not set!\r\n" );
        return;
    }

    size_t caseNameLen = wcslen( caseName ) + 1;
    
    // If there is already a case name, let's clear out the memory
    if( m_currentCaseName )
    {
        ClearTestCaseName();
    }

    m_currentCaseName = new WCHAR[caseNameLen];
    wcscpy( m_currentCaseName, caseName );
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let the user add steps to the class array.

Arguments:

  lpwszFormat - An array of wide characters that is the "step" the user
                wishes to add to the array.  This is the formated string.

  ... - A list of arguments used with the formated string passed in to form
        the step

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::AddStep( const WCHAR* lpwszFormat, ... )
{
    // DebugPrint("  CStepsLog::AddStep()\r\n");

    // Make sure we were passed in a valid pointer
    if ( !lpwszFormat )
    {
        DebugPrint( "  CStepsLog::AddStep():!lpwszFormat is true!\r\n" );
        return;
    }

    va_list args;
    va_start( args, lpwszFormat );

    WCHAR step[STEP_SIZE];

    vswprintf( step, lpwszFormat, args );

    // DebugPrint( "CStepsLog::AddStep():step = '%S'\r\n", step );

    // Get the length of the information the user wishes to add (plus space for a \0)
    size_t newStepLength = wcslen( step ) + 1;
    // DebugPrint( "CStepsLog::AddStep():newStepLength = '%d'\r\n", newStepLength );

    // Add the step if there isn't already one here
    if( ( !m_steps[m_currentStep] ) && ( m_currentStep < MAX_STEPS ) )
    {
        // DebugPrint( "CStepsLog::AddStep():Creating Step '%d'.\r\n", m_currentStep );

        m_steps[m_currentStep] = new WCHAR[newStepLength];
        if( !m_steps[m_currentStep] )
        {
            DebugPrint( "CStepsLog::AddStep():Couldn't allocate memory!!\r\n" );
            return;
        }

        wcscpy( m_steps[m_currentStep], step );

        m_currentStep++;
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let the user add a variation to the m_variations array.

Arguments:

  lpwszFormat - An array of wide characters that is the "variation" the user
                wishes to add to the array.  This is the formated string.

  ... - A list of arguments used with the formated string passed in to form
        the variation

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::AddVariation( const WCHAR* lpwszFormat, ... )
{
    // DebugPrint("CStepsLog::AddVariation()\r\n");

    // Make sure we were passed in a valid pointer
    if ( !lpwszFormat )
    {
        DebugPrint( "  CStepsLog::AddVariation():!lpwszFormat is true!\r\n" );
        return;
    }

    va_list args;
    va_start( args, lpwszFormat );

    WCHAR variation[STEP_SIZE];

    vswprintf( variation, lpwszFormat, args );

    // DebugPrint( "CStepsLog::AddVariation():variation = '%S'\r\n", variation );

    // Get the length of the information the user wishes to add (plus space for a \0)
    size_t newVariationLength = wcslen( variation ) + 1;
    // DebugPrint( "CStepsLog::AddVariation():newVariationLength = '%d'\r\n", newVariationLength );

    // Add the step if there isn't already one here
    if( ( !m_variations[m_currentVariation] ) && ( m_currentVariation < MAX_VARIATIONS ) )
    {
        // DebugPrint( "CStepsLog::AddVariation():Creating Variation '%d'.\r\n", m_currentVariation );

        m_variations[m_currentVariation] = new WCHAR[newVariationLength];
        if( !m_variations[m_currentVariation] )
        {
            DebugPrint( "CStepsLog::AddVariation():Couldn't allocate memory!!\r\n" );
            return;
        }

        wcscpy( m_variations[m_currentVariation], variation );

        m_currentVariation++;
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will return a pointer to the step that the user requested

Arguments:

  index - The index of the step the user wishes to access.  Must be 0 based, and
          cannot exceed m_currentStep

Return Value:

------------------------------------------------------------------------------*/
WCHAR* CStepsLog::GetStep( const size_t index )
{
    if( ( index > m_currentStep ) || ( index < 0 ) )
    {
        return NULL;
    }

    return m_steps[index];
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will return a pointer to the variation that the user requested

Arguments:

  index - The index of the step the user wishes to access.  Must be 0 based, and
          cannot exceed m_currentStep

Return Value:

------------------------------------------------------------------------------*/
WCHAR* CStepsLog::GetVariation( const size_t index )
{
    if( ( index > m_currentVariation ) || ( index < 0 ) )
    {
        return NULL;
    }

    return m_variations[index];
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will let the user set the type of log to output (HTML, or plain text)

Arguments:

  bUseHTML - Boolean variable.  If true, the log will be output in HTML format.  If false, it will be plain text.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::SetHTMLLogFormat( BOOL bUseHTML )
{
    DebugPrint("CStepsLog::SetHTMLLogFormat\r\n");

    if ( !m_LogFile )
    {
        m_bHTMLLogFormat = bUseHTML;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will open a file to log the desired test steps.

Arguments:

  lpszLogFileName - Pointer to a null-terminated string that specifies the filename to which the log output will be directed.  This member is NULL if the user wishes to use the default filename.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::OpenLog( char* lpszLogFileName = NULL )
{
    DebugPrint("CStepsLog::OpenLog\r\n");

    // If we don't already have a log open, let's create one
    if ( !m_LogFile )
    {
        // If the user does not specify a log file name
        // lets use the default
        if ( lpszLogFileName ) // Use the user defined log name
        {
            m_LogFile = fopen( lpszLogFileName, "w+" );
        }
        else  // Use the default log name
        {
            char* lpwszFileName = NULL;

            if ( m_bHTMLLogFormat )
            {
                lpwszFileName = new char[wcslen( DEFAULT_DRIVE ) + wcslen( DEFAULT_LOG_NAME ) + wcslen( HTML_FILE_EXT ) + 1];
                if( !lpwszFileName )
                {
                    DebugPrint( "CStepsLog::OpenLog(): Couldn't allocate memory!\r\n" );
                    return;
                }

                sprintf( lpwszFileName, "%S%S%S", DEFAULT_DRIVE, DEFAULT_LOG_NAME, HTML_FILE_EXT );
            }
            else
            {
                lpwszFileName = new char[wcslen( DEFAULT_DRIVE ) + wcslen( DEFAULT_LOG_NAME ) + wcslen( TEXT_FILE_EXT ) + 1];
                if( !lpwszFileName )
                {
                    DebugPrint( "CStepsLog::OpenLog(): Couldn't allocate memory!\r\n" );
                    return;
                }
                
                sprintf( lpwszFileName, "%S%S%S", DEFAULT_DRIVE, DEFAULT_LOG_NAME, TEXT_FILE_EXT );
            }

            m_LogFile = fopen ( lpwszFileName, "w+" );

            // Clean up our memory
            delete[] lpwszFileName;
            lpwszFileName = NULL;
        }
        
        // Verify our log file was opened.  If not, print a debug line,
        // if it was, see if we need to export the HTML Header
        if ( !m_LogFile )
        {
            DebugPrint( "Failed to open the Steps Log!\r\n" );
        }
        else if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_HEADER );
        }
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will log the test case name to the file.

Arguments:

  lpszCaseName - Pointer to a null-terminated string that specifies the test case name.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::LogTestCase()
{
    DebugPrint("CStepsLog::LogTestCase()\r\n");

    // if our log file is opened, let's output some formatting (if needed)
    if ( m_LogFile )
    {
        // If the log is in HTML format, let's set it up appropriately, otherwise set it up for TEXT mode
        if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, L"      <TR>\n        <TD>\n" );
        }
        else // text mode
        {
            fwprintf( m_LogFile, L"\n\n" );
        }
        
        LogCaseName();
        LogSteps();
        LogVariations();

        // If the log is in HTML format, let's close it out appropriately
        if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, L"        </TD>\n      </TR>\n" );
        }
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will log the test case name to the file.

Arguments:

  lpszCaseName - Pointer to a null-terminated string that specifies the test case name.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::LogCaseName()
{
    DebugPrint("  CStepsLog::LogCaseName()\r\n");

    // Incrememt the total number of test cases
    ++m_numTestCases;

    // if our log file is opened, and the user passed in a
    // valid pointer, let's log the case name to the file
    if ( m_LogFile )
    {
        // If the log is in HTML format, let's set it up appropriately
        if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, L"          <H3>%d. ", m_numTestCases );
        }
        
        // Log the case name itself
        fwprintf( m_LogFile, m_currentCaseName );

        // If the log is in HTML format, let's close it up appropriately, otherwise close it for TEXT mode
        if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, L"</H3>\n" );
        }
        else
        {
            fwprintf( m_LogFile, L"\n" );
        }
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will log a test variation to the current test case that's being logged.

Arguments:

  lpszVariation - Pointer to a null-terminated string that specifies the test case variation.

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::LogVariations()
{
    DebugPrint("  CStepsLog::LogVariations()\r\n");

    // if our log file is opened
    // pointer, let's log the test steps
    if( m_LogFile )
    {
        // DebugPrint("CStepsLog::LogVariations():m_LogFile is opened.\r\n");

        // Output our opening tag for HTML or TEXT mode
        if( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_VAR_HEADER, m_numTestCases, m_numTestCases );
        }
        else
        {
            fwprintf( m_LogFile, L"\n" );
        }

        for( size_t variationNum=0; variationNum < m_currentVariation; ++variationNum )
        {
            // Format our variation output for HTML or TEXT
            if( m_bHTMLLogFormat )
            {
                fwprintf( m_LogFile, HTML_VAR_BEGIN );
            }
            else // text mode
            {
                fwprintf( m_LogFile, L"  " );
            }

            // Print the variation itself
            fwprintf( m_LogFile, m_variations[variationNum] );

            // If the log is in HTML Format, close variation Tag
            if( m_bHTMLLogFormat )
            {
                fwprintf( m_LogFile, HTML_VAR_END );
            }
            else // text mode
            {
                fwprintf( m_LogFile, L"\n" );
            }
        }

        // Output a closting tag for HTML format mode
        if( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_VAR_FOOTER );
        }
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will log the test steps to the log file.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::LogSteps()
{
    DebugPrint("  CStepsLog::LogSteps()\r\n");

    // if our log file is opened
    // pointer, let's log the test steps
    if( m_LogFile )
    {
        // DebugPrint("CStepsLog::LogSteps():m_LogFile is opened.\r\n");

        // If the log is in HTML Format mode, let's display things properly
        if( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_STEP_HEADER );
        }
        else // text mode
        {
            fwprintf( m_LogFile, L"\n" );
        }

        for( size_t stepNum=0; stepNum < m_currentStep; ++stepNum )
        {
            // Format our step output for HTML or TEXT
            if( m_bHTMLLogFormat)
            {
                fwprintf( m_LogFile, HTML_STEP_BEGIN );
            }
            else
            {
                fwprintf( m_LogFile, L"  %d. ", stepNum+1 );
            }

            // Print the step itself
            fwprintf( m_LogFile, m_steps[stepNum] );


            if( m_bHTMLLogFormat )
            {
                fwprintf( m_LogFile, HTML_STEP_END );
            }
            else // text mode
            {
                fwprintf( m_LogFile, L"\n" );
            }
        }

        // If the log is in HTML Format, close our List Tag
        if( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_STEP_FOOTER );
        }
    }
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  This routine will close the log file if the user is done.
  This routine will be called when the destructor is called.

Arguments:

Return Value:

------------------------------------------------------------------------------*/
void CStepsLog::CloseLog()
{
    DebugPrint("CStepsLog::CloseLog()\r\n");

    // If the log is open, let's close it
    if ( m_LogFile )
    {
        // If the log file is in HTML format, append the HTML footer
        if ( m_bHTMLLogFormat )
        {
            fwprintf( m_LogFile, HTML_FOOTER );
        };

        if ( fclose ( m_LogFile ) == EOF )
        {
            DebugPrint("Failed to close the Steps Log!\r\n");
        };
        
        m_LogFile = NULL;
    }
};
