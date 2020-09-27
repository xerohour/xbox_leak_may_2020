

                             ActiveWire USB Software Library

                            

Last Updated : Oct.20th, 1999
                            

Doc\
	This folder contains documents for ActiveWire USB board.
	The same can be viewed on our web site at http://www.activewireinc.com.

                            
Driver\
	This folder contains ActiveWire Windows driver installation files.
	After you receive your ActiveWire USB, you must install these drivers.
                                
         1. Plug the ActiveWire board into your USB port.
         2. The Windows operating system will automatically detect ActiveWire USB
	    and prompt you to install a driver.
	    Specify location of this folder then Windows will automatically install them.
         3. That's all it takes to get started! 
            Restart Windows if you are asked to do so. It's usually not necessary.
                                
	USB makes driver installation quick and easy.
	Now, let's start exploring possibility of ActiveWire USB!!

	Driver Revision history:

		1.02 (May 19th,1999)	:	Fixed problem in OCX with Visual Basic 

		1.03 (Jun.22nd,1999)	:	Added DLL interface support with Visual Basic.
						Enhanced error message reporting.

		1.04 (Oct.20th,1999)	:	Added Windows 2000 support.
						Fixed a few minor firmware/DLL bugs.

                                

AwusbIO.exe
        This is a sample application to control I/O port of ActiveWire USB board.
        It's for you to try your ActiveWire USB board.
                                
                                
Src\
	This folder contains source files of the sample applications.
	VisualC++\ sub-folder contains code written using Microsoft Visual C++ 5.0 and Microsoft Foundation Class (MFC) Library. 
	You can also use Microsoft Visual C++ 6.0.
	VisualBaisc\ sub-folder contains 2 examples written using Microsoft Visual Basic 5.0.
	One example uses DLL interface, the another uses ActiveX (OCX) interface.
	You can also use Microsoft Visual Basic 6.0.
	Feel free to use it as a reference for your own design.
                                
                                
HTML\
	This folder contains samples of HTML file with Java Script using following ActiveX control.
	Microsoft Internet Explorer 4.0 or above is required.
	Please set Security option of Internet Explorer to Low when you browse this page.

AwusbIO.ocx  
	This is ActiveX Control for the ActiveWire USB board.
        To install ActiveX Control in your system, first install ActiveWire Windows driver, 
	then type in command like following at command prompt.
                                
	 C:>	c:\windows\system\regsvr32.exe	d:\your_folder\AwusbIO.ocx
                                
	In this example, I assumed your Windows directory is C:\Windows, and you saved the .OCX 
	file in d:\your_folder\.
	You must perform this command again with new pathname if you moved/renamed the .OCX file. 
	A window should pop up saying the library is successfully registerd.
        If pop-up window indicates failure or error, try re-install the driver.

RegClear.exe
	This is a utility program written specifically for troubleshooting ActiveWire USB driver installation.
	If there were an anomally during first time driver installation (power failure, corrupted disk, etc.),
	the Windows Registry Database will maintain incorrect information, preventing proper driver installation.
        If that occurs, run this utility and restart Windows. Then simply repeat the procedure for driver installation.
        This utility was actually written so that we could easily repeat driver installation for testing purposes. 


For any further questions, please contact tech_support@activewireinc.com
            
    
 
