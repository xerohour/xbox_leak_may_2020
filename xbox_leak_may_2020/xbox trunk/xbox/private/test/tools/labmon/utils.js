/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.js

Abstract:

    Common functions

Author:

    Josh Poley (jpoley)

Notes:

*****************************************************************************/
var filesystem = WScript.CreateObject("Scripting.FileSystemObject");
//var driver = "DSN=LabTest;UID=admin;PWD=;";
var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=TestLab;UID=TestLabUser;PWD=TestLab1.User";
var shell = WScript.CreateObject("WScript.Shell");

function SetProperty(name, val)
    {
    var records = null;

    try
        {
        records = WScript.CreateObject("ADODB.Recordset");
        records.Open("SELECT * FROM Properties WHERE Property='" + name + "'", driver, 1, 2);

        if(records.EOF)
            {
            records.AddNew();
            records("Property") = ""+name;
            records("Comments") = "";
            }

        records("Value") = ""+val;
        records.Update();
        }
    catch(e)
        {
        shell.LogEvent(2, "XBOX LAB SetProperty(): " + e.description + " (" + e.number + ")");
        }

    if(records != null)
        {
        records.Close();
        WScript.DisconnectObject(records);
        }
    }

function GetProperty(name)
    {
    var val = "";
    var records = null;

    try
        {
        records = WScript.CreateObject("ADODB.Recordset");
        records.Open("SELECT * FROM Properties WHERE Property='" + name + "'", driver, 0, 1);

        if(!records.EOF)
            val = ""+records("Value");
        }
    catch(e)
        {
        shell.LogEvent(2, "XBOX LAB GetProperty(): " + e.description + " (" + e.number + ")");
        }

    if(records != null)
        {
        records.Close();
        WScript.DisconnectObject(records);
        }
    return RTrim(val);
    }

function BoxIsNotResponding(name)
    {
    try
        {
        dbgmon.Connect(name);
        dbgmon.Disconnect();
        return false;
        }
    catch(e)
        {
        return true;
        }
    }

function FormatShortDate(d)
    {
    var date;
    if(d == undefined || d == null) date = new Date();
    else date = new Date(d);
    if(isNaN(date.getMonth())) return "Never";
    return (date.getMonth()+1) + "/" + date.getDate() + "/" + date.getYear() + " " + FormatTime(date);
    }

function FormatTime(d)
    {
    var am = " AM";
    var date;
    if(d == undefined || d == null) date = new Date();
    else date = new Date(d);
    var hours = date.getHours();
    if(isNaN(date.getMonth())) return "Never";
    if(hours >= 12)
        {
        am = " PM";
        hours-=12;
        }
    if(hours == 0)
        {
        hours = 12;
        }
    return hours + ":" + (date.getMinutes()<10?"0":"") + date.getMinutes() + am;
    }


/*****************************************************************************

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

*****************************************************************************/
function SendMail(alias, subject, body, headerval)
    {
    var from = "a" + parseInt(Math.random()*1000);
    var cdonts = null;
    
    try
        {
        cdonts = WScript.CreateObject("CDONTS.NewMail");

        // Send mail to only one alias if in debug mode
        if(debugAlias != undefined)
            {
            if(debugAlias != "") alias = debugAlias;
            }

        var names = alias.split(";");

        alias = "";
        for(i in names)
            {
            if(names[i] == "" || names[i] == " ") {}
            else if(names[i].indexOf("@") >= 0) alias += names[i] + "; ";
            else alias += names[i] + "@microsoft.com; ";
            }

        cdonts.MailFormat = 0; // for MIME
        cdonts.BodyFormat = 0; // for HTML
 
        cdonts.Importance = 1; // 0 - lowest, 1 - normal, 2 - highest priority (Urgent)

        cdonts.From = from;
        if(headerval != null && headerval != undefined) cdonts.Value("XBOXLAB") = headerval;
        cdonts.To = alias;
        cdonts.Subject = subject;
        cdonts.Body = body;
        cdonts.Send();
        }
    catch(e)
        {
        shell.LogEvent(2, "XBOX LAB SendMail(): " + e.description + " (" + e.number + ")");
        }
    }

/*****************************************************************************

Routine Description:

    GetArgument

    Returns the value of a command line argument passed to the script

Arguments:

    the name of the value to retrieve

Return Value:

    the argument passed in

Notes:

    script.js /arg=bla

    GetArgument("arg") == "bla"

*****************************************************************************/
function GetArgument(name)
    {
    name = "/" + name + "=";
    for(var each = new Enumerator(WScript.Arguments); !each.atEnd(); each.moveNext())
        {
        var item = ""+each.item();
        var start = item.search(new RegExp(name, "i"));
        if(start >= 0)
            {
            return item.substr(start+name.length);
            }
        }

    return null;
    }


function UrlDecode(url)
    {
    if(url == null || url == undefined) return "";
    url = "" + url;
    var temp, x, encodedChar, result;
    temp = url;
    result = "";
    x = url.indexOf("%");
    while(x > -1)
        {
        encodedChar=temp.slice(x+1,x+3);
        encodedChar="0x" + encodedChar;
        encodedChar=parseInt(encodedChar);
        encodedChar=String.fromCharCode(encodedChar);
        //concatenate the results here
        result=result + temp.slice(0,x);
        result=result + encodedChar;
        temp=temp.slice(x+3, url.length);
        x=temp.indexOf("%");
        }
    result=result + temp;
    return result;
    }

function UrlEncode(url)
    {
    if(url == null || url == undefined) return "";
    url = "" + url;
    var special_chars, hexChars, urllength;
    var stringpos, temp, stringchar;
    var charOne, charTwo;
    hex_chars="0123456789ABCDEF";
    special_chars="%`~!@#$^&*()-_=+[{]}\\|;:,<.>/? ";
    urllength=url.length;
    stringpos=0;
    temp="";
    do
        {
        stringchar=url.slice(stringpos, stringpos+1);
        if (special_chars.indexOf(stringchar)>=0)
            {
            // get character unicode number and convert to hex
            stringchar=stringchar.charCodeAt(0);
            charOne=stringchar % 16;
            charTwo=(stringchar - charOne)/16;
            stringchar="" + hex_chars.charAt(charTwo);
            stringchar=stringchar + hex_chars.charAt(charOne);
            stringchar="%" + stringchar;
            }
        temp=temp + stringchar;
        stringpos++;
        } while (stringpos<=urllength-1);
    return temp;
    }


function RTrim(str)
    {
    // strip trailing whitespaces
    return (""+str).replace(/\s*$/g, "");
    }

/*
function SaveRecordSet(ctn, rs, property, val)
    {
    var retry = 3;
    while(retry)
        {
        try
            {
            //WScript.Echo("1");
            rs(property) = val;
            //WScript.Echo("2");
            rs.Update();
            //WScript.Echo("3");
            break;
            }
        catch(e)
            {
            shell.LogEvent(2, "XBOX DBGMON.WSF SaveRecordSet: " + e.description + "(ec: " + e.number + ")");
            WScript.Echo("RETRYING!");
            //WScript.Echo("4");
            //rs.CancelUpdate();
            //WScript.Echo("5");
            //ctn.RollbackTrans();
            //ctn.BeginTrans();
            //WScript.Echo("6");
            WScript.Sleep(100);
            }
        --retry;
        }
    //ctn.CommitTrans();
    }

function Retry(cnt)
    {
    var retry = 3;
    while(retry)
        {
        try
            {
            cnt.CommitTrans();
            break;
            }
        catch(e)
            {
            shell.LogEvent(2, "XBOX DBGMON.WSF SaveRecordSet: " + e.description + "(ec: " + e.number + ")");
            WScript.Echo("RETRYING!");
            WScript.Sleep(50);
            }
        --retry;
        }
    }

*/
