#ifndef _CLIENT_ITEMTOOLTIP_H
#define _CLIENT_ITEMTOOLTIP_H

#include <libamanita/sdl/aComponent.h>
#include <libamanita/sdl/aTooltip.h>
#include "../game/Item.h"

class aFont;

class ItemToolTip : public aTooltip {
private:
	static const char *slotNames[];
	static const char *qtyNames[];
	static const char *matNames[];
	SDL_Rect *rects;
	aFont **fonts;
	item *itm;

public:
	ItemToolTip(item *i=0);
	virtual ~ItemToolTip();

	void setItem(item *i);
	item *getItem() { return itm; }
	void updateItemData();
	virtual bool paintToolTip(aTooltipEvent &tte);
};


#endif
