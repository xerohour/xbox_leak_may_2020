<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    details.asp

Abstract:

    Display the results from the given build

Arguments:

    osbuild     [form, qs]  os build number to display
    hwbuild     [form, qs]  hw build number to display
    comp        [form, qs]  [optional] component name to display
    search      [form, qs]  [optional] searches component, subcomponent, function, 
                            variation, and logString for the value (*value*)
    where       [form, qs]  [optional] overrides the other optional switches
                            and defines the SQL WHERE clause

Author:

    Josh Poley (jpoley)

Revision History:
    04-04-2000  Created
    04-05-2000  Added the Edit button
    04-06-2000  Added the search and where commands

-->

<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    // sql settings
    var SQLQuery;
    var where = "((SystemConfig.osBuild='" + Request("osbuild") + "') AND (SystemConfig.hwBuild='" + Request("hwbuild") + "'))";
    if(Request("comp").Count > 0 && Request("comp") != "All")
        {
        where += " AND (Log.component='" + Request("comp") + "')";
        }
    if(Request("search").Count > 0 && Request("search") != "")
        {
        where += " AND ((Log.component LIKE '%" + Request("search") + "%') OR (Log.subcomponent LIKE '%" + Request("search") + "%') OR (Log.function LIKE '%" + Request("search") + "%') OR (Log.variation LIKE '%" + Request("search") + "%')  OR (Log.logString LIKE '%" + Request("search") + "%'))";
        }
    if(Request("where").Count > 0)
        {
        where = "((SystemConfig.osBuild='" + Request("osbuild") + "') AND (SystemConfig.hwBuild='" + Request("hwbuild") + "')) AND (" + Request("where") + ")";
        }

    //SQLQuery = "SELECT Log.component, Log.subcomponent, Log.function, Log.variation, Log.logLevel, Log.logString, Log.bugID, Log.tester, Log.logLevel, SystemConfig.boxID, Log.threadID, Log.testID, Log.timestamp FROM Log INNER JOIN SystemConfig ON Log.testID = SystemConfig.testID WHERE ("+where+") ORDER BY Log.component, Log.subcomponent, Log.function, Log.timestamp, Log.variation;";
    SQLQuery = "SELECT * FROM Log INNER JOIN SystemConfig ON Log.testID = SystemConfig.testID WHERE ("+where+") ORDER BY Log.component, Log.subcomponent, [function], Log.timestamp, Log.variation;";

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

    var component = "";
    var subcomponent = "";
    var functName = "";
    var fpass = 0;
    var ffail = 0;
    var spass = 0;
    var sfail = 0;
    var cpass = 0;
    var cfail = 0;
    var totalpass = 0;
    var totalfail = 0;
    var variations = "<TR><TD>Variation</TD><TD>Comment</TD><TD>BugID</TD><TD>Tester</TD><TD>Box ID</TD><TD></TD></TR>";
    var functions = "";
    var subcomponents = "";
    var row = 0;
    var id = 0;

    if(!records.EOF)
        {
        functName = ""+records("function");
        subcomponent = ""+records("subcomponent");
        component = (""+records("component")).toUpperCase();
        }

    function BuildOutput(f, s, c)
        {
        if(f || s || c)
            {
            total = ffail + fpass;
            if(total != 0)
                {
                ffail = Math.round(ffail*10000/total)/100;
                fpass = Math.round(fpass*10000/total)/100;
                }
            functions += "<TR CLASS=HAND onClick='ExpandCollapse(this, TBL"+id+");'><TH ID=TI"+id+">+</TH><TH ALIGN=LEFT>"+functName+"</TH><TH>"+ffail+"%</TH><TH>"+fpass+"%</TH><TH>"+total+"</TH></TR><TR STYLE='display:none' ID=TBL" + (id++) + "><TD></TD><TD COLSPAN=4><TABLE CELLSPACING=0 CELLPADDING=3 BORDER=0 STYLE='table-layout:fixed'><TR><TD WIDTH=110 HEIGHT=0></TD><TD WIDTH=270></TD><TD WIDTH=40></TD><TD WIDTH=80></TD><TD WIDTH=100></TD></TR>" + variations + "</TABLE></TD></TR>";
            variations = "<TR><TD>Variation</TD><TD>Comment</TD><TD>BugID</TD><TD>Tester</TD><TD>Box ID</TD><TD></TD></TR>";
            row = 0;
            fpass = 0;
            ffail = 0;
            functName = ""+records("function");
            }
        if(s || c)
            {
            total = sfail + spass;
            if(total != 0)
                {
                sfail = Math.round(sfail*10000/total)/100;
                spass = Math.round(spass*10000/total)/100;
                }
            subcomponents += "<TR CLASS=HAND onClick='ExpandCollapse(this, TBL"+id+");'><TH ID=TI"+id+">+</TH><TH ALIGN=LEFT>"+subcomponent+"</TH><TH>"+sfail+"%</TH><TH>"+spass+"%</TH><TH>"+total+"</TH></TR><TR STYLE='display:none' ID=TBL" + (id++) + "><TD></TD><TD COLSPAN=4><TABLE CELLSPACING=0 CELLPADDING=3 BORDER=0 STYLE='table-layout:fixed'><TR><TH WIDTH=9 HEIGHT=0></TH><TH WIDTH=490></TH><TH WIDTH=40></TH><TH WIDTH=40></TH><TH WIDTH=40></TH></TR>" + functions + "</TABLE></TD></TR>";
            spass = 0;
            sfail = 0;
            functions = "";
            subcomponent = ""+records("subcomponent");
            }
        if(c)
            {
            total = cfail + cpass;
            if(total != 0)
                {
                cfail = Math.round(cfail*10000/total)/100;
                cpass = Math.round(cpass*10000/total)/100;
                }
            Response.Write("<TR CLASS=HAND onClick='ExpandCollapse(this, TBL"+id+");'><TH ID=TI"+id+">+</TH><TH ALIGN=LEFT>");
            Response.Write(component);
            Response.Write("</TH><TH>"+cfail+"%</TH><TH>"+cpass+"%</TH><TH>"+total+"</TH></TR><TR STYLE='display:none' ID=TBL" + (id++) + "><TD></TD><TD COLSPAN=4><TABLE CELLSPACING=0 CELLPADDING=3 BORDER=0 STYLE='table-layout:fixed'><TR><TH WIDTH=9 HEIGHT=0></TH><TH WIDTH=500></TH><TH WIDTH=40></TH><TH WIDTH=40></TH><TH WIDTH=40></TH></TR>");
            Response.Write(subcomponents);
            Response.Write("</TABLE></TD></TR>");
            Response.Flush();
            cpass = 0;
            cfail = 0;
            component = (""+records("component")).toUpperCase();
            subcomponents = "";
            }
        }
%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE>Results for OS Build: <%=Request("osbuild")%> on HW Build: <%=Request("hwbuild")%></TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<SCRIPT>
function ExpandCollapse(h, o)
    {
    if(o.style.display == "none")
        {
        o.style.display = "block";
        h.cells(0).innerHTML = "-";
        }
    else
        {
        o.style.display = "none";
        h.cells(0).innerHTML = "+";
        }
    }

function ExpandAll()
    {
    for(var i=0; i < totalEntries; i++)
        {
        eval("TI" + i + ".innerHTML = '-';");
        eval("TBL" + i + ".style.display = 'block';");
        }
    }

function CollapseAll()
    {
    for(var i=0; i < totalEntries; i++)
        {
        eval("TI" + i + ".innerHTML = '+';");
        eval("TBL" + i + ".style.display = 'none';");
        }
    }

</SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2>Results for OS Build: <U><%=Request("osbuild")%></U> on HW Build: <U><%=Request("hwbuild")%></U></H2>

<%=Date()%> <P>&nbsp;<P>

<TABLE CELLSPACING=0 CELLPADDING=3 BORDER=0 STYLE="table-layout:fixed">
<!-- Column header Row -->
<TR><TH WIDTH=10 HEIGHT=0></TH><TH WIDTH=510></TH><TH WIDTH=40></TH><TH WIDTH=40></TH><TH WIDTH=40></TH></TR>
<TR><TH CLASS=ROWHEAD COLSPAN=2><IMG ALT="Expand All" BORDER=0 SRC="include/button2down.gif" onClick="ExpandAll();" STYLE="cursor:hand"> <IMG ALT="Collapse All" BORDER=0 SRC="include/button2up.gif" onClick="CollapseAll();" STYLE="cursor:hand"> Component / Subcomponent / Function</TH><TH CLASS=ROWHEAD>Fail</TH><TH CLASS=ROWHEAD>Pass</TH><TH CLASS=ROWHEAD>Total</TH></TR>

<!-- Data Rows -->
<%
while(!records.EOF)
    {
    BuildOutput(functName!=""+records("function"), subcomponent!=""+records("subcomponent"), component!=(""+records("component")).toUpperCase());

    // adjust counts
    if(records("logLevel") & XLL_EXCEPTION || records("logLevel") & XLL_BREAK || records("logLevel") & XLL_FAIL)
        {
        ++ffail;
        ++sfail;
        ++cfail;
        ++totalfail;
        color = "FF2222";
        }
    else if(records("logLevel") == XLL_PASS)
        {
        ++fpass;
        ++spass;
        ++cpass;
        ++totalpass;
        color = "00FF00";
        }
    else if(records("logLevel") == XLL_BLOCK)
        {
        color = "AAAA00";
        }
    else
        {
        color = "AAAA00";
        }

    comp = (""+records("component")).replace(/\s*$/, "");
    subcomp = (""+records("subcomponent")).replace(/\s*$/, "");
    funct = (""+records("function")).replace(/\s*$/, "");
    varn = (""+records("variation")).replace(/\s*$/, "");
    comment = (""+records("logString")).substr(0, 50);
    comment = comment.replace(/\s*$/, "");

    // add the item to the list
    //editlink = "edit.asp?testid="+records("testID")+"&component="+Server.URLEncode(comp)+"&subcomponent="+Server.URLEncode(subcomp)+"&function="+Server.URLEncode(funct)+"&variation="+Server.URLEncode(varn)+"&logString="+Server.URLEncode(comment);
    //variations += "<TR CLASS=ROW" + row + " STYLE='color="+color+"'><TD>" + varn + "</TD><TD>" + comment + "</TD><TD>" + records("bugID") + "</TD><TD><A HREF='mailto:"+(""+records("tester")).replace(/\s*$/, "")+"'>"+(""+records("tester")).replace(/\s*$/, "")+"</A></TD><TD><A HREF='testinfo.asp?boxid="+(""+records("boxID")).replace(/\s*$/, "")+"&testid=" + records("testID") + "' TARGET='_blank'>"+records("boxID")+"</A></TD><TD><A HREF='"+editlink+"' TARGET='_blank'><IMG SRC='include/edit.gif' BORDER=0></A></TD></TR>\r\n";
    variations += "<TR CLASS=ROW" + row + " STYLE='color="+color+"'><TD>" + varn + "</TD><TD>" + comment + "</TD><TD>" + records("bugID") + "</TD><TD><A HREF='mailto:"+(""+records("tester")).replace(/\s*$/, "")+"'>"+(""+records("tester")).replace(/\s*$/, "")+"</A></TD><TD>"+(""+records("boxID")).replace(/\s*$/, "")+"</TD></TR>\r\n";
    row = row?0:1;

    records.MoveNext();
    }

BuildOutput(true, true, true);

total = totalfail + totalpass;
if(total != 0)
    {
    totalfail = Math.round(totalfail*10000/total)/100;
    totalpass = Math.round(totalpass*10000/total)/100;
    }

records.Close();
%>

<!-- Column Footer Row -->
<TR><TH CLASS=ROWFOOT>&nbsp;</TH><TH ALIGN=RIGHT CLASS=ROWFOOT>Totals:</TH><TH CLASS=ROWFOOT><%=totalfail%>%</TH><TH CLASS=ROWFOOT><%=totalpass%>%</TH><TH CLASS=ROWFOOT><%=total%></TH></TR>

</TABLE>
</CENTER>


<SCRIPT>
var totalEntries = <%=id%>;
</SCRIPT>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->

<DIV ID=DEBUG></DIV>
</BODY>
</HTML>
