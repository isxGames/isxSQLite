/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#ifndef SERVICE
#define SERVICE_SELF
#define SERVICE(_name_,_callback_,_variable_) extern HISXSERVICE _variable_;extern void __cdecl _callback_(ISXInterface *pClient, unsigned int MSG, void *lpData);
#endif
////////////
// All custom services provided by isxSQLite are declared below.  The service functions are defined in Services.cpp
////////////


SERVICE("SQLite Service",SQLiteService,hSQLiteService);











// ----------------------------------------------------
#ifdef SERVICE_SELF
#undef SERVICE_SELF
#undef SERVICE
#endif