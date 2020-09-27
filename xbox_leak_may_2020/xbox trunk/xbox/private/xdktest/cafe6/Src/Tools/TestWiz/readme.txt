********************  TEST WIZARDS README *************************
INTRODUCTION


Q: What is Test Wizard?
A: It is a custom App Wizard extension DLL that is used to create CAFE automated tests.

Q: Who needs this tool?
A: Anybody who wants to write a new CAFE test script.

Q: What advandages does Test Wizard have?
A: The start up time to create a new test is greatly reduced since it automatically generates a CAFE type makefile and all the necessary source files, hiding all of the grunge work so the user is able to focus on writing the test cases instead of worrying about the underlying support code. 
It also provides an intuitive UI to the CAFE API's so that the user can get a headstart on writing the script.

Q: What kind of documentation is available?
A: This readme.txt file briefly explains how to install and use the Test Wizard.
You can also read TIPS.TXT for troubleshooting Test Wizard or refer to CUSTAPI.TXT to learn how to customize what API's, Categories, Areas and Subareas get displayed in the Script Dialog.
CUSTAPI.TXT also explains the general format of APIDATA.TXT which is used internally by Test Wizard.

Q: Who do I contact for support, bug reports or feature requests?
A: EnriqueP or IvanL

**************  INSTALLING TEST WIZARD  *********************8

**** Tree Diagram


CAFE
     TOOLS
	  TESTWIZ
                   BIN  (contains the files you need: testwiz.dll and apidata.txt )
		   SRC 	( source files, you can ghost these out)

To install the Test Wizard:
1.Create a new APPWIZ subdirectory under your IDE\BIN directory. You must use a debug version of the IDE.( A version of test wizard will soon be available that will run with the retail IDE).
2. Copy all the files in the TESTWIZ\BIN directory to IDE\BIN\APPWIZ

To Use the Test Wiz:
1.  Start IDE; File.New; select New Project
2. Specify the name of your new test and the path to the new location.
3. Select "testwiz.dll" from 'Project Type' drop down; <Create>.
4. The next dialog is self explanatory.  
     You can either skip <Edit Script> and simply create the project skeleton by pressing "Finish" (I recommend this for now)
OR
    You can click on  <Edit Script> if you want to use the API dialog to start writing your test.(There is a limited number of API's available at this point)
5. After you are done reading the confirmation dialog hit "OK"

The test wizard will create a new project with all the correct Project settings and source files. If you build this project you should get a working automation test exe.

If you run into a problem using testwiz or building the resulting project please refer to TIPS.TXT for some trouble shooting tips:
 

if you would like to add your own API's to the test wizards UI please read CUSTAPI.TXT to learn how to customize the test wizard:
 
 
 Any feedback and bugs reports are welcome. Feel free to contact EnriqueP if you have any questions,

 Thanks and Happy coding!
