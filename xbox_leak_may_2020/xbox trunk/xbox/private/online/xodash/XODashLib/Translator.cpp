//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "utilities.h"
#include "Translator.h"
#include "StringObj.h"

static const CHAR* rgszTranslateSection[] =
{
	"NoLanguageXlate", 
    "EnglishXlate",
    "JapaneseXlate",
    "GermanXlate",
    "FrenchXlate",
    "SpanishXlate",
    "ItalianXlate"
};

CTranslator* CTranslator::pinstance = 0;// initialize pointer
CTranslator::CTranslator() 
{ 

}

CTranslator* CTranslator::Instance () 
{
   if (pinstance == 0)  // is it the first call?
   {  
      pinstance = new CTranslator; // create sole instance
	  pinstance->m_nCurLanguage = -1;
	  pinstance->m_nCurGameRegion = -1;
	  pinstance->m_hXlate = INVALID_HANDLE_VALUE;
   }
   return pinstance; // address of sole instance
}

HRESULT CTranslator::Initialize()
{
	m_nCurLanguage = XGetLanguage() ;
	m_nCurGameRegion = XGetGameRegion();
    
    // BUGBUG: Currently default to US....need to get other country codes
    if( FAILED( m_LocaleInfo.LoadLocale( m_nCurLanguage, 103) ) )
    {
        return E_FAIL;
    }
    
	return LoadXlate();
}

HRESULT CTranslator::Initialize( int nCountryCode )
{
	m_nCurLanguage = XGetLanguage() ;
	m_nCurGameRegion = XGetGameRegion();
    
    if( FAILED( m_LocaleInfo.LoadLocale( m_nCurLanguage, nCountryCode ) ) )
    {
        return E_FAIL;
    }
    
	return LoadXlate();
}

HRESULT CTranslator::LoadXlate()
{
	ASSERT(m_nCurLanguage>=0);
	ASSERT(m_nCurLanguage < countof(rgszTranslateSection));
	HRESULT hr = S_OK;
	TCHAR* rgchXlateData = NULL;
    
	do
	{
		m_hXlate = XGetSectionHandle(rgszTranslateSection[m_nCurLanguage]);
		if(m_hXlate == INVALID_HANDLE_VALUE)
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
			BREAKONFAIL2(hr, "CTranslator::LoadXlate- fail to get SectionHandle for %s", rgszTranslateSection[m_nCurLanguage]);
		}
		
		rgchXlateData =(TCHAR*) XLoadSectionByHandle(m_hXlate);
		ASSERT(*(WORD*)rgchXlateData == 0xFEFF);
		if (m_hXlate == INVALID_HANDLE_VALUE || !rgchXlateData || *(WORD*)rgchXlateData != 0xFEFF)
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA	);
			BREAKONFAIL2(hr, "Unable to load translation \"%hs\" from section, data maybe corrupted", rgszTranslateSection[m_nCurLanguage] );
		}

		{
			TCHAR* pch = rgchXlateData + 1; // Skip Unicode BOM
			TCHAR* pchLineStart = rgchXlateData + 1;
			TCHAR* pchLineEnd = rgchXlateData + 1;

			while (*pchLineEnd != 0)
			{
        
				while (*pchLineEnd != '\n' && *pchLineEnd!=0)
					pchLineEnd++ ;

				if(*pchLineStart != '[' && *pchLineStart != ';' && *pchLineStart != 0x0D && *pchLineStart != ' ')
				{
					ParseLine(pchLineStart, pchLineEnd);
				}
						
				pchLineEnd++;
				pchLineStart = pchLineEnd;
			}

		}

	} while(0);

	if(m_hXlate != INVALID_HANDLE_VALUE)
	{
		ASSERT(XFreeSectionByHandle(m_hXlate));
	}
  
	return hr;
}


bool CTranslator::ParseLine(TCHAR* pchStart, TCHAR* pchEnd)
{
	TCHAR* pchKeyEnd = NULL;	

	for (pchKeyEnd = pchStart; pchKeyEnd <= pchEnd; pchKeyEnd++)
	{
		if(*pchKeyEnd == '=')
			break;
	}
	ASSERT(pchKeyEnd < pchEnd);

	TCHAR* pchStringEnd = NULL;

	ASSERT(*(pchKeyEnd+1) == '"');
	
	for (pchStringEnd = pchKeyEnd+2; pchStringEnd <= pchEnd; pchStringEnd++)
	{
		if(*pchStringEnd == '"')
			break;
	}
	
	ASSERT(*pchStringEnd == '"');

	CStringObj StringId(pchStart, pchKeyEnd-pchStart);
	CStringObj StringText(pchKeyEnd+2, pchStringEnd - (pchKeyEnd+2));

	m_TranslateMap.insert(TranslateMap::value_type(StringId, StringText));

	return true;
}


const TCHAR* CTranslator::Translate(TCHAR* szString)
{
	CStringObj KeyStr(szString);
	TranslateMapIterator iter = m_TranslateMap.find(KeyStr);

//	ASSERT(iter!= m_TranslateMap.end());
	if(iter == m_TranslateMap.end())
		return NULL;
	return (*iter).second.c_str();
}

void CTranslator::Cleanup()
{
	m_TranslateMap.erase(m_TranslateMap.begin(), m_TranslateMap.end());
}

void CTranslator::SetLanguage(DWORD dwLang)
{
	if(XC_LANGUAGE_UNKNOWN == dwLang)
	{
		m_nCurLanguage +=1;
		if (m_nCurLanguage > 6)
		{
			m_nCurLanguage = 1;
		}
	}
	else
	{
		if(dwLang >0 && dwLang <=6)
		{
			m_nCurLanguage = dwLang;
		}
	}

	XSetValue(XC_LANGUAGE, REG_DWORD, (DWORD*)&m_nCurLanguage, 4);
	Cleanup();
	Initialize();
}
