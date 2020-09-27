<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    editSchedule.asp

Abstract:

    

Arguments:

    required:
    jobid   ID of the job

    optional POST elements:
    del     set to "true" to delete the given jobid
    xyz     modifies the entry in the DB where the column name == xyz
            Only available in admin mode


Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Job Details</SPAN></TD></TABLE>";
    var row = 0;
    var html = "";

    // Delete a record?
    if(admin && Request("del") == "true")
        {
        var SQLQuery = "DELETE FROM [Scheduler] WHERE [Job ID]=" + Request("jobid") + ";";
        records.Open(SQLQuery, driver, 1, 2);
        Response.Redirect("scheduler.asp");
        }

    // sql settings
    var SQLQuery = "SELECT * FROM [Scheduler] WHERE [Job ID]=" + Request("jobid") + ";";

    // connect & get records
    if(admin) records.Open(SQLQuery, driver, 1, 2);
    else records.Open(SQLQuery, driver, 0, 1);

    if(admin && records.EOF)
        {
        records.AddNew();
        records("Job ID") = Request("jobid");
        records("Occurs On") = "";
        records("Last Result") = 0;
        records("Script") = "";
        records.Update();
        }

    if(!records.EOF)
        {
        if(admin && Request.Form.Count>1)
            {
            // -1 for the submit button
            for(i=0; i<Request.Form.Count-1; i++)
                {
                if(Request.Form(i+1) == "null")
                    ;
                else if(records.Fields(i).Type == 135)
                    {
                    var time = ""+Request.Form(i+1);
                    var m=time.match(/(\d*)\:(\d*) (.*)/);
                    if(m != null)
                        {
                        var d = new Date();
                        if(m[3].toUpperCase() == "AM") d.setHours(parseInt(m[1]));
                        else  d.setHours(parseInt(m[1])+12);
                        d.setMinutes(parseInt(m[2]));
                        records(i) = d.getVarDate();
                        }
                    }
                else
                    {
                    records(i) = Request.Form(i+1);
                    }
                }
            records.Update();

            if(Request("Submit") == "OK")
                {
                records.Close();
                Response.Redirect("scheduler.asp");
                }

            // if they changed the job id, we need to adjust the query string
            if(Request("Job ID") != Request("jobid"))
                {
                records.Close();
                Response.Redirect("editSchedule.asp?jobid=" + Request("Job ID"));
                }
            }
        }
%>
<HTML>
<HEAD>
    <TITLE>Job Details</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>


<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->
<FORM METHOD=POST ACTION="editSchedule.asp?jobid=<%=Request("jobid")%>">
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">
<%
if(records.EOF)
    {
    Response.Write("No Entry with this Job ID");
    }
else
    {
    for(i=0; i<records.Fields.Count; i++)
        {
        if(row) img = "in";
        else img = "out"
        html = "<TR><TD WIDTH=20><IMG SRC='images/littlecircle"+img+".gif'></TD><TH WIDTH=180 STYLE='cursor:default;'>" + records.Fields(i).Name + "</TH><TD WIDTH=180>";

        if(admin && i<4)
            html += "<INPUT STYLE='width:175' NAME='" + records.Fields(i).Name + "' TYPE=TEXT VALUE='";

        if(records.Fields(i).Name == "Time")
            html += FormatTime(records(i));
        else if(records.Fields(i).Type == 135)
            html += FormatShortDate(records(i));
        else
            html += records(i);

        if(admin && i<4)
            html += "'></TD></TR>";
        else
            html += "</TD></TR>";

        html += "<TR HEIGHT=2><TD></TD><TD></TD><TD></TD></TR>"
        Response.Write(html);
        row == 0 ? row = 1 : row = 0;
        }
    }
records.Close();
if(admin)
    {
    if(row) img = "in";
    else img = "out"
    html = "<TR><TD WIDTH=20><IMG SRC='images/littlecircle"+img+".gif'></TD><TH WIDTH=180 STYLE='cursor:default;'></TH><TD ALIGN=CENTER>";
    html += "<INPUT STYLE='width:50' ID=SUBMIT NAME=SUBMIT TYPE=SUBMIT VALUE='OK'> <INPUT STYLE='width:50' TYPE=BUTTON onClick='document.location=\"scheduler.asp\"' VALUE='Cancel'> <INPUT STYLE='width:50' ID=SUBMIT NAME=SUBMIT TYPE=SUBMIT VALUE='Apply'></TD></TR>";
    Response.Write(html);
    }
%>
</TABLE>
</FORM>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
