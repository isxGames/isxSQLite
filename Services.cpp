/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// This file contains the source code necessary to attach to innerspace services.
// It also contains any custom services provided by isxSQLite (which would be 
// declared in Services.h.)
//--------------------------------
#include "isxSQLite.h"

// Definitions for InnerSpace service handlers.
// NOTE:  All custom service handlers are defined and declared in Services.h.
HISXSERVICE hPulseService;
HISXSERVICE hMemoryService;
HISXSERVICE hHTTPService;
HISXSERVICE hTriggerService;
HISXSERVICE hModulesService;
HISXSERVICE hSystemService;

// Forward declarations of callbacks for InnerSpace services
// NOTE:  All custom service callback function declarations and definitions are in Services.h.
void __cdecl PulseService(bool Broadcast, unsigned int MSG, void *lpData);
void __cdecl MemoryService(bool Broadcast, unsigned int MSG, void *lpData);
void __cdecl TriggerService(bool Broadcast, unsigned int MSG, void *lpData);
void __cdecl HTTPService(bool Broadcast, unsigned int MSG, void *lpData);
void __cdecl ModulesService(bool Broadcast, unsigned int MSG, void *lpData);
void __cdecl SystemService(bool Broadcast, unsigned int MSG, void *lpData);	

// Declare/Define custom services
#define SERVICE(_name_,_callback_,_variable_) HISXSERVICE _variable_=0;
#include "Services.h"
#undef SERVICE

void isxGamesExtension::ConnectServices()
{
	// connect to any services.  Here we connect to "Pulse" which receives a
	// message every frame (after the frame is displayed) and "Memory" which
	// wraps "detours" and memory modifications
	//
	// NOTE:  Custom services are handled below (ie, we "register" custom services, not "connect" to them.
	hMemoryService=pISInterface->ConnectService(this,"Memory",MemoryService);
	hHTTPService=pISInterface->ConnectService(this,"HTTP",HTTPService);
	hTriggerService=pISInterface->ConnectService(this,"Triggers",TriggerService);
	hModulesService=pISInterface->ConnectService(this,"Modules",ModulesService);
	hSystemService=pISInterface->ConnectService(this,"System",SystemService);
}

void isxGamesExtension::DisconnectServices()
{
	// gracefully disconnect from services
	if (hPulseService)
		pISInterface->DisconnectService(this,hPulseService);
	if (hMemoryService)
	{
		pISInterface->DisconnectService(this,hMemoryService);
		// memory modifications are automatically undone when disconnecting
		// also, since this service accepts messages from clients we should reset our handle to
		// 0 to make sure we dont try to continue using it
		hMemoryService=0; 
	}
	if (hHTTPService)
	{
		pISInterface->DisconnectService(this,hHTTPService);
	}
	if (hTriggerService)
	{
		pISInterface->DisconnectService(this,hTriggerService);
	}
	if (hModulesService)
	{
		pISInterface->DisconnectService(this,hModulesService);
	}
	if (hSystemService)
	{
		pISInterface->DisconnectService(this,hSystemService);
	}
}

void isxGamesExtension::RegisterServices()
{
	// register any services.  Here we demonstrate a service that does not use a
	// callback
	// set up a 1-way service (broadcast only)
	// broadcast a message, which is worthless at this point because nobody will receive it
	// (nobody has had a chance to connect)

#define SERVICE(_name_,_callback_,_variable_) _variable_=pISInterface->RegisterService(this,_name_,_callback_);
#include "Services.h"
#undef SERVICE
}

void isxGamesExtension::UnRegisterServices()
{
	// shutdown our own services

#define SERVICE(_name_,_callback_,_variable_) _variable_=pISInterface->ShutdownService(this,_variable_);
#include "Services.h"
#undef SERVICE
}

// The "Pulse" service is registered seperately from other services.  This is to ensure that the extension is 
// fully loaded and ready to go before anything might be checked every pulse.
void isxGamesExtension::ConnectPulseService()
{
	hPulseService=pISInterface->ConnectService(this,"Pulse",PulseService);
	return;
}


void __cdecl PulseService(bool Broadcast, unsigned int MSG, void *lpData)
{
	if (MSG==PULSE_PULSE)
	{
		Pulse();
	}
}

void __cdecl MemoryService(bool Broadcast, unsigned int MSG, void *lpData)
{
	// no messages are currently associated with this service (other than
	// system messages such as client disconnect), so do nothing.
}

void __cdecl TriggerService(bool Broadcast, unsigned int MSG, void *lpData)
{
	// no messages are currently associated with this service (other than
	// system messages such as client disconnect), so do nothing.
}


//#define SpewHTTPService
void __cdecl HTTPService(bool Broadcast, unsigned int MSG, void *lpData)
{
#define pReq ((HttpFile*)lpData)

	#ifdef SpewHTTPService
	DebugSpew(TOFILEOVERWRITE,"HTTPService Report for '%s'",pReq->URL);
	printf("HTTPService Report for '%s'",pReq->URL);
	#endif

	switch(MSG)
	{
		case HTTPSERVICE_FAILURE:
		{
			// HTTP request failed to retrieve document
			break;
		}
		case HTTPSERVICE_SUCCESS:
		{
			// HTTP request successfully retrieved document
			break;
		}
	}


#undef pReq
	return;
}


void __cdecl SQLiteService(ISXInterface *pClient, unsigned int MSG, void *lpData)
{
	switch(MSG)
	{
	case ISXSERVICE_CLIENTADDED:
		// This message indicates that a new client has been added to the service
		// pClient is 0, because this message is a system message from Inner Space
		// lpData is an ISXInterface* that is the pointer to the new client
		{
			// use lpData as the actual type, not as void *.  We can make a new
			// variable to do this:

			//ISXInterface *pNewClient=(ISXInterface *)lpData;
			//printf("VGService client added: %X",pNewClient);

			// You may use the client pointer (pNewClient here) as an ID to track client-specific
			// information.  Some services such as the memory service do this to automatically
			// remove memory modifications made by an extension when that extension is unloaded.
		}
		break;
	case ISXSERVICE_CLIENTREMOVED:
		// This message indicates that a client has been removed from the service
		// pClient is 0, because this message is a system message from Inner Space
		// lpData is an ISXInterface* that is the pointer to the removed client
		{
			// use lpData as the actual type, not as void *.  We can make a new
			// variable to do this:

			//ISXInterface *pRemovedClient=(ISXInterface *)lpData;
			//printf("VGService client removed: %X",pRemovedClient);
		}
		break;
	}
}

void __cdecl ModulesService(bool Broadcast, unsigned int MSG, void *lpData)
{
#define pModule ((_ModuleServiceLibrary*)lpData)

	switch(MSG)
	{
		case MODULESERVICE_LOADLIBRARY:
			break;

		case MODULESERVICE_FREELIBRARY:
			break;


	}

	return;

#undef pModule
}

void __cdecl SystemService(bool Broadcast, unsigned int MSG, void *lpData)
{
//	virtual HISXSERVICE ConnectService(ISXInterface *pClient, const char *ServiceName, fISService NotifyCallback)=0;
//	virtual bool ServiceRequest(ISXInterface *pClient, HISXSERVICE hService, unsigned int MSG, void *lpData)=0;
//	virtual void DisconnectService(ISXInterface *pClient, HISXSERVICE hService)=0;

	switch(MSG)
	{
		case SYSTEMSERVICE_DIAGNOSTICS:
		{
			return;
		}
		case SYSTEMSERVICE_CRASHLOG:
		{
			return;
		}
	}
	
}