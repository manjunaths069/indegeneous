#ifndef __PEMAPI_h__
#define __PEMAPI_h__
/*
 * $Id: pemapi.h,v 1.67 1998/10/26 20:43:07 lkopnick Exp $
 *
 * File: pemapi.h
 *---------------------------------------------------------------
 * Description: PEM API for user developed applications
 *	PLEASE see file 'docs' and
 *	and the PATROL Event Manager Programming API
 *	for additionnal documentation.
 *	NOTE:
 *	The word 'application' or 'program' refer to the 
 *	c-program using the API.
 *----------------------------------------------------------------
 * Author       : Max Mattini
 * Creation date: Mar 10, 1994
 *
 * Last check-in date: $Date: 1998/10/26 20:43:07 $
 *
 * Copyright (c) 1995 BMC Software, Inc.
 *----------------------------------------------------------------
 * *** Modification history is at the bottom of this file ***
 */

#define PEMAPI_RCS_ID "$Id: pemapi.h,v 1.67 1998/10/26 20:43:07 lkopnick Exp $"

#include <stdio.h> 	/* import FILE 		*/
#include <time.h> 	/* import time_t 	*/

#if defined(_WIN32) && !defined(_PATROL_BUILD) && !defined(STATIC_PEMAPI)
#  define DLL_IMPORT	__declspec(dllimport)
#else
#  define DLL_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PEMN_API_VERSION "3.1"

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	please refer to file 'docs' for Manual Pages of API or
 *	the PATROL Event manager's API Guide.
 *	
 *	This header file is structured as following:
 *
 * API Data Structures
 * 	Global Enumeration 
 * 	Misc. definitions
 * 	Object Handle 
 * 	Routine Handlers 
 * 	Routine callback 
 * 	Array type and indexes
 * 
 * API Routines
 * 	Event Access Routines
 * 	Connection Management
 * 	Security Management	
 * 	Send/Receive
 * 	Event Management
 * 	Application Management
 * 	Data Management
 * 	Miscellaneous routine	
 * 	Specific Blocking Mode
 * 	Read only API
 * 
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Global enumeration
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

typedef enum {
	EV_TYPE_INFORMATION = 1, /*! should be one*/
	EV_TYPE_CHANGE_STATUS,
	EV_TYPE_ERROR,
	EV_TYPE_WARNING,
	EV_TYPE_ALARM, 
	EV_TYPE_RESPONSE,
        EV_TYPE_ACK,
	EV_TYPE_MAX
	} PemnEventTypeEnum;

typedef enum {
	EV_TYPE_OPEN = 1, /*! should be one*/
	EV_TYPE_ACKNOWLEDGED,
	EV_TYPE_CLOSED,
	EV_TYPE_ESCALATED,
	EV_TYPE_DELETED 
	} PemnEventStatusEnum;

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Misc. definitions
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/* 
 * typedef: 
 *	PemnClientData
 *------------------------------------------------------------------
 * Description:
 * 	generic client data type
 *------------------------------------------------------------------ 
 */
typedef void * PemnClientData; 


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Object Handle
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/* 
 * typedef: 
 *	PemnEventHandle
 *------------------------------------------------------------------
 * Description:
 * 	The handle used by the program to access any event 
 *	attributes (description, type, status, ..).
 *------------------------------------------------------------------ 
 */
typedef void* PemnEventHandle;


/* 
 * typedef: 
 *	PemnCommHandle
 *------------------------------------------------------------------
 * Description:
 * 	Communication handle to the agent/PEM returned by PemnOpen
 *	This handle is used with all subsequent call to the agent (e.g
 *	PemnSend, PemnReceive).
 *------------------------------------------------------------------ 
 */
typedef void *PemnCommHandle;


/* 
 * typedef: 
 *	PemnCommHandle
 *------------------------------------------------------------------
 * Description:
 * 	Event report handle. 
 *	Currently it is a string but this may change in the future.
  *------------------------------------------------------------------ 
 */
typedef char* PemnReportHandle;



/* 
 * typedef: 
 *	PemnVarObjHandle
 *------------------------------------------------------------------
 * Description: V31
 * 	Abstract PATROL variable type handle.
 *      Returned by the API in answer to PemnGetVarObj(). 
 *	Currently it is a string but this may change in the future.
 *------------------------------------------------------------------ 
 */
typedef char *PemnVarObjHandle;


/* 
 * typedef: 
 *	PemnApplObjHandle
 *------------------------------------------------------------------
 * Description:
 * 	Abstract application type handle.
 *      Returned by the API in answer to PemnGetApplObj(). 
 *	Access to the attributes of the application type object is
 *	achieved using PemnGetApplObjxxx
 *------------------------------------------------------------------ 
 */
typedef void *PemnApplObjHandle;


/* 
 * typedef: 
 *	PemnInstObjHandle
 *------------------------------------------------------------------
 * Description:
 * 	Abstract application instance handle.
 *      Returned by the API in answer of PemnGetInstObj(). 
 *	Access to the attributes of the application instance object is
 *	achieved using PemnGetInstObjxxx
 *------------------------------------------------------------------ 
 */
typedef void *PemnInstObjHandle;


/* 
 * typedef: 
 *	PemnParamObjHandle
 *------------------------------------------------------------------
 * Description:
 * 	Abstract parameter handle.
 *      Returned by the API in answer of PemnGetParamObj() or
 *	PemnGetComputerParamObj().  
 *	Access to the attributes of the application type object is
 *	achieved using PemnGetParamObjxxx
 *------------------------------------------------------------------ 
 */
typedef void *PemnParamObjHandle;


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Routine Handlers
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/*
 * Constant:  (V31)
 *------------------------------------------------------------------
 * Purpose : The API passes the constants below to the message handler.
 *	A default message handler is called by the API but the
 *	programmer can define his/her own message handler (see 
 *	PemnRegisterUserMessageHandler).
 *------------------------------------------------------------------
 */
#define	PEMN_MSG_WARN  0
#define	PEMN_MSG_INFO  1 
#define	PEMN_MSG_ERROR 2
typedef void (*PemnUserMessageHandler) (char *pcMessageToDisplay, int iMsgType);


/* 
 * typedef: 
 *	PemnReceiveEventHandler
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives an event matching the user defined filter
 *	criteria it calls the EventReceivedHandler.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	oEventHandle - event handle
 *------------------------------------------------------------------ 
 */
typedef void (*PemnReceiveEventHandler)(
	PemnCommHandle 	hComm,
	PemnEventHandle oEventHandle
	);


/* 
 * typedef: 
 *	PemnReportEventHandler (V31)
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives an event report
 *	it calls the PemnReportEventHandler registereed bt
 *	PemnEventReport.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	hReport - report handle.
 *------------------------------------------------------------------ 
 */
typedef void (*PemnReportEventHandler)(
	PemnCommHandle 		hComm,
	PemnReportHandle	hReport
	);

/* 
 * typedef: 
 *	PemnArchiveEventHandler (V31)
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives the result of event archiving from
 *	the agent, it calls the PemnArchiveEventHandler registered
 *	by PemnEventArchive 
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	pcResult - archive results
 *------------------------------------------------------------------ 
 */
typedef void (*PemnArchiveEventHandler)(
	PemnCommHandle 	hComm,
	char *pcResult
	);

/* 
 * typedef: 
 *	PemnQueryEventHandler 
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives an event as a result of a query 
 *	(see PemnEventQuery & PemnEventRangeQuery) it calls the 
 *	QueryReceivedHandler.
 *      The PEM API differentiates between events resulting from a query
 *	and the one from a normal receive (see PemnEventReceivedHandler).
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	oEventHandle - event handle
 *------------------------------------------------------------------ 
 */
typedef PemnReceiveEventHandler PemnQueryEventHandler;




/* 
 * typedef: 
 *	PemnGetEventClassHandler 
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives the result of a get event class request 
 *	(see PemnGetEventClass) it calls the program defined
 *	PemnGetEventClassHandler.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	pcEventClassText - is the string returned by the agent
 *		containing all fields from the event catalog
 *------------------------------------------------------------------ 
 */
typedef void (*PemnGetEventClassHandler)(
	PemnCommHandle 	hComm,
	char 	       *pcEventClassText
	);



/* 
 * typedef: 
 *	PemnGetEventClassListHandler 
 *------------------------------------------------------------------
 * Description:
 * 	When the PEM API receives the result of a get event class list request 
 *	(see PemnGetEventClassList) it calls the program defined
 *	PemnGetEventClassListHandler.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - communication handle
 *	pcEventClassList - is the string returned by the agent
 *		containing the list of event classes.
 *		It contains  ERROR if event catalog could not be found.
 *		It contains NONE if no event classes exists.
 *------------------------------------------------------------------ 
 */
typedef void (*PemnGetEventClassListHandler)(
	PemnCommHandle 	hComm,
	char* 		pcEventClassList
	);


/* 
 * typedef: 
 *	PemnGetListHandler
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls this programmer's defined handler
 * 	when a list object is received from the agent.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - The communication handle
 *	pcList - List (a string) where each component is separated by '\n'.
 *		The List format is as follow:
 *
 *	Success =>
 *		<full-obj-name> '\n'
 *		<comp1> '\n'
 *		<comp2> '\n'
 *		.........
 *		<compn> '\n'
 *	Success but empty list =>
 *		<full-object-name> '\n'
 *		"NULL_OBJ" '\n'
 *	Error (object not found) =>
 *		<full-object-name> '\n'
 *		"ERROR" '\n'
 *
 */
typedef void (*PemnGetListHandler)(
	PemnCommHandle	 hComm,	 	
	char		*pcList		 
	);

/* 
 * typedef: (V31)
 *	PemnGetVarObjHandler
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls the programmer's defined handler
 * 	when an PATROL variable object is received from the agent.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - The communication handle
 *	hObj -  Handle to the application object.
 */
typedef void (*PemnGetVarObjHandler)(
	PemnCommHandle	  hComm,
	PemnVarObjHandle hObj	 
	);


/* 
 * typedef: 
 *	PemnGetApplObjHandler
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls the programmer's defined handler
 * 	when an application object is received from the agent.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - The communication handle
 *	hObj -  Handle to the application object.
 */
typedef void (*PemnGetApplObjHandler)(
	PemnCommHandle	  hComm,
	PemnApplObjHandle hObj	 
	);

/* 
 * typedef: 
 *	PemnGetInstObjHandler
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls the programmer's defined handler
 * 	when an application instance object is received from the agent.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - The communication handle
 *	hObj -  Handle to the application instance object.
 */
typedef void (*PemnGetInstObjHandler)(
	PemnCommHandle	  hComm, 	
	PemnInstObjHandle hObj		 
	);

/* 
 * typedef: 
 *	PemnGetParamObjHandler
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls the programmer's defined handler
 * 	when a parameter object is received from the agent.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm - The communication handle
 *	hObj -  Handle to the parameter object.
 */
typedef void (*PemnGetParamObjHandler)(
	PemnCommHandle	   hComm,
	PemnParamObjHandle hObj	 	
	);


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Routine callbacks
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/* 
 * typedef: 
 *	PemnCommCallback
 *------------------------------------------------------------------
 * Description:
 * 	The PEM API calls the programmer's defined callback
 * 	when the connection is opened/closed.
 *------------------------------------------------------------------
 * Arguments:
 *	pTheObject - Reserved for future use.
 *	pClientData - Data the programmer had passed in PemnOpen().
 *	hComm - The communication handle
 */
typedef void (*PemnCommCallback)(
	void		*pTheObject,	/* reserved for future use */	 
	void		*pClientData,
	PemnCommHandle	 hComm	 	
	);



typedef void (*PemnBDisconnectCallback) (
	PemnCommHandle          hComm,
	PemnClientData  	pClientData
	);


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Array types and indexes
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/* 
 * typedef: (V31)
 *	PemnApplAttrEnum, PemnApplAttrArray
 *------------------------------------------------------------------
 * Description: 
 *	The 'PemnApplAttrArray' type defines the 
 *	read-only attributes of an application.
 *	The 'PemnApplAttrEnum' defines the index to these attributes.
 *------------------------------------------------------------------ 
 */
typedef enum {
	PEMN_APPLNAME=0,	/* should be always ZERO */
	PEMN_APPLWORSTINST,
	PEMN_APPLSTATE,
	PEMN_APPLMASTER,
	PEMN_APPLMINOR,
	PEMN_APPL_MAX_ATTR	/* should be always last */
} PemnApplAttrEnum;

typedef char* PemnApplAttrArray[PEMN_APPL_MAX_ATTR];





/* 
 * typedef: (V31)
 *	PemnInstAttrEnum, PemnInstAttrArray
 *------------------------------------------------------------------
 * Description: 
 *	The 'PemnInstAttrArray' type defines the 
 *	read-only attributes of an instance.
 *	The 'PemnInstAttrEnum' defines the index to these attributes.
 *------------------------------------------------------------------ 
 */
typedef enum {
	PEMN_APPLINSTNAME=0,	/* should be always ZERO */
	PEMN_APPLINSTWORSTPARAM,
	PEMN_APPLINSTSTATUS,
	PEMN_APPLINSTRULESTATE,
	PEMN_INSTCREATEICON,
	PEMN_APPLINSTPARENT,
	PEMN_INST_MAX_ATTR	/* should be always last */
} PemnInstAttrEnum;

typedef  char* PemnInstAttrArray[PEMN_INST_MAX_ATTR];

/* 
 * typedef: (V31)
 *	PemnParamAttrEnum, PemnParamAttrArray
 *------------------------------------------------------------------
 * Description: 
 *	The 'PemnParamAttrArray' type defines the 
 *	read-only attributes of an parameter.
 *	The 'PemnParamAttrEnum' defines the index to these attributes.
 *------------------------------------------------------------------ 
 */
typedef enum {
	PEMN_PARAMNAME=0,	/* should be always ZERO */
	PEMN_PARAMCURRENTTIME,
	PEMN_PARAMPOLLINGINT,
	PEMN_PARAMRETRIES,
	PEMN_PARAMCURRENTVALUE,
	PEMN_PARAMSTATE,
	PEMN_PARAMOUTPUTMODE,
	PEMN_PARAMAUTOSCALE,
	PEMN_PARAM_Y_AXIS_MIN,
	PEMN_PARAM_Y_AXIS_MAX,
	PEMN_PARAM_MAX_ATTR	/* should be always last */
} PemnParamAttrEnum;

typedef  char* PemnParamAttrArray[PEMN_PARAM_MAX_ATTR];

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Event Access Routines
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/*
 * Function:  
 *	PemnGetxxx - access  methods
 *------------------------------------------------------------------
 * Purpose : 
 *	Get the event attribute specvified by xxx.
 *------------------------------------------------------------------
 * Arguments:
 *	pEhandle - Handle to the event.
 *------------------------------------------------------------------
 *		
 */

DLL_IMPORT int    PemnGetEid(	PemnEventHandle pEhandle);
DLL_IMPORT int    PemnGetStatus(	PemnEventHandle pEhandle); 
DLL_IMPORT int    PemnGetSeverity(	PemnEventHandle pEhandle); 

#define PemnGetType     PemnGetSeverity

DLL_IMPORT time_t PemnGetTime(	PemnEventHandle pEhandle); 
DLL_IMPORT char  *PemnGetNode(	PemnEventHandle pEhandle); 
DLL_IMPORT char  *PemnGetOrigin(	PemnEventHandle pEhandle); 
DLL_IMPORT char  *PemnGetDesc(	PemnEventHandle pEhandle); 

/* new in V31 */
DLL_IMPORT char* PemnGetOwner(	PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetDiary(	PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetHandler(	PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetExpectancyStr(
			PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetEventCatalogName(
			PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetClassName(	PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetArgs(	PemnEventHandle pEhandle);
DLL_IMPORT char* PemnGetSourceId(	PemnEventHandle pEhandle);

DLL_IMPORT int   PemnGetNseverity(	PemnEventHandle pEhandle);


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Connection Management
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */
	
/*
 * Function:  
 *	PemnOpen
 *------------------------------------------------------------------
 * Purpose : 
 * 	Open a connection to a PATROL agent.
 *	The caller specifies the host name and port number. For other
 *	communication attributes see PemnSet/GetCommAttributes below.
 *
 * 	When the connection is opened successfully the OpenCallback is called.
 * 	If the connection could not be opened then CloseCall back is called.
 *
 *	IMPORTANT pOpenCallback should call the PemnSendIdentity()
 *      immediatly to authenticate the connection. (V31*)
 *------------------------------------------------------------------
 * Arguments:
 *	iPort - port number of the agent/PEM
 *	pcRemoteHost - Host name of the agent/PEM.
 *	pCloseCallback, pOpenCallback - PEM API calls these
 *		routines when the connection with agent/PEM is closed/opened
 *	pCloseClientData, pOpenClientData - The PEM API passes 
 *		pCloseClientData (resp. pOpenClientData) 
 *		when calling pCloseCallback (resp. pOpenCallback).
 *	phComm - address in the application of the variable which 
 *		will holds the communication handles.
 *		The PEM API store internally the address of where to 
 *		return the Comm. handler.
 *
 *		THE VARIABLE HOLDING THIS COMMUNICATION
 *		HANDLE SHOULD BE A STATIC VARIABLE IN THE PROGRAM.
 *		
 */
DLL_IMPORT void PemnOpen(
	int 			iPort,
	char 		       *pcRemoteHost,
	PemnCommCallback  	pCloseCallback,
	PemnClientData		pCloseClientData,				
	PemnCommCallback	pOpenCallback,
	PemnClientData		pOpenClientData,
	PemnCommHandle         *phComm			
	);

/* 
 * Function: 
 * 	PemnClose
 *------------------------------------------------------------------
 * Purpose : 
 * 	Close communication with peer 
 *------------------------------------------------------------------
 */
DLL_IMPORT void PemnClose(	PemnCommHandle hComm);


/*
 * Function:  
 * 	PemnBOpen (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Open a connection with agent and block until
 *	1- Connection is established
 *	2- Agent authenticates connection.
 *------------------------------------------------------------------
 * Arguments:
 *	iPort - port number of the agent/PEM
 *	pcHostName - Host name of the agent/PEM.
 *	pcUserName - User name for authentication
 *	pcEncryptedPassword - password already encrypted using PemnEncrypt()
 *	iMaxRetry - Maximum time the API will try to connect to
 *		this agent.
 *	pDisconnectCallback, pClientData -
 *		The callback defined by the programmer to be called
 *		if agent disconnects during this operation.
 *	pClientData - client data passed to the above callback.
 *	phComm - address of the application of the variable which 
 *		will holds the communication handles.
 *		The PEM API store internally the address of where to 
 *		return the Comm. handler.
 *
 *		THE VARIABLE HOLDING THIS COMMUNICATION
 *		HANDLE SHOULD BE A STATIC VARIABLE IN THE PROGRAM.
 *------------------------------------------------------------------
 * Blocking call: V3N
 *	Returns (Read-Only) the API message string.
 *
 *------------------------------------------------------------------
 *		
 */
DLL_IMPORT char*	PemnBOpen(
	PemnCommHandle          *phComm,
	char 			*pcHostName,
	int			iPort,
	char			*pcUserName,			
	char			*pcEncryptedPassword,
	int			iMaxRetry,
	PemnBDisconnectCallback pDisconnectCallback,
	PemnClientData  	pClientData
	);	

/*
 * Function:  
 * 	PemnBClose (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Close a connection to an agent an block until it is done.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnBOpen)
 */
DLL_IMPORT char*	PemnBClose(
	PemnCommHandle         hComm);

/* 
 * Function: 
 * 	PemnLoop
 *------------------------------------------------------------------
 * Purpose : 
 * 	Application  main loop (see example 1 in example.c) 
 *------------------------------------------------------------------
 */
DLL_IMPORT void PemnLoop(void);


#define PEMN_UDP_TRANSPORT "UDP"
#define PEMN_TCP_TRANSPORT "TCP"
/* 
 * Function: 
 * 	PemnSetCommAttributes
 *------------------------------------------------------------------
 * Purpose : 
 *	Set the communication attributes used by the API.
 *------------------------------------------------------------------
 *	(V31)
 *	minimum heartbeat    	 5*1000 - 5  seconds 
 *	default heartbeat       30*1000	- 30 seconds 
 *	maximum heartbeat    30*60*1000 - 30 minutes 
 *	
 *	minimum timeout	       	  300 - 0.3 sec. 
 *	default timeout        2*1000 - 2.0 seconds 
 *	maximum timeout	    3*60*1000 - 3 minutes 
 *	
 *	minimum retries	            2
 *	default retries	            4
 *	maximum retries	           10
 *	transport is PEMN_UDP_TRANSPORT (default) or PEMN_TCP_TRANSPORT
 *
 * NOTE:
 * 	TCP support is only available with 3.1 agents.
 *------------------------------------------------------------------
 */
DLL_IMPORT void PemnSetCommAttributes(
 	int 	iHeartbeat,
 	int 	iTimeout,
 	int 	iRetries,
	char 	*pcTransport
	);


/* 
 * Function: 
 * 	PemnGetCommAttributes (V31)
 *------------------------------------------------------------------
 * Purpose : 
 *	Get the communication attributes used by the API.
 *------------------------------------------------------------------  
 * Defaults values:
 * 	heartbeat  = 300;
 * 	timeout    = 1000;	1-second
 * 	retries    = 3;
 * Returns:
 *      The current transport protocol:
 *	PEMN_UDP_TRANSPORT (default) or PEMN_TCP_TRANSPORT
 *------------------------------------------------------------------
 */
DLL_IMPORT char* PemnGetCommAttributes(
 	int 	*piHeartbeat,
 	int 	*piTimeout,
 	int 	*piRetries
	);


/*
 * Function:  (V31)
 * 	PemnBPing
 *------------------------------------------------------------------
 * Purpose :
 *	PATROL Ping an agent on <HostName> and <Port> 	
 *
 *------------------------------------------------------------------
 * Returns: PEMN_OK if PATROL agent is in OK state
 *	"ALARM\n" agent is in ALARM state
 *	"WARN\n" agent is in WARN state
 *	"OFFLINE\n" agent is in OFFLINE state
 *	"VOID\n" agent is in VOID state
 *	"UNKNOWN\n" agent is in UNKNOWN state
 *
 *	PEMN_INTERRUPT if no answer during <MsMaxTimeToWait> milliseconds.
 *
 * NOTE:    You should call PemnCheckInit() before the first call to 
 * 	    PemnBPing() otherwise your program may crash.
 * Example: 
 *	    (void) PemnCheckInit(); 	
 *	     for (i=0; i<12;i++){
 *		printf("ping to %s on port %d is:\n",
 *			"myhost", 100+i
 *			PemnBPing("myhost", 100+i,300)
 *			);
 *	     }
 * 
 */
DLL_IMPORT char* PemnBPing(
	char 	*pcHostName,
	int	iPort,
	int	iMsMaxTimeToWait
	);


/*
 * Function:  (V31)
 * 	PemnSetLocalPort, PemnGetLocalPort
 *------------------------------------------------------------------
 * Purpose :
 *	Set/Get the local port. To be used if
 *	API user communicate with agent accross
 *	fire wall.
 *
 *------------------------------------------------------------------
 */
DLL_IMPORT void PemnSetLocalPort(int iLocalPort);
DLL_IMPORT int  PemnGetLocalPort(void);


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Security Management
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/*
 * Function:  
 * 	PemnSendIdentity 
 *------------------------------------------------------------------
 * Purpose :
 *	Send username and encrypted password to agent. If the program
 *	does not send the username/password the API will not
 *	work.
 *	IMPORTANT This routine should be called immediatly
 *	from the Open callback (see PemOpen)
 *      Returns TRUE on success, FALSE on failure
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcUserName - user name to be used to log at the agent host 
 *	pcEncryptedPasswd - the encrypted password the agent will use to 
 *	 	authenticate the user. 
 */
DLL_IMPORT int PemnSendIdentity(
	PemnCommHandle 	hComm,
	char 		*pcUserName, 
	char 		*pcEncryptedPasswd
	);

/*
 * Function:  
 * 	PemnEncrypt 
 *------------------------------------------------------------------
 * Purpose :
 *	Encrypt the password 
 *	The result can be stored in a configuration file.
 *------------------------------------------------------------------
 * Arguments:
 *	buff- buffer to contain the encrypted password
 *	iLen - the length of the password (should be >= 50) 
 *	pcPasswd - tthe original password 
 */
DLL_IMPORT int PemnEncrypt(char *buff, int iLen,  char *pcPasswd);


/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Send/Receive
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

#define PEMN_ARG_STRING 	"RE_ARG_STRING"
#define PEMN_ARG_FLOAT 		"RE_ARG_FLOAT"
#define PEMN_ARG_INT 		"RE_ARG_INT"

#define STD_EVENT_CATALOG 	"0"

/* 
 * Function:  
 * 	PemnSend
 *------------------------------------------------------------------
 * Purpose : 
 * 	Send an event with a dynamic nb. or aguments. 
 *	The event-class should have been defined 
 *	in an event catalog (or the Standard Catalog).
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcEventCatalogName- The name of the event catalog 
 *		(or STD_EVENT_CATALOG if using the
 *	  	standard event catalog of PATROL).
 *	pcEventClassName- The name of the event class.
 *	pcEventOrigin - the origin of this event (will appear in PEM windows)
 *	pcOldState, pcNewState - Can be one of the following
 * 		"VOID" | "OFFLINE" | "OK" | "WARN"| "ALARM"
 * 	eEventType - event type
 *	iEventNseverity - event severity [1..5]
 *	... - zero to 20 arguments passed to the event description.
 * Returns:
 *  	Return the local id of the event sent. (V31)
 */
DLL_IMPORT int PemnSend(
	PemnCommHandle 	  hComm,			
	char 		 *pcEventCatalogName, 
	char 		 *pcEventClassName, 
	char 		 *pcEventOrigin,
	char		 *pcOldState, 
	char		 *pcNewState,
	PemnEventTypeEnum eEventType,
	int  		 iEventNseverity,
	...
	);
/* 
 * Function:  
 * 	PemnBSend (V31)
 *------------------------------------------------------------------
 * Purpose : 
 * 	Send an event with a dynamic nb. or aguments. 
 *	The event-class should have been defined 
 *	in an event catalog (or the Standard Catalog).
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcEventCatalogName- The name of the event catalog 
 *		(or STD_EVENT_CATALOG if using the
 *	  	standard event catalog of PATROL).
 *	pcEventClassName- The name of the event class.
 *	pcEventOrigin - the origin of this event (will appear in PEM windows)
 *	pcOldState, pcNewState - Can be one of the following
 * 		"VOID" | "OFFLINE" | "OK" | "WARN"| "ALARM"
 * 	eEventType - event type
 *	iEventNseverity - event severity [1..5]
 *	... - zero to 20 arguments passed to the event description.
 * Returns:
 *  	Return the local id (read-only string) of the event sent. (V31)
 */
DLL_IMPORT char* PemnBSend(
	PemnCommHandle 	  hComm,
	int		 iMsMaxTimeToWait,			
	char 		 *pcEventCatalogName, 
	char 		 *pcEventClassName, 
	char 		 *pcEventOrigin,
	char		 *pcOldState, 
	char		 *pcNewState,
	PemnEventTypeEnum eEventType,
	int  		 iEventNseverity,
	...);


/*
 * Function:  
 * 	PemnReceive (V31*)
 *------------------------------------------------------------------
 * Purpose : 
 * 	If a received event matches the filter then the EventReceivedHandler 
 *	is called. 
 *------------------------------------------------------------------
 * Arguments:
 *	Start Time Filter - format "MMddhhmm[yy]" 
 *	End Time Filter   - format "MMddhhmm[yy]"
 *      StatusMask Filter- The value of pcStatusMask is a string containing 
 *		one or more Status-tag separated by commas:
 *		O: OPEN,
 *		A: ACKNOWLEDGED,
 *		C: CLOSED,
 *		E: ESCALATED,
 *		D: DELETED.
 *
 *		Examples:
 *		"O,A,C,D" 	all statuses except ESCALATED
 *		"O,A,C,E,D" 	all statuses
 *		"O,C"		only OPEN and CLOSED events
 *      TypeMask Filter- The value of pcTypeMask is a string containing 
 *		one or more Type-tag separated by commas:
 *		I: INFORMATION,
 *		S: CHANGE_STATUS,
 *		E: ERROR,
 *		W: WARNING,
 *		A: ALARM, 
 *		R: RESPONSE.
 *
 *		Examples:
 *		"S,E,W,A,R" 	all types except INFORMATION
 *		"I,S,E,W,A,R" 	all types
 *		"W,A"		only WARNING and ALARM events
 * 	pcNodeFilter - Node Name
 * 		Name of the node where the event happened or "" for any node.
 * 	pcOriginFilter - Origin Filter:
 * 		Name of the application where the event happened 
 *		or "" for any origin.
 * 	pcPatternFilter - Pattern Filter 
 * 		Any pattern to match in the description of the event.
 *	pEventHandler - the receive event handler
 *	pcEidRange - (V31*) the string describing the event-id range.
 *		valid values: 
 *		x	The range is reduced to the value of x
 *		x/y	any value between and including x, y
 *		-/y	any positive value equal or smaller than y
 *		x/-	any positive value equal or bigger  than x
 *			x and y are positive cardinal value smaller 
 *			than xFFFFFFFF.
 *		-/-	any positive cardinal value smaller than xFFFFFFFF
 *	
 *  	pcEvClass - (V31*) The exact match of event class or "" for any classe.
 */
DLL_IMPORT void PemnReceive(	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusMaskFilter,
	char		*pcTypeMaskFilter,
	char 		*pcNseverityFilter,/*V31*/
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	PemnReceiveEventHandler pEventHandler
	);	

	
/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Event Management
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

#define PEMN_ACKNOWLEDGED "ACKNOWLEDGED"
#define PEMN_CLOSED 	  "CLOSED"
#define PEMN_DELETED 	  "DELETED"

/*
 * Function:  
 * 	PemnEventRangeManage 
 *------------------------------------------------------------------
 * Purpose : 
 *	Manage all events in the specified event-id range.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcEidRange - the string describing the event-id range.
 *		valid values: 
 *		x	The range is reduced to the value of x
 *		x/y	any value between and including x, y
 *		-/y	any positive value equal or smaller than y
 *		x/-	any positive value equal or bigger  than x
 *			x and y are positive cardinal value smaller 
 *			than xFFFFFFFF.
 *		-/-	any positive cardinal value smaller than xFFFFFFFF
 *	pcNewStatus - The desired event new event status
 *		Can be one of the following:
 *		PEMN_ACKNOWLEDGED 
 * 		PEMN_CLOSED 	
 *		PEMN_DELETED 	
 *	pcUsrName - the event handler username which will appear 
 *		in the event diary.	
 *	iMaxCount - max number of event to be affected by this operations.
 *		
 *	The choice of iMaxCount should be judiscious since big values mean
 *	'more work' ro be done by the agent.
 *
 *	Managing events from the API follows the same rules of managing
 *	event from PEM or PATROL consoles: you cannot acknowledge a closed
 *	event, etc..
 *
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise	
 *  
 */
DLL_IMPORT int PemnEventRangeManage(	
	PemnCommHandle 	hComm,
	char		*pcEidRange,
	char		*pcNewStatus,
	char		*pcUsrName,
	int 		iMaxCount
	);

/*
 * Function:  
 * 	PemnEventRangeQuery (V31*)
 *------------------------------------------------------------------
 * Purpose :
 *	Query all events in the specified event-id range.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcEidRange - the string describing the event-id range.
 *		valid values: 
 *		x	The range is reduced to the value of x
 *		x/y	any value between and including x, y
 *		-/y	any positive value equal or smaller than y
 *		x/-	any positive value equal or bigger  than x
 *			x and y are positive cardinal value smaller 
 *			than xFFFFFFFF.
 *		-/-	any positive cardinal value smaller than xFFFFFFFF
 *	iQueryLength - max number of events to be returned by this query.
 *	pQueryEventHandler - (V31*) the handler to be called when an event
 *	     	matching the range is returned by the agent 
 *
 *	The choice of iQueryLength should be judiscious since big values mean
 *	'more work' ro be done by the agent.
 *
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 */
DLL_IMPORT int PemnEventRangeQuery(	
	PemnCommHandle 		hComm,
	char			*pcEidRange,
	int 			iQueryLength,
	PemnQueryEventHandler   pQueryEventHandler 
	);


/*
 * Function:  
 * 	PemnSetPersistentFilter,PemnBSetPersistentFilter (V31*)
 *------------------------------------------------------------------
 * Purpose :
 *	Update in the agent the persistent filter for this connnection 
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcStartTimeFilter,...,pcPatternFilter - 
 *		see PemnReceive.
 *
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *	
 */
DLL_IMPORT int PemnSetPersistentFilter(
	PemnCommHandle 	hComm,	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusMaskFilter,
	char		*pcTypeMaskFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass
	);
DLL_IMPORT char* PemnBSetPersistentFilter(
	PemnCommHandle 	hComm,	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	int 		iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnEventManage
 *------------------------------------------------------------------
 * Purpose :
 *	Manage the event.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pEh - the event handler.
 *	pcNewStatus - The desired event new event status
 *		Can be one of the following:
 *		PEMN_ACKNOWLEDGED 
 * 		PEMN_CLOSED 	
 *		PEMN_DELETED 
 *
 *	Managing events from the API follows the same rules of managing
 *	event from PEM or PATROL consoles: you cannot acknowledge a closed
 *	event, etc..
 *
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *	
 */
DLL_IMPORT int PemnEventManage(
	PemnCommHandle 		hComm,	
	PemnEventHandle		pEh, 
	char		       *pcNewStatus
	);

/*
 * Function:  
 * 	PemnAddDiary
 *------------------------------------------------------------------
 * Purpose :
 *	Add text to the diary of the event.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pEh - the event handler.
 *	pcTextToAdd - text to add to the diary
 *
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *	
 */
DLL_IMPORT int PemnAddDiary(
	PemnCommHandle 		hComm,	
	PemnEventHandle		pEh, 
	char 			*pcTextToAdd
	);

/*
 * Function:  
 * 	PemnEventQuery, PemnBEventQuery (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Query all events matching the specified filter.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcStartTimeFilter,...,pcPatternFilter - 
 *		see PemnReceive.
 *	iQueryLength - max number of events to be returned by this query.
 *
 *	pQueryEventHandler -  the handler to be called when an event
 *	     	matching the range is returned by the agent 
 *
 *	pcEvSep - String delimiting two events.
 *	pcEvFormat - the format string in which the user specifies
 *		the fields to be extracted from the event catalog.
 *	Any of the following PEM macros can be used:
 *	%{EV_ID}		event id
 *	%{EV_STATUS}		event status: OPEN, CLOSE....
 *	%{EV_TYPE}		event type: ALARM, WARNING,....
 *	%{EV_TIME}		time the event occured
 *	%{EV_NODE}		node where this event happened
 *	%{EV_ORIGIN}		the origin or context in which is event happened
 *	%{EV_DESC}		event description with dynamic argument(s)
 *	%{EV_DIARY}		the diary of this event
 *	%{EV_HANDLER}		the handler of this event
 *	%{EV_OWNER}		the owner of this event (typically a 
 *				UNIX account name)
 *	%{EV_NAME}		the name of the event class 
 *	%{EV_CLASS_NAME}	(same as EV_NAME), it is the name of the 
 *				event class
 *	%{EV_CATALOG}		the name of the event catalog (e.g the 
 *				appl. name).
 *	%{EV_EXPECTANCY}	the life expectancy of the event
 *	%{EV_NSEVERITY}		the numerical severity [1,5] of the event
 *	%{EV_ARGS}		the dynamic arguments separated by tabs 
 *				passed to this event when it was triggered.
 *	%{EV_ARG1}		the first dynamic argument.
 *	%{EV_ARG2}		the second dynamic argument.
 *	%{EV_EXPERT_ADVICE}	The expert advice
 *      %{EV_SNMP_SUPPORT}	snmp support
 *      %{EV_CTG_DESC}		Description of event 
 *      %{EV_ESCL_TEXT}		Escalation command script.	
 *      %{EV_NOTIFY_TEXT}	Notification command script.
 *      %{EV_ACK_TEXT}		Ack command text 
 *
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 *	The choice of iQueryLength should be judiscious since big values mean
 *	'more work' ro be done by the agent.
 *
 * Blocking call: V3N
 *	Returns (Read-Only) the events according to format 
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *	
 */
DLL_IMPORT int PemnEventQuery(
	PemnCommHandle 	hComm,	
	int		iQueryLength,
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusMaskFilter,
	char		*pcTypeMaskFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	PemnQueryEventHandler   pQueryEventHandler 
	);
DLL_IMPORT char* PemnBEventQuery(
	PemnCommHandle 	hComm,	
	int		iQueryLength,
	char		*pcEvSep,
	char		*pcEvFormat,
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	int		iMsMaxTimeToWait
	);



/*
 * Function:  
 * 	PemnEventReport, PemnBEventReport (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to generate a report
 *	of all events matching the specified filter.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcFormat - reserved for future use
 *	pcStartTimeFilter,...,pcPatternFilter - 
 *		see PemnReceive.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 *
 * Blocking call: V3N
 *	Returns (Read-Only) the event report according to format 
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *	
 */
DLL_IMPORT int PemnEventReport(
	PemnCommHandle 	hComm,
	char		*pcFormat,	/* reserved */	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	PemnReportEventHandler   pReportEventHandler
	);	
DLL_IMPORT char* PemnBEventReport(
	PemnCommHandle 	hComm,	
	char		*pcFormat, /* reserved */	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	int		iMs
	);


#define PEMN_WRITE 	"W"
#define PEMN_APPEND 	"A"

/*
 * Function:  
 * 	PemnEventArchive, PemnBEventArchive (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Append/Write matching events in a file
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcEvFormat - the format string in which the user specifies
 *		the fields to be archived. 
 *	Any of the following PEM macros can be used:
 *	%{EV_ID}		event id
 *	%{EV_STATUS}		event status: OPEN, CLOSE....
 *	%{EV_TYPE}		event type: ALARM, WARNING,....
 *	%{EV_TIME}		time the event occured
 *	%{EV_NODE}		node where this event happened
 *	%{EV_ORIGIN}		the origin or context in which is event happened
 *	%{EV_DESC}		event description with dynamic argument(s)
 *	%{EV_DIARY}		the diary of this event
 *	%{EV_HANDLER}		the handler of this event
 *	%{EV_OWNER}		the owner of this event (typically a 
 *				UNIX account name)
 *	%{EV_NAME}		the name of the event class 
 *	%{EV_CLASS_NAME}	(same as EV_NAME), it is the name of the 
 *				event class
 *	%{EV_CATALOG}		the name of the event catalog (e.g the 
 *				appl. name).
 *	%{EV_EXPECTANCY}	the life expectancy of the event
 *	%{EV_NSEVERITY}		the numerical severity [1,5] of the event
 *	%{EV_ARGS}		the dynamic arguments separated by tabs 
 *				passed to this event when it was triggered.
 *	%{EV_ARG1}		the first dynamic argument.
 *	%{EV_ARG2}		the second dynamic argument.
 *	%{EV_EXPERT_ADVICE}	The expert advice
 *      %{EV_SNMP_SUPPORT}	snmp support
 *      %{EV_CTG_DESC}		Description of event 
 *      %{EV_ESCL_TEXT}		Escalation command script.	
 *      %{EV_NOTIFY_TEXT}	Notification command script.
 *      %{EV_ACK_TEXT}		Ack command text
 *
 *	pcEvSep - String delimiting two events.
 *	pcFileName - Full file name if starts with '/'
 *		otherwise file name in $PATROL_LOG
 *	pcFileOperation, one of the following
 *		PEMN_APPEND | PEMN_WRITE
 * 
 *	pcStartTimeFilter,...,pcPatternFilter - 
 *		see PemnReceive.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 *
 * Blocking call: V3N
 *	Returns (Read-Only) PEMN_OK 
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *	
 */
DLL_IMPORT int PemnEventArchive(
	PemnCommHandle 	hComm,	
	char		*pcEvFormat,
	char		*pcEvSep,
	char		*pcFileName,
	char		*pcFileOperation,
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	PemnArchiveEventHandler   pArchiveEventHandler
	);
DLL_IMPORT char* PemnBEventArchive(
	PemnCommHandle 	hComm,	
	char		*pcEvFormat,
	char		*pcEvSep,
	char		*pcFileName,
	char		*pcFileOperation,	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	int		iMs
	);


/*
 * Function:  
 * 	PemnBEventManageIfMatch, PemnEventManageIfMatch (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to manage (ack/delete/close)
 *	all events matching the specified filter.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	iQueryLength - max number of events to be returned by this query.
 *	pcNewStatus - The desired event new event status
 *		Can be one of the following:
 *		PEMN_ACKNOWLEDGED 
 * 		PEMN_CLOSED 	
 *		PEMN_DELETED 
 *	pcStartTimeFilter,...,pcPatternFilter - 
 *		see PemnReceive.
 *
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 *	The choice of iQueryLength should be judiscious since big values mean
 *	'more work' ro be done by the agent.
 *
 * Blocking call: V3N
 *	Returns (Read-Only) the event report according to format 
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *	
 */
DLL_IMPORT char* PemnBEventManageIfMatch(
	PemnCommHandle 	hComm,	
	int		iQueryLength,
	char		*pcNewStatus,	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass,
	int		iMs
	);
DLL_IMPORT int PemnEventManageIfMatch(
	PemnCommHandle 	hComm,	
	int		iQueryLength,
	char		*pcNewStatus,	
	char		*pcStartTimeFilter,	
	char		*pcEndTimeFilter,	
	char		*pcStatusFilter,
	char		*pcTypeFilter,
	char 		*pcNseverityFilter,
	char		*pcNodeFilter,
	char		*pcOriginFilter,
	char		*pcPatternFilter,
	char		*pcEidrange,
	char		*pcEvClass
	);


/*
 * Function:  
 * 	PemnGetEventClass 
 *------------------------------------------------------------------
 * Purpose :
 *	Return (as ASCII string) the selected fileds from the Event Class
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen).
 *	pcEventCatalog - Event Catalog name(e.g application name).
 *	pcEventClass -  Event Class name
 *	pcEvFormat - the format string in which the user specifies
 *		the fields to be extracted from the event catalog.
 *		Any of the following PEM macros can be used:
 *		%{EV_EXPERT_ADVICE}	The expert advice
 *              %{EV_SNMP_SUPPORT}	snmp support
 *              %{EV_CTG_DESC}		Description of event 
 *              %{EV_ESCL_TEXT}		Escalation command script.	
 *              %{EV_NOTIFY_TEXT}	Notification command script.
 *              %{EV_ACK_TEXT}		Ack command text             
 *
 *	pEventClassHandler - the handler to be called when the API
 *		receives the answer from the agent.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *   
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 */
DLL_IMPORT int PemnGetEventClass(
	PemnCommHandle 		  hComm,	
	char			  *pcEventCatalog,	
	char			  *pcEventClass,	
	char			  *pcEvFormat,
	PemnGetEventClassHandler  pEventClassHandler
	);
DLL_IMPORT char* PemnBGetEventClass(
	PemnCommHandle 		  hComm,	
	char			  *pcEventCatalog,	
	char			  *pcEventClass,	
	char			  *pcEvFormat,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetEventClassList 
 *------------------------------------------------------------------
 * Purpose :
 *	Return the list of event classes from the Event Catalog.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen).
 *	pcEventCatalog - Event Catalog name (e.g application name).
 *	pEventClassListHandler - the handler to be called when the API
 *		receives the answer from the agent.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *   
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise
 */ 
DLL_IMPORT int PemnGetEventClassList(
	PemnCommHandle 		  	hComm,	
	char			  	*pcEventCatalog,	
	PemnGetEventClassListHandler  	pEventClassListHandler
	);
DLL_IMPORT char* PemnBGetEventClassList(
	PemnCommHandle 		  	hComm,	
	char			  	*pcEventCatalog,	
	int 				iMsMaxTimeToWait
	);


/*
 * Constants:  
 *------------------------------------------------------------------
 * Purpose :
 * 	The following preprocessor-constanats are used in 
 *	the 'pcOperation' field of PemnDefineEventClass
 *
 *------------------------------------------------------------------
 */
#define PEMN_PSL_NOTIFICATION 	"PN"	
#define PEMN_PSL_ESCALATION 	"PE"	
#define PEMN_PSL_MGT		"PM"	
#define PEMN_OS_NOTIFICATION 	"ON"	
#define PEMN_OS_ESCALATION 	"OE"	
#define PEMN_OS_MGT 		"OM"	


/*
 * Constants:  
 *------------------------------------------------------------------
 * Purpose :
 * 	The following preprocessor-constanats are used in 
 *	the 'pcOperation' field of PemnDefineEventClass
 *	see describtion below. V31
 *
 *------------------------------------------------------------------
 */
#define PEMN_EC_OPERATIONS_ADD 			"ADD"	
#define PEMN_EC_OPERATIONS_REPLACE 		"REPLACE"	
#define PEMN_EC_OPERATIONS_DELETE 		"DELETE"	

/*
 * Function:  
 * 	PemnDefineEventClass (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Define a volatile event class. 
 *	The event class definition will remain while the
 *	agent is running but it will be lost once the
 *	agents exits.
 *	If the event class exist already, then it depends
 *	on the operation:
 *	ADD -	will add only if it is new
 *	REPLACE - will overwrite the old one.
 *	DELETE - will be deleted.
 *      NOTE: Currently only REPLACE is implemented.
 *
 *	To define event commands see PemnSetEventClassCmd
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen).
 *	pcEventCatalog - Event Catalog name (e.g application name).
 *	pcEventClass - Event class name.
 *	pcOwner - The default owner of the class
 *	pcDesc - The event description in the catalog.
 *	pcExpertAdvice - Expert advice text
 *	pcReserved1,2,3 - reserved for future use
 *	pcOperation - ADD | REPLACE | DELETE
 *   
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise
 */ 
DLL_IMPORT int PemnDefineEventClass(
	PemnCommHandle 		  	hComm,	
	char			  	*pcEventCatalog,	
	char			  	*pcEventClass,	
	char				*pcOwner,
	char 				*pcDesc,
	char 				*pcExpertAdvice,
	char				*pcReserved1,
	char				*pcReserved2,
	char				*pcReserved3,
	char				*pcOperation
	);


/*
 * Function:  
 * 	PemnSetEventClassCmd (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Define event command (Notification | Escalation | Management)
 *	in a volatile event class. 
 *	The event command definition will remain while the
 *	agent is running but it will be lost once the
 *	agents exits.
 *	If the command exists already, then the effect depends
 *	on the operation:
 *	ADD -	will add the command only if command is not defined.
 *	REPLACE - will overwrite the old one.
 *	DELETE - the command be deleted.
 *
 *      NOTE: Currently only REPLACE is implemented.
 *
 *	To define event commands see PemnSetEventClassCmd
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen).
 *	pcEventCatalog - Event Catalog name (e.g application name).
 *	pcEventClass - Event class name.
 *	pcCmdType - The type of command PSL/OS, (Notification | 
 *		Escalation | Management) 
 *	pcCmdText - The PSL or OS text of the command.
 *	pcOperation - ADD | REPLACE | DELETE
 *	pcSpecificData - additional data specific to the commnad
 *		currently it is used only as the escalation interval
 *		for the escalation command
 *   
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise
 */ 
DLL_IMPORT int PemnSetEventClassCmd(
	PemnCommHandle 		  	hComm,
	char			  	*pcEventCatalog,	
	char			  	*pcEventClass,		
	char			  	*pcCmdType,	
	char			  	*pcCmdText,	
	char			  	*pcOperation,
	char				*pcSpecificData	
	);

/* 
 * Function: 
 *	PemnFlushEvent
 *------------------------------------------------------------------
 * Purpose : 
 * 	Flush PATROL Events from the agent's memory to disk.
 * 	All events are now available in PEM.log.
 * 	They can be read by the PEM READ API or dumped using 'dump_events'
 *------------------------------------------------------------------
 */
DLL_IMPORT void PemnFlushEvent(PemnCommHandle hComm);

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Application Management
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/*
 * Function:  
 * 	PemnGetApplObj, PemnBGetApplObj
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the application object.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *	pGetObjHandler- the handler to be called by the framework
 *		when the object is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the application handle or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetApplObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	PemnGetApplObjHandler	pGetObjHandler
	);
DLL_IMPORT PemnApplObjHandle PemnBGetApplObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetInstObj, PemnBGetInstObj 
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the instance object.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *      pcApplInstName- name of the instance.
 *	pGetObjHandler- the handler to be called by the framework
 *		when the object is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the instance handle or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetInstObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	PemnGetInstObjHandler	pGetObjHandler
	);
DLL_IMPORT PemnInstObjHandle PemnBGetInstObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetParamObj, PemnGetComputerParamObj, 
 * 	PemnBGetParamObj, PemnBGetComputerParamObj 
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the parameter object.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *      pcApplInstName- name of the instance.
 *      pcParamName- name of the parameter.
 *	pGetObjHandler- the handler to be called by the framework
 *		when the object is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the parameter handle or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *
 */
DLL_IMPORT int PemnGetParamObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	char		       *pcParamName,
	PemnGetParamObjHandler	pGetObjHandler
	);
DLL_IMPORT PemnParamObjHandle PemnBGetParamObj(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	char		       *pcParamName,
	int 			iMsMaxTimeToWait
	);
DLL_IMPORT int PemnGetComputerParamObj(
	PemnCommHandle 		hComm,
	char		       *pcParamName,
	PemnGetParamObjHandler	pGetObjHandler
	);
DLL_IMPORT PemnParamObjHandle PemnBGetComputerParamObj(
	PemnCommHandle 		hComm,
	char		       *pcParamName,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetVarObj, PemnBGetVarObj 
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the PATROL  object.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *	pGetObjHandler- the handler to be called by the framework
 *		when the object is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the varaible handle or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetVarObj(
	PemnCommHandle 		hComm,
	char		       *pcVarName,
	PemnGetVarObjHandler	pGetObjHandler
	);
DLL_IMPORT PemnVarObjHandle PemnBGetVarObj(
	PemnCommHandle 		hComm,
	char		       *pcVarName,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetApplList, PemnBGetApplList
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the computer's list of applications.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pGetListHandler- the handler to be called by the API framework
 *		when the list is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the application list or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetApplList(
	PemnCommHandle 		hComm,
	PemnGetListHandler	pGetListHandler
	);
DLL_IMPORT char* PemnBGetApplList(
	PemnCommHandle 		hComm,
	int 			iMsMaxTimeToWait
	);

/*
 * Function:  
 * 	PemnGetInstList, PemnBGetInstList 
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the list of instances for
 *	a given application.
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *	pGetListHandler- the handler to be called by the framework
 *		when the list is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result  
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the application list or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise  
 *
 */
DLL_IMPORT int PemnGetInstList(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	PemnGetListHandler	pGetListHandler
	);
DLL_IMPORT char* PemnBGetInstList(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	int 			iMsMaxTimeToWait
	);

/*
 * Function:  
 * 	PemnGetParamList, PemnGetComputerParamList 
 *	PemnBGetParamList, PemnGetComputerParamList (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the list of parameter for
 *	a given application instance (or computer).
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcApplTypeName- name of the application.
 *      pcApplInstName- name of the instance.
 *	pGetListHandler- the handler to be called by the framework
 *		when the list is received.
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *   
 * Blocking call: V3N
 *	Returns (Read-Only) the parameter list or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetParamList(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	PemnGetListHandler	pGetListHandler
	);
DLL_IMPORT char* PemnBGetParamList(
	PemnCommHandle 		hComm,
	char		       *pcApplTypeName,
	char		       *pcApplInstName,
	int 			iMsMaxTimeToWait
	);
DLL_IMPORT int PemnGetComputerParamList(
	PemnCommHandle 		hComm,
	PemnGetListHandler	pGetListHandler
	);
DLL_IMPORT char* PemnBGetComputerParamList(
	PemnCommHandle 		hComm,
	int 			iMsMaxTimeToWait
	);

/*
 * Function:  
 * 	PemnGetVarList, PemnBGetVarList (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to send the list of variable.  
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcNode - same as in PSL Get() function
 *	pcKeyword - same as like in PSL Get() function
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Blocking call: V3N
 *	Returns (Read-Only) the variable list or
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 *
 */
DLL_IMPORT int PemnGetVarList(
	PemnCommHandle 		hComm,
	char			*pcNode,
	char			*pcKeyword,
	PemnGetListHandler	pGetListHandler
	);
DLL_IMPORT char* PemnBGetVarList(
	PemnCommHandle 		hComm,
	char			*pcNode,
	char			*pcKeyword,
	int 			iMsMaxTimeToWait
	);


/*
 * Function:  
 * 	PemnGetApplObjInfo 
 *------------------------------------------------------------------
 * Purpose :
 *	Access (Read-Only) the application info string..
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- the Application Object handle (passed to the handler)
 *	pcFormatString - (Reserved for future use) 
 *		Format string describing how the info. will pe presented.(V31) 
 *   
 * Returns the following attributes separated by tabs ('\t'):
 *
 *	Full application name
 * 	Application's worst instance name
 * 	State
 * 	Master revision
 * 	Minor version
 *
 * 
 *	
 */
DLL_IMPORT char *PemnGetApplObjInfo( 
	PemnApplObjHandle hObj,
	char 		  *pcFormatString 
	);


/*
 * Function:  
 * 	PemnGetInstObjInfo 
 *------------------------------------------------------------------
 * Purpose :
 *	Access (Read-Only) the instance info string.
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- the Instance Object handle (passed to the handler)
 *	pcFormatString - (Reserved for future use) 
 *		Format string describing how the info. will pe presented.(V31)
 *   
 * Returns the following attributes separated by tabs ('\t'):
 *
 *  	Instance name
 * 	Instance's worst parameter.
 * 	Status.
 * 	Rule state
 *      Instance create icon
 *      Parent instance (or NULL) (V31)
 */
DLL_IMPORT char *PemnGetInstObjInfo( 
	PemnInstObjHandle hObj,
	char *pcFormatString );


/*
 * Function:  
 * 	PemnGetParamObjInfo 
 *------------------------------------------------------------------
 * Purpose :
 *	Access (Read-Only) the parametr info string..
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- the Application Object handle (passed to the handlere)
 *	pcFormatString - (Reserved for future use) 
 *		Format string describing how the info. will pe presented.(V31)
 *   
 * Returns the following attributes separated by tabs ('\t'):
 *
 * 	Full parameter name
 * 	Parameter`s current time.
 * 	Polling interval.
 * 	Retries counter.
 * 	Current value.
 * 	State.
 * 	Output mode.
 * 	Autoscale value.
 * 	Y axis min value.	
 * 	Y axis max value.	
 */
DLL_IMPORT char *PemnGetParamObjInfo(
	PemnParamObjHandle hObj,
	char *pcFormatString );



/*
 * Function:  
 * 	PemnGetApplObjAttributes (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Given an application handle, return the array of the
 *	application attributes or NULL if error.
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- The application handle.
 *   
 *	Returns the array of application attributes.  
 *
 */
DLL_IMPORT PemnApplAttrArray *PemnGetApplObjAttributes( PemnApplObjHandle hObj);

/*
 * Function:  
 * 	PemnGetInstObjAttributes (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Given an instance handle, return the array of the
 *	instance attributes.
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- The instance handle.
 *   
 *	Returns the array of instance attributes or NULL if error.  
 *
 */
DLL_IMPORT PemnInstAttrArray *PemnGetInstObjAttributes( PemnInstObjHandle hObj); 



/*
 * Function:  
 * 	PemnGetParamObjAttributes (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Given an parameter handle, return the array of the
 *	parameter attributes.
 *------------------------------------------------------------------
 * Arguments:
 *	hObj- The parameter handle.
 *   
 *	Returns the array of parameter attributes or NULL if error.  
 *
 */
DLL_IMPORT PemnParamAttrArray *PemnGetParamObjAttributes(PemnParamObjHandle hObj); 


/*
 * Function:  
 * 	PemnBPslExec (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to execute a PSL script defined in 
 *	<Text>
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *      iWaitForResults - should be 
 *		True  if caller will wait for result
 *		False if caller wants only that the agent acknowldge reception
 *		of the script.
 *      pcText - PSL script text.
 *
 *	NOTE: the PEM API does not check if the script is 
 *	free from compilation error before sending it
 *	to the agent. It is the caller responsibility.
 *
 * Blocking call: V3N
 *	Returns (Read-Only) result of PSL execution
 *	        (Read-Only) the API message string.
 *
 */
DLL_IMPORT char* PemnBPslExec(
	PemnCommHandle 	  hComm,
	int		 iMsMaxTimeToWait,
	int		 iWaitForResults,
	char 		 *pcText
	);

/*
 * Function:  
 * 	PemnBSetVarObj (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Request the agent to set the PATROL variable.
 *	This function equivalent to PSL set(VarName,VarValue);
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *      pcVarName - variable name 
 *      pcVarValue - value to set 
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result.
 *
 * Blocking call: V3N
 *	Returns (Read-Only) the API message string.
 *
 */
DLL_IMPORT PemnVarObjHandle PemnBSetVarObj(
	PemnCommHandle 		hComm,
	char		       *pcVarName,
	char		       *pcVarValue,
	int 			iMsMaxTimeToWait
	);

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Data Management
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */


/*
 * Function:  V31
 * 	PemnBPutStream, PemnPutStream (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	- Send a stream of bytes to the agent. 
 *	- The agent buffers the stream data in $PATROL_TMP/<FileName>
 *	- An optional event can be sent. The purpose of the event
 *	  is to accomplish further processing via its notification
 *	  commnad executed at the agent.
 *	- The event has access to <FileName> <Operation> and <StreamId>
 *	- The event can be user defined or from the standard catalog
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	iStreamSize - size of stream
 *	pcStreamData - the data stream (can contain (char) 0)
 *	pcCatalogName, pcClassName - 
 *		IF both NULL THEN no event is sent.
 *		ELSIF one in (pcCatalogName, pcClassName) is "" THEN 
 *		  send the standard event "RemProcess"
 *		ELSE 
 *                sent the event designated by 
 *		  (pcCatalogName, pcClassName).
 * 	
 *	When sending a user defined event. The event description in
 *	the event catalog should contain 3 and only 3 dynamic arguments 
 *	to pass (in this order):
 *		- <FileName> 
 *		- <Operation> and 
 *		- <StreamId>
 *	iTag - reserved for future use.
 *	pcOperation - When the agent receives this stream:
 * 		-it will store it in
 *  		$PATROL_TMP/pcFileName if <Operation> is PEMN_WRITE.
 * 		-it will append it to $PATROL_TMP/pcFileName
 *  		if <Operation> is PEMN_APPEND
 * 	Max Stream 8096 char
 *
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Blocking call: V3N
 *	Returns (Read-Only) PEMN_OK
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 * 
 */
DLL_IMPORT char* PemnBPutStream(
	PemnCommHandle 		hComm,
	int			iStreamSize,
	char 			*pcStreamData,
	char			*pcCatalogName,
	char			*pcClassName,
	int			iTag,		/* reserved for future use*/
	char 			*pcFileName,	
	char 			*pcOperation,	
	char 			*pcStreamId,
	char			*pcKey4,	/* reserved for future use*/	
	int 			iMsMaxTimeToWait
	);
DLL_IMPORT int PemnPutStream(
	PemnCommHandle 		hComm,
	int			iStreamSize,
	char 			*pcStreamData,
	char			*pcCatalogName,
	char			*pcClassName,
	int			iTag,		/* reserved for future use*/
	char 			*pcFileName,
	char 			*pcOperation,	
	char 			*pcStreamId,
	char			*pcKey4		/* reserved for future use*/
	);


/*
 * Function:  V31
 * 	PemnBPutFile, PemnPutFile (V31)
 *------------------------------------------------------------------
 * Purpose :
 *
 * 	Send the file to agent
 *  	The agent will write the content of the file in  
 *  	"$PATROL_REMOTE/<destination-file>".
 *  	Also standard event RemProcess will be sent to the remote
 *  	agent unless user send own event
 *  	The standard event RemProcess and any user defined event
 *  	will carry the three dynamic arguments:
 *  	- /tmp/MyFile (as first event dynamic arg.)
 *  	- Operation (reserved for future use)
 *  	- MyFileId (as third event dynamic arg.)
 *  	A notification commnad can be executed at the remote agent
 *  	to complete the processing of the file sent
 *
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcSrcFullFileName - source file name 
 *	pcDstFileName - destination file name 
 *
 *	pcCatalogName, pcClassName - 
 *		IF both NULL THEN no event is sent.
 *		ELSIF one in (pcCatalogName, pcClassName) is "" THEN 
 *		  send the standard event "RemProcess"
 *		ELSE 
 *                sent the event designated by 
 *		  (pcCatalogName, pcClassName).
 * 	
 *	When sending a user defined event. The event description in
 *	the event catalog should contain 3 and only 3 dynamic arguments 
 *	to pass (in this order):
 *		- <FileName> 
 *		- <Operation> and 
 *		- <StreamId>
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Blocking call: V3N
 *	Returns (Read-Only) PEMN_OK
 *	        (Read-Only) the API message string.
 * NON-blocking call:
 *	Returns True  (1) if operation is sent to the agent successfully
 *	        False (0) otherwise 
 * 
 */

DLL_IMPORT int PemnPutFile(
	PemnCommHandle 		hComm,
   	char 			*pcSrcFullFileName,
    	char 			*pcDstFileName,
    	char 			*pcCatalogName,
    	char 			*pcClassName,
    	char 			*pcId
	);
DLL_IMPORT char* PemnBPutFile(
	PemnCommHandle 		hComm,
   	char 			*pcSrcFullFileName,
    	char 			*pcDstFileName,
    	char 			*pcCatalogName,
    	char 			*pcClassName,
    	char 			*pcId,
	int 			iMs
	);

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Miscellaneous routine
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */

/*
 * Function:  
 * 	PemnRegisterUserMessageHandler (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Register your own message processing function (see typedef
 *	of PemnUserMessageHandler above). It will be
 *	called by the framework to display error/warning/info message
 *	instead of the default one.
 *------------------------------------------------------------------
 * Arguments:
 *	pMessageHandler- your erro/warning/info routine.
 *   
 *
 */
DLL_IMPORT void PemnRegisterUserMessageHandler(
	PemnUserMessageHandler pMessageHandler);


/*
 * Function:  V31
 * 	PemnGetLastMessage (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Return as a read-only string the last message (error, warning
 *	or information) issued by the PEM API
 *------------------------------------------------------------------
 *
 */
DLL_IMPORT char * PemnGetLastMessage(void);



/*
 * Function:  (V31)
 * 	PemnCheckInit
 *------------------------------------------------------------------
 * Purpose :
 *	Check if PEM library is initiated and initialize it if
 *	not.
 *
 *------------------------------------------------------------------
 * Returns: True if PEM lib was intialized
 *	    False if not.
 * 
 */
DLL_IMPORT int PemnCheckInit(void);

/*
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 *	Specific Blocking Mode
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 */


/*
 * Constants: V31
 *	Results returned by Blocking API functions 
 *------------------------------------------------------------------
 * Purpose :
 * 	PEMN_ERROR- returned if error
 * 	PEMN_FILE_ERROR- returned if error in some of data management operations
 * 	PEMN_TIMEOUT - returned when the blocking function waits for
 *		'iMsMaxTimeToWait' milliseconds without receiving the
 *		results from the agent.   
 * 	PEMN_NULL - returned if the requested object does not exist.
 * 	PEMN_INTERRUPT - returned if blocking function is interrupted.
 * 	PEMN_DISCONNECT - returned if the coonection with agent is
 *		disconnected while in the blocking function.
 * 	PEMN_OK - returned if results successfull.
 *------------------------------------------------------------------
 */
extern DLL_IMPORT char *PEMN_ERROR;
extern DLL_IMPORT char *PEMN_FILE_ERROR;
extern DLL_IMPORT char *PEMN_TIMEOUT;
extern DLL_IMPORT char *PEMN_NULL;
extern DLL_IMPORT char *PEMN_INTERRUPT;
extern DLL_IMPORT char *PEMN_DISCONNECT;
extern DLL_IMPORT char *PEMN_OK;
extern DLL_IMPORT char *PEMN_NOT_AVAILABLE;
	/* Service not available. Probably old agent */

/*
 * Constant:  (V31)
 *------------------------------------------------------------------
 * Purpose : Used in specifyng the time the blocking mode
 *	routine will block waiting for results from the agent.
 *	
 *	USE_DEFAULT_TIMEOUT	
 *		use default (see PemnBGetWaitingTime and PemnBSetWaitingTime)
 *	BLOCK_AND_WAIT_FOR_RESULT
 *		block forever waiting for results
 *------------------------------------------------------------------
 */
#define USE_DEFAULT_TIMEOUT 	   0
#define BLOCK_AND_WAIT_FOR_RESULT -1

/*
 * Function:  (V31)
 * 	PemnBGetWaitingTime, PemnBSetWaitingTime
 *------------------------------------------------------------------
 * Purpose :
 *	Set/Get the default MaxWaitingTime (in millissec)
 *	for the blocking mode operations.
 *	Itially this default value is set to 60*1000 (1 muinute).
 *
 *	NOTE <NewDefaultTimeout> cannot be smaller than 100 millisec.
 *
 * Exanmple:
 *	Calling PemnBPslExec with USE_DEFAULT_TIMEOUT
 *	.... PemnBPslExec(hComm, USE_DEFAULT_TIMEOUT, 0, "get_vars(\"/\");")
 *		
 *
 *------------------------------------------------------------------
 */
DLL_IMPORT int  PemnBGetWaitingTime(void);
DLL_IMPORT void PemnBSetWaitingTime(int iNewDefaultTimeout);


/*
 * Function:  
 * 	PemnBExitFunction (V31)
 *------------------------------------------------------------------
 * Purpose :
 *	Abort blocking mode operation.
 *	This routine can be called from the signal handler (e.g SIGINT)
 *	of the program to abort a blocking mode operation.
 *	
 *------------------------------------------------------------------
 *
 */
DLL_IMPORT void PemnBExitFunction(void);


/*
 *------------------------------------------------------------------
 *	Scheduling  
 * ------------------------------------------------------------------
 */

/*
 * Function:  V31
 * 	PemnBEventScheduleAdd, PemnBEventScheduleModify,
 *	PemnBEventScheduleList, PemnBEventScheduleDelete, 
 *	PemnBEventScheduleResume, PemnBEventScheduleSuspend
 *------------------------------------------------------------------
 * Purpose :
 *	Event Scheduling. By scheduling event you are scheduling their
 *	Notification command which will be executed when the event is 
 *	triggered.
 *
 * 	PemnBEventScheduleAdd -Trigger an event at the specified GMT time.
 *	PemnBEventScheduleModify-Modify a scheduled event.
 *	PemnBEventScheduleList-List all events scheduled for execution.
 *	PemnBEventScheduleDelete-Cancel an event scheduled for execution. 
 *	PemnBEventScheduleResume-Resume an event previously suspended.
 *	PemnBEventScheduleSuspend-Suspend an event scheduled for execution
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 *	pcTime - GMT time when this event will be scheduled.
 *		format "MMddhhmm[yy]"
 *	pcEventCatalogName- The name of the event catalog 
 *		(or STD_EVENT_CATALOG if using the
 *	  	standard event catalog of PATROL).
 *	pcEventClassName- The name of the event class.
 *	pcEventOrigin - the origin of this event (will appear in PEM windows)
 * 	eEventType - event type
 *	iEventNseverity - event severity [1..5]
 *	pcSchId - Scheduing id returned by PemnBEventScheduleAdd
 *      iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Blocking call: V3N
 *	Returns (Read-Only) PEMN_OK
 *	        (Read-Only) the API message string.
 * 
 */
DLL_IMPORT char *PemnBEventScheduleAdd(
	PemnCommHandle 	  hComm,
	char 		 *pcTime, 
	char 		 *pcEventCatalogName, 
	char 		 *pcEventClassName, 
	char 		 *pcEventOrigin,
	PemnEventTypeEnum eEventType,
	int  		 iEventNseverity,
	int		iMsMaxTimeToWait,
	...
	);
DLL_IMPORT char *PemnBEventScheduleModify(
	PemnCommHandle 	hComm,
	char		 *pcSchId,
	char 		 *pcTime, 
	char 		 *pcEventCatalogName, 
	char 		 *pcEventClassName, 
	char 		 *pcEventOrigin,
	PemnEventTypeEnum eEventType,
	int  		 iEventNseverity,
	int		iMsMaxTimeToWait,
	...
	);
DLL_IMPORT char *PemnBEventScheduleList(
	PemnCommHandle 	hComm,
	int		iMsMaxTimeToWait
	);
DLL_IMPORT char *PemnBEventScheduleDelete(
	PemnCommHandle 	hComm,
	char		 *pcSchId,
	int		iMsMaxTimeToWait
	);
DLL_IMPORT char *PemnBEventScheduleResume(
	PemnCommHandle 	hComm,
	char		 *pcSchId,
	char 		 *pcTime,
	int		iMsMaxTimeToWait
	);
DLL_IMPORT char *PemnBEventScheduleSuspend(
	PemnCommHandle 	hComm,
	char		 *pcSchId,
	int		iMsMaxTimeToWait
	);


/*
 *------------------------------------------------------------------
 * IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
 *	All the functions below are Undocumented routine: use at own risk :-)  
 *------------------------------------------------------------------
 */

typedef PemnClientData PemnUserData;

DLL_IMPORT int PemnIs30(PemnCommHandle hComm);


DLL_IMPORT void PemnInit(int iBufferSize);

DLL_IMPORT PemnUserData PemnGetUserData(
	PemnCommHandle 	hComm
	);
DLL_IMPORT void PemnSetUserData(
	PemnCommHandle 	hComm,
	PemnUserData    pData
	);



/*
 * Function:  PemnBGetStream (not supported or documented)
 *------------------------------------------------------------------
 * Purpose : Copy a portion of a file to a pre-allocated buffer.
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 * 	iOffsetInSrcFile - offset from begining of file <SrcFileName>
 * 	pcSrcFileName - name of source file: either full path or relative 
 *		name in $PATROL_HOME/remote directory
 * 	iDestStreamBufferSize - size of caller allocxated buffer
 * 	pcDestStreamBuffer - address of caller allocated buffer
 * 	iTag - reserved for future use
 * 	pcStreamId - name caller gives to this stream operation
 * 	pcKey - reserved for future use
 *  iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Returns (Read-Only) the number of bytes moved (as a string) 
 * or PEMN_FILE_ERROR if error
 * 
 */
DLL_IMPORT char* PemnBGetStream(
	PemnCommHandle 		hComm,
	int			iOffsetInSrcFile,	
	char 		*pcSrcFileName,
	int			iDestStreamBufferSize,
	char 		*pcDestStreamBuffer,
	int			iTag,		/* reserved for future use*/
	char 		*pcStreamId,
	char		*pcKey,		/* reserved for future use*/	
	int 		iMsMaxTimeToWait
	);

/*
 * Function:  PemnBGetFile (not supported or documented)
 *------------------------------------------------------------------
 * Purpose : Copy a file 
 *
 *------------------------------------------------------------------
 * Arguments:
 *	hComm- the Comm handler (returned previously by PemnOpen)
 * 	pcSrcFileName - name of source file: either full path or relative 
 *		name in $PATROL_HOME/remote directory
 * 	pcDstFileName - full path of destination file.
 * 	iTag - reserved for future use
 * 	pcId - name caller gives to this file operation
 * 	pcKey - reserved for future use
 *  iMsMaxTimeToWait - timeout in millisecond waiting for result 
 *
 * Returns (Read-Only) the number of bytes copied (as a string) 
 * or PEMN_FILE_ERROR if error
 * 
 */
DLL_IMPORT char* PemnBGetFile(
	PemnCommHandle 		hComm,
   	char 		*pcSrcFileName,
    char 		*pcDstFileName,
    char 		*pcId,
	int			iTag,		/* reserved for future use*/
	char		*pcKey,		/* reserved for future use*/
	int 		iMsMaxTimeToWait
	);


/*
 *------------------------------------------------------------------
 *	Synonymes for backward compatibility
 *
 * WARNING DON'T USE THE NAMES BELOW THEY WILL NOT BE USED
 * IN V3.2 OR HIGHER
 *------------------------------------------------------------------
 */


typedef  PemnEventStatusEnum 	PemEventStatusEnum; 
typedef  PemnEventTypeEnum 	PemEventTypeEnum; 
typedef  PemnEventHandle 	PemrEventHandle; 
typedef  PemnReceiveEventHandler PemnEventHandler ;
typedef  PemnReceiveEventHandler PemnEventReceivedHandler;


#define PemrGetEid			PemnGetEid
#define PemrGetStatus 			PemnGetStatus
#define PemrGetSeverity 		PemnGetSeverity
#define PemrGetType			PemnGetType 
#define PemrGetTime			PemnGetTime 
#define PemrGetNode			PemnGetNode 
#define PemrGetOrigin			PemnGetOrigin

#define PemrGetDesc 			PemnGetDesc
#define PemrGetOwner			PemnGetOwner
#define PemrGetDiary			PemnGetDiary
#define PemrGetHandler			PemnGetHandler
#define PemrGetExpectancyStr		PemnGetExpectancyStr
#define PemrGetEventCatalogName		PemnGetEventCatalogName
#define PemrGetClassName		PemnGetClassName
#define PemrGetArgs			PemnGetArgs
#define PemrGetSourceId			PemnGetSourceId
#define PemrGetNseverity		PemnGetNseverity


#define PEMA_WRITE 	PEMN_WRITE
#define PEMA_APPEND 	PEMA_APPEND


#define PEMA_ERROR 		PEMN_ERROR	
#define PEMA_TIMEOUT		PEMN_TIMEOUT
#define PEMA_NULL		PEMN_NULL
#define PEMA_INTERRUPT		PEMN_INTERRUPT
#define PEMA_DISCONNECT		PEMN_DISCONNECT
#define PEMA_OK			PEMN_OK
#define PEMA_NOT_AVAILABLE	PEMN_NOT_AVAILABLE	


#define PemaGetApplObjAttributes	PemnGetApplObjAttributes
#define PemaGetInstObjAttributes	PemnGetInstObjAttributes 
#define PemaGetParamObjAttributes	PemnGetParamObjAttributes

#define PemaGetApplObjInfo		PemnGetApplObjInfo
#define PemaGetInstObjInfo		PemnGetInstObjInfo
#define PemaGetParamObjInfo		PemnGetParamObjInfo

#define PemaGetApplObj			PemnGetApplObj
#define PemaGetInstObj			PemnGetInstObj
#define PemaGetParamObj			PemnGetParamObj
#define PemaGetComputerParamObj		PemnGetComputerParamObj
#define PemaGetVarObj			PemnGetVarObj

#define PemaGetApplList			PemnGetApplList
#define PemaGetInstList			PemnGetInstList
#define PemaGetParamList		PemnGetParamList
#define PemaGetComputerParamList	PemnGetComputerParamList
#define PemaGetVarList			PemnGetVarList

#define PemaBSetVarObj			PemnBSetVarObj
#define PemaBPutStream			PemnBPutStream

#define PemaBGetApplObj			PemnBGetApplObj
#define PemaBGetInstObj			PemnBGetInstObj
#define PemaBGetParamObj		PemnBGetParamObj
#define PemaBGetComputerParamObj	PemnBGetComputerParamObj
#define PemaBGetVarObj			PemnBGetVarObj
#define PemaBGetApplList		PemnBGetApplList
#define PemaBGetInstList		PemnBGetInstList
#define PemaBGetParamList		PemnBGetParamList
#define PemaBGetComputerParamList	PemnBGetComputerParamList
#define PemaBGetVarList			PemnBGetVarList


#define PemaGetListHandler		PemnGetListHandler
#define PemaGetVarObjHandler		PemnGetVarObjHandler
#define PemaGetApplObjHandler		PemnGetApplObjHandler
#define PemaGetInstObjHandler		PemnGetInstObjHandler
#define PemaGetParamObjHandler		PemnGetParamObjHandler

#define PemaVarObjHandle		PemnVarObjHandle
#define PemaApplObjHandle		PemnApplObjHandle
#define PemaInstObjHandle		PemnInstObjHandle
#define PemaParamObjHandle		PemnParamObjHandle

#define PemaApplAttrEnum		PemnApplAttrEnum
#define PemaInstAttrEnum		PemnInstAttrEnum
#define PemaParamAttrEnum		PemnParamAttrEnum

#define PemaApplAttrArray		PemnApplAttrArray	
#define PemaInstAttrArray		PemnInstAttrArray
#define PemaParamAttrArray		PemnParamAttrArray

#define PemnBGetTimeout		PemnBGetWaitingTime
#define PemnBSetTimeout		PemnBSetWaitingTime

#ifdef __cplusplus
}
#endif

/*
 *----------------------------------------------------------------
 * Modification History:
 * 
 * $Log: pemapi.h,v $
 * Revision 1.67  1998/10/26 20:43:07  lkopnick
 * Added return status code to PemnSendIdentity function.
 * Vantive: 41046
 *
 * Revision 1.66  1998/02/18 03:12:30  dtam
 * + added acknowledge type (color) for event mgr
 * Vantive: 44287
 *
 * Revision 1.65  1997/11/12 05:46:46  dbonnell
 * Added EV_TYPE_MAX to PemnEventTypeEnum.
 *
 * Revision 1.64  1997/02/12 23:50:26  dcato
 * fixed build of PEM API DLL on NT so necessary functions are exported
 * from the DLL
 *
 * Revision 1.63  1996/06/25  20:21:23  mmattini
 * Add NT DLL spec. PemnIs30
 *
 * Revision 1.62  1996/06/25  18:40:42  mmattini
 * add PemnIs30 to test agent version
 *
 * Revision 1.61  1996/06/07  22:08:17  dcato
 * + define DLL_IMPORT depending on usage:
 *   (1) building pemapi.dll
 *   (2) using pemapi.dll
 *   (3) building or using pemapi.lib
 * + added extern to PEMN_* vars to avoid duplicate definitions
 *
 * Revision 1.60  1996/06/07  18:56:09  mmattini
 * DLL spec for exported variables
 *
 * Revision 1.59  1996/06/05  19:44:59  mmattini
 * Add PemnInit()
 *
 * Revision 1.58  1996/05/31  20:01:22  mmattini
 * Add DLL_IMPORT/DLL_EXPORT in order to build PEM API DLL
 *
 * Revision 1.57  1996/05/22  13:08:26  mmattini
 * + Patrol APi ping routine (called PemnBPing() ) did not return
 *   the state of the computer.
 *
 * Revision 1.56  1996/05/09  14:31:11  mmattini
 * Add comment to undocumented functions
 *
 * Revision 1.55  1996/05/06  21:31:14  mmattini
 * Vantive  11718 - Re-org requirement for a GetFile
 *
 * Revision 1.54  1996/05/03  16:27:31  mmattini
 * + PatrolView/NT 3.1 support.
 *
 * Revision 1.53  1996/04/27  14:02:04  mmattini
 * User Data for api
 *
 * Revision 1.52  1996/04/23  13:36:14  mmattini
 * event history of two
 *
 * Revision 1.51  1996/04/16  21:46:19  mmattini
 * #vantive 10917
 *
 * Revision 1.50  1996/04/16  15:34:59  mmattini
 * version changed name
 *
 * Revision 1.49  1996/04/16  15:26:12  mmattini
 * + Add #define API_VERSION to PATROL API
 * + PEM PSL and API functions functions can use the nice time format
 *   providedd by 'convdate'. Thanks Mike for the moving it to 'misc'.
 *
 * Revision 1.48  1996/04/03  15:44:01  mmattini
 * + Fix bug in PEM API support for event_schedule
 *
 * Revision 1.47  1996/04/02  23:55:49  mmattini
 * + Access event scheduling from the PEM API
 *
 * Revision 1.46  1996/04/01  22:39:14  mmattini
 * Fix memory leak in PastrolCli event query
 * Clean header file
 *
 * Revision 1.45  1996/03/30  15:33:38  mmattini
 * + Structure the PEM API header file to follow the same
 *   structure as the reference manual.
 *
 * + Cleanup
 *
 * Revision 1.44  1996/03/29  19:20:10  mmattini
 * No more backup PEM files
 *
 * Revision 1.43  1996/03/27  23:26:57  mmattini
 * + Clean PEM API
 *
 * Revision 1.42  1996/03/24  14:16:49  mmattini
 * + Provide backward compatibility for PEM API:
 *   When connecting to 3.1 agent	=> all API services are provided
 *   When connecting to 3.0 agent	=> only 3.0 API services are provided
 * + If PatrolCli is using udp and agent is 3.0
 *   PatrolCli receives an error message that this is an old agent.
 *
 * Revision 1.41  1996/03/22  00:11:16  mmattini
 * + fix compilation in emcons
 * + cleanup session code
 * + use SesPing in the API
 *
 * Revision 1.40  1996/03/09  23:43:53  mmattini
 * + add PSL remote_send_file
 *
 * Revision 1.39  1996/03/07  22:18:27  mmattini
 * Add support to stream send in PEM api
 *
 * Revision 1.38  1996/02/24  00:04:10  mmattini
 * Add more comment
 *
 * Revision 1.37  1996/02/22  17:23:41  mmattini
 * + Fix bug in Filter Context matching algorithm
 *
 * Revision 1.36  1996/02/21  23:27:08  mmattini
 * Add parent instance as a new attribute
 *
 * Revision 1.35  1996/02/21  22:53:34  mmattini
 * PatrolCli
 * + Fix bug in PatrolCli execpsl system("%DUMP ALL");
 * PEM's Event Catalog
 * + Acknowldge command implemented and tested.
 *   The user defined Acknowldge command is executed whenever the
 *   operator presses 'Acknowledge'.
 *
 * Revision 1.34  1996/02/20  18:08:26  dcato
 * put extern "C" { } around prototypes for C++ compilers
 *
 * Revision 1.33  1996/02/16  22:11:32  mmattini
 * + Add PatrolCli event report [FILTER]
 * + PemnBOpen return session errors to PatrolCli
 * + Make provision by the PEM API to support PatrolCli -lp option used
 *   in firewall (see PemnSet/GetLocalPort).
 * + ACL is tested for the PEM API.
 * + Add PemnBEventReport() and PemnEventReport()
 *
 * Revision 1.32  1996/02/15  18:24:29  mmattini
 * Add to the command line
 * event showfilter
 * event ack/del/close
 *
 * Revision 1.31  1996/02/13  23:26:04  mmattini
 * Now you can now execute PSL from the command line
 *
 * Revision 1.30  1996/02/12  15:45:05  mmattini
 * + Added a Disconnect callback to PemnBOpen();
 * + CTRL-C implemented for PemnBOpen();
 * + When a connection is disconnected during a blocking call,
 *   the API framework cancels the call and returns <disconnect>\n.
 * + Added BLOCK_AND_WAIT_FOR_RESULT for blocking mode operations.
 *   The Blocking mode operation will loop forever until
 *   the results are obtained.
 * + For Coddy SAP KM, added PemnBSetVarObj() to set the value of
 *   a variable.
 *
 * Revision 1.29  1996/02/08  20:48:28  mmattini
 * Interim version of PEM API for Mark to compile
 *
 * Revision 1.28  1996/02/07  23:40:12  mmattini
 *   - Object Attributes are separated by \n
 *   - Error | Null | Interrupt | Timeout strings defined in header.
 *   - Get event class list return output in a consistent way.
 *   - Get var list return output in a consistent way.
 * - Infrastructure for event aliases
 *
 * Revision 1.27  1996/02/07  21:08:21  mmattini
 * Added DEF_TIMEOUT and format to PemnGetxxxInfo
 *
 * Revision 1.26  1996/02/07  01:00:33  mmattini
 * - Blocking mode separate routines
 *
 * Revision 1.25  1996/02/06  15:30:29  mmattini
 * Fix PemnBOpen bug
 *
 * Revision 1.24  1996/01/31  21:26:35  mmattini
 * - Clean all PEM sentences. No compaint of PEM stubs anymore ?
 * - Fix two bugs in event mapping optimization (3rd phase)
 * - Allow event desc to carry up to 10,000 of text.
 * - Add host name and port nb to PEM.log
 * - Cleanup event_trigger calls in the agent (not finished)
 * - Make the default persistent filter accept INFO but
 *   reject event with num erical-severity <= 2
 * - For readability, allow standard events to be non numbers.
 *
 * Revision 1.23  1996/01/29  21:54:45  mmattini
 * +  PEM now does use attribute->portnum and attribute->connection
 *   You can delete this fields if you want.
 * + In order to make the PEM API easier to program, add
 *   blocking version of:
 *   Open connection -> PemnBOpen()
 *   Send event -> PemnBSend()
 *   Close conection -> PemnBClose()
 *
 * Revision 1.22  1996/01/22  20:05:57  mmattini
 * + add API get-var
 * + third phase of MetMapEvent optimization
 *
 * Revision 1.21  1996/01/17  22:47:28  mmattini
 * - Add PemnGetVarList() to provide similar PSL get_vars()
 *   in the API.
 * - Add %{EV_ARG1} and %{EV_ARG2} to return the dynamic events
 *   argument 1 or 2.
 *
 * Revision 1.20  1995/12/21  20:54:40  mmattini
 * Update API
 *
 * Revision 1.19  1995/12/19  20:10:13  mmattini
 * ++> PEM API:
 *   The attributes of applications, instances and param. are returned now
 *   in a read-only array.
 *   Encryption was enhanced.
 *
 * Revision 1.18  1995/12/08  22:12:58  mmattini
 * - add infrastructure for GetEventClassList in PEM API
 *
 * Revision 1.17  1995/12/06  19:41:58  mmattini
 * + Add to the PEM API:  PemnGetEventClass()
 *   to get event catalog info from the API.
 *
 * Revision 1.16  1995/11/21  21:12:08  mmattini
 * + Add authentication to PEM API
 *
 * Revision 1.15  1995/11/20  23:34:32  mmattini
 * + Add PSL event_catalog_get()
 * + Improve cmd line support.
 *
 * Revision 1.14  1995/11/17  20:17:03  mmattini
 * + Added new PEM API routines needed in the PATROL command line interpreter.
 *   (see next message).
 *
 * Revision 1.13  1995/11/09  18:57:41  mmattini
 * o Add infrastucture for V31 PEM API and command line.
 *
 * Revision 1.12  1995/11/07  18:27:10  mmattini
 * o PEM usability improvement:
 * - Using "File->Save Statistics .." user can save the Event Repository
 *   statistics on a file.
 * - Using "File->Save Events .." user can save the Event List
 *   on a file.
 * - You can now filter based on event class and event id.
 *
 * Revision 1.11  1995/09/29  15:59:57  mmattini
 * Add status and type mask
 *
 * Revision 1.10  1995/09/28  19:12:15  mmattini
 * Make the following services available from the PEM Notify API:
 *  - PemnEventRangeManage		close/del/ack event range
 *  - PemnEventRangeQuery		query events based on event range
 *  - PemnSetPersistentFilter	set persistent filter in agent
 *  - PemnEventManage		close/del/ack a single event
 *  - PemnAddDiary			add diary to an event
 *  - PemnEventQuery		query events according to filter
 *
 * Revision 1.9  1995/09/08  14:56:37  mmattini
 * change comment in PEM API.
 *
 * Revision 1.8  1995/08/17  20:24:16  mmattini
 * File privileges (contd)
 *
 * Revision 1.7  1995/08/13  16:26:42  mmattini
 * + PEM API share same communication code with with PEM/Console.
 *
 * Revision 1.6  1995/08/12  20:47:09  mmattini
 * Cleanup the PEM API and add an example
 *
 * Revision 1.5  1995/08/11  14:44:22  mmattini
 * Implement PemnSend instead of #define it.
 *
 * Revision 1.4  1995/08/09  13:54:26  mmattini
 * Add the missing PemnReceive api routine
 *
 * Revision 1.3  1995/07/04  17:19:38  mmattini
 * Add more comments to PEM API header file
 *
 * Revision 1.2  1995/06/29  16:09:32  mmattini
 * Add 'docs' for pemapi documentation.
 *     'example.c' for pemapi coding example.
 * Fix warning on OSF in cfile.c
 *
 * Revision 1.1  1995/06/28  22:23:01  mmattini
 * + Rename pem/emglobal/cf_api.h to pemapi.h to match installation
 *   scripts.
 * + Make all definitions in pemapi.h consistent (prefix Pem...)
 *
 *
 */
#endif /* __PEMAPI_h__ */

/* Don't add anything after this line !!  */
