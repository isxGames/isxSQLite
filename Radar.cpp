/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).
	Copyright 2011-2025 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what
	rights are granted under this license.
*/
//--------------------------------
// This file contains the "radar" implementation.  The base class can be found
// in RadarBase.cpp/RadarBase.h
// 
// Declarations are in Radar.h  
//--------------------------------
#include "isxSQLite.h"

LGUIFactory<LGUIGameRadar> RadarFactory("gameradar");

LGUIGameRadar::LGUIGameRadar(char *p_Factory, LGUIElement *p_pParent, char *p_Name):LGUIRadar(p_Factory,p_pParent,p_Name)
{
	RadarID=Radars.GetUnused();
	Radars[RadarID]=this;

	ShowMe = true;
	ShowNPCs = true;
	ShowPCs = true;

	ColorizeLabels = true;

	pTooltip = 0;
	NumFilters = 0;
}

LGUIGameRadar::~LGUIGameRadar(void)
{
	if (pExtension)
		Radars[RadarID]=0;
}

void LGUIGameRadar::ConvertRGB(char *in, char *out)
{
	while(char c=*in)
	{
		if (c=='\\')
		{
			if (in[1]=='#')
			{
				*out = '\a';	out++;
				in++;
				continue;
			}
		}
		*out=c;
		in++;
		out++;
	}
	*out = 0;
}

void LGUIGameRadar::UpdatePoints()
{
	bool FilterThis = false;
	LGUIGameRadar::ActorTypes ActorType = LGUIGameRadar::ActorTypes::Unknown;
	uint32_t THIS_ACTOR_ID = 0;			// placeholder

	if (Blips.size() == 0)
		return;

	// Determine playing character's location and rotation, then set these values accordingly. 
	// Setting to 0 here as a placeholder.
	Origin.X = 0.0f;
	Origin.Y = 0.0f;
	Origin.Z = 0.0f;
	Rotation = 0.0f;


	// Iterate through the game actors. (fake loop here as a placeholder)
	int tmp1 = 0;
	for (tmp1 = 0; tmp1 <= 3; tmp1++)
	{
		// Determine the actor's "type" (for colorization).  Setting to 0 here as a placeholder.
		// ActorType = GetActorType();  // example

		// Placeholder
		THIS_ACTOR_ID = 0;

		// Filter out any as necessary (routine placeholder)
		if      ( ActorType == LGUIGameRadar::ActorTypes::Me  && (!ShowMe) )			continue;
		else if ( ActorType == LGUIGameRadar::ActorTypes::NPC && (!ShowNPCs) )			continue;
		else if ( ActorType == LGUIGameRadar::ActorTypes::PC && (!ShowPCs) )			continue;
		else if ( ActorType == LGUIGameRadar::ActorTypes::Corpse && (!ShowCorpses))		continue;

		foreach( LGUIGameRadar::FilterNode *pNode,j,RadarFilters)
		{
			if (!pNode)	break;

			if (_stricmp(pNode->RadarName.c_str(), GetName()) == 0)
			{
				// compare the name of the actor with the filter's "keyword"
				/*
				if (stristr(THIS_ACTOR_NAME), (char*)pNode->Keyword.c_str())
					FilterThis = true;
				*/
			}
		}
		if (FilterThis)
		{
			FilterThis = false;
			continue;
		}


		// All appropriate actors have been filtered.  At this point, actors should be on the radars.
		// Using placeholders below.

		// Set actor's X/Y/Z location
		POINT3F pt;
		pt.X= 0.0f;
		pt.Y= 0.0f;
		pt.Z= 0.0f;

		if (ActorType == LGUIGameRadar::ActorTypes::Corpse)
		{
			// psudo code (placeholder)
			/* 
			UpdateBlipType(THIS_ACTOR_ID, "ACTOR TYPE");
			sprintf(buf, "Corpse of %s", THIS_ACTOR_NAME);
			UpdateBlipLabel(THIS_ACTOR_ID,buf);
			continue;
			*/
		}

		if (ActorType == LGUIGameRadar::ActorTypes::Me)
		{
			// convert to X=east/west, Y=north/south, Z=altitude
			Origin.X = 0.0f;
			Origin.Y = 0.0f;
			Origin.Z = 0.0f;
			Rotation = 0.0f;
		}

		UpdateBlip(THIS_ACTOR_ID,pt);

		if (ColorizeLabels)
		{
			switch (ActorType)
			{
				case LGUIGameRadar::ActorTypes::NPC:
				{
					// psudo code (placeholder) 
					/* 
					uint32_t Color = GetRBGDifficultyColorForThisActor();
					UpdateBlipLabel(pActor->ID, THIS_ACTOR_NAME, Color);
					break;
					*/
				}
				default:
					break;
			}
		}
		else
		{
			UpdateBlipLabel(THIS_ACTOR_ID, "THIS_ACTOR_NAME");
		}
	}

	return;
}

bool LGUIGameRadar::IsTypeOf(char *TestFactory)
{
	return (!stricmp(TestFactory,"gameradar")) || LGUIRadar::IsTypeOf(TestFactory);
}

void LGUIGameRadar::BlipLMouseUp(LGUIRadarBlip *pBlip)
{
}

void LGUIGameRadar::BlipLMouseDown(LGUIRadarBlip *pBlip)
{
	// In many games, left clicking on an actor would "target" that actor.  
	// Note that pBlip->BlipID will be the actor's "ID"
}

void LGUIGameRadar::BlipRMouseUp(LGUIRadarBlip *pBlip)
{
}

void LGUIGameRadar::BlipRMouseDown(LGUIRadarBlip *pBlip)
{
}

void LGUIGameRadar::BlipHover(LGUIRadarBlip *pBlip, POINT2I &Pos)
{
	// Set a tooltip for a radar blip when hovering over it.
	if (!pTooltip)				return;
	if (!pBlip)					return;

	HoverID=pBlip->BlipID;

	// clear current tooltip text
	pTooltip->Reset();

	// Next, go through the list of game actors and find the one that matches the pBlip->BlipID (actor ID) for this particular blip.  Then, if it's 
	// an actor for which a tooltip is appropriate, keep going below.  Otherwise, just return;

	// set position
	pTooltip->SetLeft(pBlip->GetLeft() + 10);
	pTooltip->SetTop(pBlip->GetTop() + pBlip->GetHeight() + 2);
	// make sure it's visible
	pTooltip->Show(true);

	pTooltip->Printf("your tooltip, likely as a buffer");

	return;
}

void LGUIGameRadar::BlipHoverStop(LGUIRadarBlip *pBlip)
{
	// hide the tooltip
	if (pTooltip)
		pTooltip->Show(false);	
}

bool LGUIGameRadar::Populate()
{
	bool FilterThis = false;
	LGUIGameRadar::ActorTypes ActorType = LGUIGameRadar::ActorTypes::Unknown;
	uint32_t THIS_ACTOR_ID = 0;			// placeholder


	// Iterate through the game actors. (fake loop here as a placeholder)
	int tmp1 = 0;
	for (tmp1 = 0; tmp1 <= 3; tmp1++)
	{
		// Determine the actor's "type" (for colorization).  Setting to 0 here as a placeholder.
		// ActorType = GetActorType();  // example

		// Placeholder
		THIS_ACTOR_ID = 0;

		// Filter out any as necessary (routine placeholder)
		if (ActorType == LGUIGameRadar::ActorTypes::Me && (!ShowMe))					continue;
		else if (ActorType == LGUIGameRadar::ActorTypes::NPC && (!ShowNPCs))			continue;
		else if (ActorType == LGUIGameRadar::ActorTypes::PC && (!ShowPCs))				continue;
		else if (ActorType == LGUIGameRadar::ActorTypes::Corpse && (!ShowCorpses))		continue;

		foreach(LGUIGameRadar::FilterNode * pNode, j, RadarFilters)
		{
			if (!pNode)	break;

			if (_stricmp(pNode->RadarName.c_str(), GetName()) == 0)
			{
				// compare the name of the actor with the filter's "keyword"
				/*
				if (stristr(THIS_ACTOR_NAME), (char*)pNode->Keyword.c_str())
					FilterThis = true;
				*/
			}
		}
		if (FilterThis)
		{
			FilterThis = false;
			continue;
		}

		// All appropriate actors have been filtered.  At this point, actors should be on the radars.
		// Using placeholders below.

		// Set actor's X/Y/Z location
		POINT3F pt;
		pt.X = 0.0f;
		pt.Y = 0.0f;
		pt.Z = 0.0f;

		AddBlip(THIS_ACTOR_ID, pt, "THIS ACTOR TYPE", 8, "THIS ACTOR NAME");
		UpdateBlipType(THIS_ACTOR_ID, "THIS ACTOR TYPE");

		if (ColorizeLabels)
		{
			switch (ActorType)
			{
				case LGUIGameRadar::ActorTypes::NPC:
				{
					// psudo code (placeholder) 
					/*
					uint32_t Color = GetRBGDifficultyColorForThisActor();
					UpdateBlipLabel(pActor->ID, "THIS_ACTOR_NAME", Color);
					break;
					*/
				}
				default:
					break;
			}
		}
		else
		{
			UpdateBlipLabel(THIS_ACTOR_ID, "THIS_ACTOR_NAME");
		}

	}
	return true;

}

bool LGUIGameRadar::FromXML(class XMLNode *pXML, class XMLNode *pTemplate)
{
	if (!pTemplate)
		pTemplate=g_UIManager.FindTemplate(XMLHelper::GetStringAttribute(pXML,"Template"));
	if (!pTemplate)
		pTemplate=g_UIManager.FindTemplate("gameradar");

	if (!LGUIRadar::FromXML(pXML,pTemplate))
		return false;

	ShowMe=XMLHelper::GetTemplateToggle(pXML,"ShowMe",pTemplate);
	ShowNPCs=XMLHelper::GetTemplateToggle(pXML,"ShowNPCs",pTemplate);
	ShowPCs=XMLHelper::GetTemplateToggle(pXML,"ShowPCs",pTemplate);
	ShowCorpses=XMLHelper::GetTemplateToggle(pXML,"ShowCorpses",pTemplate);
	ColorizeLabels=XMLHelper::GetTemplateToggle(pXML,"ColorizeLabels",pTemplate);

	Populate();   // this function populates the radar

	return true;
}

void LGUIGameRadar::OnCreate()
{
	char buf[500];	strcpy(buf, "Radar Tooltip");
	char buf2[500];	strcpy(buf2, "tooltip");

	//pTooltip=(LGUITooltip*)FindUsableChild("Radar Tooltip","tooltip");
	pTooltip = (LGUITooltip*)FindUsableChild(buf,buf2);
	if (!pTooltip)
	{
		printf("No tooltip named \"Radar Tooltip\" in the radar ui xml file!");
	}
}

#pragma region ISXEQ2 Global Variable Definitions
CIndex<class LGUIGameRadar *> Radars;
#pragma endregion
