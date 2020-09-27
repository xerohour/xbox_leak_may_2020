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

//Connect, hello, user info
function setup(qwXRG){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_qwXIP.dwLo);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,qwXRG.dwHi,qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage
function xqserver_msgdeadxrg_normal(){
	setup(l_qwXRG);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXRG(0,++l_dwSequence,l_qwXRG.dwHi,l_qwXRG.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Lower bounds of XRG parameter
function xqserver_msgdeadxrg_lowerboundXRG(){
	qwXRG=new QWORD(0x00000000,0x00000000);
	setup(qwXRG);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXRG(0,++l_dwSequence,qwXRG.dwHi,qwXRG.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Upper bounds of XRG parameter
function xqserver_msgdeadxrg_upperboundXRG(){
	qwXRG=new QWORD(0x00000000,0x00000000);
	setup(qwXRG);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXRG(0,++l_dwSequence,qwXRG.dwHi,qwXRG.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Dead XRG when no items are present
function xqserver_msgdeadxrg_nodataforXRG(){
	setup(l_qwXRG);
	engine.MsgDeadXRG(0,++l_dwSequence,l_qwXRG.dwHi,l_qwXRG.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Dead XRG when no queues are present
function xqserver_msgdeadxrg_noqueues(){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXRG(0,++l_dwSequence,l_qwXRG.dwHi,l_qwXRG.dwLo);
	engine.MsgDeadXRG(0,++l_dwSequence,l_qwXRG.dwHi,l_qwXRG.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Send multiple dead xrg messages
function xqserver_msgdeadxrg_multiple(){
	setup(l_qwXRG);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	for (i=0;i<10;i++){
		engine.MsgDeadXRG(0,++l_dwSequence,l_qwXRG.dwHi,l_qwXRG.dwLo);
	}//endfor
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var WshNetwork = new ActiveXObject("WScript.Network");
var engine = WScript.CreateObject("qtest.engine");
engine.Init();

var l_dwSessionID=0;
var l_dwSequence=0;

var l_qwXIP= new QWORD(engine.InternetAddress(WshNetwork.ComputerName),l_dwSessionID);
var l_qwXRG= new QWORD(g_dwXRGNum,g_dwLifetimeID);

TestCase(xqserver_msgdeadxrg_normal);
TestCase(xqserver_msgdeadxrg_lowerboundXRG);
TestCase(xqserver_msgdeadxrg_upperboundXRG);
TestCase(xqserver_msgdeadxrg_nodataforXRG);
TestCase(xqserver_msgdeadxrg_noqueues);
TestCase(xqserver_msgdeadxrg_multiple);

engine.Cleanup();