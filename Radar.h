#pragma once
#include "RadarBase.h"
#include <cstdint>
#include <ISXDK.h>

class LGUIGameRadar :
	public LGUIRadar
{
public:

	enum class ActorTypes : int64_t
	{
		Unknown = 0x0,
		None = 0x0,
		Me = 0x1,
		PC = 0x2,
		NPC = 0x4,
		Corpse = 0x8,
	};
	class FilterNode
	{
	public:
		std::string RadarName;
		std::string Keyword;
	};


	LGUIGameRadar(char* p_Factory, LGUIElement* p_pParent, char* p_Name);
	~LGUIGameRadar(void);
	bool IsTypeOf(char* TestFactory);
	virtual void UpdatePoints();
	bool FromXML(class XMLNode* pXML, class XMLNode* pTemplate = 0);
	bool Populate();
	void ConvertRGB(char* in, char* out);

	void OnCreate();

	uint32_t RadarID;
	bool ShowMe;
	bool ShowNPCs;
	bool ShowPCs;
	bool ShowCorpses;

	bool ColorizeLabels;

	uint32_t NumFilters;

	virtual void BlipLMouseUp(LGUIRadarBlip* pBlip);
	virtual void BlipLMouseDown(LGUIRadarBlip* pBlip);
	virtual void BlipRMouseUp(LGUIRadarBlip* pBlip);
	virtual void BlipRMouseDown(LGUIRadarBlip* pBlip);
	virtual void BlipHover(LGUIRadarBlip* pBlip, POINT2I& Pos);
	virtual void BlipHoverStop(LGUIRadarBlip* pBlip);

	LGUITooltip* pTooltip;
};

#pragma region ISXEQ2 Global Variable Declarations
extern CIndex<class LGUIGameRadar*> Radars;
#pragma endregion
#pragma once
