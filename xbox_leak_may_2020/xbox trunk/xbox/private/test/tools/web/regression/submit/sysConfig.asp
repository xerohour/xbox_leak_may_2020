<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    sysConfig.asp

Abstract:

    Write a system configuration entry into the xbox os regression db

    Look at the Logging Spec for detailed information about this page.

Arguments:

    testID      [form, qs]  guid        Unique ID to group test cases to a single "run", this id also links entries in the different tables
    boxID       [form, qs]  char[10]    Name/id of the xbox test was run on
    macAddr     [form, qs]  char[13]    Network MAC address
    osBuild     [form, qs]  char[13]    os build (00.00.0000.00)
    hwBuild     [form, qs]  char[13]    hw build (00.00.0000.00)
    testIniVer  [form, qs]  char[10]    version of the test .ini file

    testID={98EBF3BA-04A9-11D1-8428-08002BE5FB88}&boxID=box123&macAddr=111111111111&osBuild=00.00.0000.00&hwBuild=00.00.0000.00&testIniVer=0

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
    var SQLQuery = "SELECT * FROM SystemConfig";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser", 2, 3);

    if(Request.QueryString.Count >= 1 || Request.Form.Count >= 1)
        {
        records.AddNew();
        if(Request("boxID").Count >= 1) records("boxID") = Request("boxID");
        if(Request("testID").Count >= 1) records("testID") = Request("testID");
        if(Request("macAddr").Count >= 1) records("macAddr") = Request("macAddr");
        if(Request("osBuild").Count >= 1) records("osBuild") = Request("osBuild");
        if(Request("hwBuild").Count >= 1) records("hwBuild") = Request("hwBuild");
        if(Request("testIniVer").Count >= 1) records("testIniVer") = Request("testIniVer");
        records.Update();
        }
    records.Close();
%>
<HTML>
<HEAD>
    <TITLE>System Configuration Submit</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="../include/regression.css">

<BODY>
<CENTER><H3>System Configuration Submit</H3>

<%=Date()%> <BR>
<P>

<TABLE CELLPADDING=4 CELLSPACING=1 BORDER=0>

<!-- Column header Row -->
<TR>
<TH>testID</TH>
<TH>boxID</TH>
<TH>macAddr</TH>
<TH>osBuild</TH>
<TH>hwBuild</TH>
<TH>testIniVer</TH>
</TR>

<!-- Data Rows -->
    <TR CLASS=ROW<%=row%>>
    <TD><%= Request("testID")%></TD>
    <TD><%= Request("boxID")%></TD>
    <TD><%= Request("macAddr")%></TD>
    <TD><%= Request("osBuild")%></TD>
    <TD><%= Request("hwBuild")%></TD>
    <TD><%= Request("testIniVer")%></TD>
    </TR>

<!-- Column Footer Row -->

</TABLE>
</CENTER>

</BODY>
</HTML>
