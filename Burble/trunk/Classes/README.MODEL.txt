TO UNDERSTAND THE TWO WAYS OF MAKING CALLS TO THE SERVER

First read RPCURLConnection.  This allows you to make requests. You can pass a userObj (and object) through to the callback function that you specify with a selector. The selector also receives an RPCURLResponse object containing an response, data and possibly error.

Check out: Asyncronous queues with the functions: 
	flushQueues
	fireQueueEvent
	sendXxxToServer
	sendXxxToServerCallback
	
Check out: Asynchronous single requests:
	startTryToRegister	- posts to a url with a data model attached (using the convertToData method)
	receiveTryToRegisterCallback
	startLeaveGroup - posts to a url with no object attached