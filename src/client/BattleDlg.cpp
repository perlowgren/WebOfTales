
#include "../config.h"
#include <stdlib.h>
#include <libamanita/net/aSocket.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aButton.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "BattleDlg.h"
#include "CharacterDlg.h"
#include "ItemToolTip.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Team.h"



aObject_Inheritance(BattleDlg,aWindow);


const int BattleDlg::Width = 272;
const int BattleDlg::Height = 301;


BattleDlg::BattleDlg()
		: aWindow(0,0,BattleDlg::Width,BattleDlg::Height) {
app.printf("BattleDlg::BattleDlg(1)");
	moveToCenter();
	setMouseMotionListener(this);
	setModal(true);
	setLocked(true);
	setOpaque(false);
	teamlen = 0;
	aButton *b;
	b = new aButton(1,19,BattleDlg::Height-32,60,20);
	b->setText("Retreat");
	add(b);
	b = new aButton(2,84,BattleDlg::Height-32,60,20);
	b->setText("Surrender");
	add(b);
	itt = new ItemToolTip();
}

BattleDlg::~BattleDlg() {
}

void BattleDlg::setOpponents(Character *c1,Character *c2) {
app.printf("BattleDlg::setOpponents(c1=%p,c2=%p)",c1,c2);
	Character *c;
	Player *p1,*p2;
	slot *s1,*s2;
	int i,j,n;
	teamlen = 0;
	for(i=0; i<18; i++) {
		s1 = &slots[i],s1->ch = 0,s1->icon = 0,s1->hp = 0,s1->deal = 0,s1->soak = 0;
		for(j=0; j<4; j++) s1->effects[j].timer = 0;
	}
	if(!c1 || !c2) return;
	for(i=0,c=c1,teamlen=1; i<2; i++,c=c2) {
		s1 = &slots[i*9];
		if(c->instanceOf(Player::getClass())) {
			p1 = (Player *)c;
			if(p1->getTeam()) {
				Team *t = p1->getTeam();
				if(t->members()>teamlen) teamlen = t->members();
				if(p1!=app.getPlayer() && app.getPlayer() && t->contains(*app.getPlayer())) p1 = app.getPlayer();
				for(j=0,n=i*9+1; n<(i+1)*9 && j<t->members(); j++) if((p2=t->getMember(j))!=p1)
					s2 = &slots[n],s2->ch = p2,s2->icon = p2->getIcon(),s2->hp = p2->getHP(32),s2->lp = p2->getLP(32),n++;
			}
			c = p1;
		}
		s1->ch = c,s1->icon = c->getIcon(),s1->hp = c->getHP(32),s1->lp = c->getLP(32);
	}
	if(teamlen>9) teamlen = 9;
	show();
}

int BattleDlg::indexOf(Character *c) {
	for(int i=0; i<18; i++) if(slots[i].ch==c) return i;
	return -1;
}

void BattleDlg::handleEvent(uint32_t id1,uint32_t id2,uint8_t type,uint16_t effect,int8_t timer) {
	Character *c1 = (Character *)Map::getItem(id1),*c2 = (Character *)Map::getItem(id2);
	int i1 = indexOf(c1),i2 = indexOf(c2);
	if(i1==-1 || i2==-1) return;
	slot &s1 = slots[i1],&s2 = slots[i2];
	s1.hp = c1->getHP(32),s1.lp = c1->getLP(32),s2.hp = c2->getHP(32),s2.lp = c2->getLP(32);
	switch(type) {
		case 1:s1.deal = 5,s2.soak = 10;setEffect(i1,i2,1,effect,timer);break;
		case 2:s1.deal = 5;setEffect(i1,i2,2,0,timer);break;
		case 3:s1.deal = 5;setEffect(i1,i2,3,0,timer);break;
		case 4:s1.deal = 5;break;
		case 5:s1.deal = 5;setEffect(i1,i1,5,0,timer);break;
		case 6:setEffect(i1,i2,6,0,-1);break;
	}
}

void BattleDlg::setEffect(int i1,int i2,int t,int v,int tm) {
	slot &s = slots[i2];
	for(int n=3; n>=1; n--) if(s.effects[n].timer==0 || s.effects[n-1].timer!=0) s.effects[n] = s.effects[n-1];
	s.effects[0] = (BattleDlg::slot::effect){ i1,t,v,tm };
}

void BattleDlg::paint(time_t time) {
	if(!slots[0].ch || slots[0].ch->act!=ACT_BATTLE) {
		hide();
		return;
	}
	int i,j,n,px = getX()+fr.l->w,py = getY()+fr.t->h,x,y,w,h,t,nw;
	SDL_Rect r;
	slot::effect *e;
	for(i=0; i<18; i++) {
		slot &s = slots[i];
		x = px,y = py,t = i==0? 0 : (i==9? 1 : (i<9? 2 : 3)),h = t<=1? 32 : 16;
		if(i>=9) x += 128;
		if(t>=2) y += 45+(i>9? i-10 : i-1)*26;
		app.gui[4]->draw(x,y,t+1);
		if(slots[i].icon>0) {
			x += t==0 || t==2? 5 : 93,y += 5;
			if(t<=1) {
				app.faces[0]->draw(x,y,s.icon);
				if(s.hp>0) {
					r = *app.btlicons->getCell(3),r.w = s.hp;
					app.btlicons->draw(x,y+h+1,r);
				}
				if(s.soak>0 || s.hp==0) app.btlicons->draw(x,y,2);
				else if(s.deal>0) app.btlicons->draw(x,y,1);
				x += t==0? 35 : -19,y += 20,nw = 15;
			} else {
				x += 16;
				app.faces[1]->draw(x,y,s.icon);
				if(s.hp>0) {
					r = *app.btlicons->getCell(7),r.w = s.hp/2;
					app.btlicons->draw(x,y+h,r);
				}
				if(s.soak>0 || s.hp==0) app.btlicons->draw(x,y,6);
				else if(s.deal>0) app.btlicons->draw(x,y,5);
				x += t==0? 19 : -19,y += 1,nw = 14;
			}
			for(j=0,n=0; j<4; j++) if(s.effects[j].timer!=0) n++;
			for(j=3,w=(n>1? nw/(n-1) : nw),n=nw; j>=0; j--) {
				e = &s.effects[j];
				if(e->type>0 && e->timer!=0) {
					app.btlicons->draw(j==0? x : (t==0 || t==2? x+n : x-n),y,8+e->type);
					if(e->timer>0) e->timer--;
					n -= w;
				}
			}
			if(s.deal>0) s.deal--;
			if(s.soak>0) s.soak--;
		}
	}
	x = px,y = py+47+8*26;
	g.fillRect(x,y,BattleDlg::Width-fr.l->w-fr.r->w,BattleDlg::Height-(47+8*26)-fr.t->h-fr.b->h,getBackground());
	aWindow::paint(time);
}

bool BattleDlg::mouseMove(aMouseMotionEvent &mme) {
	return false;
}

bool BattleDlg::actionPerformed(aActionEvent &ae) {
	if(aWindow::actionPerformed(ae)) return true;
	return false;
}


