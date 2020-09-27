// FILE:      library\hardware\pcibridge\specific\bt848\Framlist.h
// AUTHOR:    P.C.Straasheijm
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   14.10.95
//
// PURPOSE: The PIP-related classes.
//
// Discription: This is a PIP related class for DMA-chip, such as BT848 who works with a
//				risc programm


#ifndef FRAMELIST_H
#define FRAMELIST_H


#include "library\general\flists.h" 
#include "library\common\gnerrors.h"

class __far RISC_Instruction: public FNode {
	 public:
		int 			left;
		int 			width;
		
		RISC_Instruction(int start = 0, int size = 0);
		
		RISC_Instruction  * Prede(void);
		RISC_Instruction  * Succe(void);		
	 };


class __far Linelist : public FList
	 {
	
	  public:
	  
	  	int 				left, width;
	  	RISC_Instruction	* startlist;
	  	Linelist (int x, int width);
	  	~Linelist(void);
	  	Error Add_Line (int l, int w);
	  	Error Rem_Line (int l, int w);
	 };
	 

class __far FrameList {

   
   public:
   
	int						left, top;		// screen coordinates
	int 						width, height;
	Linelist  		__far	*(__far *lines_array);
	
	FrameList (int x, int y, int height, int width);
	~FrameList(void);
	
	Error Initialize(void);          
	
	void AddRect(int x, int y, int w, int h);

	void RemRect(int x, int y, int w, int h);

};                                           
	          
	          
#endif	          