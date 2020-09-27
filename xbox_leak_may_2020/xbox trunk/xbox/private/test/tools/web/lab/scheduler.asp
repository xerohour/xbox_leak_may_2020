<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    scheduler.asp

Abstract:

    Displays the current job schedule list

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
    var lastJob = 0;

    // sql settings
    var SQLQuery = "SELECT * FROM Scheduler ORDER BY [Job ID];";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only

    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Lab Scheduler</SPAN></TD></TABLE>";
%>
<HTML>
<HEAD>
    <TITLE>XBOX Lab Scheduler</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<FORM>
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=35>Job</TH>
<TH CLASS=ROWHEAD WIDTH=60>Time</TH>
<TH CLASS=ROWHEAD WIDTH=50>Days</TH>
<TH CLASS=ROWHEAD WIDTH=200>Script</TH>
<TH CLASS=ROWHEAD WIDTH=115>Last Run On</TH>
<TH CLASS=ROWHEAD WIDTH=55 ALIGN=RIGHT>Result</TH>
<%
if(admin)
    {
    Response.Write("<TH CLASS=ROWHEAD WIDTH=28>Edit</TH>");
    Response.Write("<TH CLASS=ROWHEAD WIDTH=28>Del</TH>");
    }
%>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>

<!-- Data Rows -->
<%
while(!records.EOF)
    {
    lastJob = parseInt(""+records("Job ID"));
    html = "<TR><TD></TD>";
    html += "<TD CLASS=ROW" + row + ">" + records("Job ID") + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=RIGHT>" + FormatTime(records("Time")) + "&nbsp;</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + ">" + records("Occurs On") + "</TD>\r\n";
    if(admin) 
        html += "    <TD CLASS=ROW" + row + "><A TITLE='Run Now' HREF='javascript:ExecuteJob(" + records("Job ID") + ")'>" + records("Script") + "</A></TD>\r\n";
    else 
        html += "    <TD CLASS=ROW" + row + ">" + records("Script") + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + ">" + FormatShortDate(records("Last Run On")) + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=RIGHT>" + records("Last Result") + "</TD>\r\n";
    if(admin)
        {
        html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editSchedule.asp?jobid=" +records("Job ID")+ "'><IMG ALT='Edit' BORDER=0 SRC='images/edit.gif'></A></TD>\r\n";
        html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editSchedule.asp?del=true&jobid=" +records("Job ID")+ "'><IMG ALT='Delete' BORDER=0 SRC='images/trash.gif'></A></TD>\r\n";
        }
    html += "    <TD></TD></TR>\r\n\r\n";

    Response.Write(html);
    records.MoveNext();
    row == 0 ? row = 1 : row = 0;
    }
records.Close();

if(admin)
    {
    html = "<TR><TD></TD>";
    html += "<TD CLASS=ROW" + row + ">" + (lastJob+1) + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=RIGHT>&nbsp;</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + "></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + "><CENTER>Add New Job</CENTER></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + "></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=RIGHT></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editSchedule.asp?jobid=" +(lastJob+1)+ "'><IMG ALT='Add New' BORDER=0 SRC='images/edit.gif'></A></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + "></TD><TD></TD></TR>\r\n\r\n";
    Response.Write(html);
    }
%>
<!-- Column footer Row -->
</TABLE></FORM>


<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
