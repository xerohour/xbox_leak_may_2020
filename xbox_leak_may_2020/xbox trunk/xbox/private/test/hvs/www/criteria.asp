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

    // sql settings
    var SQLQuery = "SELECT * FROM [Acceptance Criteria] ORDER BY [Test Name]";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only

    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>HVS Test Criteria</SPAN></CENTER>";
%>
<HTML>
<HEAD>
    <TITLE>HVS Test Criteria</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/hvs.css">
</HEAD>

<BODY>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<P>
<CENTER>
<TABLE CELLPADDING=6 CELLSPACING=1 BORDER=0>

<!-- Column header Row -->
<TR>
<%
for(i=0; i<records.Fields.Count; i++)
    {
    Response.Write("<TH>" + records.Fields(i).Name + "</TH>")
    }
%>
</TR>





<!-- Data Rows -->
<% while(!records.EOF)
    { 
    Response.Write("<TR CLASS=ROW" + row + ">");
    for(i=0; i<records.Fields.Count; i++)
        {
        Response.Write("<TD NOWRAP>" + RTrim(records(i)) + "</TD>")
        }
    Response.Write("</TR>\r\n");
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
