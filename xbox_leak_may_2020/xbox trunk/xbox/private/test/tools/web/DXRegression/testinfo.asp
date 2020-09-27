<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    testinfo.asp

Abstract:

    Display the configuration and state information about the given box for 
    the specified test.

Arguments:

    boxid       [form, qs]  name of the box
    testid      [form, qs]  the associated test id

Author:

    Josh Poley (jpoley)

Revision History:
    04-04-2000  Created

-->

<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    // sql settings
    var SQLQuery = "SELECT SystemConfig.testID, SystemConfig.boxID, SystemConfig.*, SystemState.* FROM SystemConfig LEFT JOIN SystemState ON SystemConfig.testID = SystemState.testID WHERE (((SystemConfig.testID)='"+Request("testid")+"') AND ((SystemConfig.boxID)='"+Request("boxid")+"'));";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=dxregression;UID=dxregression;PWD=dxregression.user");

%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE>Test Info for <%=Request("boxid")%></TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2>Test Info for <U><%=Request("boxid")%></U></H2>Test <%=Request("testid")%><P>

<%=Date()%> <P>&nbsp;<P>

<TABLE CELLPADDING=4 CELLSPACING=0 BORDER=0>

<!-- Column header Row -->
<TR><TH CLASS=ROWHEAD>Property</TH><TH CLASS=ROWHEAD>Value</TH></TR>

<!-- Data Rows -->
<%
for(i=0; i<records.Fields.Count; i++)
    {
    if(records.Fields(i).Name == "testID" || records.Fields(i).Name == "timestamp" || records.Fields(i).Name == "boxID")
        continue;

    Response.Write("<TR><TH>" + records.Fields(i).Name + "</TH><TD>" + records(i) + "</TD></TR>");
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
