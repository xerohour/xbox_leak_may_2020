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

//DESC: Connect, add items and store quid of first item
function setup(qwUserId,dwQType,dwQTypeDataLen){
	var bstrData="";
	for (i=0; i<dwQTypeDataLen; i++){
		bstrData+=String.fromCharCode(i);
	}//endfor	
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	l_qwXIP.dwLo=++l_dwSessionID;
	engine.MsgDeadXIP(0,++l_dwSequence,l_qwXIP.dwHi,l_qwXIP.dwLo);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,++l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	for (i=0; i<g_iItems2Add; i++){
		engine.MsgAdd(l_dwSessionID,++l_dwSequence, QWORD2CHARCODES(qwUserId),1, dwQType, bstrData, dwQTypeDataLen);
	}//endfor
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,dwQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0,0,10,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,S_OK);
	if (0>=engine.GetListReplyElement("numitems")){
		FAIL();
	}//endif
	qwQUID.dwHi=engine.GetListReplyItemElement(1,"HiQUID");
	qwQUID.dwLo=engine.GetListReplyItemElement(1,"LoQUID");
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage
function xqserver_msgdelete_normal(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Lower bounds user id
function xqserver_msgdelete_lowerbounduserid(){
	qwUserId=new QWORD(0x00000000,0x00000001);
	setup(qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,qwUserId.dwHi,qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Upper bounds user id
function xqserver_msgdelete_upperbounduserid(){
	qwUserId=new QWORD(0xFFFFFFFF,0xFFFFFFFF);
	setup(qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,qwUserId.dwHi,qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Invalid user id; user id can not be 0
function xqserver_msgdelete_invaliduserid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,0x00000000,0x00000000,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Lower bounds queue type
function xqserver_msgdelete_lowerboundsqtype(){
	setup(g_qwUserId,g_dw0QType,g_dw0QTypeMinDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dw0QType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Start quid 0; special case
function xqserver_msgdelete_zeroquid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,0x00000000,0x00000000,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Delete all but last item
function xqserver_msgdelete_deleteafew_v1(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-2);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Delete all but last using 0 as start; special case
function xqserver_msgdelete_deleteafew_v2(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,0x00000000,0x00000000,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-2);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Invalid to use 0 as end quid
function xqserver_msgdelete_invalidquid_v1(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,0x00000000,0x00000000);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Invalid to use 0 as start and end quid
function xqserver_msgdelete_invalidquid_v2(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,0x00000000,0x00000000,0x00000000,0x00000000);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Delete first item
function xqserver_msgdelete_deletefirst(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-1);
	if (qwQUID.dwLo+1!=engine.GetListReplyItemElement(1,"LoQUID")){
		FAIL();
	}//endif
}//endmethod

//DESC: Delete second item
function xqserver_msgdelete_deletesecond(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo+1,qwQUID.dwHi,qwQUID.dwLo+1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-1);
	if (qwQUID.dwLo!=engine.GetListReplyItemElement(1,"LoQUID")){
		FAIL();
	}//endif
}//endmethod

//DESC: Delete last item
function xqserver_msgdelete_deletelast(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-1,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-1);
	if (qwQUID.dwLo!=engine.GetListReplyItemElement(1,"LoQUID")){
		FAIL();
	}//endif
	if (qwQUID.dwLo+g_iItems2Add-3!=engine.GetListReplyItemElement(g_iItems2Add-2,"loquid")){
		FAIL();
	}//endif
}//endmethod

//DESC: Delete invalid range; start and stop quid's reversed
function xqserver_msgdelete_invalidrange(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add,qwQUID.dwHi,qwQUID.dwLo);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Try to delete beyond known quids
function xqserver_msgdelete_nonexistquid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add*2,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add*2);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Try to delete from an invalid queue type
function xqserver_msgdelete_invalidqtype(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,0x12345678,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Try to delete items from a queue for a user that does not exist; remain connected
function xqserver_msgdelete_nonexistuserid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi+1,g_qwUserId.dwLo+1,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	ExpectedState(engine,CONNECTED);
}//endmethod

//DESC: Delete first and last items then try to delete range where those items are start and last quid
function xqserver_msgdelete_rangeexceedsbeginandend(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-1,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-2);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-2);
	if (qwQUID.dwLo+1!=engine.GetListReplyItemElement(1,"LoQUID")){
		FAIL();
	}//endif
	if (qwQUID.dwLo+g_iItems2Add-3!=engine.GetListReplyItemElement(g_iItems2Add-3,"LoQUID")){
		FAIL();
	}//endif
}//endmethod

//DESC: Mismatched session id
function xqserver_msgdelete_badsessionid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID-1,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: 0 session id
function xqserver_msgdelete_0sessionid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(0,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var engine = WScript.CreateObject("qtest.engine");
engine.Init();

var WshNetwork = new ActiveXObject("WScript.Network");
var qwQUID=new QWORD(0x00000000,0x00000000);

var l_dwSessionID=0;
var l_dwSequence=0;

var l_qwXIP= new QWORD(engine.InternetAddress(WshNetwork.ComputerName),l_dwSessionID);
var l_qwXRG= new QWORD(g_dwXRGNum,g_dwLifetimeID);

TestCase(xqserver_msgdelete_normal);
TestCase(xqserver_msgdelete_lowerbounduserid);
TestCase(xqserver_msgdelete_upperbounduserid);
TestCase(xqserver_msgdelete_invaliduserid);
TestCase(xqserver_msgdelete_lowerboundsqtype);
TestCase(xqserver_msgdelete_zeroquid);
TestCase(xqserver_msgdelete_deleteafew_v1);
TestCase(xqserver_msgdelete_deleteafew_v2);
TestCase(xqserver_msgdelete_invalidquid_v1);
TestCase(xqserver_msgdelete_invalidquid_v2);
TestCase(xqserver_msgdelete_deletefirst);
TestCase(xqserver_msgdelete_deletesecond);
TestCase(xqserver_msgdelete_deletelast);
TestCase(xqserver_msgdelete_invalidrange);
TestCase(xqserver_msgdelete_nonexistquid);
TestCase(xqserver_msgdelete_invalidqtype);
TestCase(xqserver_msgdelete_nonexistuserid);
TestCase(xqserver_msgdelete_rangeexceedsbeginandend);
TestCase(xqserver_msgdelete_badsessionid);
TestCase(xqserver_msgdelete_0sessionid);

engine.Cleanup();