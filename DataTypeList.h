/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#ifndef DATATYPE
#define DATATYPE_SELF
#define DATATYPE(_class_,_variable_,_inherits_) extern class _class_ *_variable_
#endif
/////////
// All Datatypes used in isxSQLite are initialized below.
/////////

// These datatypes are registered immediately upon loading the extension
#ifndef IGNORE_EXT_DTYPES
DATATYPE(isxSQLiteType,pisxSQLiteType,0);
#endif

// These datatypes are registered after all other initialization routines have completed.  (Note:  Most datatypes
// fall into this category.)
#ifndef EXT_DTYPES_ONLY
DATATYPE(SQLiteType,pSQLiteType,0);
DATATYPE(SQLiteDBType,pSQLiteDBType,0);
DATATYPE(SQLiteQueryType,pSQLiteQueryType,0);
DATATYPE(SQLiteTableType,pSQLiteTableType,0);
#endif

// ----------------------------------------------------
#ifdef DATATYPE_SELF
#undef DATATYPE_SELF
#undef DATATYPE
#endif