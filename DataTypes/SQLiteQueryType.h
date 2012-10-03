/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once

class SQLiteQueryType : public LSTypeDefinition
{
public:
	enum SQLiteQueryTypeMembers
	{
		ID,
		NumRows,
		NumFields,
		GetFieldName,
		GetFieldIndex,
		GetFieldDeclType,
		GetFieldType,
		GetFieldValue,
		FieldIsNULL,
		LastRow,
	};
	enum SQLiteQueryTypeMethods
	{
		NextRow,
		Reset,
		Finalize,
		Set
	};

	SQLiteQueryType() : LSType("sqlitequery")
	{
		TypeMember(ID);
		TypeMember(NumRows);
		TypeMember(NumFields);
		TypeMember(GetFieldName);
		TypeMember(GetFieldIndex);
		TypeMember(GetFieldDeclType);
		TypeMember(GetFieldType);
		TypeMember(GetFieldValue);
		TypeMember(FieldIsNULL);
		TypeMember(LastRow);

		TypeMethod(Finalize);
		TypeMethod(Reset);
		TypeMethod(NextRow);
		TypeMethod(Set);
	}

	virtual bool GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char * argv[], LSOBJECT &Dest);
	virtual bool GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMEMBER pMethod, int argc, char * argv[]);
	virtual bool ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen);
	virtual LSTypeDefinition *GetVariableType()
	{
		return (LSTypeDefinition*)pSQLiteQueryType;
	}
	virtual bool FromText(LSOBJECTDATA &ObjectData, int argc, char *argv[])
	{
		if (!argc) 
			ObjectData.Int = 0;
		else
			ObjectData.Int = atoi(argv[0]);
		return true;
	}
	void InitVariable(LSOBJECTDATA &ObjectData)
	{
		ObjectData.Int = 0;
	}
	void FreeVariable(LSOBJECTDATA &ObjectData)
	{
	}
};