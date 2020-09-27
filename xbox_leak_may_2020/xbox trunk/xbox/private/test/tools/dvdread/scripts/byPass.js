
main();

function ErrorCount()
    {
    this.buffCompare = 0;
    this.failThenFail = 0;
    this.failThenFailSame = 0;
    this.failThenSuccess = 0;
    this.longRead = 0;
    this.totalReads = 0;
    this.firmware = "";
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

    var lastLBA = -1;
    var lba = 0;
    var skipLBA = 0;
    var run = 1;

    var errorList = new Array;

    // loop through all files gathering stats
    for(; !each.atEnd(); each.moveNext())
        {
        filename = "" + each.item();

        offset = Math.max(filename.lastIndexOf("_")+1, filename.lastIndexOf("/")+1, filename.lastIndexOf("\\")+1);
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
                    if(lba < lastLBA)
                        {
                        ++run;
                        }
                    if(lba < skipLBA) continue;

                    if(errorList[run] == undefined || errorList[run] == null)
                        errorList[run] = new ErrorCount;

                    // buffer mismatch
                    if(line.indexOf("buff1 != buff2") != -1)
                        errorList[run].buffCompare = errorList[run].buffCompare + 1;

                    // 1 success, 1 failure
                    else if(line.indexOf("succeeded") != -1)
                        errorList[run].failThenSuccess = errorList[run].failThenSuccess + 1;

                    // err1 == err2
                    else if(line.indexOf("Same Error") != -1)
                        {
                        if(line.indexOf("70.05.81") == -1) // skip command errors
                            errorList[run].failThenFailSame = errorList[run].failThenFailSame + 1;
                        }

                    // err1 != err2
                    else if(line.indexOf("!= Err2") != -1)
                        errorList[run].failThenFail = errorList[run].failThenFail + 1;

                    // Read took > 1 sec
                    else if(line.indexOf("Read took") != -1)
                        errorList[run].longRead = errorList[run].longRead + 1;
                    }
                else if(line.indexOf("End of security section") != -1)
                    {
                    skipLBA = lba+288;
                    }
                else if(line.indexOf("Total blocks read") != -1)
                    {
                    errorList[run].totalReads = parseInt(line.substr(4, 11));
                    }
                else if(line.indexOf("DRIVE: Firmware") != -1)
                    {
                    if(errorList[run] == undefined || errorList[box] == null)
                        errorList[run] = new ErrorCount;
                    errorList[run].firmware = line.substr(26);
                    }

                lastLBA = lba;
                } // while(!eof)
            file.Close();


            file = filesystem.OpenTextFile("byPass.csv", 8, true);
            file.WriteLine("\r\n\r\n" + box + ", " + errorList[1].firmware);
            file.Write("Pass, ");
            for(i in errorList)
                {
                file.Write(i + ", ");
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
            file.Close();

            errorList = new Array;
            run = 1;
            }
        }


    WScript.Echo("Done");
    }

function GetPath()
    {
    var name = WScript.ScriptFullName;
    return name.substr(0, name.lastIndexOf("\\")+1);
    }
