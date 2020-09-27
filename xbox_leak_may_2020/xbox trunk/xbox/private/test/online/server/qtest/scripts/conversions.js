/*----------------------------------------------------------------------------
 *  Copyright © 2000 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 03/19/01
 *  Developer: Sean Wohlgemuth
 *  Description: Windows Scripting Host Script
 *----------------------------------------------------------------------------
 */

//////////////////////////////////////////////////////////////////////////////
// QWORD structure
//////////////////////////////////////////////////////////////////////////////

function QWORD(dwHi,dwLo){
	this.dwHi=dwHi;
	this.dwLo=dwLo;
}//endstructure

//////////////////////////////////////////////////////////////////////////////
// Make a DWORD from 2 WORDS
//////////////////////////////////////////////////////////////////////////////

function DWORD(wHi,wLo){
	return (wHi<<16)+wLo;
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Set low WORD of a DWORD
//////////////////////////////////////////////////////////////////////////////

function LODWORD(dwValue){
	return dwValue&0xFFFF;
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Set high WORD of a DWORD
//////////////////////////////////////////////////////////////////////////////

function HIDWORD(dwValue){
	return (dwValue>>16)&0xFFFF;
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Set low BYTE of a WORD
//////////////////////////////////////////////////////////////////////////////

function LOWORD(wValue){
	return wValue&0xFF;
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Set high BYTE of a WORD
//////////////////////////////////////////////////////////////////////////////

function HIWORD(wValue){
	return (wValue>>8)&0xFF;
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert a QWORD to character codes in a BSTR
//////////////////////////////////////////////////////////////////////////////

function QWORD2CHARCODES(dwValue){
	return	String.fromCharCode((dwValue.dwHi>>24)&0xFF)+
			String.fromCharCode((dwValue.dwHi>>16)&0xFF)+
			String.fromCharCode((dwValue.dwHi>>8)&0xFF)+
			String.fromCharCode(dwValue.dwHi&0xFF)+
			String.fromCharCode((dwValue.dwLo>>24)&0xFF)+
			String.fromCharCode((dwValue.dwLo>>16)&0xFF)+
			String.fromCharCode((dwValue.dwLo>>8)&0xFF)+
			String.fromCharCode(dwValue.dwLo&0xFF);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert character codes of a BSTR to a QWORD
//////////////////////////////////////////////////////////////////////////////

function CHARCODES2QWORD(bstrValue, index){
	return new QWORD(	(bstrValue.charCodeAt(index)<<24)+
						(bstrValue.charCodeAt(index+1)<<16)+
						(bstrValue.charCodeAt(index+2)<<8)+
						bstrValue.charCodeAt(index+3),
						(bstrValue.charCodeAt(index+4)<<24)+
						(bstrValue.charCodeAt(index+5)<<16)+
						(bstrValue.charCodeAt(index+6)<<8)+
						bstrValue.charCodeAt(index+7));
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert a DWORD to character codes in a BSTR
//////////////////////////////////////////////////////////////////////////////

function DWORD2CHARCODES(dwValue){
	return	String.fromCharCode((dwValue>>24)&0xFF)+
			String.fromCharCode((dwValue>>16)&0xFF)+
			String.fromCharCode((dwValue>>8)&0xFF)+
			String.fromCharCode(dwValue&0xFF);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert character codes of a BSTR to a DWORD
//////////////////////////////////////////////////////////////////////////////

function CHARCODES2DWORD(bstrValue, index){
	return	(bstrValue.charCodeAt(index)<<24)+
			(bstrValue.charCodeAt(index+1)<<16)+
			(bstrValue.charCodeAt(index+2)<<8)+
			bstrValue.charCodeAt(index+3);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert a WORD to character codes in a BSTR
//////////////////////////////////////////////////////////////////////////////

function WORD2CHARCODES(wValue){
	return	String.fromCharCode((wValue>>8)&0xFF)+
			String.fromCharCode(wValue&0xFF);
}//endmethod

//////////////////////////////////////////////////////////////////////////////
// Convert character codes of a BSTR to a WORD
//////////////////////////////////////////////////////////////////////////////

function CHARCODES2WORD(bstrValue, index){
	return	(bstrValue.charCodeAt(index)<<8)+
			bstrValue.charCodeAt(index+1);
}//endmethod

