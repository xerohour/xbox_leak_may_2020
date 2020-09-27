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

//////////////////////////////////////////////////////////////////////////////
// Test Cases
//////////////////////////////////////////////////////////////////////////////

function xqserver_rapidconnectdisconnect(){
	for (i=0;i<100;i++){
		engine.Connect(g_xqserver,g_iPort);
		engine.Disconnect();
	}//endfor
}//endmethod

/////////////////////////////////////////////////////////////////////////////
// Execute Test Cases
//////////////////////////////////////////////////////////////////////////////

var engine = WScript.CreateObject("qtest.engine");
engine.Init();

TestCase(xqserver_rapidconnectdisconnect);

engine.Cleanup();