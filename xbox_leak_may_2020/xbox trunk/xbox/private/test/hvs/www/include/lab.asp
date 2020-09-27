<%
    // Database Settings
    var records = Server.CreateObject("ADODB.Recordset");
    //var driver = "DSN=LabTest;UID=admin;PWD=;";
    var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=HVS;UID=HVSUser;PWD=HVS.User";

function GetUsername()
    {
    var name = ""+Request.ServerVariables("AUTH_USER");
    name = name.substr(name.indexOf("\\")+1);
    return name;
    }

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

function FormatNumberWCommas(num)
    {
    var str = ""+parseInt(num);
    var len = str.length;
    var result = "";
    var end = len % 3;
    while(len >= 3)
        {
        section = str.substr(len-3, 3);
        if(result == "")
            result = section;
        else
            result = section + "," + result;
        len -= 3;
        }
    if(end)
        {
        section = str.substr(0, end);
        if(result == "")
            result = section;
        else
            result = section + "," + result;
        }


    return result;
    }

function RTrim(str)
    {
    return (""+str).replace(/\s*$/g, ""); // strip trailing whitespaces
    }

function LTrim(str)
    {
    return (""+str).replace(/^\s*/g, ""); // strip leading whitespaces
    }

function Format2Places(number)
    {
    return Math.round(100*number)/100;
    }


function SendMail(alias, cc, from, subject, body)
    {
    var cdonts = null;

    try
        {
        cdonts = Server.CreateObject("CDONTS.NewMail");

        if(from == "" || from == null) from = "xgirl@microsoft.com";

        var names = alias.split(";");
        alias = "";
        for(i in names)
            {
            if(names[i] == "" || names[i] == " ") {}
            else if(names[i].indexOf("@") >= 0) alias += names[i] + "; ";
            else alias += names[i] + "@microsoft.com; ";
            }

        names = cc.split(";");
        cc = "";
        for(i in names)
            {
            if(names[i] == "" || names[i] == " ") {}
            else if(names[i].indexOf("@") >= 0) cc += names[i] + "; ";
            else cc += names[i] + "@microsoft.com; ";
            }

        cdonts.MailFormat = 0; // for MIME
        cdonts.BodyFormat = 0; // for HTML
        cdonts.Importance = 1; // 0 - lowest, 1 - normal, 2 - highest priority (Urgent)

        cdonts.From = from;
        cdonts.To = alias;
        cdonts.Cc = cc;
        cdonts.Subject = subject;
        cdonts.Body = body;
        cdonts.Send();
        }
    catch(e)
        {
        }
    }

function UrlDecode(url) {
	var temp, x, encodedChar, result;		
	temp = url;
	result = "";
	x = url.indexOf("%");       
	while (x>-1)
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

function UrlEncode(url) {
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
			/*get character unicode number and 
			convert to hex*/
			stringchar=stringchar.charCodeAt(0);
			charOne=stringchar % 16;
			charTwo=(stringchar - charOne)/16;
			stringchar="" + hex_chars.charAt(charTwo);
			stringchar=stringchar + hex_chars.charAt(charOne);				
			stringchar="%" + stringchar;
			}
		temp=temp + stringchar;
		stringpos++;
		}
	while (stringpos<=urllength-1);
	return temp;
}  

%>
