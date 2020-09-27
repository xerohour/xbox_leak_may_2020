<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    table.asp

Abstract:

    Display all rows from the given table

Arguments:

    required:
        table   [form, qs]  the name of the database table

    optional:
        where   [form, qs]  A SQL WHERE statement (minus the WHERE keyword)
        order   [form, qs]  A field name to sort by

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    // sql settings
    var SQLQuery = "SELECT * FROM [" + Request("table") + "]";

    if(Request("where").Count >= 1)
        SQLQuery += " WHERE (" +Request("where") + ")";

    if(Request("order").Count >= 1)
        SQLQuery += " ORDER BY " +Request("order") + "";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only

    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>"+Request("table")+"</SPAN></TD></TABLE>";
%>
<HTML>
<HEAD>
    <TITLE>HVS: <%=Request("table")%></TITLE>
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
