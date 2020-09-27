<%@ Language=JavaScript EnableSessionState=False%>
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    Response.AddHeader("Content-Type", "text/plain");

    var records = Server.CreateObject("ADODB.Recordset");
    var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=HVS;UID=HVSUser;PWD=HVS.User";

    // sql settings
    var SQLQuery = "SELECT * FROM Results";

    //
    // connect & get records
    //
    try {
        records.Open(SQLQuery, driver, 1, 2);
        }
    catch(e)
        {
        //if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
        Response.Write("Unable to open recordset.\nError: " + e.description + " (" + e.number + ")");
        Response.End();
        }

    //
    // try the debug IP as the machine name if we couldnt find the given name
    //
    try
        {
        records.AddNew();
        var d = new Date();
        records("Timestamp") = d.getVarDate();

        if(Request("macAddr").Count >= 1) records("MAC Address") = Request("macAddr");
        if(Request("devType").Count >= 1) records("Device Type") = Request("devType");
        if(Request("mfg").Count >= 1) records("Manufacturer") = Request("mfg");
        if(Request("fw").Count >= 1) records("Firmware Version") = Request("fw");
        if(Request("hw").Count >= 1) records("Hardware Version") = Request("hw");
        if(Request("sn").Count >= 1) records("Serial Number") = Request("sn");
        if(Request("test").Count >= 1) records("Test Name") = Request("test");
        if(Request("variation").Count >= 1) records("Variation") = Request("variation");
        if(Request("status").Count >= 1) records("Status") = parseInt(Request("status"));
        if(Request("numOps").Count >= 1) records("Number of Operations") = Request("numOps");
        if(Request("boxInfo").Count >= 1) records("Box Information") = Request("boxInfo");
        if(Request("testConfig").Count >= 1) records("Config Settings") = Request("testConfig");
        if(Request("notes").Count >= 1) records("Notes") = Request("notes");

        // save the data
        records.Update();

        Response.Write("DB Updated");
        }
    catch(e)
        {
        Response.Write("Unable to edit recordset\nError: " + e.description + " (" + e.number + ")");
        //if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
        }

    try
        {
        records.Close();
        }
    catch(e)
        {
        Response.Write("Unable to close recordset\nError: " + e.description + " (" + e.number + ")");
        //if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
        }
%>
