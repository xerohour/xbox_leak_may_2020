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
// Test Cases
//////////////////////////////////////////////////////////////////////////////

function xqserver_simple(){

	//Setup
	var l_qwUserId = new QWORD(0,0);
	var l_dwQType=0;

	//Simulate PS connection and queue establishment
	PS.Connect(g_xqserver,g_iPort);
	PS.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);
	PS.MsgUserInfo(++l_dwSessionID,++l_dwSequence,g_qwUserId.dwHi,g_qwUserId.dwLo,l_qwXIP.dwHi,l_dwSessionID,l_qwXRG.dwHi,l_qwXRG.dwLo,l_qwXIP.dwHi,g_wPort);
	ConfirmUserInfoAck(PS);

	//Simulate FD connection
	FD.Connect(g_xqserver,g_iPort);
	FD.MsgHello(0,++l_dwSequence,g_dwVersion,g_bstrDescription);

	//Turn on tickle listening
	FD.TickleListen(true,g_wPort);

	PS.MsgAdd(l_dwSessionID,++l_dwSequence,QWORD2CHARCODES(g_qwUserId),1,g_dw0QType,g_bstr0QTypeData,g_dw0QTypeDataSize);
	
	//Wait for client tickle
	if (true==FD.WaitForTickle(60000)){
		FAIL();
	}//endif

	//Store tickle info
	l_qwUserId.dwHi=FD.GetTickleElement("HiUserId");
	l_qwUserId.dwLo=FD.GetTickleElement("LoUserId");
	l_dwQType=FD.GetTickleElement("QType");
	l_qwQUID=new QWORD(0,0);

	//Request list spawned by tickle
	FD.MsgList(l_dwSessionID,++l_dwSequence,l_qwUserId.dwHi,l_qwUserId.dwLo,l_dwQType,g_qwCookie.dwHi,g_qwCookie.dwLo,l_qwQUID.dwHi,l_qwQUID.dwLo,100,0xFFFFFFFF);
	if (true==FD.WaitForReply(60000)){
		FAIL();
	}//endif
	if (QMSG_LIST_REPLY!=FD.GetReplyType()){
		FAIL();
	}//endif
	if (S_OK!=FD.GetListReplyElement("hr")){
		FAIL();
	}//endif

	//Retrieve all items we are told about
	l_iItems=FD.GetListReplyElement("NumItems");
	for (i=1;i<=l_iItems;i++){
		bstrData=FD.GetListReplyItemElement(i,"data");
		l_qwQUID.dwHi=FD.GetListReplyItemElement(i,"HiQUID");
		l_qwQUID.dwLo=FD.GetListReplyItemElement(i,"LoQUID");

		//Delete item
		FD.MsgDelete(l_dwSessionID,++l_dwSequence,l_dwQType,l_qwUserId.dwHi,l_qwUserId.dwLo,l_qwQUID.dwHi,l_qwQUID.dwLo,l_qwQUID.dwHi,l_qwQUID.dwLo);
	}//endfor

	//Stop listening for tickles
	FD.TickleListen(false,0);

}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var WshNetwork = new ActiveXObject("WScript.Network");
var PS = WScript.CreateObject("qtest.engine");  //Presence server simulator
PS.Init();
var FD = WScript.CreateObject("qtest.engine");  //Front door simulator
FD.Init();

var l_dwSequence=0;
var l_dwSessionID=0;

var l_qwXIP=new QWORD(PS.InternetAddress(WshNetwork.ComputerName),l_dwSessionID);
var l_qwXRG=new QWORD(++g_dwLifetimeID,g_dwXRGNum);

TestCase(xqserver_simple);

FD.Cleanup();
PS.Cleanup();