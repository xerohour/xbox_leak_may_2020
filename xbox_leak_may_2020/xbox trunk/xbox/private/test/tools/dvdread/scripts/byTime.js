
main();

function ErrorCount()
    {
    this.buffCompare = 0;
    this.failThenFail = 0;
    this.failThenSuccess = 0;
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
    var outfile;

    var errorList = new Array;
    var numReads = 0;

    outfile = filesystem.OpenTextFile("byTime.csv", 2, true);
    outfile.WriteLine("Chunk, Buffer Mismatch, 2 Failures, 1 Failure");

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
                if(line.indexOf("Blocks ") == 0)
                    {
                    ++numReads;
                    }
                else if(line.indexOf("Block ") == 0)
                    {
                    if(line.indexOf("buff1 != buff2") != -1)
                        outfile.WriteLine(numReads + ", 1, 0, 0");
                    else if(line.indexOf("succeeded") != -1)
                        outfile.WriteLine(numReads + ", 0, 0, 1");
                    else 
                        outfile.WriteLine(numReads + ", 0, 1, 0");
                    }
                }
            file.Close();
            }
        }

    outfile.Close();

    WScript.Echo("Done");
    }

function GetPath()
    {
    var name = WScript.ScriptFullName;
    return name.substr(0, name.lastIndexOf("\\")+1);
    }
