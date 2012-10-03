/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"

bool isxSQLiteType::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	TESTCALLS_LOG_TOTEXT;

	if (!ObjectData.Ptr)
		return false;

	strcpy(buf,"isxsqlite");
	return true;
}

bool isxSQLiteType::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Dest)
{
	TESTCALLS_LOG_GETMEMBER;

	if (!ObjectData.Ptr)
		return false;

	switch((isxSQLiteTypeMembers)pMember->ID)
	{
		case Version:
		{
			VersionInfo verInfo;
			Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(verInfo.GetProductVersion());
			Dest.Type = pStringType;
			return true;
		}
		case IsReady:
			Dest.Int = (int)gExtensionLoadDone;
			Dest.Type = pBoolType;
			return true;
		case IsLoading:
			Dest.Int = (int)gExtensionLoading;
			Dest.Type = pBoolType;
			return true;
		case InQuietMode:
			Dest.Int = gQuietMode;
			Dest.Type = pBoolType;
			return true;
	}


	return false;
}

bool isxSQLiteType::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
	TESTCALLS_LOG_GETMETHOD;

	if (!ObjectData.Ptr)
		return false;

	switch((isxSQLiteTypeMethods)pMethod->ID)
	{
		case QuietMode:
		{
			if (gQuietMode)
			{
				gQuietMode = false;
				printf("isxSQLite:: QuietMode INACTIVE");
			}
			else
			{
				gQuietMode = true;
				printf("isxSQLite:: QuietMode ACTIVE");
			}
			return true;
		}
	}

	return false;
}