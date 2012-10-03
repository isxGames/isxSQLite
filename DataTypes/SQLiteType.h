/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

class SQLiteType : public LSTypeDefinition
{
public:
	enum SQLiteTypeMembers
	{
		OpenDB,
		GetQueryByID,
		Escape_String
	};
	enum SQLiteTypeMethods
	{
	};

	SQLiteType() : LSType("sqlite")
	{
		TypeMember(Escape_String);
		TypeMember(OpenDB);
		TypeMember(GetQueryByID);
	}

	virtual bool GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char * argv[], LSOBJECT &Dest);
	virtual bool GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMEMBER pMethod, int argc, char * argv[]);
	virtual bool ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen);

	bool FromData(LSOBJECTDATA &ObjectData, LSOBJECT &Source)	{return false;}
	bool FromText(LSOBJECTDATA &ObjectData, char *Source)	{return false;}
	void InitVariable(LSOBJECTDATA &ObjectData)	{ObjectData.Ptr = 0;}
	void FreeVariable(LSOBJECTDATA &ObjectData)	{return;}
};