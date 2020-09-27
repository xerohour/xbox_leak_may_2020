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

//DESC: Connect, hello, user info
function setup(qwXIP,dwTickleIP){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_qwXIP.dwLo);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,++l_qwXRG.dwLo,dwTickleIP,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage
function xqserver_msgdeadxip_normal(){
	setup(l_qwXIP,l_qwXIP.dwHi);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_dwSessionID);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Lower bounds XIP
function xqserver_msgdeadxip_lowerboundXIP(){
	qwXIP=new QWORD(0x00000000,0x00000000);
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	engine.MsgUserInfo(qwXIP.dwLo,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,qwXIP.dwHi,qwXIP.dwLo,l_qwXRG.dwHi,++l_qwXRG.dwLo,0x00000001,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(qwXIP.dwLo,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXIP(0,++l_dwSequence,qwXIP.dwHi,qwXIP.dwLo);
	engine.MsgList(qwXIP.dwLo,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Upper bounds XIP
function xqserver_msgdeadxip_upperboundXIP(){
	qwXIP=new QWORD(0xFFFFFFFF,0xFFFFFFFF);
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	engine.MsgUserInfo(qwXIP.dwLo,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,qwXIP.dwHi,qwXIP.dwLo,l_qwXRG.dwHi,++l_qwXRG.dwLo,0x00000001,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(qwXIP.dwLo,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgDeadXIP(0,++l_dwSequence,qwXIP.dwHi,qwXIP.dwLo);
	engine.MsgList(qwXIP.dwLo,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: No data in queue
function xqserver_msgdeadxip_nodata(){
	setup(l_qwXIP,l_qwXIP.dwHi);
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_dwSessionID);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: No queues
function xqserver_msgdeadxip_noqueues(){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_dwSessionID);
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_dwSessionID);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Send multiple
function xqserver_msgdeadxip_multiple(){
	setup(l_qwXIP,l_qwXIP.dwHi);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	for (i=0;i<10;i++){
		engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_dwSessionID);
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

TestCase(xqserver_msgdeadxip_normal);
TestCase(xqserver_msgdeadxip_lowerboundXIP);
TestCase(xqserver_msgdeadxip_upperboundXIP);
TestCase(xqserver_msgdeadxip_nodata);
TestCase(xqserver_msgdeadxip_noqueues);
TestCase(xqserver_msgdeadxip_multiple);

engine.Cleanup();