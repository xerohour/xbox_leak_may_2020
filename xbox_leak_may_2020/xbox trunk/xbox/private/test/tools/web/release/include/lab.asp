<%
    var admin = false;

    // Database Settings
    var records = Server.CreateObject("ADODB.Recordset");

function FormatShortDate(d)
    {
    var date = new Date(d);
    if(isNaN(date.getMonth())) return "Never";
    return (date.getMonth()+1) + "/" + date.getDate() + "/" + date.getYear() + " " + FormatTime(date);
    }

function FormatTime(d)
    {
    var am = " AM";
    var date = new Date(d);
    var hours = date.getHours();
    if(isNaN(date.getMonth())) return "Never";
    if(hours > 12)
        {
        am = " PM";
        hours-=12;
        }
    else if(hours == 0)
        {
        hours = 12;
        }
    return hours + ":" + (date.getMinutes()<10?"0":"") + date.getMinutes() + am;
    }

function RTrim(str)
    {
    return (""+str).replace(/\s*$/g, ""); // strip trailing whitespaces
    }

function LTrim(str)
    {
    return (""+str).replace(/^\s*/g, ""); // strip leading whitespaces
    }

%>
