#ifndef LOCALE_H
#define LOCALE_H
/*************************************************************************************************\
AccountData.h		: Interface for all locale related info
Creation Date		: 2/8/2002 6:31:00 PM
Library				: .lib
Copyright Notice	: (C) 2000 Microsoft
Author				: Victor Blanco
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

//*************************************************************************************************
#include "xonline.h"
#include "xoconst.h"

#define REGION_AMERICA				0
#define REGION_EUROPE				1
#define REGION_ASIA					2

#define LANGUAGE_ENGLISH            1
#define LANGUAGE_SPANISH            2
#define LANGUAGE_FRENCH             3
#define LANGUAGE_GERMAN             4
#define LANGUAGE_JAPANESE           5
#define LANGUAGE_ITALIAN            6

static const CHAR* szLocaleDir[] =
{
	"NoLanguage", 
    "English",
    "Japanese",
    "German",
    "French",
    "Spanish",
    "Italian"
};

/**************************************************************************************************
CLASS DESCRIPTION
UserLocaleInfo:  Structure that stores all of the necessary locale related
                 values based on the region/language settings
**************************************************************************************************/
struct UserLocaleInfo
{
    char               m_cDateSeperator;  // m.d.y vs. m/d/y
    int                m_bMonthFirstFormatting;  // m/d/y vs. d/m/y
    char               m_cTimeSeperator;  // hh:mm vs. hh.mm
    int                m_bTwentyFourHourTime;
    char               m_cIntegerSeperator;  // , vs ' ' vs .
};



/**************************************************************************************************
CLASS DESCRIPTION
LocaleInfo:  Class that deals with all of the necessary locale related
             functions based on the region/language settings
**************************************************************************************************/
class LocaleInfo
{
public:
    LocaleInfo();
    ~LocaleInfo();
    HRESULT LoadLocale( int nLanguage, int nCountry );
    static HRESULT UpdateLocaleInfo();
    

private:
    UserLocaleInfo  m_LocaleData;
};

//*************************************************************************************************
#endif  // end of file ( Locale.h )