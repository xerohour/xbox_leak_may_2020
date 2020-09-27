########## How to run perf BVT test ########

1. In your razzle build window, set an environment variable:
    set PERFBVT_SERVER=aaa.bbb.ccc.ddd
   where aaa.bbb.ccc.ddd is the IP address of your win2k dev machine.
   You can get the IP address of your win2k machine using ipconfig command.

2. Build test\perfbvt
    This will generate a perfbvt.xbe file that you copy to your xbox machine.
    You will also have a win32 executable - perfbvt\win2k\obj\i386\perfbvt_server.exe.

3. Run perfbvt_server.exe on your win2k dev machine.

    perfbvt_server [options]

    -o output-file
        Save the result to a log file
        Default behavior is to stdout

    -t test-selection
        Specify a test selection

        Test selection is specified in the following format:
            test-name[,repeat-count[,test-param]]
        
        test-name is the name of the test function (case-sensitive)
        repeat-count specifies how many times the test is run
        test-param is a string parameter for test

        You can use multiple -t options to specify more than one test to run.

    -i test-selection-file
        Read the specified file for test selections

        Here is a sample test-selection-file:

            ALL # Run all tests once

            # Don't run DiskWriteVariable and DiskReadRandomAccess
            DiskWriteVariable,0
            DiskReadRandomAccess,0

            # Run DiskRead4KBlock twice and
            # pass d:\disktest.dat as a parameter
            DiskReadSeq4KBlock,2,d:\disktest.dat

    If there are no -t or -i options, all test functions
    are run once by default.

4. Run perfbvt.xbe on your xbox machine.

5. Test results are sent to the output logfile on your win2k machine
   as well as to the kernel debugger window.



########## How to add your own test case ##########

1. Define your test function and add it to the appropriate
   .cpp files under perfbvt\xbox.

   Your test function should be declared as:

   DECLARE_TEST_FUNCTION(YourTestFunctionName)
   {
        ULONG result;

        // Do your test here ...
        //
        // If your test function needs a parameter, you can specify
        // it when your start perfbvt_server on your win2k dev machine
        // (see instructions above for more details). The parameter
        // is accessed through the testparam variable inside your function.

        // If you return a non-zero value, it'll be
        // interpreted as how long your test took (in milliseconds).
        // A log entry is automatically emitted as:
        //  YourTestFunctionName: d.ddd (seconds)
        //
        // You can also emit your own log entry inside your test function.
        // In that case, you should return 0 to suppress the standard log entry.
        // There are two functions you can call:
        //
        //  VOID LogEntry(CHAR* entryname, ULONG result, CHAR* unit=NULL);
        //      This prints out a standard log entry of the form:
        //          entryname: result unit
        //      The result is assumed to have a scale of 1000 and
        //      is printed as d.ddd (with 3 digits after the decimal point).
        //      If you don't specified a unit, the default is seconds.
        //
        //  VOID Print(CHAR* format, ...);
        //      This is similar to DbgPrint.
        //      The output is sent to the server as well as the kernel debugger.

        return result;
   }

   For consistency, you should call ReadTimestamp() function to get
   the processor timestamp counter. The return value is a ULONG, in milliseconds.

2. Now add your test to the global table in perfbvt\xbox\perfbvt.cpp.
   You need to add two lines (near the beginning of the file,
   use existing test functions as examples).

    DECLARE_TEST_FUNCTION(YourTestFunctionName);
    ...
    TestSelection alltests[] = {
        ...
        DEFINE_TEST_ENTRY(YourTestFunctionName);
    };

3. If you're adding a test case that requires network interaction with
   the win2k server, please come and talk to me.

