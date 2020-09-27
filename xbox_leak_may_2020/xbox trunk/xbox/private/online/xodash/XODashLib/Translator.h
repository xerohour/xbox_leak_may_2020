#pragma once
#include <map>
#include "StringObj.h"
#include "locale.h"

using namespace std;


typedef map <CStringObj, CStringObj> TranslateMap;
typedef TranslateMap::iterator TranslateMapIterator;

class CTranslator
  {
  public:
      static CTranslator* Instance();
      LocaleInfo m_LocaleInfo;
	  const TCHAR* Translate(TCHAR*);
	  HRESULT Initialize();
      HRESULT Initialize( int nCountryCode );
	  void Cleanup();

	  void SetLanguage(DWORD dwLang = XC_LANGUAGE_UNKNOWN);
	  inline DWORD GetCurLanguage() { return m_nCurLanguage;}
  protected:
      CTranslator(const CTranslator&);
      CTranslator& operator= (const CTranslator&);
	  CTranslator();
  private:
      static CTranslator* pinstance;

	  HRESULT LoadXlate();
	  bool ParseLine(TCHAR* pchStart, TCHAR* pchEnd);
	  HANDLE m_hXlate;
	  TranslateMap m_TranslateMap;

	  long m_nCurLanguage;
	  long m_nCurGameRegion;
  };