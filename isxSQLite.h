/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// This is the primary header for isxSQLite.  All source files should include this
// header.
//--------------------------------
#pragma once
#include "Defines.h"

#ifdef USE_LIBISXGAMES
#include <isxGames.h>
#else
#include "libisxgames\libisxgames.h"
#endif

extern char SQLite_Version[];
extern char ModulePath[MAX_PATH];
using namespace std;

#include "Internals.h"
#include "Utilities.h"
#include "Commands.h"
#include "TopLevelObjects.h"
#include "ExtTopLevelObjects.h"
#include "Services.h"
#include "Globals.h"
#include "Pulse.h"
#include "DataTypes.h"
