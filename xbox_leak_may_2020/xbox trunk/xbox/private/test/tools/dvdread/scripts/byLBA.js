
main();

function ErrorCount()
    {
    this.buffCompare = 0;
    this.failThenFail = 0;
    this.failThenFailSame = 0;
    this.failThenSuccess = 0;
    this.longRead = 0;
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

    var errorList = new Array;

    // loop through all files gathering stats
    for(; !each.atEnd(); each.moveNext())
        {
        filename = "" + each.item();
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

                    if(errorList[lba] == undefined || errorList[lba] == null)
                        errorList[lba] = new ErrorCount;

                    // buffer mismatch
                    if(line.indexOf("buff1 != buff2") != -1)
                        errorList[lba].buffCompare = errorList[lba].buffCompare + 1;

                    // 1 success, 1 failure
                    else if(line.indexOf("succeeded") != -1)
                        errorList[lba].failThenSuccess = errorList[lba].failThenSuccess + 1;

                    // single error
                    else if(line.indexOf("Single Error") != -1)
                        errorList[lba].failThenSuccess = errorList[lba].failThenSuccess + 1;

                    // err1 == err2
                    else if(line.indexOf("Same Error") != -1)
                        {
                        if(line.indexOf("70.05.81") == -1) // skip command errors
                            {
                            errorList[lba].failThenFailSame = errorList[lba].failThenFailSame + 1;
                            }
                        }


                    // err1 != err2
                    else if(line.indexOf("!= Err2") != -1)
                        errorList[lba].failThenFail = errorList[lba].failThenFail + 1;

                    // Read took > 1 sec
                    else if(line.indexOf("Read took") != -1)
                        errorList[lba].longRead = errorList[lba].longRead + 1;
                    }
                else if(line.indexOf("End of security section") != -1)
                    {
                    skipLBA = lba+288;
                    }
                }
            file.Close();
            }
        }

    file = filesystem.OpenTextFile("byLBA.csv", 2, true);
    file.WriteLine("LBA, Buffer Mismatch, 1 Error, 2 Errors Diff, 2 Errors Same, Long Reads");
    for(i in errorList)
        {
        file.WriteLine(i + ", " + errorList[i].buffCompare + ", " + errorList[i].failThenSuccess + ", " + errorList[i].failThenFail + ", " + errorList[i].failThenFailSame + ", " + errorList[i].longRead);
        }
    file.Close();

    WScript.Echo("Done");
    }

function GetPath()
    {
    var name = WScript.ScriptFullName;
    return name.substr(0, name.lastIndexOf("\\")+1);
    }
