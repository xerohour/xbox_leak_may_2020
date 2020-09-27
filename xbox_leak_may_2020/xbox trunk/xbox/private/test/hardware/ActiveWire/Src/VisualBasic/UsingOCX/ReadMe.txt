This is a sample Visual Basic project.

This example uses ActiveX component(OCX) in Visual Basic.


To install ActiveX Control, first install ActiveWire Windows driver, then follow steps below.

        1. Download the OCX file and save to your local hard drive.
        2. Open a DOS box, and type in command like following ;

                   C:>    c:\windows\system\regsvr32.exe    d:\your_folder\AwusbIO.ocx

        In this example, I assumed your Windows directory is C:\Windows, and you saved the .OCX file in d:\your_folder\.
        You must perform this command again with new pathname if you moved/renamed the .OCX file. 
        A window should pop up saying the library is successfully registered.
        If pop-up window indicates failure or error, try re-install the driver.
