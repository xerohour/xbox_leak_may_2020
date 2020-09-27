<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    summary.asp

Abstract:

    Display how many tests passed and failed on each build.

Arguments:

    none

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
    var SQLQuery = "SELECT SystemConfig.osBuild, SystemConfig.hwBuild, Log.logLevel, Count(Log.logLevel) AS NumEntries FROM SystemConfig INNER JOIN Log ON SystemConfig.testID = Log.testID GROUP BY SystemConfig.osBuild, SystemConfig.hwBuild, Log.logLevel ORDER BY SystemConfig.osBuild DESC, SystemConfig.hwBuild DESC;";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=dxregression;UID=dxregression;PWD=dxregression.user");

    var XLL_EXCEPTION =            0x00000001;  // Enables logging of exception events
    var XLL_BREAK     =            0x00000002;  // Enables logging to break into the debugger
    var XLL_FAIL      =            0x00000004;  // Enables logging of failure events
    var XLL_WARN      =            0x00000008;  // Enables logging of warning events
    var XLL_BLOCK     =            0x00000010;  // Enables logging of blocking events
    var XLL_PASS      =            0x00000020;  // Enables logging of pass events
    var XLL_VARIATION =            0x00000040;  // Enables logging of variation start/end events
    var XLL_INFO      =            0x00000080;  // Enables logging of informational events

    var osbuild = "";
    var hwbuild = "";
    var fail = 0;
    var pass = 0;
    var blocked = 0;
    var total = 0;

    function BuildOutput()
        {
        Response.Write("<TR CLASS=ROW" + row + "><FORM METHOD=POST ACTION='details.asp?osbuild="+osbuild+"&hwbuild="+hwbuild+"'>");
        total = fail + pass + blocked;
        if(total != 0)
            {
            fail = Math.round(fail*10000/total)/100;
            pass = Math.round(pass*10000/total)/100;
            blocked = Math.round(blocked*10000/total)/100;
            }
        Response.Write("<TD>" + osbuild + "</TD><TD>" + hwbuild + "</TD>");
        Response.Write("<TD ALIGN=RIGHT>" + fail + "%</TD><TD ALIGN=RIGHT>" + pass + "%</TD><TD ALIGN=RIGHT>" + blocked + "%</TD>");
        Response.Write("<TD ALIGN=RIGHT>" + total + "</TD>");

        var SQLQuery = "SELECT Log.component FROM Log INNER JOIN SystemConfig ON Log.testID = SystemConfig.testID GROUP BY SystemConfig.osBuild, SystemConfig.hwBuild, Log.component HAVING (((SystemConfig.osBuild)='"+osbuild+"') AND ((SystemConfig.hwBuild)='"+hwbuild+"')) ORDER BY Log.component;";
        // connect & get records
        var records2 = Server.CreateObject("ADODB.Recordset");
        records2.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser");
        Response.Write("<TD><SELECT ALIGN=CENTER STYLE='width:100' SIZE=1 NAME='comp'><OPTION>All</OPTION>");
        while(!records2.EOF)
            {
            Response.Write("<OPTION>" + records2("component") + "</OPTION>");
            records2.MoveNext();
            }
        Response.Write("</SELECT></TD>");
        records2.Close();

        Response.Write("<TD><INPUT ALIGN=CENTER NAME=SEARCH STYLE='width:80'></TD><TD ALIGN=CENTER><INPUT TYPE=SUBMIT VALUE='Go' STYLE='width:30'></TD>");

        Response.Write("</FORM></TR>");
        row = row?0:1;
        Response.Flush();
        }
%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE>xBox Regression Summary</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' onClick="c();" STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2>xBox Regression Summary</H2>

<%=Date()%> <P>&nbsp;<P>

<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TH CLASS=ROWHEAD WIDTH=90>OS Build</TH>
<TH CLASS=ROWHEAD WIDTH=90>HW Build</TH>
<TH CLASS=ROWHEAD WIDTH=50 STYLE="text-align:right">Fail</TH>
<TH CLASS=ROWHEAD WIDTH=50 STYLE="text-align:right">Pass</TH>
<TH CLASS=ROWHEAD WIDTH=50 STYLE="text-align:right">Blocked</TH>
<TH CLASS=ROWHEAD WIDTH=50 STYLE="text-align:right">Total</TH>
<TH CLASS=ROWHEAD WIDTH=120 STYLE="text-align:center">Component</TH>
<TH CLASS=ROWHEAD WIDTH=100 STYLE="text-align:center">Full Search</TH>
<TH CLASS=ROWHEAD WIDTH=50 STYLE="text-align:center">Details</TH>
</TR>

<!-- Data Rows -->
<% while(!records.EOF)
    {
    if(records("osBuild") != osbuild || records("hwBuild") != hwbuild)
        {
        if(osbuild != "")
            {
            BuildOutput();
            }
        fail = 0;
        pass = 0;
        blocked = 0;
        osbuild = "" + records("osBuild");
        hwbuild = "" + records("hwBuild");
        }
    if(records("logLevel") & XLL_EXCEPTION || records("logLevel") & XLL_BREAK || records("logLevel") & XLL_FAIL) fail += parseInt(records("NumEntries"));
    else if(records("logLevel") == XLL_PASS) pass += parseInt(records("NumEntries"));
    else if(records("logLevel") == XLL_BLOCK) blocked += parseInt(records("NumEntries"));

    records.MoveNext();
    }

BuildOutput();

records.Close();
%>

<!-- Column Footer Row -->
</TABLE>
<DIV ID=COOL STYLE="position:absolute"></DIV>
</CENTER>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
