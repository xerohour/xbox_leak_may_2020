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

//DESC: Connect, etc, add number of items and check that they added properly
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
		engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(qwUserId),1,dwQType,bstrData,dwQTypeDataLen);
	}//endfor
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,dwQType,0x00000000,0x00000000,0x00000000,0x00000000,1,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	qwQUID.dwHi=engine.GetListReplyItemElement(1,"HiQUID");
	qwQUID.dwLo=engine.GetListReplyItemElement(1,"LoQUID");
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

//DESC: Normal usage
function xqserver_msglist_normal(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Lower bounds of User ID
function xqserver_msglist_lowerbounduserid(){
	var qwUserId= new QWORD(0x00000000,0x00000001);
	setup(qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Upper bounds of User ID
function xqserver_msglist_upperbounduserid(){
	var qwUserId= new QWORD(0xFFFFFFFF,0xFFFFFFFF);
	setup(qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Invalid User ID; User ID cannot by 0
function xqserver_msglist_invaliduserid(){
	var qwUserId= new QWORD(0x00000000,0x00000000);
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,0x80070057);
}//endmethod

//DESC: Lower bounds cookie
function xqserver_msglist_lowerboundcookie(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,0x00000000,0x00000000,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Upper bounds cookie
function xqserver_msglist_upperboundcookie(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,0xFFFFFFFF,0xFFFFFFFF,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Special 0 as start quid
function xqserver_msglist_specialzero(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: List item #2 and greater
function xqserver_msglist_item2andgreater(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo+1,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-1);
}//endmethod

//DESC: Lower bounds of Max Items
function xqserver_msglist_lowerboundmaxitem(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,1,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Upper bounds of Max Items
function xqserver_msglist_upperboundmaxitem(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,0xFFFFFFFF,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod

//DESC: Special Max Items; send 0 as max items expect total back
function xqserver_msglist_invalidmaxitems(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
}//endmethod


//DESC: Start quid out of range; expect 0 items returned
function xqserver_msglist_quidoutofrange(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add+1,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: Start quid does not exist; expect number of items in the queue -2 because it only list items AFTER the start quid
function xqserver_msglist_nonexistquid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgDelete(l_dwSessionID,++l_dwSequence,g_dwAllowDuplicatesQType,g_qwUserId.dwHi,g_qwUserId.dwLo,qwQUID.dwHi,qwQUID.dwLo,qwQUID.dwHi,qwQUID.dwLo+1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo+1,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add-2);
}//endmethod

//DESC: Lower bounds non exist userid
function xqserver_msglist_nonexistuserid(){
	var qwUserId=new QWORD(g_qwUserId.dwHi+1,g_qwUserId.dwLo+1);
	engine.Disconnect();
	engine.Connect(g_xqserver,g_iPort);
	engine.MsgHello(0,l_dwSequence++,g_dwVersion,g_bstrDescription);
	engine.MsgList(l_dwSessionID,++l_dwSequence,qwUserId.dwHi,qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,ERROR_NO_SUCH_USER);
}//endmethod

//DESC: Non existent queue type
function xqserver_msglist_badqtype(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,0x12345678,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,0x80070057);
}//endmethod

//DESC: Just list first item
function xqserver_msglist_firstitem(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,1,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	if (qwQUID.dwHi!=engine.GetListReplyItemElement(1,"hiquid")){
		FAIL();
	}//endif
	if (qwQUID.dwLo!=engine.GetListReplyItemElement(1,"loquid")){
		FAIL();
	}//endif
}//endmethod

//DESC: Just list second item
function xqserver_msglist_seconditem(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo+1,1,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	if (qwQUID.dwHi!=engine.GetListReplyItemElement(1,"hiquid")){
		FAIL();
	}//endif
	if ((qwQUID.dwLo+1)!=engine.GetListReplyItemElement(1,"loquid")){
		FAIL();
	}//endif
}//endmethod

//DESC: Just list last item
function xqserver_msglist_lastitem(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo+g_iItems2Add-1,1,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	if (qwQUID.dwHi!=engine.GetListReplyItemElement(1,"hiquid")){
		FAIL();
	}//endif
	if ((qwQUID.dwLo+g_iItems2Add-1)!=engine.GetListReplyItemElement(1,"loquid")){
		FAIL();
	}//endif
}//endmethod

//DESC: Bad session id
function xqserver_msglist_badsessionid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID+1,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,0x80070057);
}//endmethod

//DESC: Issue command multiple times
function xqserver_msglist_multiple(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	for (i=0; i<10; i++){
		engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
		ConfirmListReplyItemCount(engine,g_iItems2Add);
	}//enfor
}//endmethod

//DESC: Check sequence number and session id in reply
function xqserver_msglist_sessionidsequencenumcheck(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
	if (l_dwSessionID!=engine.GetMessageElement("sessid")){
		FAIL();
	}//endif
	if (l_dwSequence!=engine.GetMessageElement("seqnum")){
		FAIL();
	}//endif
}//endmethod

//DESC: 0 session id
function xqserver_msglist_0sessionid(){
	setup(g_qwUserId,g_dwAllowDuplicatesQType,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(0,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyHResult(engine,S_OK);
	engine.MsgList(0,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,qwQUID.dwHi,qwQUID.dwLo,g_iItems2Add,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,g_iItems2Add);
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

TestCase(xqserver_msglist_normal);
TestCase(xqserver_msglist_lowerbounduserid);
TestCase(xqserver_msglist_upperbounduserid);
TestCase(xqserver_msglist_invaliduserid);
TestCase(xqserver_msglist_lowerboundcookie);
TestCase(xqserver_msglist_upperboundcookie);
TestCase(xqserver_msglist_specialzero);
TestCase(xqserver_msglist_item2andgreater);
TestCase(xqserver_msglist_lowerboundmaxitem);
TestCase(xqserver_msglist_upperboundmaxitem);
TestCase(xqserver_msglist_invalidmaxitems);
TestCase(xqserver_msglist_quidoutofrange);
TestCase(xqserver_msglist_nonexistquid);
TestCase(xqserver_msglist_nonexistuserid);
TestCase(xqserver_msglist_badqtype);
TestCase(xqserver_msglist_firstitem);
TestCase(xqserver_msglist_seconditem);
TestCase(xqserver_msglist_lastitem);
TestCase(xqserver_msglist_multiple);
TestCase(xqserver_msglist_badsessionid);
TestCase(xqserver_msglist_0sessionid);
TestCase(xqserver_msglist_sessionidsequencenumcheck);

engine.Cleanup();