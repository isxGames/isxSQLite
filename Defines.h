/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

////////////////////////////
// VERSION
#define EXTENSION_NAME										"isxSQLite"

#define _EXT_VER_YEAR										2020
#define _EXT_VER_MONTH										08
#define _EXT_VER_DAY										12

#ifdef USE_LIBISXGAMES
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#define EXTENSION_CLASS										isxGamesExtension
#define __isxSQLiteVersion									BOOST_PP_STRINGIZE(_EXT_VER_YEAR) BOOST_PP_STRINGIZE(_EXT_VER_MONTH) BOOST_PP_STRINGIZE(_EXT_VER_DAY)
#define _EXT_VER_STR										BOOST_PP_STRINGIZE(_EXT_VER_YEAR) "." BOOST_PP_STRINGIZE(_EXT_VER_MONTH) "." BOOST_PP_STRINGIZE(_EXT_VER_DAY)
#else
#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

#define __isxSQLiteVersion									STRINGIZE(PPCAT(PPCAT(_EXT_VER_YEAR, _EXT_VER_MONTH),_EXT_VER_DAY))
#define _EXT_VER_STR										__isxSQLiteVersion
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
