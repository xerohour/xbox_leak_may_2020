#include "stdafx.h"
#pragma hdrstop
#include <locale.h>
#include <vccolls.h>
#include "BuildLogging.h"
#include "msgboxes2.h"
#include "util2.h"
#include "vcprojectengine.h"
#include "FileRegistry.h"
#include "bldhelpers.h"
#include "buildengine.h"
#include "xmlfile.h"

///////////////////////////////////////////////////////////////////////////////
// Log helpers:

CBldLogEnabler::CBldLogEnabler(VCConfiguration *pProjCfg, IVCBuildEngine* pBuildEngine)
{ 
	m_spBuildEngineImpl = pBuildEngine;
	if (m_spBuildEngineImpl)
		m_spBuildEngineImpl->OpenProjectConfigurationLog(pProjCfg); 
}

CBldLogEnabler::~CBldLogEnabler()
{ 
	if (m_spBuildEngineImpl)
		m_spBuildEngineImpl->CloseProjectConfigurationLog();
}

BOOL CDynamicBuildEngine::HaveLogging()
{
	return (m_pLogFile != NULL);
}

STDMETHODIMP CDynamicBuildEngine::OpenProjectConfigurationLog(VCConfiguration *pProjCfg)
{
	if (m_pLogFile != NULL) return S_OK;	// Already open
	if (CVCProjectEngine::s_bBuildLogging == VARIANT_FALSE)
		return S_FALSE;

	if (m_strLogFile.IsEmpty())
	{
		VSASSERT(pProjCfg != NULL, "Trying to open a log file without a config to go with it");
		RETURN_ON_NULL2(pProjCfg, S_FALSE);

		HRESULT hr = S_OK;
		CComQIPtr<IVCPropertyContainer> spPropContainer = pProjCfg;
		VSASSERT(spPropContainer != NULL, "Config is not a property container!?!");
		CComBSTR bstrProjDir;
		if (spPropContainer)
		{
			hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);	// should never have macros in it
			VSASSERT(SUCCEEDED(hr), "Project directory property always available!");
		}
		CStringW strProjDir = bstrProjDir;
		CDirW dirProj;
		dirProj.CreateFromKnown(strProjDir);

		CComBSTR bstrIntDir;
		if (spPropContainer)
		{
			hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_IntermediateDirectory, &bstrIntDir);
			if (hr != S_OK || bstrIntDir.Length() == 0)
			{
				hr = spPropContainer->GetEvaluatedStrProperty(VCCFGID_OutputDirectory, &bstrIntDir);
				if (hr != S_OK || bstrIntDir.Length() == 0)
				{
					bstrIntDir = bstrProjDir;
					hr = S_OK;
				}
			}
			VSASSERT(hr == S_OK && bstrIntDir.Length() > 0, "At least one of Intermediate, Output, or Project directory properties always available!");
		}
		CStringW strIntDir = bstrIntDir;

		CStringW strTmp;
		strTmp.Format(L"%s\\BuildLog.htm", strIntDir);

		CPathW FilePath;
		FilePath.CreateFromDirAndRelative(dirProj, strTmp);
		FilePath.GetFullPath(m_strLogFile);
		if (m_spBuildErrorContext)
		{
		    	// REVIEW(KiP): this is a weird test. do we need this ?
			CStringA strLogFileA = m_strLogFile;
			CStringW strLogFileW = strLogFileA;
			if (strLogFileW != m_strLogFile)
			{
				CStringW strMsg;
				strMsg.Format(IDS_ERR_UNICODE_PATH_PRJ0024, strLogFileW);
				CVCProjectEngine::AddProjectError(m_spBuildErrorContext, strMsg, L"PRJ0024", spPropContainer);
			}
		}

		// create the directory if it doesn't exist...
		// otherwise we definately won't be able to open the log file
		CDirW dirInt;
		dirInt.CreateFromPath( FilePath );
		if( !dirInt.ExistsOnDisk() )
			dirInt.CreateOnDisk();
	}
	if ((m_pLogFile = fopenW(m_strLogFile, m_bFirstUsage ? L"w+t" : L"a+")) == NULL)
	{
		// ATLTRACE("Opening project log file %s failed\n", (const wchar_t *) m_strLogFile);
		return E_FAIL;
	}
	else
	{
		if (m_bFirstUsage)
		{
			CStringW strTrueHeader = L"<html>\n<head>\n";
			CComBSTR bstrEncoding;
			CXMLFile::DetermineDefaultFileEncoding(bstrEncoding);
			CStringW strEncoding = bstrEncoding;
			CStringW strMidHeader;
			strMidHeader.Format(IDS_HTML_HEADER_MARKER, strEncoding);
			strTrueHeader += strMidHeader;
			strTrueHeader += L"</head>\n<body>\n<pre>\n";

			LogTrace((eLogSectionTypes)(eLogHeader|eLogStartSection), strTrueHeader);
			if (m_cReuseLogFile > 0)
				m_bFirstUsage = FALSE;
		}

		SetCurrentLogSection(eLogHeader);

		CStringW strTopText;
		strTopText.LoadString(IDS_HTML_HEADER);
		CStringW strPreTop;
		strPreTop.LoadString(IDS_HTML_PRE_TOP);
		CStringW strPostTop;
		strPostTop.LoadString(IDS_HTML_POST_TOP);
	
		CStringW strHeader = strPreTop + strTopText + strPostTop;
		
		// LogTrace(L"<h1>BuildLog</h1>\n");
		LogTrace(eLogHeader, strHeader);
	}

	return S_OK;
}

STDMETHODIMP CDynamicBuildEngine::CloseProjectConfigurationLog()
{
	if (!m_pLogFile) return S_OK;

	if (m_cReuseLogFile == 0)
	{
		SetCurrentLogSection(eLogFooter);
		CStringW strPreFooter;
		strPreFooter.LoadString(IDS_HTML_PRE_FOOTER);
		CStringW strPostFooter;
		strPostFooter.LoadString(IDS_HTML_POST_FOOTER);

		CStringW strFooter = strPreFooter + strPostFooter;

		LogTrace(eLogFooter, strFooter);
		m_bFirstUsage = TRUE;
		m_cReuseLogFile = 0;
		m_strLogFile.Empty();
	}
	FlushLogSection(eLogAll);

	fclose (m_pLogFile);
	m_pLogFile = NULL;
	return S_OK;
}

CVCStringWList* CDynamicBuildEngine::FindLogBuffer(eLogSectionTypes logSection, BOOL bAdvanceOne /* = FALSE */, 
	eLogSectionTypes* pNewLogSection /* = NULL */)
{
	logSection = (eLogSectionTypes)(logSection & eLogAll);	// mask off any modifiers

	if (bAdvanceOne)
	{
		if (logSection == eLogAll || logSection == eLogNoSection)
			logSection = eLogHeader;
		else if (logSection == eLogHeader)
			logSection = eLogEnvironment;
		else if (logSection == eLogEnvironment)
			logSection = eLogCommand;
		else if (logSection == eLogCommand)
			logSection = eLogOutput;
		else if (logSection == eLogOutput)
			logSection = eLogResults;
		else if (logSection == eLogResults)
			logSection = eLogFooter;
		else if (logSection == eLogFooter)
			logSection = eLogNoSection;

		if (pNewLogSection)
			*pNewLogSection = logSection;
	}
	else if (logSection == eLogAll)
		logSection = m_logCurrentSection;

	switch (logSection)
	{
	case eLogHeader:
		return &m_logHeaderLines;
	case eLogEnvironment:
		return &m_logEnvironmentLines;
	case eLogCommand:
		return &m_logCommandLines;
	case eLogOutput:
		return &m_logOutputLines;
	case eLogResults:
		return &m_logResultsLines;
	case eLogFooter:
		return &m_logFooterLines;
	default:
		return NULL;
	}
}

#define DBG_BUFSIZE 2048
#define MAX_DBG_BUFSIZE (512*DBG_BUFSIZE)	// 1 meg worth.

void CDECL CDynamicBuildEngine::LogTrace(eLogSectionTypes logSection, LPCOLESTR szBuffer)
{
	if (CVCProjectEngine::s_bBuildLogging == VARIANT_FALSE)
		return;

	if (!m_pLogFile)
		return;   	

	CVCStringWList* pLogList = FindLogBuffer(logSection);	// not figuring out the right section == write immediately
	if (pLogList == NULL || (logSection & m_logCurrentSection))
	{
		CStringA strBufferA = szBuffer;		// yes, ANSI -- 'cause we're writing an ANSI log file
		if (fputs(strBufferA, m_pLogFile) == EOF)
			CloseProjectConfigurationLog();
	}
	else
	{
		if (logSection & eLogStartSection)
			pLogList->AddHead(szBuffer);
		else
			pLogList->AddTail(szBuffer);
	}
}

void CDynamicBuildEngine::FlushSingleLogSection(CVCStringWList* pLogList)
{
	if (pLogList == NULL)
		return;

	while (m_pLogFile && !pLogList->IsEmpty())
	{
		CStringA strLine = pLogList->RemoveHead();	// yes, ANSI -- 'cause we're writing an ANSI log file
		if (fputs(strLine, m_pLogFile) == EOF)
			CloseProjectConfigurationLog();
	}
}

void CDynamicBuildEngine::FlushLogSection(eLogSectionTypes logSection)
{
	if (CVCProjectEngine::s_bBuildLogging == VARIANT_FALSE)
		return;
	if( !m_pLogFile)
		return;

	CVCStringWList* pLogList = NULL;
	if ((logSection & eLogAll) != eLogAll)	// flush a single section
	{
		pLogList = FindLogBuffer(logSection);
		FlushSingleLogSection(pLogList);
	}
	else	// go through all of them
	{
		eLogSectionTypes tmpLogSection = eLogNoSection;
		while (m_pLogFile)
		{
			pLogList = FindLogBuffer(tmpLogSection, TRUE, &tmpLogSection);
			if (pLogList == NULL)
				break;

			FlushSingleLogSection(pLogList);
		}
	}
}

void CDynamicBuildEngine::SetCurrentLogSection(eLogSectionTypes logSection)
{
	FlushLogSection(eLogAll);
	m_logCurrentSection = logSection;
}

eLogSectionTypes CDynamicBuildEngine::GetCurrentLogSection()
{
	return m_logCurrentSection;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// CBldCommandLineEC

int CBldCommandLineEC::s_cnt = 0;

HRESULT CBldCommandLineEC::CreateInstance(IVCBuildErrorContext** ppContext, CBldCommandLineEC** ppContextObj,
	IVCBuildEngine* pBuildEngine)
{
	if (ppContext)
		*ppContext = NULL;
	if (ppContextObj)
		*ppContextObj = NULL;

	IVCBuildErrorContext *pVar;
	CComObject<CBldCommandLineEC> *pObj;
	HRESULT hr = CComObject<CBldCommandLineEC>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = (IVCBuildErrorContext*)pObj;
		pVar->AddRef();
		if (ppContext)
			*ppContext = pVar;
		if (ppContextObj)
			*ppContextObj = pObj;
		pObj->m_spBuildEngine = pBuildEngine;
	}
	return hr;
}

STDMETHODIMP CBldCommandLineEC::AddError(BSTR bstrMessage, BSTR bstrHelpKeyword, BSTR bstrFile, long nLine, BSTR bstrFullMsg )
{
	m_nErrors++;
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	// Format the Message
	CStringW strFullMessage = bstrMessage;
	if( strFullMessage.IsEmpty() )
	{
		if( nLine )
			strFullMessage.Format(L"%s(%d) : error %s : %s\n",bstrFile, nLine, bstrHelpKeyword, bstrMessage);
		else
			strFullMessage.Format(L"%s : error %s : %s\n",bstrFile, bstrHelpKeyword, bstrMessage);
	}
	else
	{
		strFullMessage += L"\n";
	}

	CVCProjectEngine::DoWriteLog(strFullMessage, FALSE);
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::AddWarning(BSTR bstrMessage, BSTR bstrHelpKeyword, BSTR bstrFile, long nLine, BSTR bstrFullMsg )
{
	m_nWarnings++;
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	// Format the Message
	CStringW strFullMessage = bstrFullMsg;
	if( strFullMessage.IsEmpty() )
	{
		if( nLine )
			strFullMessage.Format(L"%s(%d) : warning %s : %s\n",bstrFile, nLine, bstrHelpKeyword, bstrMessage);
		else
			strFullMessage.Format(L"%s : warning %s : %s\n",bstrFile, bstrHelpKeyword, bstrMessage);
	}
	else
	{
		strFullMessage += L"\n";
	}
	
	CVCProjectEngine::DoWriteLog(strFullMessage, FALSE);
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::AddInfo(BSTR bstrMessage)
{
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	CStringW strChar;
	if( bstrMessage && bstrMessage[0] )
	{
//		strChar.Format(L"%d>%s", m_nContext,bstrMessage);
		strChar.Format(L"%s", bstrMessage);
	}
	else
	{
//		strChar.Format(L"%d>\n", m_nContext);
		strChar.Format(L"\n");
	}
	CVCProjectEngine::DoWriteLog(strChar, VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::AddLine(BSTR bstrMessage)
{
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	CStringW strChar;
	if( bstrMessage && bstrMessage[0] )
	{
//		strChar.Format(L"%d>%s", m_nContext,bstrMessage);
		strChar.Format(L"%s", bstrMessage);
	}
	else
	{
//		strChar.Format(L"%d>\n", m_nContext);
		strChar.Format(L"\n");
	}
	CVCProjectEngine::DoWriteLog(strChar, VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::WriteLog(BSTR bstrMessage)
{
	if (m_bShowOutput == VARIANT_FALSE)
		return S_OK;

	CStringW strChar;
	if( bstrMessage && bstrMessage[0] )
	{
//		strChar.Format(L"%d>%s", m_nContext, bstrMessage);
		strChar.Format(L"%s",  bstrMessage);
	}
	else
	{
//		strChar.Format(L"%d>\n", m_nContext);
		strChar.Format(L"\n");
	}
	CVCProjectEngine::DoWriteLog(strChar, VARIANT_TRUE);
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::ClearWindow()
{
	m_nWarnings = 0;
	m_nErrors = 0;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_BaseDirectory(BSTR* pbstrBaseDir)
{
	return m_bstrDir.CopyTo(pbstrBaseDir);
}

STDMETHODIMP CBldCommandLineEC::put_BaseDirectory(BSTR bstrBaseDir)
{
	m_bstrDir = bstrBaseDir;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_ProjectName(BSTR* pbstrName)
{
	return m_bstrName.CopyTo(pbstrName);
}

STDMETHODIMP CBldCommandLineEC::put_ProjectName(BSTR bstrName)
{
	m_bstrName = bstrName;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_Warnings( long *pnWrn )
{
	CHECK_POINTER_NULL(pnWrn);
	*pnWrn = m_nWarnings;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_Errors( long *pnErr )
{
	CHECK_POINTER_NULL(pnErr);
	*pnErr = m_nErrors;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_AssociatedBuildEngine(IDispatch** ppBldEngine)
{
	CHECK_POINTER_NULL(ppBldEngine);
	CComQIPtr<IDispatch> spDispBldEngine = m_spBuildEngine;
	*ppBldEngine = spDispBldEngine.Detach();
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::put_AssociatedBuildEngine(IDispatch* pBldEngine)
{
	m_spBuildEngine = pBldEngine;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::get_ShowOutput(VARIANT_BOOL* pbShow)
{
	CHECK_POINTER_NULL(pbShow);
	*pbShow = m_bShowOutput;
	return S_OK;
}

STDMETHODIMP CBldCommandLineEC::put_ShowOutput(VARIANT_BOOL bShow)
{
	m_bShowOutput = bShow;
	return S_OK;
}
