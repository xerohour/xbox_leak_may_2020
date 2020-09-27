//	Copyright	Calibre UK Ltd 1999	
//	File name	Cali2c32.h
//	Description	Include file for cali2c32.lib
//				Defines all functions within the PCI I2C library
//	Version		1.0 As written
//	Date		1 December 1999

#ifndef __CALI2C32_H__
#define __CALI2C32_H__

extern __declspec(dllimport) int WINAPI setup (int, int, int);
extern __declspec(dllimport) int WINAPI sendaddress (int, int, int);
extern __declspec(dllimport) int WINAPI writebyte(int, int);
extern __declspec(dllimport) int WINAPI readbyte(int, int);
extern __declspec(dllimport) int WINAPI sendstop(int);
extern __declspec(dllimport) int WINAPI restart (int, int, int);
extern __declspec(dllimport) int WINAPI getstatus(int);
extern __declspec(dllimport) int WINAPI recover(int);
extern __declspec(dllimport) int WINAPI slavelastbyte(int);
extern __declspec(dllimport) int WINAPI dllissue(void);





#endif