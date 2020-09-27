//#----------------------------------------------------------------------------
//
// Copyright (C) 1996 - 1999  Microsoft Corporation.  All Rights Reserved.
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
//  File:       msapcdlg.h
//
//  Synopsis:   Resource ID's for custom dialog 
//
//-----------------------------------------------------------------------------
#ifndef _MSAPC_DLG_H_
#define _MSAPC_DLG_H_

//
//  This resource is a string containing the friendly name of the Realm
//  This string can not be longer than 40 characters.
//
#define IDS_FRIENDLY_NAME   1001    

//
//  Optional.  This resource is the label string for the user name input field
//  This string must contain a '&' 
//  This string can not be longer than 12 chars, not including the '&' character
//
#define IDS_USERNAME_LABEL  1002

//
//  Optional.  This resource is the label string for the password input field
//  This string must contain a '&' 
//  This string can not be longer than 12 chars, not including the '&' character
//
#define IDS_PASSWORD_LABEL  1003

//
//  Optional.  This resource is the message string displayed at the lower 
//  message box area of the dialog.  This message string is displayed as soon 
//  as the dialog is pop-up; it instructs the user to type in their user name
//  and password to connect to the internet site.
//  This string can not be longer than 128 characters.
//
#define IDS_ENTERNORP       1004

//
//  Optional.  This resource is the message string displayed in a pop-up
//  window if users forget to enter their name before hitting the OK button.
//
#define IDS_NOUSERNAME      1005

//
//  Optional.  This resource is the message string displayed in a pop-up
//  window if users forget to enter their password before hitting the OK button.
//
#define IDS_NOPASSWORD      1006

//
//  Optional.  This resource is the label string for the check box where 
//  users indicate to save their password persistently
//  This string must contain a '&' 
//  This string can not be longer than 50 chars, not including the '&' character
//
#define IDS_SAVEPWD_LABEL   1007

//
//  Optional.  This resource is the message string displayed when users 
//  have entered a password shorter than the minimum required length.
//  This string can not be longer than 128 characters.
//
#define IDS_PWDTOOSHORT     1008

#endif // _MSAPC_DLG_H_
