/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#ifndef COMMAND
#define COMMAND_SELF
#define COMMAND(name,cmd,parse,hide) extern int cmd(int argc, char *argv[])
#endif

/////////
// All commands added by isxSQLite to the InnerSpace console are initialized below.
/////////

#ifdef USE_LIBISXGAMES
COMMAND("DebugSpew",CMD_DebugSpew,true,false);
COMMAND("GetURL",CMD_GetURL,true,false);
#endif


// "sqlite" is a command for testing code.
COMMAND("sqlite",CMD_TestSQLite,true,false);


// ----------------------------------------------------
#ifdef COMMAND_SELF
#undef COMMAND_SELF
#undef COMMAND
#endif