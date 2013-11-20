#pragma once
#include "LGUIelement.h"

enum eTextAlign
{
	ALIGN_LEFT=0,
	ALIGN_CENTER=1,
	ALIGN_RIGHT=2,

	VALIGN_TOP=3,
	VALIGN_CENTER=4,
	VALIGN_BOTTOM=5,
};

struct _CreateText : public _CreateElement
{
	char *Text;
	_CreateFont Font;
//	char *Font;
//	unsigned int FontSize;
//	unsigned int Color;

	bool bWrap;
	eTextAlign Alignment;
	eTextAlign VerticalAlignment;
};

class LGUIText : public LGUIElement
{
public:
	LGUI_API LGUIText(char *p_Factory, LGUIElement *p_pParent, char *p_Name);
	LGUI_API ~LGUIText(void);
	LGUI_API bool IsTypeOf(char *TestFactory);
	LGUI_API class LSTypeDefinition *GetLSType();

	LGUI_API static unsigned int SplitFixed(char *Value, unsigned int MaxChars, unsigned int &Width);
	LGUI_API static unsigned int Split(char *Value, unsigned int MaxWidth, LGUIFont *pFont, unsigned int &Width);
	LGUI_API unsigned int Split(char *Value);

	LGUI_API void Render();
	LGUI_API bool RenderCustom();

	LGUI_API void Create(_CreateText &CreateInfo, class XMLNode *pTemplate=0);

	LGUI_API void RenderLeft(char *Value);
	LGUI_API void RenderCenter(char *Value);
	LGUI_API void RenderRight(char *Value);
	LGUI_API void RecalculateSize(bool RecalculateChildren);


	LGUI_API bool FromXML(class XMLNode *pXML, class XMLNode *pTemplate=0);

	LGUI_API void SetText(char *NewText);

	// -- XML Properties --
	char *Text;
	LGUIFont *pFont;

	bool bWrap;
	eTextAlign Alignment;
	eTextAlign VerticalAlignment;
	// --------------------

	char *NonVariable;
	unsigned int nLines;
};
