To use the 2kidmaster and several things have to be available.



1. An xbox running the x2kIdServer program.
2. Access to the corpnet to be able to send packets to wallyho-ts
This machine has the Ncipher box in it which can decode the online key.
This machine should have the 2kcspservice.exe in the directory 2kNCipherService
running. Can be run as the following
2kcspservice -install  --Will install it as a service on that machine.
2kcspservice -remove   --Will stop and remove the service.
2kcspservice -console  --Will run it as a console app and you can see the
printf output. This is not recommended as when its a service it runs
regardless of log on user. This is fully multithreaded so that multiple
simultaneous users can used this Ncipher.


**NOTE** the 2kIdMaster program will only run on a box that has the NCipher installed.
So you will need to run it on a machine that has it.


 
2kIdmaster /1 will do a memory test on the GeneratePerBoxID. Run this on its own
machine as the memory for the entire machine is watched. You can use perfmon
to monitor just this process remotely however.

2kIdmaster /3 100 wallyhox will connect to the xbox. Generate an ID and send this to the 
Xbox and write it to the EEPROM. We will then read the EEPROM and send it back to the 2k
machine. We will then connect to the NCipher box on the corpnet and decode the 1024 bit
cipher of the the online key. This will be compared to what comes back from the Xbox.
The test is complete.
The 100 is the number of loops of this test. For every even loop we will set japanese.
So if you stop on an even loop your machine may reboot in japanese mode.


2kIdmaster /5 will do a parameter check test.

2kIdmaster /6 <Loop Number> will test the remote connection to the NCipher Box.

2kIdmaster /7 <Loop Number> <xbox machine name>
will read the EEPROM off an Xbox when that Xbox is running x2kIdServer.
It will then dump this to a file called EEPROM... on your drive.


2kIdmaster /8  1 will lock 0 will unlock.
This will lock or unlock your drive. Xbox needs to run X2kIdServer.


2kIdmaster /9 <directory> will open all the *.dat files in that directory
and print the eeproms contents


2kIdmaster /10 <directory of eeproms> <FlexDataFile>
This will Open every file in the directory of eeproms and compare it to the
FlexDataFile. If its in the FlexDataFile. It will be decrypted and
compared to the EEPROMS contents.
eg: 2kidmaster /10 .\SampleData\EEPROMS .\SampleData\Database\SSt....


To get the EEPROMS. You will used XEEPROMDump. This program need to be burned
to a CD with a corresponding tahoma.ttf font file. The program need to have
its font file location corrected for d:\ drive for a CD over T drive
When its for a harddrive location. Upon running this app. You can install an MU
and it will collect the EEPROM data there. You can pull it out and put it back
in. It will still collect the EEPROM data but will overwrite the file there.
Once you have this all collected. Do not look at the MU in the dash.
This will delete the contents. Instead hook it up to a dev kit and use
xbcp to copy the data off the MU. The subdiretory should be ffcc0000.

As follows
Port 1 Top is F:\ Bottom is G:\
Port 2 Top is H:\ Bottom is I:\
Port 3 Top is J:\ Bottom is K:\
Port 4 Top is L:\ Bottom is M:\


2kIdmaster /11 <Onlineencryptedkey>
This will decrypt the key by accessing the NCipher service.
eg: 2kidmaster /11 THMTSdIf/RZo2MrxcZ3equhQiDPN3/CBfndBDxIV9GWBTRyos7bnRCVyOgpvPW7QEJW74jAkFlVHuZV0wLAU3RdpchlLFI+aznSOp13NWsod2Y+jr796h6gYxIHphIOuU1mHJt/7pzOi6ROv6l1d+e08v6lyT48XUTSg3CmMDIE=

