<%@ Language=JavaScript EnableSessionState=False%>
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    Response.AddHeader("Content-Type", "text/plain");

    var records = Server.CreateObject("ADODB.Recordset");
    var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=HVS;UID=HVSUser;PWD=HVS.User";

    var SQLQuery = "SELECT * FROM [Acceptance Criteria] WHERE [Test Name] LIKE '" +Request("test")+ "%';";

    //
    // connect & get records
    //
    try {
        records.Open(SQLQuery, driver, 0, 1);
        }
    catch(e)
        {
        Response.Write("Criteria: 0.0\r\nTest: " + Request("test") + "\r\nError: " + e.description + " (" + e.number + ")");
        Response.End();
        }

    //
    // Update the record
    //
    if(!records.EOF)
        {
        Response.Write("Criteria: " + records("Percentage") + "\r\nNotes: "+records("Notes")+"\r\nTest: " + Request("test") + "\r\nError: none" );
        }
    else
        {
        Response.Write("Criteria: 0.0\r\nTest: " + Request("test") + "\r\nError: No Records");
        }

    try {
        records.Close();
        }
    catch(e){}
%>
