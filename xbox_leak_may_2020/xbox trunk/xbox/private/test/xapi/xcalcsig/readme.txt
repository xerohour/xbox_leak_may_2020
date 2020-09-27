This is a program to test the XCalculateSignature* APIs. To run this test, copy the test file (test.dat) in the testdata directory to the t:\ directory of the Xbox and run this program from there. The program should run indefinitely.

The program will try to read the file and do piecemeal signature calculation in random-sized chunks. The signature will be compared to a reference to make sure signatures generated are identical irrespective of the chunk sizes.

This program also randomly corrupts the data and makes sure that the signature generated does not match with the reference.

You can feel free to replace test.dat with your favorite file.
