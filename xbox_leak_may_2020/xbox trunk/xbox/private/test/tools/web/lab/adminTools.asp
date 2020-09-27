<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    adminTools.asp

Abstract:

    Displays the current status of the lab machines

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

    if(!admin) Response.Redirect("default.asp");

    // sql settings
    var SQLQuery;

    if(Request("stopdbgmon").Count >= 1)
        {
        SQLQuery  = "SELECT * FROM Properties WHERE Property='dbgmonStop';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = "1";
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        }

    if(Request("stopdbgmon") == "Stop")
        {
        SQLQuery  = "SELECT * FROM Properties WHERE Property='dbgmonStop';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = "1";
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        }
    else if(Request("stopdbgmon") == "Start")
        {
        /* TODO
        SQLQuery  = "SELECT * FROM Properties WHERE Property='dbgmonStop';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = "1";
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        */
        }
    else if(Request("stopscheduler") == "Stop")
        {
        SQLQuery  = "SELECT * FROM Properties WHERE Property='schedulerStop';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = "1";
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        }
    else if(Request("stopscheduler") == "Start")
        {
        /* TODO
        SQLQuery  = "SELECT * FROM Properties WHERE Property='schedulerStop';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = "1";
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        */
        }
    else if(Request("dbgmonDelay").Count >= 1)
        {
        SQLQuery  = "SELECT * FROM Properties WHERE Property='dbgmonDelay';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = ""+Request("dbgmonDelay");
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        }
    else if(Request("notifierDbgEmail").Count >= 1)
        {
        SQLQuery  = "SELECT * FROM Properties WHERE Property='notifierDbgEmail';";
        records.Open(SQLQuery, driver, 1, 2);
        records("Value") = ""+Request("notifierDbgEmail");
        records.Update();
        records.Close();
        Response.Redirect("adminTools.asp"); // clean the query string
        }
    else if(Request("purgeBoxName").Count >= 1)
        {
        SQLQuery  = "DELETE FROM BoxList WHERE [MAC Address]='"+Request("purgeBoxName")+"';";
        records.Open(SQLQuery, driver, 1, 2);
        SQLQuery  = "DELETE FROM TestHistory WHERE [MAC Address]='"+Request("purgeBoxName")+"';";
        records.Open(SQLQuery, driver, 1, 2);
        Response.Redirect("adminTools.asp"); // clean the query string
        }

    SQLQuery  = "SELECT * FROM Properties;";
    records.Open(SQLQuery, driver, 1, 1);

    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Admin Tools</SPAN></TD></TABLE>";
%>
<HTML>
<HEAD>
    <TITLE>XBOX Lab</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<H3 STYLE='border-top:1px solid #7CB021'>Timestamps</H3>
    <BLOCKQUOTE>
    <TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">
        <TR><TD WIDTH=90></TD><TD WIDTH=120></TD><TD WIDTH=55></TD><TD WIDTH=260></TD></TR>

        <%records.MoveFirst(); records.Find("Property LIKE 'schedulerLastRun%'");%>
        <TR><TH STYLE='cursor:default;'>Scheduler</TH><TD><%=records("Value")%></TD><TD></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>

        <%records.MoveFirst(); records.Find("Property LIKE 'dbgmonLastRun%'");%>
        <TR><TH STYLE='cursor:default;'>Monitor</TH><TD><%=records("Value")%></TD><TD></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>

        <%records.MoveFirst(); records.Find("Property LIKE 'notifierLastRun%'");%>
        <TR><TH STYLE='cursor:default;'>Email</TH><TD><%=records("Value")%></TD><TD></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>

        </TABLE>
    </BLOCKQUOTE>

<H3 STYLE='border-top:1px solid #7CB021'>Properties</H3>
    <BLOCKQUOTE>
    <TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">
        <TR><TD WIDTH=90></TD><TD WIDTH=120></TD><TD WIDTH=55></TD><TD WIDTH=260></TD></TR>

        <FORM METHOD=POST ACTION="adminTools.asp">
        <%records.MoveFirst(); records.Find("Property LIKE 'schedulerStop%'");%>
        <TR><TH STYLE='cursor:default;'>Scheduler is</TH><TD><%if(""+records("Value")=="1") Response.Write("Not Running"); else Response.Write("Running")%></TD><TD><INPUT TYPE=SUBMIT NAME='stopscheduler' VALUE='<%if(""+records("Value")=="1") Response.Write("Start"); else Response.Write("Stop")%>' STYLE='width:48'></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>
        </FORM>

        <FORM METHOD=POST ACTION="adminTools.asp">
        <%records.MoveFirst(); records.Find("Property LIKE 'dbgmonStop%'");%>
        <TR><TH STYLE='cursor:default;'>Lab Monitor is</TH><TD><%if(""+records("Value")=="1") Response.Write("Not Running"); else Response.Write("Running")%></TD><TD><INPUT TYPE=SUBMIT NAME='stopdbgmon' VALUE='<%if(""+records("Value")=="1") Response.Write("Start"); else Response.Write("Stop")%>' STYLE='width:48'></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>
        </FORM>

        <FORM METHOD=POST ACTION="adminTools.asp">
        <%records.MoveFirst(); records.Find("Property LIKE 'dbgmonDelay%'");%>
        <TR><TH STYLE='cursor:default;'>Lab Monitor Delay</TH><TD><INPUT STYLE='width:95' NAME='dbgmonDelay' TYPE=TEXT VALUE='<%=RTrim(records("Value"))%>'> </TD><TD><INPUT TYPE=SUBMIT VALUE='Update' STYLE='width:48'></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>
        </FORM>

        <FORM METHOD=POST ACTION="adminTools.asp">
        <%records.MoveFirst(); records.Find("Property LIKE 'notifierDbgEmail%'");%>
        <TR><TH STYLE='cursor:default;'>Email Debug Alias</TH><TD><INPUT STYLE='width:95' NAME='notifierDbgEmail' TYPE=TEXT VALUE='<%=RTrim(records("Value"))%>'> </TD><TD><INPUT TYPE=SUBMIT VALUE='Update' STYLE='width:48'></TD><TD><%=records("Comments")%></TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>
        </FORM>

        </TABLE>
    </BLOCKQUOTE>


<H3 STYLE='border-top:1px solid #7CB021'>Tools</H3>
    <BLOCKQUOTE>
    <TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">
        <TR><TD WIDTH=90></TD><TD WIDTH=120></TD><TD WIDTH=55></TD><TD WIDTH=260></TD></TR>

        <FORM METHOD=POST ACTION="adminTools.asp">
        <TR><TH STYLE='cursor:default;'>Purge Box</TH><TD><INPUT STYLE='width:95' NAME='purgeBoxName' TYPE=TEXT VALUE=''></TD><TD><INPUT TYPE=SUBMIT VALUE='Delete' STYLE='width:48'></TD><TD>Enter the MAC Address of a box to purge from the DB</TD></TR>
        <TR HEIGHT=2><TD></TD><TD></TD><TD></TD><TD></TD></TR>
        </FORM>

        </TABLE>
    </BLOCKQUOTE>

<%
records.Close();
%>
<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
