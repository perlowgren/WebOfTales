
#include "../config.h"
#include <stdlib.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aButton.h>
#include <libamanita/sdl/aTextbox.h>
#include <libamanita/aMath.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "IncarnateDlg.h"
#include "CharacterDlg.h"
#include "ItemToolTip.h"



aObject_Inheritance(IncarnateDlg,aWindow);


const int IncarnateDlg::Width = 310;
const int IncarnateDlg::Height = 245;

const char *IncarnateDlg::strchr[] = { "Gender","Race","Divinity","Portrait","Skin","Hair","Face" };
const char *IncarnateDlg::strgender[] = { "Female","Male" };

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
const SDL_Rect IncarnateDlg::slots1[SLOTS] = {
	{51,24,16,16},{51,41,16,16},{126,24,16,16},{126,41,16,16},{51,7,16,16},{126,7,16,16},
	{51,58,16,16},{126,58,16,16},{-1},{-1},{-1},{-1},{-1},{-1},{-1},{-1},
};


IncarnateDlg::IncarnateDlg()
		: aWindow(0,0,IncarnateDlg::Width,IncarnateDlg::Height,TOP_CAPTION) {
app.printf("IncarnateDlg::IncarnateDlg(1)");
	moveToCenter();
	setBackground(app.gui[2]);
	setMouseMotionListener(this);
	pl = 0;
	int i,px,py;
	aIconButton *b;
	for(px=fr.l->w+131,py=fr.t->h+86,i=0; i<7; i++,py+=14) if(i!=2) {
		chrb[i] = b = new aIconButton(10+i,px,py);
		b->setImage(app.gui[0],18,19,20,21);
		b->setActionListener(this);
		add(b);
	}
	for(px=fr.l->w+278,py=fr.t->h+26,i=ATT_PRE; i<=ATT_BOD; i++,py+=14) {
		if(i==ATT_MIN) py += 9;
		attb[i] = b = new aIconButton(i,px,py);
		b->setImage(app.gui[0],18,19,20,21);
		b->setActionListener(this);
		add(b);
	}
	races = new aButton(100,fr.l->w+19,getHeight()-fr.b->h-25,60,20);
	races->setText("Races");
	races->setActionListener(this);
	add(races);
	reset = new aButton(COM_ID_CANCEL,fr.l->w+85,getHeight()-fr.b->h-25,60,20);
	reset->setText("Reset");
	reset->setActionListener(this);
	add(reset);
	ok = new aButton(COM_ID_OK,fr.l->w+151,getHeight()-fr.b->h-25,60,20);
	ok->setText("Ok");
	ok->setActionListener(this);
	add(ok);
	setModal(true);
	setLocked(true);
	itt = new ItemToolTip();
}

IncarnateDlg::~IncarnateDlg() {
}

void IncarnateDlg::setPlayer(Player *p) {
	pl = p,prevrace = pl->inc.race,icon = 0,skin = 0,hair = 0,face = 0;
app.printf("IncarnateDlg::setPlayer(pl=%p)",pl);
	setTopCaption(pl->getName(),app.fonts[FONT_WHITE_B]);
	updatePlayer();
}

void IncarnateDlg::updatePlayer() {
	int i;
	const race &r = racedata[pl->inc.race];
	const race::gender &g = r.g[pl->inc.gen];
	pl->deleteItems();
	pl->deleteEquipment();
	pl->createItems(g.itm,g.nitm);
	for(i=pl->items()-1; i>=0; i--) pl->equipItem(i);
	icon = pl->setIcon(icon);
	skin = pl->setSkin(skin);
	hair = pl->setHair(hair);
	face = pl->setFace(face);
	pl->updateAvatar();
	chrb[3]->setEnabled(g.icon[0]!=-1 && g.icon[1]!=-1);
	chrb[4]->setEnabled(r.skin[0]!=-1 && r.skin[1]!=-1);
	chrb[5]->setEnabled(g.hair[0]!=-1 && g.hair[1]!=-1);
	chrb[6]->setEnabled(g.face[0]!=-1 && g.face[1]!=-1);
	pts = r.attpts;
	for(i=ATT_PRE; i<=ATT_BOD; i++) {
		pl->atts[i].c.value = r.atts[i],pts -= aMath::fibonacci[2+r.atts[i]]-1;
		attb[i]->setEnabled(true);
	}
	pl->update();
}

void IncarnateDlg::paint(time_t time) {
	static const char *num = "%d";
	static const char *chr = "%c";
	aWindow::paint(time);
	const race &r = racedata[pl->inc.race];
	aFont &f1 = *app.fonts[FONT_WHITE_B];
	aFont &f2 = *app.fonts[FONT_ORANGE_B];
	aFont &f3 = *app.fonts[FONT_WHITE];
	int i,h = 14,px = getX()+fr.l->w,py = getY()+fr.t->h,x,y;
	int fy1 = py+h-(h-f1.getLineSkip())/2+f1.getDescent(),fy3 = py+h-(h-f3.getLineSkip())/2+f3.getDescent();

	for(x=px+9,y=fy1+84,i=0; i<7; i++,y+=h)
		if(i!=2) f1.print(x,y,strchr[i]);
		else f2.print(x,y,strchr[i]);
	for(x=px+156,y=fy3+25,i=ATT_PRE; i<=ATT_RES; i++,y+=h) {
		if(i==ATT_MIN || i==ATT_ATK) y += 9;
		f3.print(x,y,CharacterDlg::stratts[i]);
		if(i>=ATT_ATK) f3.printf(x+60,y,ALIGN_RIGHT,num,pl->atts[i].b.sum);
	}
	for(x=px+156+71,y=fy3+141,i=ATT_HP; i<=ATT_SP; i++,y+=h) {
		if(i==ATT_MIN || i==ATT_ATK) y += 9;
		f3.print(x,y,CharacterDlg::stratts[i]);
		f3.printf(x+60,y,ALIGN_RIGHT,num,pl->atts[i].b.sum);
	}

	app.faces[0]->draw(px+7,py+7,pl->icon);
	pl->paintAvatarImage(px+88,py+68,SW,0);
	pl->paintAvatarImage(px+88+20,py+68,NE,0);
	for(i=0; i<SLOTS; i++) if(pl->slot[i].index>=0 && slots1[i].x!=-1)
		app.items[slots1[i].w==16]->draw(px+slots1[i].x,py+slots1[i].y,
			itemdata[pl->slot[i].index].icon);

	x = px+70,y = fy1+84;
	f1.print(x,y,strgender[pl->inc.gen]);
	f1.print(x,y+=h,r.name);
	f2.printf(x,y+=h,"%d (-%d)",pl->div.num,r.div);
	f1.printf(x,y+=h,chr,'A'+icon);
	f1.printf(x,y+=h,chr,'A'+skin);
	f1.printf(x,y+=h,chr,'A'+hair);
	f1.printf(x,y+=h,chr,'A'+face);

	for(x=px+154,y=py+8,i=0; i<pts; i++,x+=8) app.gui[0]->draw(x,y,106);
	for(x=px+156+87,y=py+26,i=ATT_PRE; i<=ATT_BOD; i++,y+=h) {
		if(i==ATT_MIN) y += 9;
		app.gui[0]->draw(x,y,107+pl->atts[i].c.sum);
		if(pl->atts[i].c.el>0) app.gui[0]->draw(x,y,117+pl->atts[i].c.el);
	}
}

bool IncarnateDlg::mouseMove(aMouseMotionEvent &mme) {
	int i;
	if(pl) for(i=0; i<SLOTS; i++) if(slots1[i].x!=-1 && contains(slots1[i],mme.x-fr.l->w,mme.y-fr.t->h)) {
		if(pl->slot[i].index>=0) {
			if(itt->getItem()!=&pl->slot[i]) {
				itt->setItem(&pl->slot[i]);
				setToolTip(itt);
			}
			return true;
		}
		break;
	}
	itt->setItem(0);
	setToolTip(0);
	return false;
}

bool IncarnateDlg::actionPerformed(aActionEvent &ae) {
	if(aWindow::actionPerformed(ae)) return true;
	unsigned long id = ae.source->getID();
app.printf("IncarnateDlg::actionPerformed(pl=%p,active=%p,id=%d)",pl,app.getPlayer(),id);
	if(pl && pl==app.getPlayer()) {
		if(id>=10 && id<20) {
			switch(id) {
				case 10:
					pl->inc.gen = pl->inc.gen? 0 : 1;
					updatePlayer();
					break;
				case 11:
				{
					int n = pl->inc.race;
					divinity &d = pl->div;
					const race *r;
					while(1) {
						n = n+1>=(int)RACES? 0 : n+1,r = &racedata[n];
						if(d.num>=r->div && (r->reqrace==-1 || r->reqrace==prevrace) &&
							d.el[SPIRIT]>=r->reqel[SPIRIT] && d.el[FIRE]>=r->reqel[FIRE] &&
							d.el[AIR]>=r->reqel[AIR] && d.el[WATER]>=r->reqel[WATER] &&
							d.el[EARTH]>=r->reqel[EARTH]) break;
					}
					pl->inc.race = n;
					updatePlayer();
					break;
				}
				case 13:icon = pl->setIcon(icon+1);break;
				case 14:skin = pl->setSkin(skin+1);break;
				case 15:hair = pl->setHair(hair+1);break;
				case 16:face = pl->setFace(face+1);break;
			}
			return true;
		} else if(id>=ATT_PRE && id<=ATT_BOD) {
			if(pts>0 && pl->atts[id].c.value<10) {
				const race &r = racedata[pl->inc.race];
				int i;
				pl->atts[id].c.value++,pts = r.attpts;
				for(i=ATT_PRE; i<=ATT_BOD; i++) pts -= aMath::fibonacci[2+pl->atts[i].c.value]-1;
				for(i=ATT_PRE; i<=ATT_BOD; i++) attb[i]->setEnabled(pts>=aMath::fibonacci[pl->atts[i].c.value+1]);
				pl->update();
			}
			return true;
		} else if(id==COM_ID_CANCEL) {
			if(pl) updatePlayer();
			return true;
		} else if(id==COM_ID_OK || id==TEXTBOX_OK) {
			if(id!=TEXTBOX_OK && pts>0) {
				aTextbox::showMessage("You have unspent Attribute points, do you want to incarnate anyway?",
					TEXTBOX_YES_NO,this,app.fonts[FONT_WHITE_B]);
			} else {
				pl->sendIncarnatePack(SEND_TO_ALL);
				setVisible(false);
				pl = 0;
			}
			return true;
		}
	}
	return false;
}


