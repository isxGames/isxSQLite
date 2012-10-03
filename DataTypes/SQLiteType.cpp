/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"

bool SQLiteType::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	TESTCALLS_LOG_TOTEXT;

	if (!ObjectData.Ptr)
		return false;

	strcpy(buf,"sqlite");
	return true;
}

bool SQLiteType::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Dest)
{
	TESTCALLS_LOG_GETMEMBER;

	if (!ObjectData.Ptr)
		return false;

	switch((SQLiteTypeMembers)pMember->ID)
	{
		case OpenDB:
		{
			if (argc != 2)
				return false;

			std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator i = gDatabases.find(argv[0]);
			if (i != gDatabases.end())
			{
				#pragma region isxSQLite_onErrorMsg
				std::string s = format("SQLite.OpenDB::  This database name is already in use.");
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)"-1",
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
				return false;
			}

			CppSQLite3DB *pDB = OpenDatabase(argv[0],argv[1]);			// if everything goes well, this opens the DB and adds it to gDatabases
			if (!pDB)
				return false;											// error message generated and sent to user in OpenDatabase() above

			Dest.CharPtr = strdup(argv[0]);
			Dest.Type = pSQLiteDBType;
			return true;
		}
		case GetQueryByID:
		{
			if (argc != 1)
				return false;
			if (!IsNumber(argv[0]))
				return false;

			std::map<int,CppSQLite3Query*>::iterator It = gQueries.find(atoi(argv[0]));
			if (It == gQueries.end())
				return false;
			
			Dest.Int = It->first;
			Dest.Type = pSQLiteQueryType;
			return true;
		}
		case Escape_String:
		{
			if (argc != 1)
				return false;
			
			char *zSQL = sqlite3_mprintf("%q",argv[0]);
			Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(zSQL);
			Dest.Type = pStringType;
			sqlite3_free(zSQL);
			return true;
		}
	}


	return false;
}

bool SQLiteType::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
	TESTCALLS_LOG_GETMETHOD;

	if (!ObjectData.Ptr)
		return false;

	//switch((SQLiteTypeMethods)pMethod->ID)
	//{
	//}

	return false;
}