<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    myBoxes.asp

Abstract:

    Displays the current status of the lab machines

Arguments:

    none

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;
    var html = "";

    var currentState = 0xFF;
    var currentBuild = "";
    var states = new Array;
    states[0] = "Crashed";
    states[1] = "Not Responding";
    states[2] = "Idle";
    states[3] = "Running";

    var user;

    if(Request("user").Count >= 1)
        user = ""+Request("user");
    else
        user = GetUsername();

    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>My Boxes</SPAN></TD></TABLE>";
%>
<HTML>
<HEAD>
    <TITLE>XBOX Lab</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 STYLE='border-top:1px solid #7CB021'><TD STYLE='cursor:hand' onClick='ExpandCollapse(BLOCKMINE);'><H3>My Boxes</H3></TD><TD STYLE='text-align:right'></TD></TABLE>
<BLOCKQUOTE ID=BLOCKMINE>
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=90>Machine</TH>
<TH CLASS=ROWHEAD WIDTH=100>Owner</TH>
<TH CLASS=ROWHEAD WIDTH=90>SW</TH>
<TH CLASS=ROWHEAD WIDTH=90>ROM</TH>
<TH CLASS=ROWHEAD WIDTH=95>Current State</TH>
<TH CLASS=ROWHEAD WIDTH=28>Info</TH>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>


<!-- Data Rows -->
<%
    var SQLQuery = "SELECT * FROM BoxList WHERE [Owner] LIKE '%"+user+"%' ORDER BY [MAC Address];";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only
    row = 0;

    while(!records.EOF)
        {
        if(RTrim(records("Owner")) != user) break;

        currentState = parseInt(records("Current State"));

        html = "<TR><TD></TD>";
        html += "<TD CLASS=ROW" + row + "><A TITLE='Open Debugger' HREF=\"javascript:Run('%SystemRoot%/system32/cmd.exe /T:0A /K %_NTDRIVE%%_NTROOT%/public/tools/razzle.cmd REMOTE /C " + RTrim(records("Debugger")) + " " + RTrim(records("Machine Name")) + "')\">" + records("Machine Name") + "</A></TD>\n";
        html += "    <TD CLASS=ROW" + row + "><A HREF='mailto:" + RTrim(records("Owner")) + "'>"+records("Owner")+"</A></TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + records("Current SW Version") + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + records("Current ROM Version") + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + states[currentState] + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editBox.asp?mac=" + records("MAC Address") + "'><IMG ALT='Edit / Details' SRC='images/edit.gif' BORDER=0></A></TD>\n";
        html += "    </TR>\n\n";

        Response.Write(html);
        records.MoveNext();
        row == 0 ? row = 1 : row = 0;
        }
    records.Close();
%>
</TABLE></BLOCKQUOTE>

<%  
%>


<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 STYLE='border-top:1px solid #7CB021'><TD STYLE='cursor:hand' onClick='ExpandCollapse(BLOCKBREAK);'><H3>Broken Boxes Assigned to Me</H3></TD><TD STYLE='text-align:right'></TD></TABLE>
<BLOCKQUOTE ID=BLOCKBREAK>
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=90>Machine</TH>
<TH CLASS=ROWHEAD WIDTH=100>Owner</TH>
<TH CLASS=ROWHEAD WIDTH=90>SW</TH>
<TH CLASS=ROWHEAD WIDTH=90>ROM</TH>
<TH CLASS=ROWHEAD WIDTH=95>Current State</TH>
<TH CLASS=ROWHEAD WIDTH=28>Info</TH>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>


<!-- Data Rows -->
<%
    var SQLQuery = "SELECT * FROM BoxList WHERE [Crash Owner] LIKE '%"+user+"%' AND [Current State]=0 ORDER BY [MAC Address];";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only
    row = 0;
    row = 0;
    while(!records.EOF)
        {
        currentState = parseInt(records("Current State"));

        html = "<TR><TD></TD>";
        html += "<TD CLASS=ROW" + row + "><A TITLE='Open Debugger' HREF=\"javascript:Run('%SystemRoot%/system32/cmd.exe /T:0A /K %_NTDRIVE%%_NTROOT%/public/tools/razzle.cmd REMOTE /C " + RTrim(records("Debugger")) + " " + RTrim(records("Machine Name")) + "')\">" + records("Machine Name") + "</A></TD>\n";
        html += "    <TD CLASS=ROW" + row + "><A HREF='mailto:" + RTrim(records("Owner")) + "'>"+records("Owner")+"</A></TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + records("Current SW Version") + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + records("Current ROM Version") + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + ">" + states[currentState] + "</TD>\n";
        html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editBox.asp?mac=" + records("MAC Address") + "'><IMG ALT='Edit / Details' SRC='images/edit.gif' BORDER=0></A></TD>\n";
        html += "    </TR>\n\n";

        Response.Write(html);
        records.MoveNext();
        row == 0 ? row = 1 : row = 0;
        }
    records.Close();
%>
</TABLE></BLOCKQUOTE>


<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
