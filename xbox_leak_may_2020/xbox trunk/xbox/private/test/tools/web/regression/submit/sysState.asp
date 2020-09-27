<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    sysState.asp

Abstract:

    Write a system state entry into the xbox os regression db

    Look at the Logging Spec for detailed information about this page.

Arguments:

    testID          [form, qs]  guid        Unique ID to group test cases to a single "run", this id also links entries in the different tables
    boxID           [form, qs]  char[10]    Name/id of the xbox test was run on
    diskBytesAvail  [form, qs]  numeric     disk space available
    memBytesAvail   [form, qs]  numeric     memory available

    testID={98EBF3BA-04A9-11D1-8428-08002BE5FB88}&boxID=box123&diskBytesAvail=6543213&memBytesAvail=6541116877

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
    var SQLQuery = "SELECT * FROM SystemState";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser", 2, 3);

    if(Request.QueryString.Count >= 1 || Request.Form.Count >= 1)
        {
        records.AddNew();
        if(Request("boxID").Count >= 1) records("boxID") = Request("boxID");
        if(Request("testID").Count >= 1) records("testID") = Request("testID");
        if(Request("diskBytesAvail").Count >= 1) records("diskBytesAvail") = Request("diskBytesAvail");
        if(Request("memBytesAvail").Count >= 1) records("memBytesAvail") = Request("memBytesAvail");
        records.Update();
        }
    records.Close();
%>
<HTML>
<HEAD>
    <TITLE>System State Submit</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="../include/regression.css">

<BODY>
<CENTER><H3>System State Submit</H3>

<%=Date()%> <BR>
<P>

<TABLE CELLPADDING=4 CELLSPACING=1 BORDER=0>

<!-- Column header Row -->
<TR>
<TH>testID</TH>
<TH>boxID</TH>
<TH>diskBytesAvail</TH>
<TH>memBytesAvail</TH>
</TR>

<!-- Data Rows -->
    <TR CLASS=ROW<%=row%>>
    <TD><%= Request("testID")%></TD>
    <TD><%= Request("boxID")%></TD>
    <TD><%= Request("diskBytesAvail")%></TD>
    <TD><%= Request("memBytesAvail")%></TD>
    </TR>

<!-- Column Footer Row -->

</TABLE>
</CENTER>

</BODY>
</HTML>
