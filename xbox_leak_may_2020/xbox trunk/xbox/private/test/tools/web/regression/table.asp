<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    table.asp

Abstract:

    Display all rows from the given table

Arguments:

    table   [form, qs]  the name of the database table

Author:

    Josh Poley (jpoley)

Revision History:
    04-03-2000  Created

-->

<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    // sql settings
    var SQLQuery = "SELECT * FROM " + Request("table");

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser");
%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE><%=Request("table")%></TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2><%=Request("table")%></H2>

<%=Date()%> <P>&nbsp;<P>

<TABLE CELLPADDING=4 CELLSPACING=1 BORDER=0>

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
        Response.Write("<TD>" + records(i) + "</TD>")
        }
    Response.Write("</TR>");
    records.MoveNext();
    row = row?0:1;
    }

records.Close();
%>

<!-- Column Footer Row -->

</TABLE>
</CENTER>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->

</BODY>
</HTML>
