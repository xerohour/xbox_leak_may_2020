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

//DESC: Connect, user info
function setup(qwUserId){
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

//DESC: Normal usage
function xqserver_msgadd_normal(){
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
}//endmethod

//DESC: Lower bounds of User ID
function xqserver_msgadd_lowerbounduserid(){
	l_qwUserId=new QWORD(0x00000000,0x00000001);
	setup(l_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
}//endmethod

//DESC: Upper bounds of User ID
function xqserver_msgadd_upperbounduserid(){
	l_qwUserId=new QWORD(0xFFFFFFFF,0xFFFFFFFF);
	setup(l_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
}//endmethod

//DESC: Invalid User ID; User ID cannot be 0
function xqserver_msgadd_invaliduserid(){
	l_qwUserId=new QWORD(0x00000000,0x00000000);
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Lower bounds of qtype
function xqserver_msgadd_lowerboundqtype(){
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
}//endmethod

//DESC: Upper bounds of qtype
function xqserver_msgadd_upperboundqtype(){
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwNthQType,g_bstrNthQTypeData,g_dwNthQTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
}//endmethod

//DESC: Invalid qtype
function xqserver_msgadd_invalidqtype(){
	setup(g_qwUserId);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwNthQType+1,g_bstrNthQTypeData,g_dw0QTypeDataSize);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send too little data
function xqserver_msgadd_invaliddata_v1(){
	setup(g_qwUserId);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize-1);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Send too much data
function xqserver_msgadd_invaliddata_v2(){
	if (g_dw0QTypeMaxDataSize>0x00005000){
		DebugPrint(ERROR,"QType MaxDataSize Too Big; ABORTING TEST");
		FAIL();
	}//endif
	setup(g_qwUserId);
	var bstrData="";
	for (i=0; i<g_dw0QTypeMaxDataSize+5; i++){
		bstrData+=String.fromCharCode(i);
	}//endfor
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,bstrData,g_dw0QTypeMaxDataSize+5);
	ExpectedState(engine,DISCONNECTED);
}//endmethod

//DESC: Bad session id
function xqserver_msgadd_badsessionid(){
	setup(g_qwUserId);
	engine.MsgAdd(l_dwSessionID+1,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwAllowDuplicatesQType,g_bstrAllowDuplicatesQTypeData,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,0);
}//endmethod

//DESC: 0 session id
function xqserver_msgadd_0sessionid(){
	setup(g_qwUserId);
	engine.MsgAdd(0,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwAllowDuplicatesQType,g_bstrAllowDuplicatesQTypeData,g_dwAllowDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwAllowDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Test duplicate elimination
function xqserver_msgadd_duplicateelimination(){
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwNoDuplicatesQType,g_bstrNoDuplicatesQTypeData,g_dwNoDuplicatesQTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
	engine.MsgAdd(l_dwSessionID,l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwNoDuplicatesQType,g_bstrNoDuplicatesQTypeData,g_dwNoDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwNoDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Make sure bad sequencing does not overright item in no duplicates queue
function xqserver_msgadd_badsequencenum(){
	if (g_dwNoDuplicatesQTypeDupOffset==0){
		DebugPrint(ERROR,"Aborting..Cannot perform test on duplicate elimination queue that has offset set to 0");
		FAIL();
	}//endif
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	var bstrData=g_bstrNoDuplicatesQTypeData;
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dwNoDuplicatesQType,bstrData,g_dwNoDuplicatesQTypeDataSize);
	WaitForTickle(engine);
	if (1!=engine.GetTickleElement("QLength")) FAIL();
	bstrData="X"+g_bstrNoDuplicatesQTypeData.substr(1);
	engine.MsgAdd(l_dwSessionID,l_dwSequence-1,QWORD2CHARCODES(g_qwUserId),1,g_dwNoDuplicatesQType,bstrData,g_dwNoDuplicatesQTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,g_dwNoDuplicatesQType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	bstrData2=engine.GetListReplyItemElement(1,"data");
	if (bstrData.charAt(0)==bstrData2.charAt(0)){
		FAIL();
	}//endif
}//endmethod

//DESC: multiple user ids usage
function xqserver_msgadd_multipleuserids(){
	l_qwUserId1=new QWORD(0x00000001,0x00000001);
	l_qwUserId2=new QWORD(0x00000001,0x00000002);
	setup(l_qwUserId1);
	engine.MsgUserInfo(l_dwSessionID,++l_dwSequence,l_qwUserId2.dwHi,l_qwUserId2.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,++l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(engine);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(l_qwUserId1)+QWORD2CHARCODES(l_qwUserId2),2,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId1.dwHi,l_qwUserId1.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
	engine.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId2.dwHi,l_qwUserId2.dwLo,g_dw0QType,g_qwCookie.dwHi,g_qwCookie.dwLo,0x00000000,0x00000000,2,0xFFFFFFFF);
	ConfirmListReplyItemCount(engine,1);
}//endmethod

//DESC: Bad user count
function xqserver_msgadd_badusercount(){
	setup(g_qwUserId);
	engine.TickleListen(true,g_wPort);
	engine.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),0,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	ExpectedState(engine,DISCONNECTED);
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

TestCase(xqserver_msgadd_normal);
TestCase(xqserver_msgadd_lowerbounduserid);
TestCase(xqserver_msgadd_upperbounduserid);
TestCase(xqserver_msgadd_invaliduserid);
TestCase(xqserver_msgadd_lowerboundqtype);
TestCase(xqserver_msgadd_upperboundqtype);
TestCase(xqserver_msgadd_invalidqtype);
TestCase(xqserver_msgadd_invaliddata_v1);
TestCase(xqserver_msgadd_invaliddata_v2);
TestCase(xqserver_msgadd_badsessionid);
TestCase(xqserver_msgadd_0sessionid);
TestCase(xqserver_msgadd_duplicateelimination);
TestCase(xqserver_msgadd_badsequencenum);
TestCase(xqserver_msgadd_multipleuserids);
TestCase(xqserver_msgadd_badusercount);

engine.Cleanup();