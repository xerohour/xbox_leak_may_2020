
main();

function ErrorCount()
    {
    this.buffCompare = 0;
    this.failThenFail = 0;
    this.failThenFailSame = 0;
    this.failThenSuccess = 0;
    this.speedReductions = 0;
    this.slowedReadPass = 0;
    this.longRead = 0;
    this.totalReads = 0;
    this.firmware = "";
    this.err0180 = 0;
    this.err0380 = 0;
    this.err0581 = 0;
    this.err062E = 0;
    this.interr0180 = 0;
    this.interr0380 = 0;
    this.interr0581 = 0;
    this.interr062E = 0;
    }

// Block 1673120: buff1 != buff2
// Read1 succeeded
// Read1 failed
function main()
    {
    var filesystem = WScript.CreateObject("Scripting.FileSystemObject");
    var each = new Enumerator(filesystem.GetFolder(GetPath()).files);
    var regx = new RegExp("\\.txt", "gi");
    var file;

    var lba = 0;
    var skipLBA = 0;
    var speedChanged = false;
    var currentSpeed = 2;

    var errorList = new Array;

    // loop through all files gathering stats
    for(; !each.atEnd(); each.moveNext())
        {
        filename = "" + each.item();

        offset = filename.lastIndexOf("_")+1;
        box = filename.substr(offset, filename.lastIndexOf(".")-offset);

        if(filename.search(regx) >= 0)
            {
            // open file
            file = filesystem.OpenTextFile(filename, 1);
            while(!file.AtEndOfStream)
                {
                line = file.ReadLine();
                if(line.indexOf("Block ") == 0)
                    {
                    lba = parseInt(line.substr(6, 10));
                    if(lba < skipLBA) continue;

                    if(errorList[box] == undefined || errorList[box] == null)
                        errorList[box] = new ErrorCount;

                    // buffer mismatch
                    if(line.indexOf("buff1 != buff2") != -1)
                        errorList[box].buffCompare = errorList[box].buffCompare + 1;

                    // 1 success, 1 failure
                    else if(line.indexOf("succeeded") != -1)
                        errorList[box].failThenSuccess = errorList[box].failThenSuccess + 1;

                    // single error
                    else if(line.indexOf("Single Error") != -1)
                        errorList[box].failThenSuccess = errorList[box].failThenSuccess + 1;

                    // err1 == err2
                    else if(line.indexOf("Same Error") != -1)
                        {
                        if(line.indexOf("70.05.81") == -1) // skip command errors
                            errorList[box].failThenFailSame = errorList[box].failThenFailSame + 1;
                        }

                    // err1 != err2
                    else if(line.indexOf("!= Err2") != -1)
                        errorList[box].failThenFail = errorList[box].failThenFail + 1;

                    // no error 
                    else
                        {
                        if(speedChanged == true)
                            errorList[box].slowedReadPass = errorList[box].slowedReadPass + 1;
                        speedChanged = false;
                        }

                    // Read took > 1 sec
                    if(line.indexOf("Read took") != -1)
                        errorList[box].longRead = errorList[box].longRead + 1;

                    if(line.search(/.*Internal.*/gi) >= 0)
                        {
                        if(line.search(/.*<01>.*<80>/gi) >= 0)
                            errorList[box].interr0180 = errorList[box].interr0180 + 1;
                        else if(line.search(/.*<03>.*<80>/gi) >= 0)
                            errorList[box].interr0380 = errorList[box].interr0380 + 1;
                        else if(line.search(/.*<05>.*<81>/gi) >= 0)
                            errorList[box].interr0581 = errorList[box].interr0581 + 1;
                        else if(line.search(/.*<06>.*<2E>/gi) >= 0)
                            errorList[box].interr062E = errorList[box].interr062E + 1;
                        }
                    else
                        {
                        if(line.search(/.*<01>.*<80>/gi) >= 0)
                            errorList[box].err0180 = errorList[box].err0180 + 1;
                        else if(line.search(/.*<03>.*<80>/gi) >= 0)
                            errorList[box].err0380 = errorList[box].err0380 + 1;
                        else if(line.search(/.*<05>.*<81>/gi) >= 0)
                            errorList[box].err0581 = errorList[box].err0581 + 1;
                        else if(line.search(/.*<06>.*<2E>/gi) >= 0)
                            errorList[box].err062E = errorList[box].err062E + 1;
                        }
                    }
                else if(line.indexOf("End of security section") != -1)
                    {
                    if(errorList[box] == undefined || errorList[box] == null)
                        errorList[box] = new ErrorCount;
                    skipLBA = lba+288;
                    }
                else if(line.indexOf("Total blocks read") != -1)
                    {
                    if(errorList[box] == undefined || errorList[box] == null)
                        errorList[box] = new ErrorCount;
                    errorList[box].totalReads = parseInt(line.substr(4, 11));
                    }
                else if(line.indexOf("DRIVE: Firmware") != -1)
                    {
                    if(errorList[box] == undefined || errorList[box] == null)
                        errorList[box] = new ErrorCount;
                    errorList[box].firmware = line.substr(26);
                    }
                else if(line.indexOf("Speed changed") != -1)
                    {
                    if(errorList[box] == undefined || errorList[box] == null)
                        errorList[box] = new ErrorCount;
                    speed = parseInt(line.substr(17, 2));
                    if(speed < currentSpeed)
                        {
                        errorList[box].speedReductions = errorList[box].speedReductions + 1;
                        speedChanged = true;
                        }
                    currentSpeed = speed;
                    }
                }
            file.Close();
            }
        }

    file = filesystem.OpenTextFile("byBox.csv", 2, true);
    file.Write("Box, ");
    for(i in errorList)
        {
        file.Write(i + ", ");
        }
    file.Write("\r\n");
    file.Write("Firmware, ");
    for(i in errorList)
        {
        file.Write(errorList[i].firmware + ", ");
        }
    file.Write("\r\n");
    file.Write("Buffer Mismatch, ");
    for(i in errorList)
        {
        file.Write(errorList[i].buffCompare + ", ");
        }
    file.Write("\r\n");
    file.Write("1 Error, ");
    for(i in errorList)
        {
        file.Write(errorList[i].failThenSuccess + ", ");
        }
    file.Write("\r\n");
    file.Write("2 Errors Diff, ");
    for(i in errorList)
        {
        file.Write(errorList[i].failThenFail + ", ");
        }
    file.Write("\r\n");
    file.Write("2 Errors Same, ");
    for(i in errorList)
        {
        file.Write(errorList[i].failThenFailSame + ", ");
        }
    file.Write("\r\n");
    file.Write("Long Reads, ");
    for(i in errorList)
        {
        file.Write(errorList[i].longRead + ", ");
        }
    file.Write("\r\n");
    file.Write("Speed Reductions, ");
    for(i in errorList)
        {
        file.Write(errorList[i].speedReductions + ", ");
        }
    file.Write("\r\n");
    file.Write("SlowedReadPass, ");
    for(i in errorList)
        {
        file.Write(errorList[i].slowedReadPass + ", ");
        }
    file.Write("\r\n");
    file.Write("Internal Error 01/80, ");
    for(i in errorList)
        {
        file.Write(errorList[i].interr0180 + ", ");
        }
    file.Write("\r\n");
    file.Write("Internal Error 03/80, ");
    for(i in errorList)
        {
        file.Write(errorList[i].interr0380 + ", ");
        }
    file.Write("\r\n");
    file.Write("Internal Error 05/81, ");
    for(i in errorList)
        {
        file.Write(errorList[i].interr0581 + ", ");
        }
    file.Write("\r\n");
    file.Write("Internal Error 06/2E, ");
    for(i in errorList)
        {
        file.Write(errorList[i].interr062E + ", ");
        }
    file.Write("\r\n");
    file.Write("Error 01/80, ");
    for(i in errorList)
        {
        file.Write(errorList[i].err0180 + ", ");
        }
    file.Write("\r\n");
    file.Write("Error 03/80, ");
    for(i in errorList)
        {
        file.Write(errorList[i].err0380 + ", ");
        }
    file.Write("\r\n");
    file.Write("Error 05/81, ");
    for(i in errorList)
        {
        file.Write(errorList[i].err0581 + ", ");
        }
    file.Write("\r\n");
    file.Write("Error 06/2E, ");
    for(i in errorList)
        {
        file.Write(errorList[i].err062E + ", ");
        }
    file.Write("\r\n");
    file.Write("Total Blocks Read, ");
    for(i in errorList)
        {
        file.Write(errorList[i].totalReads + ", ");
        }
    file.Close();

    WScript.Echo("Done");
    }

function GetPath()
    {
    var name = WScript.ScriptFullName;
    return name.substr(0, name.lastIndexOf("\\")+1);
    }
