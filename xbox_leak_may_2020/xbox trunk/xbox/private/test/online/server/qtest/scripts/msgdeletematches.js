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

//DESC: Connect, add items
function setup(qwUserId,dwQType){
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_qwXIP.dwLo);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,++l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
}//endmethod
//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: normal usage case
function xqserver_msgdeletematches_normal(){
	setup(g_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: lower bounds on user id
function xqserver_msgdeletematches_lowerbounduserid(){
	l_qwUserId=new QWORD(0x00000000,0x00000001);
	setup(l_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId.dwHi,l_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId.dwHi,l_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: upper bounds on user id
function xqserver_msgdeletematches_upperbounduserid(){
	l_qwUserId=new QWORD(0xFFFFFFFF,0xFFFFFFFF);
	setup(l_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId.dwHi,l_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId.dwHi,l_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: invalid user id
function xqserver_msgdeletematches_invaliduserid(){
	l_qwUserId=new QWORD(0x00000000,0x00000000);
	setup(g_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: no match
function xqserver_msgdeletematches_nomatch(){
	setup(g_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset+1,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen+1),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: try to delete from the wrong queue type
function xqserver_msgdeletematches_wrongqueuetype(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwAllowDuplicatesQType,g_bstrAllowDuplicatesQTypeData,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwAllowDuplicatesQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: bad session id; command ignored
function xqserver_msgdeletematches_badsessionid(){
	setup(g_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID-1,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: 0 session id; ignore session id and process command
function xqserver_msgdeletematches_0sessionid(){
	setup(g_qwUserId,g_dwMatchingDeleteQType);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(0,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: lower bounds on user id
function xqserver_msgdeletematches_multipleuserid(){
	l_qwUserId1=new QWORD(0x00000001,0x00000001);
	l_qwUserId2=new QWORD(0x00000001,0x00000002);
	setup(l_qwUserId1,g_dwMatchingDeleteQType);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,l_qwUserId2.dwHi,l_qwUserId2.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,++l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId1)+QWORD2CHARCODES(l_qwUserId2),2,g_dwMatchingDeleteQType,g_bstrMatchingDeleteQTypeData,g_dwMatchingDeleteQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId1.dwHi,l_qwUserId1.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId2.dwHi,l_qwUserId2.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgDeleteMatches(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId1)+QWORD2CHARCODES(l_qwUserId2),2,g_dwMatchingDeleteQType,0,g_bstrMatchingDeleteQTypeData.substring(g_dwMatchingDeleteQTypeMatchAttrOffset,g_dwMatchingDeleteQTypeMatchAttrOffset+g_dwMatchingDeleteQTypeMatchAttrLen),g_dwMatchingDeleteQTypeMatchAttrLen);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId1.dwHi,l_qwUserId1.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId2.dwHi,l_qwUserId2.dwLo,g_dwMatchingDeleteQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var engine = WScript.CreateObject("qtest.engine");
engine.Init();

var WshNetwork = new ActiveXObject("WScript.Network");

var l_dwSessionID=0;
var l_dwSequence=0;

var l_qwXIP= new QWORD(engine.InternetAddress(WshNetwork.ComputerName),l_dwSessionID);
var l_qwXRG= new QWORD(g_dwXRGNum,g_dwLifetimeID);

TestCase(xqserver_msgdeletematches_normal);
TestCase(xqserver_msgdeletematches_lowerbounduserid);
TestCase(xqserver_msgdeletematches_upperbounduserid);
TestCase(xqserver_msgdeletematches_invaliduserid);
TestCase(xqserver_msgdeletematches_nomatch);
TestCase(xqserver_msgdeletematches_wrongqueuetype);
TestCase(xqserver_msgdeletematches_badsessionid);
TestCase(xqserver_msgdeletematches_0sessionid);
TestCase(xqserver_msgdeletematches_multipleuserid);

engine.Cleanup();