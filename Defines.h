/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

////////////////////////////
// VERSION
#define __isxSQLiteVersion									"20130609"
//
///////////////////////////

////////////////////////////
// SETTINGS
#define TESTCALLS				0						// Default: 0 - Logs all datatype method/member and TLO calls
#if TESTCALLS
#define TESTCALLS_TOFILE		1						// Default: 1 - Spew TESTCALLS output to file rather than the debugger
#endif
//
////////////////////////////

////////////////////////////
// DO NOT EDIT BELOW THIS LINE
////////////////////////////
#if TESTCALLS
#if TESTCALLS_TOFILE
#define TESTCALLS_LOG_GETMETHOD	\
	{ \
		DebugSpew(TESTCALLS_TOFILE "%d\t%s::%s (%s)",pISInterface->GetRuntime(),this->GetName(),pMethod->Name,GetArgvStringForTestingCalls(argc,argv).c_str()); \
	}
#define TESTCALLS_LOG_GETMEMBER \
	{ \
		DebugSpew(TESTCALLS_TOFILE "%d\t%s::%s (%s)",pISInterface->GetRuntime(),this->GetName(),pMember->Name,GetArgvStringForTestingCalls(argc,argv).c_str()); \
	}
#define TESTCALLS_LOG_TOTEXT \
	{ \
		DebugSpew(TESTCALLS_TOFILE "%d\t%s::ToText()", pISInterface->GetRuntime(),this->GetName()); \
	}
#else
#define TESTCALLS_LOG_GETMETHOD	\
	{ \
		DebugSpew("%d\t%s::%s (%s)",pISInterface->GetRuntime(),this->GetName(),pMethod->Name,GetArgvStringForTestingCalls(argc,argv).c_str()); \
	}
#define TESTCALLS_LOG_GETMEMBER \
	{ \
		DebugSpew("%d\t%s::%s (%s)",pISInterface->GetRuntime(),this->GetName(),pMember->Name,GetArgvStringForTestingCalls(argc,argv).c_str()); \
	}
#define TESTCALLS_LOG_TOTEXT \
	{ \
		DebugSpew("%d\t%s::ToText()", pISInterface->GetRuntime(),this->GetName()); \
	}
#endif
#else
#define TESTCALLS_LOG_GETMETHOD
#define TESTCALLS_LOG_GETMEMBER
#define TESTCALLS_LOG_TOTEXT
#endif