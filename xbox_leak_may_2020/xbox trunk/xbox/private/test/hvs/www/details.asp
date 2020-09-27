<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    details.asp

Abstract:

    Display all rows from the given query

Arguments:

    required:
        device  [form, qs]  the Hardware device tested
        mfg     [form, qs]  manufacturer of the hw device
        fw      [form, qs]  the firmware to display
        test    [form, qs]  test name

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 1;

    // sql settings
    var SQLQuery;

    SQLQuery = "SELECT [Timestamp], [Variation], [Status], [Number of Operations], [Box Information], [Config Settings], [Notes] FROM [Results] WHERE [Device Type] LIKE '"+Request("device")+"%' AND [Manufacturer] LIKE '"+Request("mfg")+"%' AND [Firmware Version] LIKE '"+Request("fw")+"%' AND [Test Name] LIKE '"+Request("test")+"%' ORDER BY [Status], [Number of Operations] DESC";

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
    <TR><TH WIDTH=120>Firmware</TH><TD><%=Request("fw")%></TD></TR>
    <TR><TH WIDTH=120>Test Name</TH><TD><%=Request("test")%></TD></TR>
</TD></TR>
</TABLE>

<P>

<CENTER>
<TABLE CELLPADDING=6 CELLSPACING=1 BORDER=0>

<TR><TH>Timestamp</TH><TH>Variation</TH><TH>Operations</TH></TR>

<!-- Data Rows -->
<% while(!records.EOF)
    { 
    Response.Write("<TR CLASS=ROW" + row + ">");
    Response.Write("<TD NOWRAP>" + FormatShortDate(records("Timestamp")) + "</TD>");
    Response.Write("<TD NOWRAP>" + RTrim(records("Variation")) + "</TD>");
    if(parseInt(records("Status")) == 0) color = "770000";
    else color = "000000";
    Response.Write("<TD ALIGN=RIGHT STYLE='color:"+color+"'>" + FormatNumberWCommas(records("Number of Operations")) + "</TD>");
    Response.Write("</TR>\r\n");
    Response.Write("<TR CLASS=ROW" + row + "><TD></TD><TD>");
    Response.Write("<TEXTAREA COLS=80 ROWS=1>" + records("Box Information") + "</TEXTAREA>");
    if(color != "000000")
        Response.Write("<BR><TEXTAREA COLS=80 ROWS=1>" + records("Notes") + "</TEXTAREA>");
    Response.Write("</TD><TD></TD></TR>\r\n");
    records.MoveNext();
    row = row?0:1;
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
