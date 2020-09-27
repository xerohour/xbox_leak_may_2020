////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// langcode.h -- maps a two character country code to an English string in accordance with ISO-639

#ifndef __LANGCODE_H__
#define __LANGCODE_H__


struct LANGUAGESTRUCT
{
	WORD    code;
	char*   string;
};

const LANGUAGESTRUCT LanguageStrings[] =
{
	{	0, "Unknown" },
    { (WORD)'a' << 8 | (BYTE)'a', "Afar" },
    { (WORD)'a' << 8 | (BYTE)'b', "Abkhazian" },
    { (WORD)'a' << 8 | (BYTE)'f', "Afrikaans" },
    { (WORD)'a' << 8 | (BYTE)'m', "Amharic" },
    { (WORD)'a' << 8 | (BYTE)'r', "Arabic" },
    { (WORD)'a' << 8 | (BYTE)'s', "Assamese" },
    { (WORD)'a' << 8 | (BYTE)'y', "Aymara" },
    { (WORD)'a' << 8 | (BYTE)'z', "Azerbaijani" },
    { (WORD)'b' << 8 | (BYTE)'a', "Bashkir" },
    { (WORD)'b' << 8 | (BYTE)'e', "Byelorussian" },
    { (WORD)'b' << 8 | (BYTE)'g', "Bulgarian" },
    { (WORD)'b' << 8 | (BYTE)'h', "Bihari" },
    { (WORD)'b' << 8 | (BYTE)'i', "Bislama" },
    { (WORD)'b' << 8 | (BYTE)'n', "Bengali" },
    { (WORD)'b' << 8 | (BYTE)'o', "Tibetan" },
    { (WORD)'b' << 8 | (BYTE)'r', "Breton" },
    { (WORD)'c' << 8 | (BYTE)'a', "Catalan" },
    { (WORD)'c' << 8 | (BYTE)'o', "Corsican" },
    { (WORD)'c' << 8 | (BYTE)'s', "Czech" },
    { (WORD)'c' << 8 | (BYTE)'y', "Welsh" },
    { (WORD)'d' << 8 | (BYTE)'a', "Danish" },
    { (WORD)'d' << 8 | (BYTE)'e', "German" },
    { (WORD)'d' << 8 | (BYTE)'z', "Bhutani" },
    { (WORD)'e' << 8 | (BYTE)'l', "Greek" },
    { (WORD)'e' << 8 | (BYTE)'n', "English"  },
    { (WORD)'e' << 8 | (BYTE)'o', "Esperanto" },
    { (WORD)'e' << 8 | (BYTE)'s', "Spanish" },
    { (WORD)'e' << 8 | (BYTE)'t', "Estonian" },
    { (WORD)'e' << 8 | (BYTE)'u', "Basque" },
    { (WORD)'f' << 8 | (BYTE)'a', "Persian" },
    { (WORD)'f' << 8 | (BYTE)'i', "Finnish" },
    { (WORD)'f' << 8 | (BYTE)'j', "Fiji" },
    { (WORD)'f' << 8 | (BYTE)'o', "Faeroese" },
    { (WORD)'f' << 8 | (BYTE)'r', "French" },
    { (WORD)'f' << 8 | (BYTE)'y', "Frisian" },
    { (WORD)'g' << 8 | (BYTE)'a', "Irish" },
    { (WORD)'g' << 8 | (BYTE)'d', "Gaelic"  },
    { (WORD)'g' << 8 | (BYTE)'l', "Galician" },
    { (WORD)'g' << 8 | (BYTE)'n', "Guarani" },
    { (WORD)'g' << 8 | (BYTE)'u', "Gujarati" },
    { (WORD)'h' << 8 | (BYTE)'a', "Hausa" },
    { (WORD)'h' << 8 | (BYTE)'i', "Hindi" },
    { (WORD)'h' << 8 | (BYTE)'r', "Croatian" },
    { (WORD)'h' << 8 | (BYTE)'u', "Hungarian" },
    { (WORD)'h' << 8 | (BYTE)'y', "Armenian" },
    { (WORD)'i' << 8 | (BYTE)'a', "Interlingua" },
    { (WORD)'i' << 8 | (BYTE)'e', "Interlingue" },
    { (WORD)'i' << 8 | (BYTE)'k', "Inupiak" },
    { (WORD)'i' << 8 | (BYTE)'n', "Indonesian" },
    { (WORD)'i' << 8 | (BYTE)'s', "Icelandic"  },
    { (WORD)'i' << 8 | (BYTE)'t', "Italian" },
    { (WORD)'i' << 8 | (BYTE)'w', "Hebrew" },
    { (WORD)'j' << 8 | (BYTE)'a', "Japanese" },
    { (WORD)'j' << 8 | (BYTE)'i', "Yiddish" },
    { (WORD)'j' << 8 | (BYTE)'w', "Javanese" },
    { (WORD)'k' << 8 | (BYTE)'a', "Georgian" },
    { (WORD)'k' << 8 | (BYTE)'k', "Kazakh" },
    { (WORD)'k' << 8 | (BYTE)'l', "Greenlandic" },
    { (WORD)'k' << 8 | (BYTE)'m', "Cambodian" },
    { (WORD)'k' << 8 | (BYTE)'n', "Kannada" },
    { (WORD)'k' << 8 | (BYTE)'o', "Korean" },
    { (WORD)'k' << 8 | (BYTE)'s', "Kashmiri" },
    { (WORD)'k' << 8 | (BYTE)'u', "Kurdish" },
    { (WORD)'k' << 8 | (BYTE)'y', "Kirghiz" },
    { (WORD)'l' << 8 | (BYTE)'a', "Latin" },
    { (WORD)'l' << 8 | (BYTE)'n', "Lingala" },
    { (WORD)'l' << 8 | (BYTE)'o', "Laothian" },
    { (WORD)'l' << 8 | (BYTE)'t', "Lithuanian" },
    { (WORD)'l' << 8 | (BYTE)'v', "Latvian"  },
    { (WORD)'m' << 8 | (BYTE)'g', "Malagasy" },
    { (WORD)'m' << 8 | (BYTE)'i', "Maori" },
    { (WORD)'m' << 8 | (BYTE)'k', "Macedonian" },
    { (WORD)'m' << 8 | (BYTE)'l', "Malayalam" },
    { (WORD)'m' << 8 | (BYTE)'n', "Mongolian" },
    { (WORD)'m' << 8 | (BYTE)'o', "Moldavian" },
    { (WORD)'m' << 8 | (BYTE)'r', "Marathi" },
    { (WORD)'m' << 8 | (BYTE)'s', "Malay" },
    { (WORD)'m' << 8 | (BYTE)'t', "Maltese" },
    { (WORD)'m' << 8 | (BYTE)'y', "Burmese" },
    { (WORD)'n' << 8 | (BYTE)'a', "Nauru" },
    { (WORD)'n' << 8 | (BYTE)'e', "Nepali" },
    { (WORD)'n' << 8 | (BYTE)'l', "Dutch" },
    { (WORD)'n' << 8 | (BYTE)'o', "Norwegian" },
    { (WORD)'o' << 8 | (BYTE)'c', "Occitan" },
    { (WORD)'o' << 8 | (BYTE)'m', "Oromo"  },
    { (WORD)'o' << 8 | (BYTE)'r', "Oriya" },
    { (WORD)'p' << 8 | (BYTE)'a', "Punjabi" },
    { (WORD)'p' << 8 | (BYTE)'l', "Polish" },
    { (WORD)'p' << 8 | (BYTE)'s', "Pashto" },
    { (WORD)'p' << 8 | (BYTE)'t', "Portuguese" },
    { (WORD)'q' << 8 | (BYTE)'u', "Quechua" },
    { (WORD)'r' << 8 | (BYTE)'m', "Rhaeto-Romance" },
    { (WORD)'r' << 8 | (BYTE)'n', "Kirundi" },
    { (WORD)'r' << 8 | (BYTE)'o', "Romanian" },
    { (WORD)'r' << 8 | (BYTE)'u', "Russian" },
    { (WORD)'r' << 8 | (BYTE)'w', "Kinyarwanda" },
    { (WORD)'s' << 8 | (BYTE)'a', "Sanskrit" },
    { (WORD)'s' << 8 | (BYTE)'d', "Sindhi" },
    { (WORD)'s' << 8 | (BYTE)'g', "Sangro" },
    { (WORD)'s' << 8 | (BYTE)'h', "Serbo-Croatian" },
    { (WORD)'s' << 8 | (BYTE)'i', "Singhalese" },
    { (WORD)'s' << 8 | (BYTE)'k', "Slovak" },
    { (WORD)'s' << 8 | (BYTE)'l', "Slovenian" },
    { (WORD)'s' << 8 | (BYTE)'m', "Samoan" },
    { (WORD)'s' << 8 | (BYTE)'n', "Shona" },
    { (WORD)'s' << 8 | (BYTE)'o', "Somali" },
    { (WORD)'s' << 8 | (BYTE)'q', "Albanian" },
    { (WORD)'s' << 8 | (BYTE)'r', "Serbian" },
    { (WORD)'s' << 8 | (BYTE)'s', "Siswati" },
    { (WORD)'s' << 8 | (BYTE)'t', "Sesotho" },
    { (WORD)'s' << 8 | (BYTE)'u', "Sudanese" },
    { (WORD)'s' << 8 | (BYTE)'v', "Swedish" },
    { (WORD)'s' << 8 | (BYTE)'w', "Swahili" },
    { (WORD)'t' << 8 | (BYTE)'a', "Tamil" },
    { (WORD)'t' << 8 | (BYTE)'e', "Tegulu" },
    { (WORD)'t' << 8 | (BYTE)'g', "Tajik" },
    { (WORD)'t' << 8 | (BYTE)'h', "Thai" },
    { (WORD)'t' << 8 | (BYTE)'i', "Tigrinya" },
    { (WORD)'t' << 8 | (BYTE)'k', "Turkmen" },
    { (WORD)'t' << 8 | (BYTE)'l', "Tagalog" },
    { (WORD)'t' << 8 | (BYTE)'n', "Setswana" },
    { (WORD)'t' << 8 | (BYTE)'o', "Tonga" },
    { (WORD)'t' << 8 | (BYTE)'r', "Turkish" },
    { (WORD)'t' << 8 | (BYTE)'s', "Tsonga" },
    { (WORD)'t' << 8 | (BYTE)'t', "Tatar" },
    { (WORD)'t' << 8 | (BYTE)'w', "Twi" },
    { (WORD)'u' << 8 | (BYTE)'k', "Ukrainian" },
    { (WORD)'u' << 8 | (BYTE)'r', "Urdu" },
    { (WORD)'u' << 8 | (BYTE)'z', "Uzbek" },
    { (WORD)'v' << 8 | (BYTE)'i', "Vietnamese" },
    { (WORD)'v' << 8 | (BYTE)'o', "Volapuk" },
    { (WORD)'w' << 8 | (BYTE)'o', "Wolof" },
    { (WORD)'x' << 8 | (BYTE)'h', "Xhosa" },
    { (WORD)'y' << 8 | (BYTE)'o', "Yoruba" },
    { (WORD)'z' << 8 | (BYTE)'h', "Chinese" },
    { (WORD)'z' << 8 | (BYTE)'u', "Zulu" }
};



void GetLanguageStringFromCode(WORD dwTwoCharCode, char* pLanguage, UINT nMaxChar)
{
	int isize = sizeof( LanguageStrings ) / sizeof( LanguageStrings[0] );

	for( int i=0; i < isize; i++ )
	{
		if( dwTwoCharCode == LanguageStrings[i].code )
			break;
	}

	if( i == isize ) // if we didn't find a match, use lang_neutral
		i = 0;

	strncpy(pLanguage, LanguageStrings[i].string, nMaxChar);
}


#endif // #ifdef __LANGCODE_H__
