#pragma once

static const OCHAR g_cszSaveMetaFileName[] = OTEXT("\\SaveMeta.xbx");
static const int   g_cchSaveMetaFileName   = ARRAYSIZE(g_cszSaveMetaFileName) - 1;

static const OCHAR g_cszTitleMetaFileName[] = OTEXT("\\TitleMeta.xbx");
static const int   g_cchTitleMetaFileName   = ARRAYSIZE(g_cszTitleMetaFileName) - 1;

static const OCHAR g_cszTitleImageFileName[] = OTEXT("\\TitleImage.xbx");
static const int   g_cchTitleImageFileName   = ARRAYSIZE(g_cszTitleImageFileName) - 1;

static const OCHAR g_cszSaveImageFileName[] = OTEXT("\\SaveImage.xbx");
static const int   g_cchSaveImageFileName   = ARRAYSIZE(g_cszSaveImageFileName) - 1;

static const OCHAR g_cszContentMetaFileName[] = OTEXT("\\ContentMeta.xbx");
static const int   g_cchContentMetaFileName   = ARRAYSIZE(g_cszContentMetaFileName) - 1;

static const WCHAR g_cszNameTag[] = L"Name";
#define g_cchNameTag (ARRAYSIZE(g_cszNameTag) - 1)

static const WCHAR g_cszTitleNameTag[] = L"TitleName";
#define g_cchTitleNameTag (ARRAYSIZE(g_cszTitleNameTag) - 1)

static const OCHAR g_cszStar[] = OTEXT("*");
static const int   g_cchStar   = ARRAYSIZE(g_cszStar) - 1;

static const OCHAR g_cszContentSearch[] = OTEXT("$C\\*");
static const int   g_cchContentSearch   = ARRAYSIZE(g_cszContentSearch) - 1;

static const OCHAR g_cszContentDir[] = OTEXT("$C\\");
static const int   g_cchContentDir   = ARRAYSIZE(g_cszContentDir) - 1;

static const WCHAR g_cszCRLF[] = L"\r\n";
#define g_cchCRLF (ARRAYSIZE(g_cszCRLF) - 1)

static const WCHAR g_cszNoCopyTrue[] = L"NoCopy=1\r\n";
#define g_cchNoCopyTrue (ARRAYSIZE(g_cszNoCopyTrue) - 1)

static const WCHAR g_chEqual = L'=';
static const WCHAR g_chUnicodeSignature = 0xfeff;

#define MAX_TAGNAME       g_cchTitleNameTag
#define MAX_VALUENAME     MAX_GAMENAME
#define MAX_METADATA_LINE (MAX_TAGNAME + 1 + MAX_VALUENAME + g_cchCRLF + 1)

#define CONTENT_DIR_NAME_LENGTH     (8 + 1 + 8) // 00000000.00000000 (does not include null terminator)
#define CONTENT_DIR_DELIMETER       '.'
#define CONTENT_DIR_DELIMETER_INDEX 8

