/*----------------------------------------------------------------------------
 *  Copyright © 2000 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 03/19/01
 *  Developer: Sean Wohlgemuth
 *  Description: Windows Scripting Host Script
 *----------------------------------------------------------------------------
 */

//////////////////////////////////////////////////////////////////////////////
// Debug print
//////////////////////////////////////////////////////////////////////////////
var TRACE=0x01;
var WARNING=0x02;
var ERROR=0x04;
var INFO=0x08;
var LOG=0x20;
var SPEW=TRACE|WARNING|ERROR|INFO|LOG;
var DEBUG=ERROR|LOG;

function DebugPrint(/*[in]*/iLevel, /*[in]*/pszDebug){
	if ((iLevel&DEBUG)>0){
		WScript.Echo(pszDebug);
	}//endif
}//endmethod