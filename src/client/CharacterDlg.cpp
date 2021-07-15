
#include "../config.h"
#include <stdlib.h>
#include <libamanita/aMath.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aTabset.h>
#include <libamanita/sdl/aScroll.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "CharacterDlg.h"
#include "ItemToolTip.h"


aObject_Inheritance(CharacterDlg,aWindow);


const int CharacterDlg::Width = 310;
const int CharacterDlg::Height = 290;

const char *CharacterDlg::strtabs[] = { "Character","Skills","Items" };
const char *CharacterDlg::strlvl[] = { "Experience","Level","Divinity","Align","Incarnation" };
const char *CharacterDlg::strchr[] = { "Gender","Race","Age","Class","Career" };
const char *CharacterDlg::strgender[] = { "Female","Male" };
const char *CharacterDlg::strelem[] = { "Spirit","Fire","Air","Water","Earth" };
const char *CharacterDlg::stratts[] = { "Presence","Power","Perception","Agility","Constitution",
														"Mind","Body","ATK","DEF","RES","HP","LP","SP" };

const char *CharacterDlg::equipMsg[] = {"",
	"You cannot equip this item!",
	"This item is only used by the other gender!",
	"You need to raise attributes to equip this item!",
	"Your skill level is to low to equip this item!",
};

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
const SDL_Rect CharacterDlg::slots1[SLOTS] = {
	{51,24,16,16},{51,41,16,16},{126,24,16,16},{126,41,16,16},{51,7,16,16},{126,7,16,16},
	{51,58,16,16},{126,58,16,16},{-1},{-1},{-1},{-1},{-1},{-1},{-1},{-1},
};
const SDL_Rect CharacterDlg::slots2[SLOTS] = {
	{7,48,32,32},{7,83,32,32},{103,83,32,32},{55,160,32,32},{55,13,32,32},{103,48,32,32},
	{13,153,32,32},{97,153,32,32},{-1},{-1},{7,118,32,32},{103,118,32,32},{-1},{-1},{-1},{-1},
};
const SDL_Rect CharacterDlg::itemArea = { 147,7,128,225 };


CharacterDlg::CharacterDlg()
		: aWindow(0,0,CharacterDlg::Width,CharacterDlg::Height,TOP_CAPTION|CLOSE_BUTTON) {
app.printf("CharacterDlg::CharacterDlg(1)");
	moveToCenter();
	int i,w = getWidth()-fr.l->w-fr.r->w,h = getHeight()-fr.t->h-fr.b->h;
	panels[0] = (panel){ new aComponent(fr.l->w,fr.t->h,w,h),app.gui[3],1 };
	panels[1] = (panel){ new aComponent(fr.l->w,fr.t->h,w,h),app.gui[3],2 };
	panels[2] = (panel){ new aComponent(fr.l->w,fr.t->h,w,h),app.gui[3],3 };
	for(i=0; i<3; i++) {
		aComponent &p = *panels[i].pane;
		p.setVisible(false);
		p.setMouseListener(this);
		p.setMouseMotionListener(this);
		add(&p);
	}
	ts = new aTabset(1000,fr.bl->w,getHeight()-8,TABSET_BOTTOM);
	ts->setTabs(strtabs,3);
	ts->setActionListener(this);
	add(ts);

	ch = 0,pl = 0,tab = 0;
	setPanel(tab);

	aIconButton *b;
	{	// Create Character panel
		aComponent &p = *panels[0].pane;
		for(i=0; i<=4; i++) {
			elemb[i] = b = new aIconButton(i,278,7+i*14);
			b->setImage(app.gui[0],18,19,20,21);
			b->setActionListener(this);
			p.add(b);
		}
	};
	{	// Create Items panel
		aComponent &p = *panels[2].pane;
		aScroll *scr = new aScroll(201,p.getWidth()-20,6,14,p.getHeight()-12);
		scr->setActionListener(this);
		p.add(scr);
		// aComponent *items = new aComponent(itemArea.x,itemArea.y,itemArea.w,itemArea.h);
	};
	itt = new ItemToolTip();
}

CharacterDlg::~CharacterDlg() {
	for(int i=0; i<3; i++) panels[i].pane->setToolTip(0);
	if(itt) { delete itt;itt = 0; }
}

void CharacterDlg::setCharacter(Character *c) {
app.printf("CharacterDlg::setCharacter()");
	ch = c;
	if(ch->instanceOf(Player::getClass())) {
		pl = (Player *)ch;
		ts->setVisible(true);
		setPanel(ts->getActiveTab());
	} else {
		pl = 0;
		ts->setVisible(false);
		setPanel(0);
	}
	setTopCaption(ch->getName(),app.fonts[FONT_WHITE_B]);
	ch->update();
	for(int i=0; i<=4; i++) elemb[i]->setEnabled(false);
}

void CharacterDlg::setTab(int n) {
	if(n==tab || n<0 || n>=3) return;
	setPanel(n);
	ts->setActiveTab(n);
}

void CharacterDlg::setPanel(int n) {
	panels[tab].pane->setVisible(false);
	tab = n;
	setBackground(panels[tab].bgimg,panels[tab].bgindex);
	panels[tab].pane->setVisible(true);
}

void CharacterDlg::paint(time_t time) {
	static const char *num = "%d";
	static const char *pct = "%d %%";
	aWindow::paint(time);
	switch(tab) {
		case 0:
		{
			aFont &f = *app.fonts[FONT_WHITE];
			int i,h = 14,px = getX()+fr.l->w,py = getY()+fr.t->h,fy = py+h-(h-f.getLineSkip())/2+f.getDescent(),x,y;

			if(pl && pl==app.getPlayer()) for(i=0; i<=4; i++)
				elemb[i]->setEnabled(pl->div.el[i]<10 && pl->div.num>=aMath::fibonacci[4+pl->div.el[i]]);

			for(x=px+9,y=fy+85,i=0; i<=4; i++,y+=h) f.print(x,y,strlvl[i]);
			for(x=px+9,y=fy+164,i=0; i<=4; i++,y+=h) f.print(x,y,strchr[i]);
			for(x=px+156,y=fy+6,i=SPIRIT; i<=EARTH; i++,y+=h) f.print(x,y,strelem[i]);
			for(x=px+156,y=fy+85,i=ATT_PRE; i<=ATT_RES; i++,y+=h) {
				if(i==ATT_MIN || i==ATT_ATK) y += 9;
				f.print(x,y,stratts[i]);
			}
			for(x=px+156+71,y=fy+201,i=ATT_HP; i<=ATT_SP; i++,y+=h) f.print(x,y,stratts[i]);

			if(ch) {
				app.faces[0]->draw(px+7,py+7,ch->icon);
				if(pl) {
					pl->paintAvatarImage(px+88,py+68,SW,0);
					pl->paintAvatarImage(px+108,py+68,NE,0);
					for(i=0; i<SLOTS; i++) if(pl->slot[i].index>=0 && slots1[i].x!=-1)
						app.items[slots1[i].w==16]->draw(px+slots1[i].x,py+slots1[i].y,
							itemdata[pl->slot[i].index].icon);

					x = px+9+117,y = fy+85;
					f.printf(x+12,y,ALIGN_RIGHT,pct,pl->getXP(100));
					f.printf(x,y+=h,ALIGN_RIGHT,num,pl->lvl);
					f.printf(x,y+=h,ALIGN_RIGHT,num,pl->div.num);
					f.printf(x,y+=h,ALIGN_RIGHT,num,pl->div.align);
					f.printf(x,y+=h,ALIGN_RIGHT,num,pl->inc.num);

					x = px+70,y = fy+164;
					f.print(x,y,strgender[pl->inc.gen]);
					f.print(x,y+14,racedata[pl->inc.race].name);
					f.printf(x,y+28,"%d",pl->inc.age);

					for(x=px+154+87,y=py+7,i=SPIRIT; i<=EARTH; i++,y+=h)  {
						app.gui[0]->draw(x,y,107);
						if(pl->div.el[i]>0) app.gui[0]->draw(x,y,117+pl->div.el[i]);
					}
				} else {
				}
				for(x=px+156+87,y=py+86,i=ATT_PRE; i<=ATT_BOD; i++,y+=h) {
					if(i==ATT_MIN) y += 9;
					app.gui[0]->draw(x,y,107+ch->atts[i].c.sum);
					if(ch->atts[i].c.el>0) app.gui[0]->draw(x,y,117+ch->atts[i].c.el);
				}
				for(x=px+156+60,y=fy+201,i=ATT_ATK; i<=ATT_RES; i++,y+=h)
					f.printf(x,y,ALIGN_RIGHT,num,ch->atts[i].b.sum);
				for(x=px+156+71+60,y=fy+201,i=ATT_HP; i<=ATT_SP; i++,y+=h)
					f.printf(x,y,ALIGN_RIGHT,num,ch->atts[i].h.sum);
			}
			break;
		}
		case 1:
		{
			if(ch) {
				aFont &f = *app.fonts[FONT_WHITE];
				int i,h = 14,px = getX()+fr.l->w,py = getY()+fr.t->h+h-(h-f.getLineSkip())/2+f.getDescent(),y;
				for(i=0,y=py; i<ch->nskills; i++,y+=h) {
					skill &s = ch->skills[i];
					const skill_prototype &p = skilldata[s.index];
					if(i==ch->attack) f.print(px+2,y,"*");
					f.print(px+20,y,p.name);
					f.printf(px+180,y,pct,100*(s.xp&0xfffff)/0x100000);
					f.printf(px+250,y,num,s.lvl);
				}
			}
			break;
		}
		case 2:
		{
			int i,n,px = getX()+fr.l->w,py = getY()+fr.t->h;
			if(ch) {
				app.faces[0]->draw(px+7,py+7,ch->icon);
				if(pl) {
					pl->paintAvatarImage(px+62,py+109,SW,0);
					pl->paintAvatarImage(px+82,py+109,NE,0);
					for(i=0; i<SLOTS; i++) if(pl->slot[i].index>=0 && slots2[i].x!=-1)
						app.items[slots2[i].w==16]->draw(px+slots2[i].x,py+slots2[i].y,
							itemdata[pl->slot[i].index].icon);
				}
				if(ch->nitm>0) {
					SDL_Rect r = g.getClip();
					g.setClip(px+itemArea.x,py+itemArea.y,itemArea.w,itemArea.h);
					for(i=0,n=itemArea.w/32; i<ch->nitm; i++) if(ch->itm[i].index>=0)
						app.items[0]->draw(px+itemArea.x+(i%n)*32,py+itemArea.y+(i/n)*32,
							itemdata[ch->itm[i].index].icon);
					g.setClip(r);
				}
			}
			break;
		}
	}
}

bool CharacterDlg::mouseDown(aMouseEvent &me) {
	if(aWindow::mouseDown(me)) return true;
app.printf("CharacterDlg::mouseDown(clicks=%d)",me.clicks);
	if(tab==2) {
		int i;
		aComponent &p = *me.source;
		if(pl) for(i=0; i<SLOTS; i++) if(p.contains(slots2[i],me.x,me.y)) {
app.printf("CharacterDlg::mouseDown(i=%d)",i);
			if(pl->slot[i].index>=0) {
app.printf("CharacterDlg::mouseDown(index=%d)",pl->slot[i].index);
				if(me.clicks==2) {
					itt->setItem(0);
					p.setToolTip(0);
					pl->sendCharacterPack(SEND_TO_ALL,CH_UNEQUIP_ITEM,i,0);
				}
			}
			return true;
		}
		if(p.contains(itemArea,me.x,me.y)) {
			i = ((me.y-p.getY()-itemArea.y)/32)*(itemArea.w/32)+((me.x-p.getX()-itemArea.x)/32);
			if(i>=0 && i<ch->nitm) {
				if(pl && me.clicks==2) {
					int n = pl->canEquipItem(pl->itm[i]);
					if(n!=0) app.console(app.fonts[FONT_RED],equipMsg[n]);
					else {	
						itt->setItem(0);
						p.setToolTip(0);
						pl->sendCharacterPack(SEND_TO_ALL,CH_EQUIP_ITEM,i,0);
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool CharacterDlg::mouseMove(aMouseMotionEvent &mme) {
	int i;
	aComponent &p = *mme.source;
	if(tab==0) {
		if(pl) for(i=0; i<SLOTS; i++) if(slots1[i].x!=-1 && p.contains(slots1[i],mme.x,mme.y)) {
			if(pl->slot[i].index>=0) {
				if(itt->getItem()!=&pl->slot[i]) {
					itt->setItem(&pl->slot[i]);
					p.setToolTip(itt);
				}
				return true;
			}
			break;
		}
	} else if(tab==2) {
		if(pl) for(i=0; i<SLOTS; i++) if(slots2[i].x!=-1 && p.contains(slots2[i],mme.x,mme.y)) {
			if(pl->slot[i].index>=0) {
				if(itt->getItem()!=&pl->slot[i]) {
					itt->setItem(&pl->slot[i]);
					p.setToolTip(itt);
				}
				return true;
			}
			break;
		}
		if(p.contains(itemArea,mme.x,mme.y)) {
			i = ((mme.y-p.getY()-itemArea.y)/32)*(itemArea.w/32)+((mme.x-p.getX()-itemArea.x)/32);
			if(i>=0 && i<ch->nitm && ch->itm[i].index>=0) {
				if(itt->getItem()!=&ch->itm[i]) {
					itt->setItem(&ch->itm[i]);
					p.setToolTip(itt);
				}
				return true;
			}
		}
	}
	itt->setItem(0);
	p.setToolTip(0);
	return false;
}

bool CharacterDlg::actionPerformed(aActionEvent &ae) {
	if(aWindow::actionPerformed(ae)) return true;
	unsigned long id = ae.source->getID();
	if(pl && pl==app.getPlayer()) {
		if(ae.source==ts) setPanel(ts->getActiveTab());
		else if(id>=SPIRIT && id<=EARTH && pl->div.el[id]<10 && pl->div.num>=aMath::fibonacci[3+pl->div.el[id]])
			pl->sendCharacterPack(SEND_TO_ALL,CH_UPGRADE_ELEM,id,1);
	}
	return false;
}


