<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    summary.asp

Abstract:

    Display all rows from the given query

Arguments:

    required:
        device  [form, qs]  the Hardware device tested
        mfg     [form, qs]  manufacturer of the hw device

    optional:
        fw      [form, qs]  the firmware to display

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 1;

    var criteria = new Array;

    // grab the acceptance criteria
    records.Open("SELECT [Test Name], [Percentage] FROM [Acceptance Criteria]", driver, 0, 1); // forward, read only
    while(!records.EOF)
        {
        criteria[RTrim(records("Test Name"))] = parseFloat(records("Percentage"));
        records.MoveNext();
        }
    records.Close();

    // sql settings
    var SQLQuery;

    if(Request("fw").Count >= 1)
        SQLQuery = "SELECT [Firmware Version], [Test Name], [Status], SUM([Number of Operations]) AS Operations, COUNT([Test Name]) As [Num Boxes] FROM [Results] WHERE [Device Type] LIKE '"+Request("device")+"%' AND [Manufacturer] LIKE '"+Request("mfg")+"%' AND [Firmware Version] LIKE '"+Request("fw")+"%' GROUP BY [Firmware Version], [Test Name], [Status] ORDER BY [Firmware Version], [Test Name], [Status]";
    else
        SQLQuery = "SELECT [Firmware Version], [Test Name], [Status], SUM([Number of Operations]) AS Operations, COUNT([Test Name]) As [Num Boxes] FROM [Results] WHERE [Device Type] LIKE '"+Request("device")+"%' AND [Manufacturer] LIKE '"+Request("mfg")+"%' GROUP BY [Firmware Version], [Test Name], [Status] ORDER BY [Firmware Version], [Test Name], [Status]";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only

    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>HVS Results</SPAN></CENTER>";

    var html = "";
    var altHtml = "";
    var numFail;
    var numPass;
    var numBoxes;
%>
<HTML>
<HEAD>
    <TITLE>HVS Results for <%=Request("device")%>: <%=Request("mfg")%></TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/hvs.css">
</HEAD>

<BODY>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<TABLE WIDTH=100% STYLE="border: 1px solid #000000;">
    <TR><TH WIDTH=120>Device</TH><TD><%=Request("device")%></TD></TR>
    <TR><TH WIDTH=120>Manufacturer</TH><TD><%=Request("mfg")%></TD></TR>
</TD></TR>
</TABLE>

<P>

<CENTER>
<TABLE CELLPADDING=6 CELLSPACING=1 BORDER=0>

<!-- Column header Row -->
<TR><TH>Firmware</TH><TH>Test Name</TH><TH>Pass</TH><TH>Fail</TH><TH>Num Boxes</TH><TH>Acceptance</TH><TH>Details</TH></TR>

<!-- Data Rows -->
<%
while(!records.EOF)
    {
    row = row?0:1;
    numFail = 0;
    numPass = 0;
    numBoxes = 0;
    passFail = "";
    testName = "";
    moveNext = true;
    validRecord = true;

    html = "<TR CLASS=ROW" + row + "><TD>" + RTrim(records("Firmware Version")) + "</TD><TD>" + RTrim(records("Test Name")) + "</TD>";
    fw = RTrim(records("Firmware Version"));
    testName = RTrim(records("Test Name"));
    numBoxes = parseInt(records("Num Boxes"));

    if(parseInt(records("Status")) == 0)
        {
        numFail = parseInt(records("Operations"));
        records.MoveNext();
        if(records.EOF) validRecord = moveNext = false;
        else
            {
            if(html != "<TR CLASS=ROW" + row + "><TD>" + RTrim(records("Firmware Version")) + "</TD><TD>" + RTrim(records("Test Name")) + "</TD>")
                validRecord = false;
            else
                numBoxes += parseInt(records("Num Boxes"));
            }
        }

    if(validRecord)
        numPass = parseInt(records("Operations"));

    passFail = "FAIL";
    if(criteria[testName] == undefined) passFail = "NA";
    else if(numFail == 0) passFail = "PASS";
    else if(numPass && ((numFail/numPass) < criteria[testName])) passFail = "PASS";
    //percentage version: else if(numPass && criteria[testName]) passFail = "FAIL (by " +Format2Places(criteria[testName] / (numFail/numPass) * 100)+ "%)";
    else if(numPass) passFail = "FAIL (by " + FormatNumberWCommas(numFail - (criteria[testName]*numPass)) + " errors)";
    Response.Write(html+"<TD ALIGN=RIGHT>"+FormatNumberWCommas(numPass)+"</TD><TD ALIGN=RIGHT>"+FormatNumberWCommas(numFail)+"</TD><TD ALIGN=RIGHT>"+numBoxes+"</TD><TD>"+passFail+"</TD><TD ALIGN=CENTER><A HREF='details.asp?device="+Request("device")+"&mfg="+Request("mfg")+"&fw="+fw+"&test="+testName+"'><IMG SRC='images/details.gif' BORDER=0></A></TD></TR>\r\n");

    if(moveNext) records.MoveNext();
    }


records.Close();
%>
</TABLE>
</CENTER>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
