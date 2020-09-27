&nbsp;<BR>
&nbsp;<BR>
<A HREF="http://xboxlab/lab/default.asp">Lab Status</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/labView.asp">Lab View</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/byBuild.asp">By Build</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/byHWVer.asp">By HW Ver</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/myBoxes.asp">My Boxes</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/scheduler.asp">Scheduler</A><BR>
&nbsp;<A HREF="http://xboxlab/lab/areaOwners.asp">Area Owners</A><BR>
<%
if(admin)
    {
    Response.Write("<DIV STYLE='font:normal 3pt Arial;'>&nbsp;</DIV>\r\n");
    Response.Write("&nbsp;<A HREF='adminTools.asp'>Tools</A><BR>\r\n");
    Response.Write("&nbsp;<A HREF='table.asp?table=Admins&order=Users'>Admin Users</A><BR>\r\n");
    }
%>

&nbsp;<BR>
&nbsp;<BR>
<A>Tools & Reference</A><BR>
&nbsp;<A HREF="http://xboxlab/symbols.asp">Symbol Generator</A><BR>
&nbsp;<A HREF="\\xboxlab\public\RFC" TARGET=_blank>RFCs</A><BR>

&nbsp;<BR>
&nbsp;<BR>
<A HREF="http://xbox/default.asp">XBOX Home</A><BR>
&nbsp;<A HREF="http://xbox/XInternal/stress.htm">Stress How To</A><BR>
&nbsp;<A HREF="http://xbox/Team/contacts/">Contact List</A><BR>
&nbsp;<A HREF="http://xboxlab/regression/default.asp">Regression</A><BR>

&nbsp;<BR>
&nbsp;<BR>
<SELECT ID=CONTACTLIST SIZE=1 onchange="CONTACTLIST.selectedIndex==0?eval(';'):window.location='mailto:'+CONTACTLIST.options[CONTACTLIST.selectedIndex].value;">
    <OPTION VALUE='' SELECTED>Contact...
    <OPTION VALUE='xblds'>XBOX-Builds
    <OPTION VALUE='xstress'>Xbox Stress Alias
    <OPTION VALUE='jimvu'>Lab Manager
    <OPTION VALUE='danrose'>Daily Freak
    </SELECT><BR>

<INPUT TYPE=checkbox ID=AUTOUPDATE <%=Request("refresh")%> onclick="AutoUpdate();"><SPAN STYLE="color:111111">Auto Refresh</SPAN>
<SCRIPT>AutoUpdate();</SCRIPT>


<P>

<CENTER>
<DIV STYLE='font:normal 8pt Arial;color:#777777'>
Last Updated At<BR>
<%
    var SQLQuery  = "SELECT * FROM Properties WHERE Property='dbgmonLastRun';";
    var indexRecords = Server.CreateObject("ADODB.Recordset");
    indexRecords.Open(SQLQuery, driver, 0, 1);
    if(indexRecords.EOF) Response.Write("Never");
    else Response.Write(indexRecords("Value"));
    indexRecords.Close();
%>
</DIV>
</CENTER>
<P>
<DIV STYLE="font:normal 6pt Courier" ID=DEBUG></DIV>
