////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  Request Handling Classes
//
////////////////////////////////////////////////////////////////////

#ifndef REQUESTS_H
#define REQUESTS_H

#include "library/general/lists.h"

////////////////////////////////////////////////////////////////////
//
//  Request Handle Class
//
////////////////////////////////////////////////////////////////////

class RequestHandle
	{
	protected:
		BOOL valid;
//		VDLocalMutex lock;

	public:
		RequestHandle(void) { valid = FALSE; }
		~RequestHandle(void) {}

		void Activate(void)
//			{ lock.Enter(); valid = TRUE; lock.Leave(); }
  			{ valid = TRUE; }

		void Passivate(void)
//			{ lock.Enter(); valid = FALSE; lock.Leave(); }
			{ valid = FALSE; }

		BOOL Valid(void) { return valid; }
	};

////////////////////////////////////////////////////////////////////
//
//  Request Store Class(es)
//
////////////////////////////////////////////////////////////////////

class RequestStore
	{
	//
	//  Request node class
	//  List node for storing requests
	//

	class RequestNode : public Node
		{
		public:
			RequestHandle	*	rh;
			DWORD					type;

			RequestNode(RequestHandle * rh, DWORD type) { this->rh = rh; this->type = type; }
		};

	protected:
		List requests;

	public:
		RequestStore(void);
		~RequestStore(void);

		Error AddRequest(RequestHandle * rh, DWORD type)
			{ RequestNode * rn = new RequestNode(rh, type); requests.Push(rn); GNRAISE_OK; }

		Error RemoveRequest(RequestHandle * rh);
		Error FindRequest(RequestHandle * rh, DWORD & type);
	};

#endif
