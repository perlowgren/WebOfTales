
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aImage.h>
#include "../Main.h"
#include "WoTClient.h"
#include "CharacterDlg.h"
#include "ItemToolTip.h"
#include "../game/Character.h"


extern char indent[32];

/*
enum {
	SLOT_CHEST				= 0,
	SLOT_LEGS				= 1,
	SLOT_WAIST				= 2,
	SLOT_FEET				= 3,
	SLOT_HEAD				= 4,
	SLOT_BACK				= 5,
	SLOT_RHAND				= 6,
	SLOT_LHAND				= 7,
	SLOT_NECK1				= 8,
	SLOT_NECK2				= 9,
	SLOT_RWRIST				= 10,
	SLOT_LWRIST				= 11,
	SLOT_FINGER1			= 12,
	SLOT_FINGER2			= 13,
	SLOT_FINGER3			= 14,
	SLOT_FINGER4			= 15,
	SLOTS						= 16,
};
*/
const char *ItemToolTip::slotNames[] = {
	"Chest","Legs","Waist","Feet","Head","Back","Hand","Hand",
	"Neck","Neck","Wrist","Wrist","Finger","Finger","Finger","Finger"
};

/*
enum {
	ITEM_QTY_BROKEN		= 0,
	ITEM_QTY_POOR,
	ITEM_QTY_COMMON,
	ITEM_QTY_ARTISAN,
	ITEM_QTY_MASTER,
	ITEM_QTY_SUPERIOR,
	ITEM_QTY_SACRED,
	ITEM_QTY_EPIC,
	ITEM_QTY_LEGENDARY,
	ITEM_QTY_MYTHICAL,
	ITEM_QTY_DIVINE,
};
*/
const char *ItemToolTip::qtyNames[] = {
	"Broken","Poor","Common","Artisan","Master","Superior",
	"Sacred","Epic","Legendary","Mythical","Divine"
};

/*
enum {
	ITEM_MAT_HEMP							= 0x00000001,
	ITEM_MAT_LINEN							= 0x00000002,
	ITEM_MAT_WOOL							= 0x00000004,
	ITEM_MAT_LEATHER						= 0x00000008,
	ITEM_MAT_RINGMAIL						= 0x00000010,
	ITEM_MAT_SCALEMAIL					= 0x00000020,
	ITEM_MAT_PLATE							= 0x00000040,

	ITEM_MAT_PAPER							= 0x00000080,
	ITEM_MAT_WOOD							= 0x00000100,
	ITEM_MAT_STONE							= 0x00000200,
	ITEM_MAT_CLAY							= 0x00000400,
	ITEM_MAT_PORCELAIN					= 0x00000800,
	ITEM_MAT_TIN							= 0x00001000,
	ITEM_MAT_COPPER						= 0x00002000,
	ITEM_MAT_BRASS							= 0x00004000,
	ITEM_MAT_IRON							= 0x00008000,
	ITEM_MAT_STEEL							= 0x00010000,
	ITEM_MAT_CRYSTAL						= 0x00020000,
	ITEM_MAT_PEARL							= 0x00040000,
	ITEM_MAT_RUBY							= 0x00080000,
	ITEM_MAT_DIAMOND						= 0x00100000,
};
*/
const char *ItemToolTip::matNames[] = {
	"Hemp","Linen","Wool","Silk","Leather","Ringmail","Scalemail","Plate",
	"Paper","Wood","Stone","Clay","Porcelain","Tin","Copper","Brass","Iron","Stel","Crystal",
	"Pearl","Ruby","Diamond",
};

ItemToolTip::ItemToolTip(item *i) : aTooltip() {
	tt.nlines = 32;
	tt.lines = (char **)malloc(sizeof(char *)*tt.nlines);
	for(int n=0; n<tt.nlines; n++) tt.lines[n] = (char *)malloc(64);
	rects = (SDL_Rect *)malloc(sizeof(SDL_Rect)*tt.nlines);
	fonts = (aFont **)malloc(sizeof(aFont *)*tt.nlines);
	setItem(i);
}
ItemToolTip::~ItemToolTip() {
	if(tt.lines) for(int i=0; i<tt.nlines; i++)
		if(tt.lines[i]) { free(tt.lines[i]);tt.lines[i] = 0; }
	if(rects) { free(rects);rects = 0; }
	if(fonts) { free(fonts);fonts = 0; }
}

void ItemToolTip::setItem(item *i) {
	if(itm==i) return;
	if((itm=i)) updateItemData();
}
	
void ItemToolTip::updateItemData() {
	static int qualityFont[11] = { FONT_BLACK_B,
		FONT_GREY_B,FONT_WHITE_B,FONT_YELLOW_B,FONT_GREEN_B,FONT_CYAN_B,
		FONT_BLUE_B,FONT_PURPLE_B,FONT_MAGENTA_B,FONT_RED_B,FONT_ORANGE_B
	};
	int i,j,k,n,y = 0;
	const item_prototype &p = itemdata[itm->index];
	for(n=0; n<tt.nlines; n++)
		*tt.lines[n] = '\0',rects[n].x = 0,rects[n].y = 0,fonts[n] = app.fonts[FONT_WHITE];
	n = 0;
	strcpy(tt.lines[n],p.name);
	fonts[n] = app.fonts[qualityFont[p.qty]],y += fonts[n]->getLineSkip();
	sprintf(tt.lines[++n],"Quality: %s",qtyNames[p.qty]);
	rects[n].y = y,y += fonts[n]->getLineSkip();
	sprintf(tt.lines[++n],"Weight: %0.2f kg",((double)p.weight)/1000);
	rects[n].y = y,y += fonts[n]->getLineSkip();
	if(p.mat>0) {
		strcpy(tt.lines[++n],"Material:");
		for(i=0,j=1,k=1; i<32; i++,j<<=1) if(p.mat&j) {
			if(k++==4) k = 0,rects[n].y = y,y += fonts[n]->getLineSkip(),n++;
			else strcat(tt.lines[n]," ");
			strcat(tt.lines[n],matNames[i]);
		}
		rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.slot>=0) {
		sprintf(tt.lines[++n],"Slot: %s",slotNames[(int)p.slot]);
		rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.qty<=5) {
		sprintf(tt.lines[++n],"Durability: %d / %d",itm->dura,p.dura);
		fonts[n] = app.fonts[FONT_GREY],rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.type&ITEM_ARMOR) {
		sprintf(tt.lines[++n],"Armor: %d",p.atts[ATT_DEF]);
		rects[n].y = y,y += fonts[n]->getLineSkip();
	} else if((p.type&ITEM_MELEE) || (p.type&ITEM_RANGED)) {
		sprintf(tt.lines[++n],"Speed: %0.2f hits / sec",((GAME_FPS)/2.) / ((double)p.speed));
		rects[n].y = y,y += fonts[n]->getLineSkip();
		sprintf(tt.lines[++n],"Damage: %d",p.atts[ATT_ATK]);
		rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	for(i=ATT_PRE; i<=ATT_CON; i++) if(p.atts[i]>0) {
		if(p.attmax[i]==0) sprintf(tt.lines[++n],"%s: +%d",CharacterDlg::stratts[i],p.atts[i]);
		else sprintf(tt.lines[++n],"%s: +%d [%d]",CharacterDlg::stratts[i],p.atts[i],p.attmax[i]);
		rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.req.lvl>0) {
		sprintf(tt.lines[++n],"Requires Level: %d",p.req.lvl);
		fonts[n] = app.fonts[FONT_RED],rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.req.gen>=0) {
		sprintf(tt.lines[++n],"Requires Gender: %s",CharacterDlg::strgender[(int)p.req.gen]);
		fonts[n] = app.fonts[FONT_RED],rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	for(i=0; i<5; i++) if(p.req.atts[i]>0) {
		sprintf(tt.lines[++n],"Requires %s: %d",CharacterDlg::stratts[i],p.req.atts[i]);
		fonts[n] = app.fonts[FONT_RED],rects[n].y = y,y += fonts[n]->getLineSkip();
	}
	if(p.notes[0]) {
		for(const char *const*c=p.notes; *c; c++) {
			strcpy(tt.lines[++n],*c);
			fonts[n] = app.fonts[FONT_YELLOW],rects[n].y = y,y += fonts[n]->getLineSkip();
		}
	}

	tt.w = 0,tt.h = 0;
	for(n=0; n<tt.nlines && *tt.lines[n]; n++) {
		if(rects[n].y<34) rects[n].x += 34;
		rects[n].w = fonts[n]->stringWidth(tt.lines[n]);
		if(rects[n].x+rects[n].w>tt.w) tt.w = rects[n].x+rects[n].w;
		if(rects[n].y+fonts[n]->getLineSkip()>tt.h) tt.h = rects[n].y+fonts[n]->getLineSkip();
	}
	if(tt.h<36) tt.h = 36;
	tt.w += _s.l->w+_s.ins.x+_s.ins.w+_s.r->w,tt.h += _s.t->h+_s.ins.y+_s.ins.h+_s.b->h;
}

bool ItemToolTip::paintToolTip(aTooltipEvent &tte) {
	if(!itm) return false;
	tt.x = tte.x,tt.y = tte.y+16;
	if(tt.x+tt.w>g.getScreenWidth()) tt.x = g.getScreenWidth()-1-tt.w;
	if(tt.y+tt.h>g.getScreenHeight()) tt.y = tte.y-1-tt.h;

	paintToolTipRect(tt.x,tt.y,tt.w,tt.h);
	int n,x = tt.x+_s.l->w+_s.ins.x,y = tt.y+_s.t->h+_s.ins.y;
	app.items[0]->draw(x,y+2,itemdata[itm->index].icon);
	for(n=0; n<tt.nlines; n++)
		fonts[n]->print(x+rects[n].x,y+rects[n].y+fonts[n]->getAscent(),tt.lines[n]);
	return true;
}

