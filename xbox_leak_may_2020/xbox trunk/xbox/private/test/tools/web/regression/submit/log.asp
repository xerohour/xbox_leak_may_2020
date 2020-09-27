<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    log.asp

Abstract:

    Write a new log entry into the xbox os regression db

    Look at the Logging Spec for detailed information about this page.

Arguments:

    testID      [form, qs]  guid        Unique ID to group test cases to a single "run", this id also links entries in the different tables
    bugID       [form, qs]  int         RAID bug ID (if one exists)
    boxID       [form, qs]  char[10]    Name/id of the xbox test was run on
    threadID    [form, qs]  int         ID of the thread test was run from
    logLevel    [form, qs]  int         the logging level of this entry (0=exception, 2=break, 4=fail, 8=warning, 16=block, 32=pass, 64=variation, 128=info)
    component   [form, qs]  char[32]    component name
    subcomponent[form, qs]  char[32]    subcomponent name
    function    [form, qs]  char[64]    function name
    variation   [form, qs]  char[32]    test variation name
    logString   [form, qs]  char[255]   log comments
    tester      [form, qs]  char[16]    tester alias

    testID={98EBF3BA-04A9-11D1-8428-08002BE5FB88}&threadID=3&logLevel=1&variation=variation1&function=function1&component=kernel&subcomponent=dvd&logString=comment+comment+comment&tester=jpoley&bugID=0

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
    var SQLQuery = "SELECT * FROM Log";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser", 2, 3);

    var skip = false;

    // skip 'variation' entries
    if(Request("logLevel").Count >= 1)
        {
        var level = parseInt(Request("logLevel"));
        if(level >= 128) skip = true;
        }

    if((Request.QueryString.Count >= 1 || Request.Form.Count >= 1) && skip==false)
        {
        records.AddNew();
        //if(Request("boxID").Count >= 1) records("boxID") = Request("boxID");
        if(Request("testID").Count >= 1) records("testID") = Request("testID");
        if(Request("threadID").Count >= 1) records("threadID") = Request("threadID");
        if(Request("logLevel").Count >= 1) records("logLevel") = Request("logLevel");
        if(Request("component").Count >= 1) records("component") = Request("component");
        if(Request("subcomponent").Count >= 1) records("subcomponent") = Request("subcomponent");
        if(Request("function").Count >= 1) records("function") = Request("function");
        if(Request("variation").Count >= 1) records("variation") = Request("variation");
        if(Request("logString").Count >= 1) records("logString") = Request("logString");
        if(Request("tester").Count >= 1) records("tester") = Request("tester");
        if(Request("bugID").Count >= 1) records("bugID") = Request("bugID");
        records.Update();
        }
    records.Close();
%>
<HTML>
<HEAD>
    <TITLE>Log Entry Submit</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="../include/regression.css">

<BODY>
<CENTER><H3>Log Entry Submit</H3>

<%=Date()%> <BR>
<P>

<TABLE CELLPADDING=4 CELLSPACING=1 BORDER=0>

<!-- Column header Row -->
<TR>
<TH>testID</TH>
<TH>bugID</TH>
<TH>boxID</TH>
<TH>threadID</TH>
<TH>logLevel</TH>
<TH>varName</TH>
<TH>functName</TH>
<TH>component</TH>
<TH>subcomponent</TH>
<TH>tester</TH>
<TH WIDTH=250>Comments</TH>
</TR>

<!-- Data Rows -->
    <TR CLASS=ROW<%=row%>>
    <TD><%= Request("testID")%></TD>
    <TD><%= Request("bugID")%></TD>
    <TD><%= Request("boxID")%></TD>
    <TD><%= Request("threadID")%></TD>
    <TD><%= Request("logLevel")%></TD>
    <TD><%= Request("varName")%></TD>
    <TD><%= Request("functName")%></TD>
    <TD><%= Request("component")%></TD>
    <TD><%= Request("subcomponent")%></TD>
    <TD><A HREF="mailto:<%= Request("tester")%>"><%= Request("tester")%></A></TD>
    <TD><%= Request("logString")%></TD>
    </TR>

<!-- Column Footer Row -->

</TABLE>
</CENTER>

</BODY>
</HTML>
