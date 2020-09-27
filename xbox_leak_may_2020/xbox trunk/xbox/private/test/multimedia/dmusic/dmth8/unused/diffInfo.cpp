#include "diffInfo.h"
#include <assert.h>
#include <stdio.h>

using namespace structDiff;

static ReportFn         report = printf;
static LevelReportFn    levelReport;        // Default is NULL
static DWORD            currentReportLevel;

//////////////////////////////////////////////////////////////////////////
//
//  Compare
//
//  This function compares two structures, given as pPMsg1 and pPMsg2
//  
//  Return Value:
//      E_FAIL   if Error occurred or invalid params
//      S_FALSE  if pPMsg1 != pPMsg2
//      S_OK     if pPMsg1 == pPMsg2
//
//////////////////////////////////////////////////////////////////////////
HRESULT structDiff::CompareImpl (LPVOID pPMsg1, LPVOID pPMsg2, Info* pDiffInfo, int nFields, int size)
{
    HRESULT hr = S_OK;  // Default to equivalency, modify only if something doesn't match.

    if (!memcmp (pPMsg1, pPMsg2, size))
    {
        return hr;
    }

    LPCSTR pLhs = ((LPCSTR) pPMsg1);
    LPCSTR pRhs = ((LPCSTR) pPMsg2);

    for (int i=0;i < size;i++, pLhs++, pRhs++)
    {
        if (*pLhs == *pRhs)
        {
            continue;
        }

        for (int j=0;j<nFields;j++)
        {
            char    value1[16];
            char    value2[16];
            long    iValue1;
            long    iValue2;

            if (i >= pDiffInfo[j].baseOffset && (i - pDiffInfo[j].baseOffset) < pDiffInfo[j].size)
            {
                iValue1 = 0;
                iValue2 = 0;

                assert (pDiffInfo[j].size <= sizeof (long));
    
                memcpy (&iValue1, ((LPCSTR) pPMsg1) + pDiffInfo[j].baseOffset, pDiffInfo[j].size);
                memcpy (&iValue2, ((LPCSTR) pPMsg2) + pDiffInfo[j].baseOffset, pDiffInfo[j].size);

                sprintf (value1, pDiffInfo[j].pszFieldFormat, iValue1);
                sprintf (value2, pDiffInfo[j].pszFieldFormat, iValue2);

                if (levelReport)
                {
                    levelReport (currentReportLevel, "PMsg1->%s = %s, pPMsg2->%s = %s\n",
                        pDiffInfo[j].pszFieldName, value1, pDiffInfo[j].pszFieldName, value2);
                }
                else
                {
                    report("PMsg1->%s = %s, pPMsg2->%s = %s\n",
                        pDiffInfo[j].pszFieldName, value1, pDiffInfo[j].pszFieldName, value2);
                }

                // Offset both sides so we don't find this element again
                int elementRemain = pDiffInfo[j].size - (i - pDiffInfo[j].baseOffset);
                pLhs += elementRemain - 1;
                pRhs += elementRemain - 1;
                i += elementRemain - 1;

                hr = S_FALSE;
                break;
            }
        }
    }


    return hr;
}

void structDiff::SetReportFn (ReportFn reportFn)
{
    if (reportFn)
    {
        report = reportFn;
    }
    else
    {
        report = printf;
    }
}

void structDiff::SetReportFn (LevelReportFn reportFn)
{
    levelReport = reportFn;
}

DWORD structDiff::SetReportLevel (DWORD level)
{
    DWORD retv = currentReportLevel;
    currentReportLevel = level;
    
    return (retv);
}
