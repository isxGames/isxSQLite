/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

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
		//////////////////
		// Although it's counter intuitive, it actually makes it easier for scripts if this function always returns true so that
		// the variable can initialize.   Then, in the ToText, GetMethod, GetMember functions immediately check for validity
		// and return false.
		//////////////////
		if (!argc || strlen(argv[0]) <= 0)
		{
			ObjectData.CharPtr = nullptr;
			return true;
		}
		
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
		{
			free(ObjectData.CharPtr);
			ObjectData.CharPtr = nullptr;
		}
	}
};