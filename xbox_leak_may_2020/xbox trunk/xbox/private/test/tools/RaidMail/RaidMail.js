/*
Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    RaidMail.js

Abstract:

    Raid email reporter

Author:

    Josh Poley (jpoley)

Revision History:
    04-11-2000  Created
    04-12-2000  Moved most configuration settings to the xml file
    04-20-2000  Added the summary report

Notes:
    at 11:00am /every:m,t,w,th,f C:\WINNT\system32\cscript.exe d:\raidmail\raidmail.js

    This script requires that an SMTP service be running. It is easiest to
    have it forward mail to the smarthost server.

*/
var xml = WScript.CreateObject("microsoft.xmldom");
xml.async = false;
xml.load(GetPath() + "\\RaidMailCfg.xml");

var numEmailsSent = 0;
var debug = false;
var results = new Array;
var nugget = "<SPAN STYLE='display:none'>"+Math.random()+"</SPAN>";

function Summary(al, a, r)
    {
    this.alias = al;
    this.active = a;
    this.resolved = r;
    }



// Run the report generator
RaidMail(xml.getElementsByTagName("SQL").item(0).text, xml.getElementsByTagName("Database").item(0).text);

// Generate the summary
BuildSummary();

WScript.Echo("Sent " + numEmailsSent + " Reports");


function SortSummary(o1, o2)
    {
    // descending sort on total
    if((o1.active + o1.resolved) < (o2.active + o2.resolved)) return 1;
    if((o1.active + o1.resolved) > (o2.active + o2.resolved)) return -1;

    // sort on alias
    if(o1.alias < o2.alias) return -1;
    if(o1.alias > o2.alias) return 1;

    // sort on # active
    if(o1.active < o2.active) return -1;
    if(o1.active > o2.active) return 1;

    return 0;
    }

function BuildSummary()
    {
    var emailHeader = ""+xml.getElementsByTagName("StyleSheet").item(0).text + ""+xml.getElementsByTagName("SummaryHeader").item(0).text + "<TABLE CELLPADDING=4 WIDTH=320>\r\n" +
        "<TR><TH>Assigned To</TH><TH>#<BR>Active</TH><TH>#<BR>Resolved</TH><TH>Total</TH></TR>\r\n";
    var emailFooter = "</TABLE>\r\n" + xml.getElementsByTagName("SummaryFooter").item(0).text;
    var body = emailHeader;

    results.sort(SortSummary);

    for(i in results)
        {
        body += "<TR><TD><A HREF='mailto:" + results[i].alias + "&subject=You have too many bugs'>" + results[i].alias + "</A></TD><TD>" + results[i].active + "</TD><TD>" + results[i].resolved + "</TD><TD>" + (results[i].active+results[i].resolved) + "</TD></TR>";
        }
    SendMail("jpoley", xml.getElementsByTagName("Subject").item(0).text + " Summary", body + emailFooter, "Raid Mail");
    }

/*

Routine Description:

    RaidMail

    Sends email using SMTP (this service must be enabled)

Arguments:

    string  sql         SQL statement of the bugs to retreive
    string  dbc         database connection string

Return Value:

    none

Notes:


*/
function RaidMail(sql, dbc)
    {
    // Note: If you get an "Object Required" error here, it is most likely
    //      because the configuration xml is invalid.
    var emailSubject = ""+xml.getElementsByTagName("Subject").item(0).text;
    var emailHeader = ""+xml.getElementsByTagName("Header").item(0).text;
    var emailFooter = ""+xml.getElementsByTagName("Footer").item(0).text;
    var styleSheet = ""+xml.getElementsByTagName("StyleSheet").item(0).text;
    emailHeader = styleSheet + emailHeader + "<TABLE CELLSPACING=0 STYLE='table-layout:fixed'>\r\n";
    emailHeader += "<TR><TH WIDTH=40 TITLE='Bug ID'>Bug ID</TH><TH WIDTH=480 TITLE='Title'>Title</TH></TR>\r\n";
    emailHeader += "<TR><TH></TH><TH><TABLE CELLSPACING=0 STYLE='table-layout:fixed'><TR><TH WIDTH=40 TITLE='Priority'>Pri</TH><TH WIDTH=40 TITLE='Severity'>Sev</TH><TH WIDTH=400 TITLE='Component'>Component: Sub Component</TH></TR></TABLE></TH></TR>\r\n";
    emailHeader += "<TR><TH></TH><TH><TABLE CELLSPACING=0 STYLE='table-layout:fixed'><TR><TH WIDTH=80 TITLE='Milestone'>Milestone</TH><TH WIDTH=80 TITLE='Opened By'>Opened By</TH><TH WIDTH=320></TH></TR></TABLE></TH></TR>\r\n";
    emailFooter = "</TABLE>\r\n" + emailFooter;

    // connect & get records
    var records = WScript.CreateObject("ADODB.Recordset");
    records.Open(sql, dbc);

    var body = emailHeader;
    var alias = "null";
    var row = 0;
    var active = 0;
    var resolved = 0;
    var color = "";

    while(!records.EOF)
        {
        if((""+records("AssignedTo")).toLowerCase() != alias)
            {
            if(alias != "null")
                {
                // Send the report
                SendMail(alias, emailSubject + " (" + (active+resolved) + " bugs)", body + emailFooter, "Raid Mail");
                results[results.length] = new Summary(alias, active, resolved);
                }
            row = 0;
            active = 0;
            resolved = 0;
            alias = (""+records("AssignedTo")).toLowerCase();
            body = emailHeader;
            }

        if((""+records("Status")) == "Active")
            {
            ++active;
            color = "#CC0000";
            }
        else
            {
            ++resolved;
            color = "#AAAA00";
            }

        body += "<TR VALIGN=TOP CLASS=ROW" + row + "><TD WIDTH=40 TITLE='Bug ID'><A HREF='http://xboxlab/raid/bugid.rdq?bugid="+ records("BugID") +"' STYLE='color:" + color + "'>" + records("BugID") + "</A></TD><TD WIDTH=480 TITLE='Title'><B>" + records("Title") + "</B></TD></TR>\r\n";
        body += "<TR VALIGN=TOP CLASS=ROW" + row + "><TD></TD><TD><TABLE CELLSPACING=0 STYLE='table-layout:fixed'><TR VALIGN=TOP><TD WIDTH=40 TITLE='Priority'>" + records("Priority") + "</TD><TD WIDTH=40 TITLE='Severity'>" + records("Severity") + "</TD><TD WIDTH=400 TITLE='Component'>" + records("Component") + ": " + records("SubComp") + "</TD></TR></TABLE></TD></TR>\r\n";
        body += "<TR VALIGN=TOP CLASS=ROW" + row + "><TD></TD><TD><TABLE CELLSPACING=0 STYLE='table-layout:fixed'><TR VALIGN=TOP><TD WIDTH=80 TITLE='Milestone'>" + records("Fix_By") + "</TD><TD WIDTH=80 TITLE='Opened By'><A HREF='mailto:" + records("OpenedBy") + "'>" + records("OpenedBy") + "</A></TD><TD WIDTH=320></TD></TR></TABLE></TD></TR>\r\n";
        row = row?0:1;
        records.MoveNext();
        }

    // Send the last person's report
    if(alias != "null")
        {
        SendMail(alias, emailSubject + " (" + (active+resolved) + " bugs)", body + emailFooter, "Raid Mail");
        results[results.length] = new Summary(alias, active, resolved);
        }

    records.Close();
    }


/*

Routine Description:

    SendMail

    Sends email using SMTP (this service must be enabled)

Arguments:

    string  alias       the email address
    string  subject     subject line for the email
    string  body        the email's message

Return Value:

    none

Notes:


*/
function SendMail(alias, subject, body, headerval)
    {
    var from = "xboxlab" + parseInt(Math.random()*1000) + "@microsoft.com";
    var cdonts = WScript.CreateObject("CDONTS.NewMail");

    // Check to see if the email should be sent to someone else
    alias = VerifyAlias(alias, xml);

    if(alias == "NoMail") return;
    if(debug == true) alias = "jpoley";
    if(alias.indexOf("@") == -1) alias += "@microsoft.com";

    cdonts.MailFormat = 0; // for MIME
    cdonts.BodyFormat = 0; // for HTML
 
    cdonts.Importance = 1; // 0 - lowest, 1 - normal, 2 - highest priority (Urgent)

    // attach any listed images
    try
        {
        var images = xml.getElementsByTagName("imglist");
        for(i=0; i<images.item(0).childNodes.length; i++)
            {
            var path = images.item(0).childNodes.item(i).text;
            cdonts.AttachURL(GetPath() + "\\" + path, path);
            }
        }
    catch(e)
        {
        //WScript.Echo("Exception: " + e + "\r\n   Error: " + e.number + "\r\n   Description: " + e.description);
        }

    if(headerval != null && headerval != undefined) cdonts.Value("XBOXLAB") = headerval;
    cdonts.From = from;
    cdonts.To = alias;
    cdonts.Subject = subject;
    cdonts.Body = body;
    cdonts.Send();
    ++numEmailsSent;
    }


/*

Routine Description:

    VerifyAlias

    Checks the configuration XML for any redirected aliases

Arguments:

    string  alias       the email address
    XMLDOM  xml         entire configuration XML Document

Return Value:

    string  new alias if redirect was found
            "NoMail" if the user should not get mail
            otherwise the original alias passed in

Notes:

    The username in the xml file MUST be lower case!

*/
function VerifyAlias(alias, xml)
    {
    try
        {
        var user = xml.getElementsByTagName(alias);
        if(user.length == 0) return alias;

        var nomail = user.item(0).getElementsByTagName("NoMail");
        if(nomail.length >= 1 && (nomail.item(0).text == "true" || nomail.item(0).text == "1")) return "NoMail";

        var redirect = user.item(0).getElementsByTagName("Redirect");
        if(redirect.length >= 1) return redirect.item(0).text;
        }
    catch(e)
        {
        }

    return alias;
    }


/*

Routine Description:

    GetPath

    This method returns the path of the current executing script

Arguments:

    none

Return Value:

    full path to the executing script

Notes:


*/
function GetPath()
    {
    var name = WScript.ScriptFullName;
    return name.substr(0, name.lastIndexOf("\\")+1);
    }

