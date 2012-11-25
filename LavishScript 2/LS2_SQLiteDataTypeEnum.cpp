// LS2Type_Foo.cpp - example from a LavishScript 2.0 module

// include LavishScript 2.0 Module stuff
#include "LS2_sqllite.h"
#include "../isxSQLite.h"

#define g_pLS2Environment LavishScript2::ILS2StandardEnvironment::s_pInstance
LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteDataTypeEnum=0;
// set up shorthand form of the LS2 value class we'll use for our DB class



namespace LavishScript2
{
	namespace Types
	{
		namespace SQLite
		{
			namespace DataTypeEnum
			{
				// i'm not sure we need any member functions for an enum.
			};
		};
	};
};

bool RegisterSQLiteDataTypeEnum()
{
	LavishScript2::LS2SmartRef<LavishScript2::ILS2CodeBoxType> pSystemEnum;
	g_pLS2Environment->ResolveType(L"System.Enum",pSystemEnum);

	if (!g_pLS2Environment->RegisterType(L"SQLite.DataTypeEnum",pSystemEnum,&g_pLS2_SQLiteDataTypeEnum))
	{
		if (!gQuietMode)
			printf("\arisxSQLite failed to register LavishScript 2 type SQLite.Enum");
		return false;
	}

	LavishScript2::ILS2CodeBoxType *pType = g_pLS2_SQLiteDBType;
	{
		LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_Enum> pEnumValue;
		g_pLS2Environment->NewEnum(1,pSystemEnum,pEnumValue);
		if (!g_pLS2_SQLiteDataTypeEnum->RegisterStaticField(L"Integer",L"",L"Integer=1",pEnumValue,0))
		{
			return false;
		}
		g_pLS2Environment->NewEnum(2,pSystemEnum,pEnumValue);
		g_pLS2_SQLiteDataTypeEnum->RegisterStaticField(L"Float",L"",L"Float=2",pEnumValue,0);

		g_pLS2Environment->NewEnum(3,pSystemEnum,pEnumValue);
		g_pLS2_SQLiteDataTypeEnum->RegisterStaticField(L"Text",L"",L"Text=3",pEnumValue,0);

		g_pLS2Environment->NewEnum(4,pSystemEnum,pEnumValue);
		g_pLS2_SQLiteDataTypeEnum->RegisterStaticField(L"Blob",L"",L"Blob=4",pEnumValue,0);			

		g_pLS2Environment->NewEnum(5,pSystemEnum,pEnumValue);
		g_pLS2_SQLiteDataTypeEnum->RegisterStaticField(L"Null",L"",L"Null=5",pEnumValue,0);			
	}

	return true;
}
void UnregisterSQLiteDataTypeEnum()
{
	if (!g_pLS2_SQLiteDataTypeEnum)
		return;
	g_pLS2_SQLiteDataTypeEnum->Delete();
	g_pLS2_SQLiteDataTypeEnum=0;
}
