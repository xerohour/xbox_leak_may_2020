/*----------------------------------------------------------------------------
 *  Copyright © 2000 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 03/19/01
 *  Developer: Sean Wohlgemuth
 *  Description: Windows Scripting Host Script
 *----------------------------------------------------------------------------
 */

//////////////////////////////////////////////////////////////////////////////
// This function will allow multiple JScript file to be included
//////////////////////////////////////////////////////////////////////////////
function include(sFile)
{
  var objFSO = WScript.CreateObject("Scripting.FileSystemObject");
  var objInc = objFSO.OpenTextFile(sFile, 1);
  return objInc.ReadAll();
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////////
eval(include("testcase.js"));
eval(include("settings.js"));
eval(include("common.js"));

//////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////

//DESC: Connect to server
function setup(){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage
function xqserver_msghello_normal(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Description string is alpha
function xqserver_msghello_alpha(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"ABCDEFGHIJKLMNOPQRASTUVWXYZabcdefghijklmnopqrstuvwxyz");
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send wrong version; too low
function xqserver_msghello_wrongversion_v1(){
	setup();
	engine.MsgHello(0,++l_dwSequence,0,g_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send wrong version; too high
function xqserver_msghello_wrongversion_v2(){
	setup();
	engine.MsgHello(0,++l_dwSequence,-1,g_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send wrong major version part
function xqserver_msghello_wrongmajorversion(){
	setup();
	var l_wMajorVersion=HIWORD(g_dwVersion);
	if (0==l_wMajorVersion)
		l_wMajorVersion++;
	else 
		l_wMajorVersion--;
	engine.MsgHello(0,++l_dwSequence,DWORD(l_wMajorVersion,LOWORD(g_dwVersion)),g_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send wrong minor version part
function xqserver_msghello_wrongminorversion(){
	setup();
	var l_wMinorVersion=LOWORD(g_dwVersion);
	if (0==l_wMinorVersion)
		l_wMinorVersion++;
	else 
		l_wMinorVersion--;
	engine.MsgHello(0,++l_dwSequence,DWORD(HIWORD(g_dwVersion),l_wMinorVersion),g_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send Null as description
function xqserver_msghello_lowerboundsdescription(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"\0");
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send upper bounds description (79chars+null)
function xqserver_msghello_upperboundsdescription_v1(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"0123456789012345678901234567890123456789012345678901234567890123456789012345678");
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send upper bounds description (80char/no null)
function xqserver_msghello_upperboundsdescription_v2(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"01234567890123456789012345678901234567890123456789012345678901234567890123456789",80);
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send exceed upper bounds description (80char+null)
function xqserver_msghello_descriptiontoolong_v1(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"01234567890123456789012345678901234567890123456789012345678901234567890123456789\0");
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send exceed upper bounds by a lot
function xqserver_msghello_descriptiontoolong_v2(){
	setup();
	var l_bstrDescription="";
	for (i=0;i<512;i++){
		l_bstrDescription+="X";
	}//endfor
	l_bstrDescription+="\0";
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,l_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send description not null terminated
function xqserver_msghello_notnullterminated(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"ABCDEFGHIJ",10);
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send description w/ too many NULLs
function xqserver_msghello_nullbuffered(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"ABCDEFGHIJ\0\0\0\0\0\0\0\0\0\0",20);
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send special characters
function xqserver_msghello_crazychars(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,"~!@#$%^&*()_+-={}|[]\\:\";'<>?,./");
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Send 2 hellos; expect drop
function xqserver_msghello_multiple(){
	setup();
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	ExpectedState(engine,CONNECTED);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send a user info command before hello
function xqserver_msghello_outofsequence(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_qwXIP.dwLo++,l_qwXRG.dwHi,l_qwXRG.dwLo++,l_qwXIP.dwHi,g_wPort);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var engine = WScript.CreateObject("qtest.engine");
engine.Init();

var WshNetwork = new ActiveXObject("WScript.Network");

var l_dwSessionID=0;
var l_dwSequence=0;

var l_qwXIP= new QWORD(engine.InternetAddress(WshNetwork.ComputerName),l_dwSessionID);
var l_qwXRG= new QWORD(g_dwXRGNum,g_dwLifetimeID);

TestCase(xqserver_msghello_normal);
TestCase(xqserver_msghello_alpha);
TestCase(xqserver_msghello_wrongversion_v1);
TestCase(xqserver_msghello_wrongversion_v2);
TestCase(xqserver_msghello_wrongmajorversion);
TestCase(xqserver_msghello_wrongminorversion);
TestCase(xqserver_msghello_lowerboundsdescription);
TestCase(xqserver_msghello_upperboundsdescription_v1);
TestCase(xqserver_msghello_upperboundsdescription_v2);
TestCase(xqserver_msghello_descriptiontoolong_v1);
TestCase(xqserver_msghello_descriptiontoolong_v2);
TestCase(xqserver_msghello_notnullterminated);
TestCase(xqserver_msghello_nullbuffered);
TestCase(xqserver_msghello_crazychars);
TestCase(xqserver_msghello_multiple);
TestCase(xqserver_msghello_outofsequence);

engine.Cleanup();