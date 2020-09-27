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
eval(include("debugprint.js"));

var bstrTestCase="";

function TestCase(func){
	bstrTestCase=func.toString().substring(func.toString().indexOf(" ")+1,func.toString().indexOf("("));
	try{
		func();
		DebugPrint(LOG,"PASS - "+bstrTestCase);
	}catch(e){
		if (e.toString()!="_!testcase!_")
			DebugPrint(LOG,"FAIL (Not testcase initiated!) - "+bstrTestCase);
		else
			DebugPrint(LOG,"FAIL - "+bstrTestCase);
	}//endcatch
}//endmethod

function FAIL(){throw "_!testcase!_";}