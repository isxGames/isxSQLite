#pragma once
#include <cstdint>
#include <ISXDK.h>

typedef uint32_t blipid;

class LGUIRadarBlip :
	public LGUIElement
{
public:
	LGUIRadarBlip(char* p_Factory, LGUIElement* p_pParent, char* p_Name);
	~LGUIRadarBlip(void);
	bool IsTypeOf(char* TestFactory);

	bool OnLMouseUp(POINT2I& pt);
	bool OnLMouseDown(POINT2I& pt);
	bool OnRMouseUp(POINT2I& pt);
	bool OnRMouseDown(POINT2I& pt);
	void OnMouseEnter(POINT2I& pt);
	void OnMouseExit(LGUIElement* pNewFocus);

	bool FromXML(class XMLNode* pXML, class XMLNode* pTemplate = 0) { return false; }

	void Render();

	unsigned char FontSize;
	uint32_t TextColor;

	blipid BlipID;
	POINT3F Point;
	char* Label;
	char* BlipType;
	LGUITexture* pTexture;
};

class LGUIRadar :
	public LGUIElement
{
public:
	LGUIRadar(char* p_Factory, LGUIElement* p_pParent, char* p_Name);
	~LGUIRadar(void);
	bool IsTypeOf(char* TestFactory);

	bool FromXML(class XMLNode* pXML, class XMLNode* pTemplate = 0);

	void Render();

	virtual void UpdatePoints() {} // Derive class, update origin and points here

	char BlipFactory[64];

	POINT3F Origin;		// center of radar (player location)
	float Rotation;		// rotation of radar (player heading)
	float RadarSize;	// size of radar box in game units, an n by n square
	bool bClipRadius;	// use radius clipping instead of box clipping
	bool bClipZ;		// height clipping
	bool bShowLabels;	// show labels?
	bool bRotation;		// Rotate radar around origin?
	bool bClipText;		// Clip text to radar edge?
	POINT2F ZClipping;	// min/max height difference from origin. X = min, Y = max

	LGUIFont* pFont;

	virtual void BlipLMouseUp(LGUIRadarBlip* pBlip) {}
	virtual void BlipLMouseDown(LGUIRadarBlip* pBlip) {}
	virtual void BlipRMouseUp(LGUIRadarBlip* pBlip) {}
	virtual void BlipRMouseDown(LGUIRadarBlip* pBlip) {}
	virtual void BlipHover(LGUIRadarBlip* pBlip, POINT2I& Pos) {}
	virtual void BlipHoverStop(LGUIRadarBlip* pBlip) {}

	// Blip Manipulation
	virtual void AddBlip(blipid ID, POINT3F& Point, char* BlipType, uint32_t Size, char* Label = 0);
	virtual void RemoveBlip(blipid ID);
	virtual void ClearBlips();
	virtual void UpdateBlip(blipid ID, POINT3F& Point);
	virtual void UpdateBlipLabel(blipid ID, char* Label, uint32_t TextColor, unsigned char FontSize);
	virtual void UpdateBlipLabel(blipid ID, char* Label, uint32_t TextColor);
	virtual void UpdateBlipLabel(blipid ID, char* Label);
	virtual void UpdateBlipLabel(blipid ID, uint32_t TextColor, unsigned char FontSize = 12);
	virtual void UpdateBlipType(blipid ID, char* BlipType);
	virtual void UpdateBlipSize(blipid ID, uint32_t Size);

	// Blip Type Manipulation
	virtual void AddBlipType(char* Name, LGUITexture* pTexture);
	virtual void AddBlipType(char* Name, class XMLNode* pXML);
	virtual void RemoveBlipType(char* Name);
	virtual void ClearBlipTypes();

	uint32_t HoverID;
protected:
	virtual LGUIRadarBlip* NewBlip();
	virtual LGUITexture* FindBlipType(char* Name);

	virtual void UpdateRadius();
	virtual void UpdateBox();

	map<string, LGUITexture*> BlipTypes;
	map<blipid, LGUIRadarBlip*> Blips;

	blipid NextUniqueID; // just used for making sure blips have unique element names
};
#pragma once
