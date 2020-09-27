/*
Copyright (C) Microsoft Corporation

Module Name:

    results.js

Abstract:

    Helper functions for creating the results

Author:

    Josh Poley (jpoley)

Revision History:


Notes:
    Excel Documentation in MSDN: 
        Office Developer Documentation/Office 2000 Language Reference/Microsoft Excel 2000 Reference
*/

var WshShell = WScript.CreateObject("WScript.Shell");
var excel = WScript.CreateObject("Excel.Application");
excel.Visible = true;


function ColumnHeader(caption, destinationRow)
    {
    excel.Sheets("Sheet1").Cells(destinationRow, 1).Font.Bold = true;
    excel.Sheets("Sheet1").Cells(destinationRow, 2).Font.Bold = true;
    excel.Sheets("Sheet1").Cells(destinationRow, 3).Font.Bold = true;
    excel.Sheets("Sheet1").Cells(destinationRow, 4).Font.Bold = true;
    excel.Sheets("Sheet1").Cells(destinationRow, 5).Font.Bold = true;

    excel.Sheets("Sheet1").Cells(destinationRow, 1).Value = caption;
    excel.Sheets("Sheet1").Cells(destinationRow, 2).Value = "Avg";
    excel.Sheets("Sheet1").Cells(destinationRow, 3).Value = "StDev";
    excel.Sheets("Sheet1").Cells(destinationRow, 4).Value = "Max";
    excel.Sheets("Sheet1").Cells(destinationRow, 5).Value = "Min";
    }

function ColumnStats(col, destinationRow)
    {
    var caption = excel.Range(col + "1").Value;
    var column = col + ":" + col;

    excel.Sheets("Sheet1").Columns("B:C").NumberFormat = "0.00";
    excel.Sheets("Sheet1").Cells(destinationRow, 1).Value = caption;
    excel.Sheets("Sheet1").Cells(destinationRow, 2).Value = excel.WorksheetFunction.Average(excel.Range(column));
    excel.Sheets("Sheet1").Cells(destinationRow, 3).Value = excel.WorksheetFunction.StDev(excel.Range(column));
    excel.Sheets("Sheet1").Cells(destinationRow, 4).Value = excel.WorksheetFunction.Max(excel.Range(column));
    excel.Sheets("Sheet1").Cells(destinationRow, 5).Value = excel.WorksheetFunction.Min(excel.Range(column));
    }

/*

Routine Description:

    Chart

    Creates a new Excel chart

Arguments:

    string  sheetname   the name of the sheet with the data
    string  chartname   the name the chart will be given
    string  range       excel range (see excel's Range() api for info)
    string  yaxis       the caption for the yaxis

Return Value:

    none

Notes:

    The xaxis is assumed to be time and the first column in the range.

*/
function Chart(sheetname, chartname, range, yaxis)
    {
    excel.Sheets(sheetname).Activate();
    excel.Range(range).Select();

    var chart = excel.Charts.Add();
    chart.ChartType = 4;
    chart.HasLegend = false;
    chart.Axes(1).HasTitle = true;
    chart.Axes(1).AxisTitle.Text = "Time (min)";
    chart.Axes(2).HasTitle = true;
    chart.Axes(2).AxisTitle.Text = yaxis;
    chart.Name = chartname;
    }

/*

Routine Description:

    CreateSummary

    This method parses all the CLIENTx-x.csv files and generates a summary

Arguments:

    string  path            full path to where the .csv files live
    int     timerRowStart   the row number where the custom timers are in the 
                            .csv file
    int     timerRowStop    ending row for the custom timers
    int     statsRowStart   the row number where the surge Results start

Return Value:

    none

Notes:

    The resultant summmary will be saved as "1summary.xls"

*/
function CreateSummary(path, timerRowStart, timerRowStop, statsRowStart)
    {
    var timers = new Array;
    var name;
    var count=0;
    var firstFile = "";

    // get list of files
    var fso = WScript.CreateObject("Scripting.FileSystemObject");
    var each = new Enumerator(fso.GetFolder(path).files);
    var regx = new RegExp("-.\\.csv", "gi");

    var stats = new Array;
    var startRow = 8;
    var row;
    var A1;
    var A2;
    
    for(row=timerRowStart; row<=timerRowStop; row++)
        {
        timers[row-startRow] = new Average(row, 2);
        }

    // loop through all files gathering stats
    for(; !each.atEnd(); each.moveNext())
        {
        name = "" + each.item();
        if(name.search(regx) >= 0)
            {
            excel.WorkBooks.Open(name);
            
            for(var i=0; i<timers.length; i++)
                {
                if(firstFile == "") timers[i].title = excel.Cells(timers[i].row, timers[i].col-1).Value;
                timers[i].Insert(excel.Cells(timers[i].row, timers[i].col).Value);
                }

            for(var i=statsRowStart; (key=excel.Cells(i, 1).Value) != null; i++)
                {
                err = excel.Cells(i, 2).Value;
                if(err > 1000 || err < 0) continue;

                // Member Profile Pages specific code
                if(key.indexOf("unknown") > 1) key = "http://mpp.brim1.passporttest.com/unknown@passporttest.com";
                else if(key.indexOf("@passporttest.com") > 1) key = "http://mpp.brim1.passporttest.com/TESTxyz@passporttest.com";
                // End Member Profile Pages specific code

                if(stats[key] == null) stats[key] = new NetStats(err);
                stats[key].connect.InsertStat(excel.Cells(i, 4).Value, excel.Cells(i, 5).Value, excel.Cells(i, 6).Value);
                stats[key].transact.InsertStat(excel.Cells(i, 7).Value, excel.Cells(i, 8).Value, excel.Cells(i, 9).Value);
                stats[key].bytesOut.InsertStat(excel.Cells(i, 10).Value, excel.Cells(i, 11).Value, excel.Cells(i, 12).Value);
                stats[key].bytesIn.InsertStat(excel.Cells(i, 13).Value, excel.Cells(i, 14).Value, excel.Cells(i, 15).Value);
                }
            if(firstFile == "")
                {
                A1 = excel.Cells(1, 1).Value;
                A2 = excel.Cells(1, 2).Value;
                firstFile = name;
                }

            excel.WorkBooks.Close();
            }
        }


    // Create a new worksheet
    excel.WorkBooks.Add();
    excel.Cells.NumberFormat = "0.00";
    excel.Range("B" + statsRowStart + ":B256").NumberFormat = "0";
    excel.Columns("F").ColumnWidth = 11;
    excel.Columns("G").ColumnWidth = 10;
    excel.Columns("H").ColumnWidth = 10;
    excel.Columns("I").ColumnWidth = 9;

    // Write the Header
    excel.Cells(2, 2).Value = "Summary";
    excel.Cells(3, 2).Value = Date();
    excel.Cells(1, 1).Value = A1;
    excel.Cells(1, 2).Value = A2;

    // Write the timers out
    excel.Cells(timerRowStart-1, 1).Font.Bold = true;
    excel.Cells(timerRowStart-1, 2).Font.Bold = true;
    excel.Cells(timerRowStart-1, 1).Value = "Operation";
    excel.Cells(timerRowStart-1, 2).Value = "Avg Time (ms)";
    for(var i=0; i<timers.length; i++)
        {
        excel.Cells(timers[i].row, timers[i].col-1).Value = timers[i].title;
        excel.Cells(timers[i].row, timers[i].col).Value = timers[i].sum/timers[i].count;
        }

    // Write the Header
    excel.Rows(statsRowStart-1).WrapText = true;
    excel.Cells(statsRowStart-1,  1).Font.Bold = true;
    excel.Cells(statsRowStart-1,  3).Font.Bold = true;
    excel.Cells(statsRowStart-1,  6).Font.Bold = true;
    excel.Cells(statsRowStart-1,  9).Font.Bold = true;
    excel.Cells(statsRowStart-1, 12).Font.Bold = true;
    excel.Cells(statsRowStart-1,  1).Value = "URL";
    excel.Cells(statsRowStart-1,  2).Value = "HTTP Status";
    excel.Cells(statsRowStart-1,  3).Value = "Connect Avg";
    excel.Cells(statsRowStart-1,  4).Value = "Connect Max";
    excel.Cells(statsRowStart-1,  5).Value = "Connect Min";
    excel.Cells(statsRowStart-1,  6).Value = "Transaction Avg";
    excel.Cells(statsRowStart-1,  7).Value = "Transaction Max";
    excel.Cells(statsRowStart-1,  8).Value = "Transaction Min";
    excel.Cells(statsRowStart-1,  9).Value = "Bytes Out Avg";
    excel.Cells(statsRowStart-1, 10).Value = "Bytes Out Max";
    excel.Cells(statsRowStart-1, 11).Value = "Bytes Out Min";
    excel.Cells(statsRowStart-1, 12).Value = "Bytes In Avg";
    excel.Cells(statsRowStart-1, 13).Value = "Bytes In Max";
    excel.Cells(statsRowStart-1, 14).Value = "Bytes In Min";

    // Write the stats
    i = statsRowStart;
    for(key in stats)
        {
        excel.Cells(i, 1).Value = key;
        excel.Cells(i, 2).Value = stats[key].status;
        excel.Cells(i, 3).Value = stats[key].connect.sum/stats[key].connect.count;
        excel.Cells(i, 4).Value = stats[key].connect.max;
        excel.Cells(i, 5).Value = stats[key].connect.min;
        excel.Cells(i, 6).Value = stats[key].transact.sum/stats[key].transact.count;
        excel.Cells(i, 7).Value = stats[key].transact.max;
        excel.Cells(i, 8).Value = stats[key].transact.min;
        excel.Cells(i, 9).Value = stats[key].bytesOut.sum/stats[key].bytesOut.count;
        excel.Cells(i,10).Value = stats[key].bytesOut.max;
        excel.Cells(i,11).Value = stats[key].bytesOut.min;
        excel.Cells(i,12).Value = stats[key].bytesIn.sum/stats[key].bytesIn.count;
        excel.Cells(i,13).Value = stats[key].bytesIn.max;
        excel.Cells(i,14).Value = stats[key].bytesIn.min;
        ++i;
        }

    excel.ActiveWorkbook.SaveAs(path + "\\1summary.xls", 1);
    excel.ActiveWorkbook.Saved = true;
    }



/*

Object Description:

    Average object

    This objects is used to keep track of the custom timers from the .csv files

Methods:

    Insert


Properties:

    row
    col
    max
    min
    sum
    count

Notes: 
    

*/
function Average(r, c)
    {
    this.title = "";
    this.row = r;
    this.col = c;
    this.max = 0;
    this.min = 9999999;
    this.sum = 0;
    this.count = 0;
    this.Insert = InsertAvg;
    }

function InsertAvg(val)
    {
    this.sum += val;
    ++this.count;
    if(this.max < val) this.max = val;
    if(this.min > val) this.min = val;
    }

/*

Object Description:

    StatAverage object

    This objects keeps track of the network stats for a given url

Methods:

    InsertStat


Properties:

    sum
    count
    max
    min

Notes: 
    

*/
function StatAverage()
    {
    this.sum = 0;
    this.count = 0;
    this.max = 0;
    this.min = 9999999;
    this.InsertStat = InsertStat;
    }

function InsertStat(val, max, min)
    {
    this.sum += val;
    ++this.count;
    if(this.max < max) this.max = max;
    if(this.min > min) this.min = min;
    }

function NetStats(s)
    {
    this.status = s;
    this.connect = new StatAverage;  // connection times
    this.transact = new StatAverage; // send/receive times
    this.bytesOut = new StatAverage; // bytes sent from client
    this.bytesIn = new StatAverage;  // bytes received by client
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





/*

function ShowFolderFileList(folderspec)
    {
    var fso = new ActiveXObject("Scripting.FileSystemObject");
    var each = new Enumerator(fso.GetFolder(folderspec).files);
    for (; !each.atEnd(); each.moveNext())
        {
        s += each.item();
        }
    }

function bla()
    {
    var objXL = WScript.CreateObject("Excel.Application");

    objXL.Visible = true;

    objXL.WorkBooks.Add;

    objXL.Columns(1).ColumnWidth = 20;
    objXL.Columns(2).ColumnWidth = 30;
    objXL.Columns(3).ColumnWidth = 40;

    objXL.Cells(1, 1).Value = "Property Name";
    objXL.Cells(1, 2).Value = "Value";
    objXL.Cells(1, 3).Value = "Description";

    objXL.Range("A1:C1").Select;
    objXL.Selection.Font.Bold = true;
    objXL.Selection.Interior.ColorIndex = 1;
    objXL.Selection.Interior.Pattern = 1; //xlSolid
    objXL.Selection.Font.ColorIndex = 2;

    objXL.Columns("B:B").Select;
    objXL.Selection.HorizontalAlignment = -4131; // xlLeft

    var intIndex = 2;


    //
    // Show command line arguments.
    //
    var colArgs = WScript.Arguments
    
    for (i = 0; i < colArgs.length; i++)
        {
        objXL.Cells(intIndex, 1).Value = "Arguments(" + i + ")";
        objXL.Cells(intIndex, 2).Value = colArgs(i);
        intIndex++;
        objXL.Cells(intIndex, 1).Select;
        }
    }


*/

