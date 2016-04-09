/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// The Pulse() function is called every time that the InnerSpace Pulse service
// broadcasts a PULSE_PULSE message (see Services.cpp.) 
//--------------------------------
#include "isxSQLite.h"

void Pulse()
{
#pragma region DO NOT EDIT -- ALL NEW CODE SHOULD GO BELOW THIS REGION
	if (gWaitPulses > 0)
	{
		gWaitPulses--;
		return;
	}

	/////////////
	// Unload isxSQLite if necessary
	if (gDoUnloadExtension)
	{
		pISInterface->ExecuteCommand("execute timedcommand 5 \"ext -unload isxSQLite\"");
		gDoUnloadExtension = false;
		gUnloadingExtension = true;
	}
	if (gUnloadingExtension  || gDidPreUnload)
		return;
	// Or, if we just want to remove all the functionality and allow the extension to run in order to accept certain events
	if (gDoUnloadExtension)
	{
		pExtension->UnRegisterTopLevelObjects();
		pExtension->UnRegisterDataTypes();
		pExtension->UnRegisterAliases();
		pExtension->UnRegisterCommands();
		if (gDetoursActive)
			pExtension->UnRegisterDetours();
		pExtension->UnRegisterTriggers();
		gDidPreUnload = true;
		gDoUnloadExtension = false;
		return;
	}
	//
	/////////////

	#ifdef USE_LIBISXGAMES
	/////////////
	// Run the patcher after authentication is complete.
	if (gRunPatcherNow)
	{
		if (!gUseTestVersion)
			RunThePatcher(false);
		else
		{
			// Since we have loaded now with the "Use Test Version" flag, remove the flag.
			pISInterface->SetSetting(MainXMLFileID,"Use Test Version",false);
			pISInterface->ExportSet(MainXMLFileID,XMLFileName);
			gUseTestVersion = false;
			RunThePatcher(true);
		}
		gRunPatcherNow = false;
	}
	//
	/////////////
	#endif

	/////////////
	// Don't go any further if certain conditions are applicable
	if (!gExtensionLoadDone)		
		return;			
	//
	/////////////
#pragma endregion

	return;
}

#ifdef USE_LIBISXGAMES
void RunThePatcher(bool UseTestVersion)
{
	std::string Command;

	// Run the patcher!
	if (pISInterface->IsAvailable())		// Remove this check eventually so that isxSQLite actually REQUIRES .NET 2.0
	{
		VersionInfo verInfo;

		if (!pISInterface->IsDomainActive("isxSQLitePatcher"))
		{
			printf("\n");
			if (!UseTestVersion)
				Command = format("dotnet isxSQLitePatcher isxGamesPatcher isxSQLite %s http://www.isxgames.com/isxsqlite/isxSQLiteManifest.xml " ,verInfo.GetProductVersion().c_str());
			else
				Command = format("dotnet isxSQLitePatcher isxGamesPatcher isxSQLite %s http://www.isxgames.com/isxsqlite/isxSQLiteManifest-TestVersion.xml ", verInfo.GetProductVersion().c_str());
			pISInterface->ExecuteCommand(Command.c_str());
			if (UseTestVersion)
				return;
		}
		else
			isxSQLitePostInitialize(0,NULL,NULL);
	}
	else
		isxSQLitePostInitialize(0,NULL,NULL);

	return;
};
#endif