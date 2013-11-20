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
#define EXTENSION_NAME										"isxSQLite"
#ifdef USE_LIBISXGAMES
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#define _EXT_VER_YEAR										2013
#define _EXT_VER_MONTH										11
#define _EXT_VER_DAY										19
#define EXTENSION_CLASS										isxGamesExtension
#define __isxSQLiteVersion									BOOST_PP_STRINGIZE(_EXT_VER_YEAR) BOOST_PP_STRINGIZE(_EXT_VER_MONTH) BOOST_PP_STRINGIZE(_EXT_VER_DAY)
#define _EXT_VER_STR										BOOST_PP_STRINGIZE(_EXT_VER_YEAR) "." BOOST_PP_STRINGIZE(_EXT_VER_MONTH) "." BOOST_PP_STRINGIZE(_EXT_VER_DAY)
#else
#define _EXT_VER_YEAR										9999
#define _EXT_VER_MONTH										12
#define _EXT_VER_DAY										31
#define __isxSQLiteVersion									"99991231"
#define _EXT_VER_STR										"99991231"
#endif
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
