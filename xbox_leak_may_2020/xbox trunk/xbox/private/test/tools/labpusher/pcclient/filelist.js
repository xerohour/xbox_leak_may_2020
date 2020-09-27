var filesystem = WScript.CreateObject("Scripting.FileSystemObject");
var infile = filesystem.OpenTextFile("dirlist.txt", 1);
var outfile = filesystem.OpenTextFile("filelist.txt", 8, true);

var cdir = /\\XC(.*)/ 
var ydir = /\\XY(.*)/

while(!infile.AtEndOfStream)
{
    var line = infile.ReadLine();

    var result = line.match(cdir);
    if(result)
    {
        outfile.WriteLine( line + ",C:" + result[1]);
    }
    else
    {
        result = line.match(ydir);
	if(result)
        {
            outfile.WriteLine( line + ",Y:" + result[1]);
        }
    }
}

//WScript.Sleep(INFINITE);