/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).
	Copyright 2011-2025 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what
	rights are granted under this license.
*/
//--------------------------------
// This file contains the "radar" base class. See BaseRadar.h for 
// declarations.
// 
// The implementation will be found in Radar.cpp/Radar.h  
//--------------------------------
#include "isxSQLite.h"

LGUIFactory<LGUIRadar> RadarFactory("radar");
LGUIRadar::LGUIRadar(char *p_Factory, LGUIElement *p_pParent, char *p_Name):LGUIElement(p_Factory,p_pParent,p_Name)
{
	Origin.X=0;
	Origin.Y=0;
	Origin.Z=0;
	Rotation=0;
	bClipZ=false;
	ZClipping.X=-10.0f;
	ZClipping.Y=10.0f;
	bClipRadius=false;
	RadarSize=100.0f;
	bShowLabels=true;
	bRotation=false;
	bClipText=false;
	NextUniqueID=1;
	strcpy(BlipFactory,"radarblip");
	HoverID=0;
	pFont=0;
}

LGUIRadar::~LGUIRadar(void)
{
	if (pFont)
	{
		pFont->Delete();
		pFont=0;
	}

	map<string,LGUITexture *>::iterator i;
	for (i=BlipTypes.begin() ; i!=BlipTypes.end(); i++)
	{
		if (i->second)
			i->second->Delete();
	}
}

bool LGUIRadar::FromXML(class XMLNode *pXML, class XMLNode *pTemplate)
{
	if (!pTemplate)
		pTemplate=g_UIManager.FindTemplate(XMLHelper::GetStringAttribute(pXML,"Template"));
	if (!pTemplate)
		pTemplate=g_UIManager.FindTemplate("radar");
	if (!LGUIElement::FromXML(pXML,pTemplate))
		return false;

	bClipZ=XMLHelper::GetTemplateToggle(pXML,"ClipY",pTemplate);
	bClipRadius=XMLHelper::GetTemplateToggle(pXML,"ClipRadius",pTemplate);
	bShowLabels=XMLHelper::GetTemplateToggle(pXML,"ShowLabels",pTemplate);
	bRotation=XMLHelper::GetTemplateToggle(pXML,"Rotation",pTemplate);
	RadarSize=XMLHelper::GetTemplateFloatValue(pXML,"RadarSize",pTemplate);
	bClipText=XMLHelper::GetTemplateToggle(pXML,"ClipText",pTemplate);

	// Font includes font typeface, height, and color as described in ISUI wiki
	pFont=FontFromXML(pXML,"Font",pTemplate);

	if (pFont)
	{
		gDefaultRadarTextFontColor = pFont->GetColor();
		gDefaultRadarTextFontSize = pFont->GetHeight();
	}

	if (pTemplate)
	{
		CIndex<XMLNode *> TemplateStack;

		XMLNode *pNode=pTemplate;
		char *Parent;
		do
		{
			TemplateStack+=pNode;
			Parent=(char*)XMLHelper::GetStringAttribute(pNode,(const char*)"Template");
			if (!Parent[0])
				break;
			pNode=g_UIManager.FindTemplate(Parent);
		}
		while (pNode);

		for (int i = TemplateStack.Size-1 ; i >=0 ; i--)
		if (pNode=TemplateStack[i])
		{
			if (pNode=XMLHelper::GetChild(pNode,"BlipTypes"))
			{
				if (pNode=XMLHelper::GetFirstChild(pNode))
				{
					do
					{
						char *sName=(char*)XMLHelper::GetStringAttribute(pNode,(const char*)"Name");
						if (sName[0])
							AddBlipType(sName,pNode);
					}
					while (pNode=XMLHelper::GetNext(pNode));
				}
			}

		}
	}

	if (XMLNode *pNode=XMLHelper::GetChild(pXML,"BlipTypes"))
	{
		if (pNode=XMLHelper::GetFirstChild(pNode))
		{
			do
			{
				char *sName=(char*)XMLHelper::GetStringAttribute(pNode,(const char*)"Name");
				if (sName[0])
					AddBlipType(sName,pNode);
			}
			while (pNode=XMLHelper::GetNext(pNode));
		}
	}

	return true;
}

bool LGUIRadar::IsTypeOf(char *TestFactory)
{
	return (!stricmp(TestFactory,"radar")) || LGUIElement::IsTypeOf(TestFactory);
}

void LGUIRadar::Render()
{
	UpdatePoints();
	if (!RadarSize)
		RadarSize=1.0f;

	if (bClipRadius)
		UpdateRadius();
	else
		UpdateBox();

	// Let blips render
	LGUIElement::Render();
}

void LGUIRadar::UpdateRadius()
{
	float MaxDistance=RadarSize*RadarSize;

	POINT2F UpperLeft;
	UpperLeft.X=Origin.X-RadarSize;
	UpperLeft.Y=Origin.Y-RadarSize;

	POINT2F ZBounds;
	ZBounds.X=ZClipping.X+Origin.Z;
	ZBounds.Y=ZClipping.Y+Origin.Z;

	float FullSize=RadarSize*2;
	for (auto i=Blips.begin() ; i!=Blips.end(); i++)
	if (LGUIRadarBlip *pBlip=i->second)
	{
		// first, clipping
		if (bClipZ && (pBlip->Point.Z<ZBounds.X || pBlip->Point.Z>ZBounds.Y))
		{
			pBlip->Show(false);
			continue;
		}
		if (GetDistance2DNoSqrt(Origin,pBlip->Point)>=MaxDistance)
		{
			pBlip->Show(false);
			continue;
		}
		pBlip->Show(true);

		// now calculate the X and Y of the element
		POINT2F Pos;
		if (bRotation && Rotation)
		{
			POINT2F PosKeep;

			PosKeep.X=pBlip->Point.X-Origin.X;
			PosKeep.Y=pBlip->Point.Y-Origin.Y;

			Pos.Y=-(PosKeep.X*(float)cos(-Rotation)-PosKeep.Y*(float)sin(-Rotation));
			Pos.X=(PosKeep.Y*(float)cos(-Rotation)+PosKeep.X*(float)sin(-Rotation));

			Pos.X+=RadarSize;
			Pos.Y+=RadarSize;

			Pos.X=((Pos.X)/FullSize)*(float)Width;
			Pos.Y=((Pos.Y)/FullSize)*(float)Height;

			Pos.X-=pBlip->GetWidth()/2;
			Pos.Y-=pBlip->GetHeight()/2;
		}
		else
		{
			Pos.X=((pBlip->Point.X-UpperLeft.X)/FullSize)*(float)Width;
			Pos.Y=((pBlip->Point.Y-UpperLeft.Y)/FullSize)*(float)Height;
			Pos.X-=pBlip->GetWidth()/2;
			Pos.Y-=pBlip->GetHeight()/2;
		}
		#if MYCOPY
		//printf("Blip %s at %.1f,%.1f (my heading %.2f)",pBlip->Label,Pos.X,Pos.Y,Rotation);
		#endif

		// and set
		pBlip->SetLeft((int)Pos.X);
		pBlip->SetTop((int)Pos.Y);
	}

}

void LGUIRadar::UpdateBox()
{
	POINT2F UpperLeft;
	UpperLeft.X=Origin.X-RadarSize;
	UpperLeft.Y=Origin.Y-RadarSize;
	POINT2F LowerRight;
	LowerRight.X=Origin.X+RadarSize;
	LowerRight.Y=Origin.Y+RadarSize;

	POINT2F ZBounds;
	ZBounds.X=ZClipping.X+Origin.Z;
	ZBounds.Y=ZClipping.Y+Origin.Z;

	float FullSize=RadarSize*2;
	for (auto i=Blips.begin() ; i!=Blips.end(); i++)
	if (LGUIRadarBlip *pBlip=i->second)
	{
		// first, clipping
		if (bClipZ && (pBlip->Point.Z<ZBounds.X || pBlip->Point.Z>ZBounds.Y))
		{
			pBlip->Show(false);
			continue;
		}

		if (pBlip->Point.X<UpperLeft.X || pBlip->Point.X>LowerRight.X ||
			pBlip->Point.Y<UpperLeft.Y || pBlip->Point.Y>LowerRight.Y)
		{
			pBlip->Show(false);
			continue;
		}
		pBlip->Show(true);

		// now calculate the X and Y of the element
		POINT2F Pos;
		if (bRotation && Rotation)
		{
			POINT2F PosKeep;

			PosKeep.X=pBlip->Point.X-Origin.X;
			PosKeep.Y=pBlip->Point.Y-Origin.Y;

			Pos.Y=-(PosKeep.X*(float)cos(-Rotation)-PosKeep.Y*(float)sin(-Rotation));
			Pos.X=(PosKeep.Y*(float)cos(-Rotation)+PosKeep.X*(float)sin(-Rotation));

			Pos.X+=RadarSize;
			Pos.Y+=RadarSize;

			Pos.X=((Pos.X)/FullSize)*(float)Width;
			Pos.Y=((Pos.Y)/FullSize)*(float)Height;

			Pos.X-=pBlip->GetWidth()/2;
			Pos.Y-=pBlip->GetHeight()/2;
		}
		else
		{
			Pos.X=((pBlip->Point.X-UpperLeft.X)/FullSize)*(float)Width;
			Pos.Y=((pBlip->Point.Y-UpperLeft.Y)/FullSize)*(float)Height;
			Pos.X-=pBlip->GetWidth()/2;
			Pos.Y-=pBlip->GetHeight()/2;
		}
		#if MYCOPY
		//printf("Blip %s at %.1f,%.1f (my heading %.2f)",pBlip->Label,Pos.X,Pos.Y,Rotation);
		#endif
		// and set
		pBlip->SetLeft((int)Pos.X);
		pBlip->SetTop((int)Pos.Y);

	}

}

// Blip Manipulation
void LGUIRadar::AddBlip(blipid uID, POINT3F &Point, char *BlipType, uint32_t Size, char *Label)
{
	if (uID == 0)
	{
		#if MYCOPY
		DebugSpew("AddBlip(%d) failed!",ID);
		#endif
		return;
	}
	LGUIRadarBlip *pBlip;
	if (pBlip=Blips[uID])
	{
		DeleteChild(pBlip);
	}
	pBlip=NewBlip();
	if (!pBlip)
	{
		#if MYCOPY
		DebugSpew("AddBlip(%d) failed!",ID);
		#endif
		return;
	}
	pBlip->SetHeight(Size);
	pBlip->SetWidth(Size);
	if (Label)
		pBlip->Label=strdup(Label);
	if (BlipType)
	{
		pBlip->BlipType=strdup(BlipType);

		if (LGUITexture *pTexture=FindBlipType(BlipType))
			pBlip->pTexture=LGUITexture::New(*pTexture);
	}
	pBlip->BlipID=uID;
	pBlip->Point=Point;
	Blips[uID]=pBlip;
}

void LGUIRadar::RemoveBlip(blipid uID)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		if (pBlip)
		{
			DeleteChild(pBlip);
			Blips[uID]=0;
		}
		else
		{
			#if MYCOPY
			printf("LGUIRadar::RemoveBlip(%u) -- No blip with that ID!",uID);
			#endif
		}
	}
}

void LGUIRadar::ClearBlips()
{
	for (auto i=Blips.begin() ; i!=Blips.end(); i++)
	{
		if (i->second)
			DeleteChild(i->second);
	}
	Blips.clear();

}

void LGUIRadar::UpdateBlip(blipid uID, POINT3F &Point)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		pBlip->Point=Point;
	}
}

void LGUIRadar::UpdateBlipLabel(blipid uID, char *Label)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		if (pBlip->Label)
		{
			free(pBlip->Label);
			pBlip->Label=0;
		}
		if (Label)
		{
			pBlip->Label=strdup(Label);

		}
	}
}

void LGUIRadar::UpdateBlipLabel(blipid uID, char *Label, uint32_t TextColor)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		if (pBlip->Label)
		{
			free(pBlip->Label);
			pBlip->Label=0;
		}
		if (Label)
		{
			pBlip->Label=strdup(Label);
		}
		pBlip->TextColor=TextColor;
	}
}

void LGUIRadar::UpdateBlipLabel(blipid uID, char *Label, uint32_t TextColor, unsigned char FontSize)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		if (pBlip->Label)
		{
			free(pBlip->Label);
			pBlip->Label=0;
		}
		if (Label)
		{
			pBlip->Label=strdup(Label);

		}
		pBlip->TextColor=TextColor;
		pBlip->FontSize=FontSize;
	}
}

void LGUIRadar::UpdateBlipLabel(blipid uID, uint32_t TextColor, unsigned char FontSize)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		pBlip->TextColor=TextColor;
		pBlip->FontSize=FontSize;
	}
}

void LGUIRadar::UpdateBlipType(blipid uID, char *BlipType)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		if (pBlip->BlipType)
		{
			if (!stricmp(pBlip->BlipType,BlipType))
				return;
			free(pBlip->BlipType);
			pBlip->BlipType=0;
		}
		if (pBlip->pTexture)
		{
			pBlip->pTexture->Delete();
			pBlip->pTexture=0;
		}
		if (BlipType)
		{
			pBlip->BlipType=strdup(BlipType);

			if (LGUITexture *pTexture=FindBlipType(BlipType))
			{
				pBlip->pTexture=LGUITexture::New(*pTexture);
			}
		}
	}
}

void LGUIRadar::UpdateBlipSize(blipid uID, uint32_t Size)
{
	if (LGUIRadarBlip *pBlip=Blips[uID])
	{
		pBlip->SetHeight(Size);
		pBlip->SetWidth(Size);
	}
}

// Blip Type Manipulation
void LGUIRadar::AddBlipType(char *sName, LGUITexture *pTexture)
{
	char Temp[512];
	strcpy(Temp,sName);
	strlwr(Temp);
	LGUITexture *pBlipTexture;
	if (pBlipTexture=BlipTypes[sName])
	{
		pBlipTexture->Delete();
	}
	pBlipTexture=LGUITexture::New(*pTexture);
	BlipTypes[sName]=pBlipTexture;
}

void LGUIRadar::AddBlipType(char *sName, class XMLNode *pXML)
{
	char Temp[512];
	strcpy(Temp,sName);
	strlwr(Temp);
	LGUITexture *pBlipTexture;
	if (pBlipTexture=BlipTypes[sName])
	{
		pBlipTexture->Delete();
	}
	pBlipTexture=TextureFromXML(pXML,0,0);
	BlipTypes[sName]=pBlipTexture;
}

void LGUIRadar::RemoveBlipType(char *sName)
{
	char Temp[512];
	strcpy(Temp,sName);
	strlwr(Temp);
	LGUITexture *pBlipTexture;
	if (pBlipTexture=BlipTypes[sName])
	{
		pBlipTexture->Delete();
		BlipTypes[sName]=0;
	}

}

void LGUIRadar::ClearBlipTypes()
{
	map<string,LGUITexture *>::iterator i;
	for (i=BlipTypes.begin() ; i!=BlipTypes.end(); i++)
	{
		if (i->second)
		{
			i->second->Delete();

		}
	}
	BlipTypes.clear();
}

LGUITexture *LGUIRadar::FindBlipType(char *sName)
{
	char Temp[512];
	strcpy(Temp,sName);
	strlwr(Temp);
	return BlipTypes[sName];
}

LGUIRadarBlip *LGUIRadar::NewBlip()
{
	char sName[128];
	sprintf(sName,"blip%u",NextUniqueID);
	NextUniqueID++;
	LGUIRadarBlip *pBlip=(LGUIRadarBlip*)g_UIManager.LoadElement(this,BlipFactory,sName);
	if (!pBlip)		return NULL;
	pBlip->MoveZTop();
	return pBlip;
}

LGUIFactory<LGUIRadarBlip> RadarBlipFactory("radarblip");

LGUIRadarBlip::LGUIRadarBlip(char *p_Factory, LGUIElement *p_pParent, char *p_Name):LGUIElement(p_Factory,p_pParent,p_Name)
{
	pTexture=0;
	BlipType=0;
	Label=0;
	//TextColor=0xFFFFFFFF;
	TextColor = gDefaultRadarTextFontColor;
	//FontSize=12;
	FontSize = gDefaultRadarTextFontSize;
}

LGUIRadarBlip::~LGUIRadarBlip(void)
{
	if (Label)
	{
		free(Label);
	}
	if (BlipType)
	{
		free(BlipType);
	}
	if (pTexture)
	{
		pTexture->Delete();
	}
}

bool LGUIRadarBlip::IsTypeOf(char *TestFactory)
{
	return (!stricmp(TestFactory,"radarblip")) || LGUIElement::IsTypeOf(TestFactory);
}

bool LGUIRadarBlip::OnLMouseUp(POINT2I &pt)
{
	((LGUIRadar *)pParent)->BlipLMouseUp(this);
	return true;
}

bool LGUIRadarBlip::OnLMouseDown(POINT2I &pt)
{
	((LGUIRadar *)pParent)->BlipLMouseDown(this);
	return true;
}

bool LGUIRadarBlip::OnRMouseUp(POINT2I &pt)
{
	((LGUIRadar *)pParent)->BlipRMouseUp(this);
	return true;
}

bool LGUIRadarBlip::OnRMouseDown(POINT2I &pt)
{
	((LGUIRadar *)pParent)->BlipRMouseDown(this);
	return true;
}

void LGUIRadarBlip::OnMouseEnter(POINT2I &pt)
{
	((LGUIRadar *)pParent)->BlipHover(this,pt);
	((LGUIRadar *)pParent)->HoverID=BlipID;
}

void LGUIRadarBlip::OnMouseExit(LGUIElement *pNewFocus)
{
	((LGUIRadar *)pParent)->BlipHoverStop(this);
}

void LGUIRadarBlip::Render()
{
#define pRadar ((LGUIRadar *)pParent)
	POINT2I RadarAbsolute;
	pRadar->GetAbsolute(RadarAbsolute);
	LGUIFont *pFont;
	if (pTexture)
	{
		RECT r;
		r.left=Absolute.X;
		r.right=Absolute.X+Width;
		r.top=Absolute.Y;
		r.bottom=Absolute.Y+Height;
		TexturedRect(r,pTexture);
		if (Label && pRadar->bShowLabels && (pFont=pRadar->pFont))
		{
			pFont->SetHeight(FontSize);
			pFont->SetColor(TextColor);
			if (Height>FontSize)
			{
				if (pRadar->bClipText)
					DrawTextClipped(pFont,Label,r.right+2,Absolute.Y+((Height-FontSize)/2),RadarAbsolute.X+pRadar->GetWidth());
				else
					DrawText(pFont,Label,r.right+2,Absolute.Y+((Height-FontSize)/2));
			}
			else
			{
				if (pRadar->bClipText)
					DrawTextClipped(pFont,Label,r.right+2,Absolute.Y,RadarAbsolute.X+pRadar->GetWidth());
				else
					DrawText(pFont,Label,r.right+2,Absolute.Y);
			}
		}
	}
	else
	{
		if (Label && (pFont=pRadar->pFont))
		{
			pFont->SetHeight(FontSize);
			pFont->SetColor(TextColor);

			if (Height>FontSize)
			{
				if (pRadar->bClipText)
					DrawTextClipped(pFont,Label,Absolute.X,Absolute.Y+((Height-FontSize)/2),RadarAbsolute.X+pRadar->GetWidth());
				else
					DrawText(pFont,Label,Absolute.X,Absolute.Y+((Height-FontSize)/2));
			}
			else
			{
				if (pRadar->bClipText)
					DrawTextClipped(pFont,Label,Absolute.X,Absolute.Y,RadarAbsolute.X+pRadar->GetWidth());
				else
					DrawText(pFont,Label,Absolute.X,Absolute.Y);
			}
		}
	}
#undef pRadar
}

