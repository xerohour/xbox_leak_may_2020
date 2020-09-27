/*----------------------------------------------------------------------------
 *  Copyright © 2000 Microsoft.  ALL RIGHTS RESERVED. 
 *----------------------------------------------------------------------------
 *  Date: 03/19/01
 *  Developer: Sean Wohlgemuth
 *  Description: Windows Scripting Host Script
 *----------------------------------------------------------------------------
 */
 
var CONNECTED=false;
var DISCONNECTED=true;

 //Description: Make sure we are in the state we expect to be in
function ExpectedState(engine, fState){
	if (fState==engine.WaitForDisconnect(1000)){
		FAIL();
	}//endif
}//endmethod

//Description: Confirm USERINFO ACK
function ConfirmUserInfoAck(engine){
	if (true==engine.WaitForReply(60000)){
		FAIL();
	}//endif
	if (MSGTYPE_ACK!=engine.GetReplyType()){
		FAIL();
	}//endif
	if (QMSG_USER_INFO!=engine.GetACKElement("AckMsgType")){
		FAIL();
	}//endif
	if (S_OK!=engine.GetACKElement("hr")){
		FAIL();
	}//endif
}//endmethod

//Description: Confirm the number of items returned in the list reply
function ConfirmListReplyItemCount(engine, iItems){
	if (true==engine.WaitForReply(60000)){
		FAIL();
	}//endif
	if (QMSG_LIST_REPLY!=engine.GetReplyType()){
		FAIL();
	}//endif
	if (S_OK!=engine.GetListReplyElement("hr")){
		FAIL();
	}//endif
	if (iItems!=engine.GetListReplyElement("numitems")){
		FAIL();
	}//endif
}//endmethod

//Description: Confirm the hr value returned in the list reply
function ConfirmListReplyHResult(engine,hr){
	if (true==engine.WaitForReply(60000)){
		FAIL();
	}//endif
	if (QMSG_LIST_REPLY!=engine.GetReplyType()){
		FAIL();
	}//endif
	if (hr!=engine.GetListReplyElement("hr")){
		FAIL();
	}//endif
}//endmethod

//Description: Wait for a tickle to be returned
function WaitForTickle(engine){
	if (true==engine.WaitForTickle(60000)){
		FAIL();
	}//endif
}//endmethod
