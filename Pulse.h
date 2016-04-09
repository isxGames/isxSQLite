/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------
// This file contains function declarations for Pulse.cpp
//--------------------------
#pragma once

extern void Pulse();
extern void InitializeSQLitePulse();
extern void ShutdownSQLitePulse();

#ifdef USE_LIBISXGAMES
extern void RunThePatcher(bool UseTestVersion);
#endif