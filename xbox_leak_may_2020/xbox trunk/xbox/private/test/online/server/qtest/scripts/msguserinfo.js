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

//DESC: Connect and send hello
function setup(){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_qwXIP.dwLo);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage of User Info 
function xqserver_userinfomsg_normal(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Lower bound User ID parameter
function xqserver_userinfomsg_lowerbounduserid(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,0x00000000,0x00000001,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Upper bound User ID parameter
function xqserver_userinfomsg_upperbounduserid(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,0xFFFFFFFF,0xFFFFFFFF,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Invalid User ID; User ID cannot be 0
function xqserver_userinfomsg_invaliduserid(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,0x00000000,0x00000000,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Lower bound of XIP parameter
function xqserver_userinfomsg_lowerboundXIP(){
	setup();
	engine.MsgUserInfo(0x00000000,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,0x00000000,0x00000000,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Upper bound of XIP parameter
function xqserver_userinfomsg_upperboundXIP(){
	setup();
	engine.MsgUserInfo(0xFFFFFFFF,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,0xFFFFFFFF,0xFFFFFFFF,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Lower bound of XRG parameter
function xqserver_userinfomsg_lowerboundXRG(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,0x00000000,0x00000000,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Upper bound of XRG parameter
function xqserver_userinfomsg_upperboundXRG(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,0xFFFFFFFF,0x00000000,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Lower bound of IP parameter
function xqserver_userinfomsg_lowerboundtickleIP(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,0x00000001,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Upper bound of IP parameter
function xqserver_userinfomsg_upperboundtickleIP(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,0xFFFFFFFE,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Tickle IP cannot be 0x00000000
function xqserver_userinfomsg_invalidtickleIP_v1(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,0x00000000,g_wPort);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Tickle IP cannot be 0xFFFFFFFF 
function xqserver_userinfomsg_invalidtickleIP_v2(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,0xFFFFFFFF,g_wPort);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Lower bound of the tickle port parameter
function xqserver_userinfomsg_lowerboundtickleport(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,0x0000);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Upper bound of tickle port parameter
function xqserver_userinfomsg_upperboundtickleport(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,0xFFFF);
	ConfirmUserInfoAck(engine);
}//endmethod

//DESC: Send multiple userinfo commands
function xqserver_userinfomsg_multiple(){
	setup();
	for (i=0; i<10; i++){
		engine.MsgUserInfo(l_dwSessionID+i,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID+i,l_qwXRG.dwHi,l_qwXRG.dwLo+1,l_qwXIP.dwHi,g_wPort);
		ConfirmUserInfoAck(engine);
	}//endfor
}//endmethod

//DESC: The sessionid and the low dword of the XIP must match
function xqserver_userinfomsg_badsessionid(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID+1,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Check sequence number and session id in reply
function xqserver_userinfomsg_sessionidsequencenumcheck(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	if (l_dwSessionID!=engine.GetMessageElement("sessid")){
		FAIL();
	}//endif
	if (0!=engine.GetMessageElement("seqnum")){
		FAIL();
	}//endif
}//endmethod

//DESC: A change in session id will destroy the queue
function xqserver_userinfomsg_destroyqueue_v1(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	l_dwSessionID++;
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: A change in xrg will destroy the queue
function xqserver_userinfomsg_destroyqueue_v2(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	l_dwSessionID++;
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo+1,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: A change in xrg will destroy the queue
function xqserver_userinfomsg_nodestroyqueue(){
	setup();
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
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

TestCase(xqserver_userinfomsg_normal);
TestCase(xqserver_userinfomsg_lowerbounduserid);
TestCase(xqserver_userinfomsg_upperbounduserid);
TestCase(xqserver_userinfomsg_invaliduserid);
TestCase(xqserver_userinfomsg_lowerboundXIP);
TestCase(xqserver_userinfomsg_upperboundXIP);
TestCase(xqserver_userinfomsg_lowerboundXRG);
TestCase(xqserver_userinfomsg_upperboundXRG)
TestCase(xqserver_userinfomsg_lowerboundtickleIP);
TestCase(xqserver_userinfomsg_upperboundtickleIP);
TestCase(xqserver_userinfomsg_invalidtickleIP_v1);
TestCase(xqserver_userinfomsg_invalidtickleIP_v2);
TestCase(xqserver_userinfomsg_lowerboundtickleport);
TestCase(xqserver_userinfomsg_upperboundtickleport);
TestCase(xqserver_userinfomsg_multiple);
TestCase(xqserver_userinfomsg_badsessionid);
TestCase(xqserver_userinfomsg_sessionidsequencenumcheck);
TestCase(xqserver_userinfomsg_destroyqueue_v1);
TestCase(xqserver_userinfomsg_destroyqueue_v2);
TestCase(xqserver_userinfomsg_nodestroyqueue);

engine.Cleanup();