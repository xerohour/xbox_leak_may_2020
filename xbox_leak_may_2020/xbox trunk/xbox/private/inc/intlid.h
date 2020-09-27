/*++

Copyright (c) 1994-1998,  Microsoft Corporation  All rights reserved.

Module Name:

    intlid.h

Abstract:

    This module contains the resource ids for the Regional Options applet.

Revision History:

--*/


//
//  Text String Constants.
//

//
//  Make sure the next two definitions are not redefined.
//     (This file included by main.cpl)
//
#ifndef IDS_NAME
  #define IDS_NAME                     1
#endif
#ifndef IDS_INFO
  #define IDS_INFO                     2
#endif

#define IDS_LOCALE_GET_ERROR           3
#define IDS_INVALID_USE_OF_NUM         4
#define IDS_INVALID_TIME_STYLE         5
#define IDS_INVALID_DATE_STYLE         6
#define IDS_NO_LZERO                   7
#define IDS_LZERO                      8
#define IDS_METRIC                     9
#define IDS_US                         10
#define IDS_LOCALE_SET_ERROR           11
#define IDS_LOCALE_NO_NUMS_IN          12
#define IDS_LOCALE_DECIMAL_SYM         13
#define IDS_LOCALE_NEG_SIGN            15
#define IDS_LOCALE_GROUP_SYM           16
#define IDS_LOCALE_TIME_SEP            17
#define IDS_LOCALE_AM_SYM              18
#define IDS_LOCALE_PM_SYM              19
#define IDS_LOCALE_DATE_SEP            20
#define IDS_LOCALE_CURR_SYM            21
#define IDS_LOCALE_CDECIMAL_SYM        22
#define IDS_LOCALE_CGROUP_SYM          23
#define IDS_LOCALE_STYLE_ERR           24
#define IDS_LOCALE_TIME                25
#define IDS_LOCALE_SDATE               26
#define IDS_LOCALE_LDATE               27
#define IDS_LOCALE_YEAR_ERROR          28

#define IDS_STYLEUH                    55
#define IDS_STYLELH                    56
#define IDS_STYLEUM                    57
#define IDS_STYLELM                    58
#define IDS_STYLELS                    59
#define IDS_STYLELT                    60
#define IDS_STYLELD                    61
#define IDS_STYLELY                    62
#define IDS_TIMECHARS                  63
#define IDS_TCASESWAP                  64
#define IDS_SDATECHARS                 65
#define IDS_SDCASESWAP                 66
#define IDS_LDATECHARS                 67
#define IDS_LDCASESWAP                 68
#define IDS_REBOOT_STRING              69
#define IDS_TITLE_STRING               70
#define IDS_SETUP_STRING               71

#define IDS_ML_PERMANENT               72
#define IDS_ML_CANNOT_MODIFY           73
#define IDS_ML_COPY_FAILED             74
#define IDS_ML_INSTALL_FAILED          75
#define IDS_ML_SETUP_FAILED            76

#define IDS_KBD_NO_DEF_LANG            80
#define IDS_KBD_NO_DEF_LANG2           81
#define IDS_KBD_SETUP_FAILED           82
#define IDS_KBD_LOAD_KBD_FAILED        83
#define IDS_KBD_UNLOAD_KBD_FAILED      84
#define IDS_KBD_NEED_LAYOUT            85
#define IDS_KBD_LOAD_LINE_BAD          86
#define IDS_KBD_NO_MORE_TO_ADD         87
#define IDS_KBD_LAYOUT_FAILED          88
#define IDS_KBD_SWITCH_LOCALE          89
#define IDS_KBD_SWITCH_TO              90
#define IDS_KBD_MOD_CONTROL            91
#define IDS_KBD_MOD_LEFT_ALT           92
#define IDS_KBD_MOD_SHIFT              93
#define IDS_KBD_CONFLICT_HOTKEY        94
#define IDS_KBD_INVALID_HOTKEY         95

#define IDS_SPANISH_NAME               96
#define IDS_DEFAULT                    97

#define IDS_CHANGE_UI_LANG             98
#define IDS_DEFAULT_USER_ERROR         99
#define IDS_CHANGE_UI_LANG_NOT_ADMIN   100

#ifndef IDS_UNKNOWN
  #define IDS_UNKNOWN                  198
#endif



//
//  Dialogs.
//

#define DLG_GENERAL                              102
#define DLG_NUMBER                               103
#define DLG_CURRENCY                             104
#define DLG_TIME                                 105
#define DLG_DATE                                 106
#define DLG_KEYBOARD_LOCALES                     107

#define DLG_REGION_SET_DEFAULT                   108
#define DLG_REGION_ADVANCED                      109

#define DLG_KEYBOARD_LOCALE_ADD                  110
#define DLG_KEYBOARD_LOCALE_EDIT                 111
#define DLG_KEYBOARD_HOTKEY_INPUT_LOCALE         112
#define DLG_KEYBOARD_HOTKEY_INPUT_LOCALE_THAI    113
#define DLG_KEYBOARD_HOTKEY_KEYBOARD_LAYOUT      114
#define DLG_KEYBOARD_HOTKEY_IME                  115



//
//  Icons.
//

#define IDI_ICON                       200
#define IDI_DEFAULT_CHECK              201   // small black circled check



//
//  Bitmaps.
//



//
//  Digit Substitution Strings.
//

#define IDS_DIGIT_SUBST_CONTEXT        900
#define IDS_DIGIT_SUBST_NONE           (IDS_DIGIT_SUBST_CONTEXT + 1)
#define IDS_DIGIT_SUBST_NATIONAL       (IDS_DIGIT_SUBST_CONTEXT + 2)



//
//  Misc. Controls.
//

#define IDC_STATIC                     -1
#define IDC_GROUPBOX1                  1001
#define IDC_GROUPBOX2                  1002
#define IDC_GROUPBOX3                  1003
#define IDC_SAMPLE1                    1004
#define IDC_SAMPLE2                    1005
#define IDC_SAMPLELBL1                 1006
#define IDC_SAMPLELBL2                 1007
#define IDC_SAMPLELBL3                 1008
#define IDC_SAMPLE1A                   1009
#define IDC_SAMPLE2A                   1010
#define IDC_SAMPLELBL1A                1011
#define IDC_SAMPLELBL2A                1012



//
//  General Page Controls.
//

#define IDC_USER_LOCALE                1030
#define IDC_SORTING_TEXT               1031
#define IDC_SORTING                    1032
#define IDC_UI_LANGUAGE_TEXT           1033
#define IDC_UI_LANGUAGE                1034
#define IDC_LANGUAGE_GROUPS            1035
#define IDC_SET_DEFAULT                1036
#define IDC_ADVANCED                   1037



//
//  Set Default Dialog Controls.
//

#define IDC_SYSTEM_LOCALE_TEXT1        1050
#define IDC_SYSTEM_LOCALE_TEXT2        1051
#define IDC_SYSTEM_LOCALE              1052



//
//  Advanced Dialog Controls.
//

#define IDC_CODEPAGES                  1060



//
//  Number and Currency Page Controls.
//

#define IDC_DECIMAL_SYMBOL             1070
#define IDC_CURRENCY_SYMBOL            1071
#define IDC_NUM_DECIMAL_DIGITS         1072
#define IDC_DIGIT_GROUP_SYMBOL         1073
#define IDC_NUM_DIGITS_GROUP           1074
#define IDC_POS_SIGN                   1075
#define IDC_NEG_SIGN                   1076
#define IDC_POS_CURRENCY_SYM           1077
#define IDC_NEG_NUM_FORMAT             1078
#define IDC_SEPARATOR                  1079
#define IDC_DISPLAY_LEAD_0             1080
#define IDC_MEASURE_SYS                1081
#define IDC_NATIVE_DIGITS_TEXT         1082
#define IDC_NATIVE_DIGITS              1083
#define IDC_DIGIT_SUBST_TEXT           1084
#define IDC_DIGIT_SUBST                1085



//
//  Time Page Controls.
//

#define IDC_TIME_STYLE                 1090
#define IDC_AM_SYMBOL                  1091
#define IDC_PM_SYMBOL                  1092



//
//  Date Page Controls.
//

#define IDC_CALENDAR_TYPE_TEXT         1100
#define IDC_CALENDAR_TYPE              1101
#define IDC_TWO_DIGIT_YEAR_LOW         1102
#define IDC_TWO_DIGIT_YEAR_HIGH        1103
#define IDC_TWO_DIGIT_YEAR_ARROW       1104
#define IDC_ADD_HIJRI_DATE             1105
#define IDC_SHORT_DATE_STYLE           1106
#define IDC_LONG_DATE_STYLE            1107
#define IDC_ADD_HIJRI_DATE_TEXT        1108



//
//  Input Locale Property Page Controls.
//

#define IDC_KBDL_INPUT_FRAME           1200
#define IDC_KBDL_LOCALE                1201
#define IDC_KBDL_LAYOUT_TEXT           1202
#define IDC_KBDL_LOCALE_LIST           1203
#define IDC_KBDL_ADD                   1204
#define IDC_KBDL_EDIT                  1205
#define IDC_KBDL_DELETE                1206
#define IDC_KBDL_DISABLED              1207
#define IDC_KBDL_DISABLED_2            1208
#define IDC_KBDL_CAPSLOCK_FRAME        1209
#define IDC_KBDL_CAPSLOCK              1210
#define IDC_KBDL_SHIFTLOCK             1211
#define IDC_KBDL_SET_DEFAULT           1213
#define IDC_KBDL_SHORTCUT_FRAME        1214
#define IDC_KBDL_ALT_SHIFT             1215
#define IDC_KBDL_CTRL_SHIFT            1216
#define IDC_KBDL_NO_SHIFT              1217
#define IDC_KBDL_INDICATOR             1218
#define IDC_KBDLA_LOCALE               1219
#define IDC_KBDLA_LAYOUT               1220
#define IDC_KBDLE_LOCALE_TXT           1221
#define IDC_KBDLE_LOCALE               1222
#define IDC_KBDLE_LAYOUT               1223
#define IDC_KBDL_ONSCRNKBD             1226
#define IDC_KBDL_UP                    1227
#define IDC_KBDL_DOWN                  1228

#define IDC_KBDL_IME_SETTINGS          1230
#define IDC_KBDL_HOTKEY_LIST           1231
#define IDC_KBDL_HOTKEY_SEQUENCE       1232
#define IDC_KBDL_HOTKEY                1233
#define IDC_KBDL_HOTKEY_FRAME          1234
#define IDC_KBDL_CHANGE_HOTKEY         1235
#define IDC_KBDLH_KEY_COMBO            1236
#define IDC_KBDLH_CTRL                 1237
#define IDC_KBDLH_L_ALT                1238
#define IDC_KBDLH_SHIFT                1239
#define IDC_KBDLH_LAYOUT_TEXT          1240
#define IDC_KBDLH_ENABLE               1241
#define IDC_KBDLH_GRAVE                1242
#define IDC_KBDLH_VLINE                1243
#define IDC_KBDLH_PLUS                 1244



//
//  Hotkey Strings.
//

#define IDS_VK_NONE                    2200
#define IDS_VK_SPACE                   2201
#define IDS_VK_PRIOR                   2202
#define IDS_VK_NEXT                    2203
#define IDS_VK_END                     2204
#define IDS_VK_HOME                    2205
#define IDS_VK_F1                      2206
#define IDS_VK_F2                      2207
#define IDS_VK_F3                      2208
#define IDS_VK_F4                      2209
#define IDS_VK_F5                      2210
#define IDS_VK_F6                      2211
#define IDS_VK_F7                      2212
#define IDS_VK_F8                      2213
#define IDS_VK_F9                      2214
#define IDS_VK_F10                     2215
#define IDS_VK_F11                     2216
#define IDS_VK_F12                     2217
#define IDS_VK_OEM_SEMICLN             2218
#define IDS_VK_OEM_EQUAL               2219
#define IDS_VK_OEM_COMMA               2220
#define IDS_VK_OEM_MINUS               2221
#define IDS_VK_OEM_PERIOD              2222
#define IDS_VK_OEM_SLASH               2223
#define IDS_VK_OEM_3                   2224
#define IDS_VK_OEM_LBRACKET            2225
#define IDS_VK_OEM_BSLASH              2226
#define IDS_VK_OEM_RBRACKET            2227
#define IDS_VK_OEM_QUOTE               2228
#define IDS_VK_A                       2229
#define IDS_VK_NONE1                   (IDS_VK_A + 26)
#define IDS_VK_0                       (IDS_VK_A + 27)
//
// the below HOTKEYS are only for CHT IMEs
//
#define IDS_RESEND_RESULTSTR_CHT       2300
#define IDS_PREVIOUS_COMPOS_CHT        2302
#define IDS_UISTYLE_TOGGLE_CHT         2304
#define IDS_IME_NONIME_TOGGLE_CHT      2306
#define IDS_SHAPE_TOGGLE_CHT           2308
#define IDS_SYMBOL_TOGGLE_CHT          2310
//
// the below HOTKEYS are only for CHS IMEs
//
#define IDS_IME_NONIME_TOGGLE_CHS      2312
#define IDS_SHAPE_TOGGLE_CHS           2314
#define IDS_SYMBOL_TOGGLE_CHS          2316
//
//
//
#define IDS_KBD_SET_HOTKEY_ERR         2320



//
//  Ordinal for LocaleDlgProc function - Input Locale Page.
//

#define ORD_LOCALE_DLG_PROC            100
