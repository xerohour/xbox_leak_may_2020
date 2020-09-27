<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    default.asp

Abstract:

    Main page

Arguments:

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;
    var deviceOptions = "";
    var mfgOptions = "";
    var lastDevice = "";
    var firstMfgList = "";

    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>HVS Results</SPAN></CENTER>";

%>
<HTML>
<HEAD>
    <TITLE>HVS</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/hvs.css">
</HEAD>

<SCRIPT>
    var mfgList = new Array;
<%
    //SELECT [Device Type], [Manufacturer], [Firmware Version], [Test Name], [Status], SUM([Number of Operations]) AS Operations, COUNT([Test Name]) As [Num Boxes] FROM [Results] GROUP BY [Device Type], [Manufacturer], [Firmware Version], [Test Name], [Status] ORDER BY [Device Type], [Manufacturer],  [Firmware Version], [Test Name], [Status]

    // generate drop downs
    var SQLQuery = "SELECT [Device Type], [Manufacturer] FROM [Results] GROUP BY [Device Type], [Manufacturer] ORDER BY [Device Type], [Manufacturer]";
    records.Open(SQLQuery, driver, 0, 1); // forward, read only

    while(!records.EOF)
        {
        if(lastDevice != RTrim(records("Device Type")))
            {
            if(lastDevice != "")
                {
                Response.Write("mfgList[\""+lastDevice+"\"] = \"" +mfgOptions+ "\";\r\n");
                if(firstMfgList == "") firstMfgList = mfgOptions;
                mfgOptions = "";
                }
            deviceOptions += "<OPTION VALUE='" + RTrim(records("Device Type")) + "'>" + RTrim(records("Device Type")) + "</OPTION>\r\n";
            lastDevice = RTrim(records("Device Type"));
            }
        
        mfgOptions += "<OPTION VALUE='" + RTrim(records("Manufacturer")) + "'>" + RTrim(records("Manufacturer")) + "</OPTION>";
        records.MoveNext();
        }
    Response.Write("mfgList[\""+lastDevice+"\"] = \"" +mfgOptions+ "\";\r\n");

    records.Close();
%>
function ChangeIt()
    {
    MFGOPTIONX.innerHTML = "<SELECT ID=MFGOPTION SIZE=1>" + mfgList[DEVOPTION.options[DEVOPTION.selectedIndex].value]; + "</SELECT>";
    }
function NextStep()
    {
    document.location = "summary.asp?device=" + UrlEncode(DEVOPTION.options[DEVOPTION.selectedIndex].value) + "&mfg=" + UrlEncode(MFGOPTION.options[MFGOPTION.selectedIndex].value);
    }
function UrlEncode(url) {
	var special_chars, hexChars, urllength;
	var stringpos, temp, stringchar;
	var charOne, charTwo;
	hex_chars="0123456789ABCDEF";
	special_chars="%`~!@#$^&*()-_=+[{]}\\|;:,<.>/? ";
	urllength=url.length;
	stringpos=0;
	temp="";	
	do
		{
		stringchar=url.slice(stringpos, stringpos+1);			
		if (special_chars.indexOf(stringchar)>=0)
			{
			stringchar=stringchar.charCodeAt(0);
			charOne=stringchar % 16;
			charTwo=(stringchar - charOne)/16;
			stringchar="" + hex_chars.charAt(charTwo);
			stringchar=stringchar + hex_chars.charAt(charOne);
			stringchar="%" + stringchar;
			}
		temp=temp + stringchar;
		stringpos++;
		}
	while (stringpos<=urllength-1);
	return temp;
}
</SCRIPT>
<BODY>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<P>
<CENTER>
<TABLE CELLPADDING=6 CELLSPACING=1 BORDER=0>


<TR><TH ROWSPAN=2 WIDTH=75><B>Select a device and manufacturer:</B></TH><TH>Device Type</TH><TH>Manufacturer</TH><TD ROWSPAN=2><INPUT TYPE=BUTTON onClick="NextStep();" VALUE="Next"></TD></TR>
<TR>

<TD>

<SELECT ID=DEVOPTION SIZE=1 onChange="ChangeIt();">
<%=deviceOptions%>
</SELECT>

</TD>
<TD>
<DIV ID=MFGOPTIONX>
<SELECT ID=MFGOPTION SIZE=1>
<%=firstMfgList%>
</SELECT>
</DIV>
</TD>

</TABLE>

<P>

</CENTER>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
