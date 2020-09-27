/*----------------------------------------------------------------------------
 *  Copyright © 2000 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 03/19/01
 *  Developer: Sean Wohlgemuth
 *  Description: Windows Scripting Host Script
 *----------------------------------------------------------------------------
 */

function GetServerVar(bstrConfigFile, bstrSection, bstrVariable){

	var fRetVal=false;
	var RetVal=0;

	var bstrLine="";
	var ForReading = 1;
	var fso = new ActiveXObject("Scripting.FileSystemObject");
	var f = fso.OpenTextFile(bstrConfigFile, ForReading, true);

	//Find section
	while (!f.AtEndOfStream){
		bstrLine=f.ReadLine();
		if ("["==bstrLine.charAt(0)){
			if (bstrLine.substring(1,bstrSection.length+1).toLowerCase()==bstrSection.toLowerCase()){
				while (!f.AtEndOfStream){
					bstrLine=f.ReadLine();
					if (bstrLine.substring(0,bstrVariable.length).toLowerCase()==bstrVariable.toLowerCase()){
						RetVal=bstrLine.substring(bstrLine.lastIndexOf("=")+1,bstrLine.length).valueOf();
						fRetVal=true;
						break;
					}//endif
				}//endmethod
				if (true==fRetVal){
					break;
				}//endif
			}//endif
		}//endif
	}//endwhile
	
	if (false==fRetVal){
		throw "server configuration parameter ["+bstrSection+"]("+bstrVariable+") does not exist";
	}//endif

	f.Close();
	
	return RetVal;

}//endmethod

function GetMatchingQueueType(bstrConfigFile, bstrVariable, bstrValue){
	
	var l_iQTypes=GetServerVar(bstrConfigFile, "Global", "NumQueueTypes").valueOf();
	for (i=0; i<l_iQTypes; i++){
		try{
			if (bstrValue.valueOf()==GetServerVar(bstrConfigFile,"Queue Type "+i,bstrVariable).valueOf()){
				return i;
			}//endif
		}catch(e){}
	}//endfor

	throw "server configuration does not contain a queue type that matches ("+bstrVariable+"="+bstrValue+")";

	return 0;

}//endmethod