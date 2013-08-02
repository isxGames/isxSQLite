/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma warning(disable : 4100)

#include "isxSQLite.h"
#include <delayimp.h>

#pragma comment(lib,"delayimp")
#pragma comment(lib,"isxdk")

#ifdef USE_LIBISXGAMES
#pragma comment(lib, "isxGames")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "tidylib.lib")
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "zlibstat.lib")
#pragma comment(lib, "libcurl.lib")
#endif
#ifdef USE_LAVISHSCRIPT2
#pragma comment(lib, "ls2module.lib")
#include "LavishScript 2\LS2_sqllite.h"
#endif

ISXPreSetup("isxSQLite",isxGamesExtension);

bool isxGamesExtension::Initialize(ISInterface *p_ISInterface)
{	
	pISInterface=p_ISInterface;
	gExtensionLoading = true;

	/////////////
	// Set some interally used class memebers
	if (pExtension)
	{
		if (pISInterface)
		{
			pExtension->pUIElementTLO = pISInterface->IsTopLevelObject("UIElement");
			pExtension->pLGUIElementDT = pISInterface->FindLSType("lguielement");
		}
	}
	//
	/////////////

	// Avoid letting people run the extension with an old version of InnerSpace
	if (pISInterface->GetBuildNumber() < 5065)
	{
		printf("\arThis version of isxSQLite requires Inner Space build %d or later.\ax",5065);
		if (!IsConsoleOpen())
		{
			// Open Console
			pISInterface->ExecuteCommand("console toggle");
		}
		return false;
	}

#ifdef USE_LIBISXGAMES
	// isxSQLite now requires .NET 2.0/3.5
	pISInterface->TryInitialize();
	if (!pISInterface->IsAvailable())
	{
		printf("isxSQLite *REQURES* .NET 2.0 and 3.5 (latest versions).  Please download these frameworks before using this extension.");
		gDoUnloadExtension = true;
		if (!IsConsoleOpen())
		{
			// Open Console
			pISInterface->ExecuteCommand("console toggle");
		}
		return false;
	}
#endif
	
	// Register the extension
	RegisterExtension();

	// retrieve basic ISData types
	// (NOTE:  These variables are declared in libisxgames.h, not Globals.h)
	pStringType=pISInterface->FindLSType("string");
	pMutableStringType=pISInterface->FindLSType("mutablestring");
	pWStringType=pISInterface->FindLSType("unistring");
	pUniStringType=pISInterface->FindLSType("unistring");
	pIntType=pISInterface->FindLSType("int");
	pUIntType=pISInterface->FindLSType("uint");
	pInt64Type=pISInterface->FindLSType("int64");
	pInt64PtrType=pISInterface->FindLSType("int64ptr");
	pBoolType=pISInterface->FindLSType("bool");
	pFloatType=pISInterface->FindLSType("float");
	pTimeType=pISInterface->FindLSType("time");
	pByteType=pISInterface->FindLSType("byte");
	pIntPtrType=pISInterface->FindLSType("intptr");
	pBoolPtrType=pISInterface->FindLSType("boolptr");
	pFloatPtrType=pISInterface->FindLSType("floatptr");
	pFloat64PtrType=pISInterface->FindLSType("float64ptr");
	pBytePtrType=pISInterface->FindLSType("byteptr");
	pPoint3fType=pISInterface->FindLSType("point3f");
	pIndexType=pISInterface->FindLSType("index");
	pVectorType=pISInterface->FindLSType("index");
	pMapType=pISInterface->FindLSType("collection");
	pSetType=pISInterface->FindLSType("set");

	isxGamesExtension::ConnectPulseService();
	ConnectServices();

	pExtension->RegisterExtDataTypes();
	pExtension->RegisterExtTopLevelObjects();

	#ifdef USE_LAVISHSCRIPT2	
	{
		LavishScript2::ILS2StandardEnvironment *pLS2Environment = 0;
		if (pISInterface->GetLavishScript2Environment(LS2MODULE_API_VERSION,(void**)&pLS2Environment))
		{
			LavishScript2::ILS2Module::AttachEnvironment(pLS2Environment);

			RegisterSQLiteDataTypeEnum();
			RegisterSQLiteDBType();
			RegisterSQLiteQueryType();
			RegisterSQLiteTableType();
		}
	}
	#endif

	// Register persistent class for .NET
	isxSQLiteClass = pISInterface->RegisterPersistentClass("isxSQLite");

	// Register Events
	isxGames_onHTTPResponse = pISInterface->RegisterEvent("isxGames_onHTTPResponse");
	isxSQLite_onErrorMsg = pISInterface->RegisterEvent("isxSQLite_onErrorMsg");
	isxSQLite_onStatusMsg = pISInterface->RegisterEvent("isxSQLite_onStatusMsg");

	// Be sure to 'detach' from any events that you attach to in the Shutdown routine
	isxSQLite_onUpdateComplete = pISInterface->RegisterEvent("isxSQLite_onUpdateComplete");
	pISInterface->AttachEventTarget(isxSQLite_onUpdateComplete,isxSQLitePostInitialize);
	isxSQLite_onDoShutdown = pISInterface->RegisterEvent("isxSQLite_onDoShutdown");
	pISInterface->AttachEventTarget(isxSQLite_onDoShutdown,isxSQLiteonDoShutdown);

	//Open xml files
	sprintf(XMLFileName,"%s\\isxSQLite.xml",ModulePath);
	MainXMLFileID = pISInterface->OpenSettings(XMLFileName);

	// Create Sets if they don't exist   (is this necessary?)
	unsigned int GeneralSetID = pISInterface->FindSet(MainXMLFileID,"General");
	if (GeneralSetID == 0)
		GeneralSetID = pISInterface->CreateSet(MainXMLFileID,"General");

	// Process main xml file settings
	ProcessMainXMLSettings();

	Initialize2();
	return true;
}

bool isxGamesExtension::Initialize2()
{
	// When compiled in "Release" configuration (i.e., without libisxgames) the patcher is bypassed entirely.  Therefore,
	// the extension should continue with Post Initialization straight away at this point.
	#ifdef USE_LIBISXGAMES
	strcpy(ExtensionName,"isxSQLite");
	strcpy(ExtensionMajorVersion, __isxSQLiteVersion);
	gRunPatcherNow = true;
	#else
	isxSQLitePostInitialize(0,NULL,NULL);
	#endif

	return true;
}

// This fires once the event isxSQLite_onOKToLoad has been received.
void __cdecl isxSQLitePostInitialize(int argc, char *argv[], PLSOBJECT)
{
	if (gExtensionLoadDone)
		return;

	pExtension->RegisterCommands();
	pExtension->RegisterAliases();
	pExtension->RegisterDataTypes();
	pExtension->RegisterTopLevelObjects();
	pExtension->RegisterServices();
	pExtension->RegisterTriggers();
	pExtension->RegisterDetours();

	gExtensionLoadDone = true;
	gExtensionLoading = false;

	// Everything worked and we're good to go
	VersionInfo verInfo;
	printf("\n \n\atisxSQLite v%s\ax", verInfo.GetProductVersion().c_str());
	return;
}

void __cdecl isxSQLiteonDoShutdown(int argc, char *argv[], PLSOBJECT)
{
	gDoUnloadExtension = true;

	return;
}

void isxGamesExtension::Shutdown()
{
	if (gExtensionLoadDone)
	{
		DisconnectServices();
		UnRegisterServices();
		
		if (!gDidPreUnload)
		{
			UnRegisterTopLevelObjects();
			UnRegisterDataTypes();
			UnRegisterAliases();
			UnRegisterCommands();
			if (gDetoursActive)
				UnRegisterDetours();
			UnRegisterTriggers();
		}
		UnRegisterExtTopLevelObjects();
		UnRegisterExtDataTypes();

		#ifdef USE_LAVISHSCRIPT2
		UnregisterSQLiteDataTypeEnum();
		UnregisterSQLiteDBType();
		UnregisterSQLiteQueryType();
		UnregisterSQLiteTableType();
		#endif
	}


	// Close/Unload xml files
	pISInterface->UnloadSet(MainXMLFileID);

	// Clear arrays
	FinalizeAllQueries();
	FinalizeAllTables();
	CloseAllDatabases();
	FreeClear(gDatabases);
	FreeClear(gQueries);
	FreeClear(gTables);

	pISInterface->DetachEventTarget(isxSQLite_onUpdateComplete,isxSQLitePostInitialize);
	pISInterface->DetachEventTarget(isxSQLite_onDoShutdown,isxSQLiteonDoShutdown);

	//////
	pISInterface->InvalidatePersistentClass(isxSQLiteClass);
	//////

	if (!gUnloadingExtension)
		printf("\n \n \n\atisxSQLite UNLOADED\ax");

	gUnloadingExtension = true;
}

void isxGamesExtension::RegisterExtension()
{
	VersionInfo verInfo;

	unsigned int ExtensionSetGUID=pISInterface->GetExtensionSetGUID("isxSQLite");
	if (!ExtensionSetGUID)
	{
		ExtensionSetGUID=pISInterface->CreateExtensionSet("isxSQLite");
		if (!ExtensionSetGUID)
		{
			return;
		}
	}

	pISInterface->SetSetting(ExtensionSetGUID,"Filename",ModuleFileName);
	pISInterface->SetSetting(ExtensionSetGUID,"Path",ModulePath);
	pISInterface->SetSetting(ExtensionSetGUID,"Version",verInfo.GetProductVersion().c_str());
}

void isxGamesExtension::RegisterCommands()
{
	#define COMMAND(name,cmd,parse,hide) pISInterface->AddCommand(name,cmd,parse,hide);
	#include "Commands.h"
	#undef COMMAND
}

void isxGamesExtension::RegisterAliases()
{
	// add any aliases
}

void isxGamesExtension::RegisterExtDataTypes()
{
	#define DATATYPE(_class_,_variable_,_inherits_) _variable_ = new _class_; pISInterface->AddLSType(*_variable_); _variable_->SetInheritance(_inherits_);
	#define EXT_DTYPES_ONLY
	#include "DataTypeList.h"
	#undef EXT_DTYPES_ONLY
	#undef DATATYPE
}

void isxGamesExtension::RegisterDataTypes()
{
	#define DATATYPE(_class_,_variable_,_inherits_) _variable_ = new _class_; pISInterface->AddLSType(*_variable_); _variable_->SetInheritance(_inherits_);
	#include "DataTypeList.h"
	#undef DATATYPE

	// Set Persistant Classes for .NET scripts
	pISInterface->SetPersistentClass(pisxSQLiteType,isxSQLiteClass);
}

void isxGamesExtension::RegisterTopLevelObjects()
{
	#define TOPLEVELOBJECT(name,funcname) pISInterface->AddTopLevelObject(name,funcname);
	#include "TopLevelObjects.h"
	#undef TOPLEVELOBJECT
}

void isxGamesExtension::RegisterExtTopLevelObjects()
{
	#define EXTTOPLEVELOBJECT(name,funcname) pISInterface->AddTopLevelObject(name,funcname);
	#include "ExtTopLevelObjects.h"
	#undef EXTTOPLEVELOBJECT
}

void isxGamesExtension::RegisterDetours()
{
	return;
}

void isxGamesExtension::RegisterTriggers()
{
	return;
}

void isxGamesExtension::UnRegisterCommands()
{
	#define COMMAND(name,cmd,parse,hide) pISInterface->RemoveCommand(name);
	#include "Commands.h"
	#undef COMMAND
}
void isxGamesExtension::UnRegisterAliases()
{
	// remove aliases
}

void isxGamesExtension::UnRegisterDataTypes()
{
	#define DATATYPE(_class_,_variable_,_inherits_) pISInterface->RemoveLSType(*_variable_); delete _variable_;
	#define IGNORE_EXT_DTYPES
	#include "DataTypeList.h"
	#undef IGNORE_EXT_DTYPES
	#undef DATATYPE
}

void isxGamesExtension::UnRegisterExtDataTypes()
{
	#define DATATYPE(_class_,_variable_,_inherits_) pISInterface->RemoveLSType(*_variable_); delete _variable_;
	#define EXT_DTYPES_ONLY
	#include "DataTypeList.h"
	#undef EXT_DTYPES_ONLY
	#undef DATATYPE
}

void isxGamesExtension::UnRegisterTopLevelObjects()
{
	#define TOPLEVELOBJECT(name,funcname) pISInterface->RemoveTopLevelObject(name);
	#include "TopLevelObjects.h"
	#undef TOPLEVELOBJECT
}

void isxGamesExtension::UnRegisterExtTopLevelObjects()
{
	#define EXTTOPLEVELOBJECT(name,funcname) pISInterface->RemoveTopLevelObject(name);
	#include "ExtTopLevelObjects.h"
	#undef EXTTOPLEVELOBJECT
}

void isxGamesExtension::UnRegisterDetours()
{
	return;
}

void isxGamesExtension::UnRegisterTriggers()
{
	return;
}