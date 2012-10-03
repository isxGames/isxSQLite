/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

class SQLiteDBType : public LSTypeDefinition
{
public:
	enum SQLiteDBTypeMembers
	{
		ExecQuery,
		TableExists,
		GetTable,
		ID
	};
	enum SQLiteDBTypeMethods
	{
		Close,
		ExecDML,
		ExecDMLTransaction,
		Set
	};

	SQLiteDBType() : LSType("sqlitedb")
	{
		TypeMember(ExecQuery);
		TypeMember(TableExists);
		TypeMember(GetTable);
		TypeMember(ID);

		TypeMethod(Close);
		TypeMethod(ExecDML);
		TypeMethod(ExecDMLTransaction);
		TypeMethod(Set);
	}

	virtual bool GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char * argv[], LSOBJECT &Dest);
	virtual bool GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMEMBER pMethod, int argc, char * argv[]);
	virtual bool ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen);
	virtual LSTypeDefinition *GetVariableType()
	{
		return (LSTypeDefinition*)pSQLiteDBType;
	}
	virtual bool FromText(LSOBJECTDATA &ObjectData, int argc, char *argv[])
	{
		if (!argc) 
		{
			ObjectData.CharPtr = nullptr;
			return true;
		}
		if (strlen(argv[0]) <= 0)
			return false;
		
		ObjectData.CharPtr=strdup(argv[0]);
		return true;
	}

	void InitVariable(LSOBJECTDATA &ObjectData)
	{
		ObjectData.CharPtr = nullptr;
	}
	void FreeVariable(LSOBJECTDATA &ObjectData)
	{
		if (ObjectData.CharPtr)
			free(ObjectData.CharPtr);
	}
};