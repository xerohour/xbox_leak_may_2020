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
eval(include("parseconfig.js"));

var g_bstrConfigFile="xqserver.cfg";

var MSGTYPE_ACK=0xffffffff;
var QMSG_HELLO = 0;
var QMSG_USER_INFO = 1;
var QMSG_ADD = 2;
var QMSG_DELETE = 3;
var QMSG_DELETE_MATCHES = 4;
var QMSG_DEAD_XIP = 5;
var QMSG_DEAD_XRG = 6;
var QMSG_LIST = 7;
var QMSG_MAX_REQ_TYPE = 7;
var QMSG_LIST_REPLY = 8;
var S_OK=0;
var ERROR_NO_SUCH_USER=0x80070525;

//Default variables
var g_dwLifetimeID=0;
var g_dwXRGNum=0;
var g_bstrDescription="v"+HIWORD(g_dwVersion)+"."+LOWORD(g_dwVersion);
var g_qwUserId=new QWORD(0x00000000,0x0000000A);
var g_dwXRG=0x0000000A;
var g_wPort=1000;
var g_qwCookie=new QWORD(0x00000000,0x0000000A);
var g_qwQUID=new QWORD(0x00000000,0x00000000);
var g_iItems2Add=7;

try{

	//QUEUE TYPE 0 (Lower bound queue type)
	var g_dw0QType=0;
	var g_dw0QTypeMinDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type 0","MinDataSize"));
	var g_dw0QTypeMaxDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type 0","MaxDataSize"));
	if (g_dw0QTypeMaxDataSize>g_dw0QTypeMinDataSize){
		g_dw0QTypeDataSize=new Number(g_dw0QTypeMinDataSize)+1;
	}else{
		g_dw0QTypeDataSize=new Number(g_dw0QTypeMinDataSize);
	}//endif
	var g_bstr0QTypeData="";
	for (i=0; i<g_dw0QTypeDataSize; i++){
		g_bstr0QTypeData+=String.fromCharCode(i);
	}//endfor

	//QUEUE TYPE Nth (Upper bound queue type)
	var g_dwNthQType=new Number(GetServerVar(g_bstrConfigFile,"Global","NumQueueTypes"))-1;
	var g_dwNthQTypeMinDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwNthQType,"MinDataSize"));
	var g_dwNthQTypeMaxDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwNthQType,"MaxDataSize"));
	if (g_dwNthQTypeMaxDataSize>g_dwNthQTypeMinDataSize){
		g_dwNthQTypeDataSize=new Number(g_dwNthQTypeMinDataSize)+1;
	}else{
		g_dwNthQTypeDataSize=new Number(g_dwNthQTypeMinDataSize);
	}//endif
	var g_bstrNthQTypeData="";
	for (i=0; i<g_dwNthQTypeDataSize; i++){
		g_bstrNthQTypeData+=String.fromCharCode(64+i);
	}//endfor

	//QUEUE TYPE Allow Duplicates (for cases that require duplicate entries)
	var g_dwAllowDuplicatesQType=new Number(GetMatchingQueueType(g_bstrConfigFile,"AllowDuplicates","Yes"));
	var g_dwAllowDuplicatesQTypeMinDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwAllowDuplicatesQType,"MinDataSize"));
	var g_dwAllowDuplicatesQTypeMaxDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwAllowDuplicatesQType,"MaxDataSize"));
	if (g_dwAllowDuplicatesQTypeMaxDataSize>g_dwAllowDuplicatesQTypeMinDataSize){
		g_dwAllowDuplicatesQTypeDataSize=new Number(g_dwAllowDuplicatesQTypeMinDataSize)+1;
	}else{
		g_dwAllowDuplicatesQTypeDataSize=new Number(g_dwAllowDuplicatesQTypeMinDataSize);
	}//endif
	var g_bstrAllowDuplicatesQTypeData="";
	for (i=0; i<g_dwAllowDuplicatesQTypeDataSize; i++){
		g_bstrAllowDuplicatesQTypeData+=String.fromCharCode(i);
	}//endfor

	//QUEUE TYPE No Duplicates
	var g_dwNoDuplicatesQType=new Number(GetMatchingQueueType(g_bstrConfigFile,"AllowDuplicates","No"));
	var g_dwNoDuplicatesQTypeDupOffset=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwNoDuplicatesQType,"DupAttrOffset"));
	var g_dwNoDuplicatesQTypeMinDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwNoDuplicatesQType,"MinDataSize"));
	var g_dwNoDuplicatesQTypeMaxDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwNoDuplicatesQType,"MaxDataSize"));
	if (g_dwNoDuplicatesQTypeMaxDataSize>g_dwNoDuplicatesQTypeMinDataSize){
		g_dwNoDuplicatesQTypeDataSize=new Number(g_dwNoDuplicatesQTypeMinDataSize)+1;
	}else{
		g_dwNoDuplicatesQTypeDataSize=new Number(g_dwNoDuplicatesQTypeMinDataSize);
	}//endif
	var g_bstrNoDuplicatesQTypeData="";
	for (i=0; i<g_dwNoDuplicatesQTypeDataSize; i++){
		g_bstrNoDuplicatesQTypeData+=String.fromCharCode(i);
	}//endfor

	//QUEUE TYPE Allow matching deletes
	var g_dwMatchingDeleteQType=new Number(GetMatchingQueueType(g_bstrConfigFile,"AllowMatches","Yes"));
	var g_dwMatchingDeleteQTypeMatchAttrOffset=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwMatchingDeleteQType,"MatchAttrOffset"));
	var g_dwMatchingDeleteQTypeMatchAttrLen=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwMatchingDeleteQType,"MatchAttrLen"));	
	var g_dwMatchingDeleteQTypeMinDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwMatchingDeleteQType,"MinDataSize"));
	var g_dwMatchingDeleteQTypeMaxDataSize=new Number(GetServerVar(g_bstrConfigFile,"Queue Type "+g_dwMatchingDeleteQType,"MaxDataSize"));
	if (g_dwMatchingDeleteQTypeMaxDataSize>g_dwMatchingDeleteQTypeMinDataSize){
		g_dwMatchingDeleteQTypeDataSize=new Number(g_dwMatchingDeleteQTypeMinDataSize)+1;
	}else{
		g_dwMatchingDeleteQTypeDataSize=new Number(g_dwMatchingDeleteQTypeMinDataSize);
	}//endif
	var g_bstrMatchingDeleteQTypeData="";
	for (i=0; i<g_dwMatchingDeleteQTypeDataSize; i++){
		g_bstrMatchingDeleteQTypeData+=String.fromCharCode(i);
	}//endfor


}catch(e){
	WScript.Echo(e.toString());
	WScript.Quit(1);
}//endcatch