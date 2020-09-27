//-----------------------------------------------------------------------------
// File: Text.cpp
//
// Desc: All text in single place to simplify localization
//
// Hist: 04.16.01 - Added for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TECH_CERT_GAME_TEXT_H
#define TECH_CERT_GAME_TEXT_H

#include "Common.h"




const UINT XC_LANGUAGE_MAX = XC_LANGUAGE_ITALIAN + 1;




extern const WCHAR* const strGAME_NAME_FORMAT   [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strSUNDAY             [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMONDAY             [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strTUESDAY            [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strWEDNESDAY          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strTHURSDAY           [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strFRIDAY             [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strSATURDAY           [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strRECONNECT_CNTRLR   [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strCONFIRM_QUIT       [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strDEMO               [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strRETURN_TO_MENU     [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strGAME_NAME          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strLOADINGX           [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strFREE               [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_START         [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_LOAD          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_OPTIONS       [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_RESUME        [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_SAVE          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_QUIT          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_VIBRATION     [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_SAVE_OPTIONS  [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strON                 [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strOFF                [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_MUSIC_VOLUME  [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMENU_EFFECT_VOLUME [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMS_XBOX            [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strINTRO              [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strPRESS_START        [ XC_LANGUAGE_MAX ];

extern const WCHAR* const strSAVE_FAILED        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strGAME_SAVED         [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strLOAD_FAILED        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strGAME_LOADED        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO_ROOM_MU         [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO_ROOM_HD         [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO_ROOM_MU_PLZ_FREE[ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO_ROOM_HD_PLZ_FREE[ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO_SAVES           [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strLOADING            [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strSAVING_MU          [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strSAVING             [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strDO_NOT_REMOVE_MU   [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strDO_NOT_POWEROFF    [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strSAVE_GAME          [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strLOAD_GAME          [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strILLUS_GRAPHICS     [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strFORMAT_DEVICE      [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strCHOOSE_LOAD        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strCHOOSE_SAVE        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strTODAY              [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNOW                [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strFORMAT_GAME        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strEMPTY_SPACE        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strA_SELECT           [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strB_BACK             [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strY_DELETE           [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strOVERWRITE          [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strYES                [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strNO                 [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strDELETE             [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strLOADING_GAME_LIST  [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strMAX_SAVED_GAMES    [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strMU_UNUSABLE        [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strMU_FULL            [ XC_LANGUAGE_MAX ]; 
extern const WCHAR* const strUNUSABLE_MU_NAME   [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strFULL_MU_NAME       [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strMU_REMOVED         [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strACTION_SAVE        [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strACTION_LOAD        [ XC_LANGUAGE_MAX ];
extern const  CHAR* const strMAX_BLOCKS         [ XC_LANGUAGE_MAX ];
extern const WCHAR* const strXHD                [ XC_LANGUAGE_MAX ];




#endif // TECH_CERT_GAME_TEXT_H
