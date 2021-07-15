
#include "../config.h"
#include <math.h>
#include <stdio.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>
#include <libamanita/aThread.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aTextfield.h>
#include "../Main.h"
#include "WoTClient.h"
#include "Message.h"
#include "MapDisplay.h"
#include "Panel.h"
#include "MapDlg.h"
#include "IncarnateDlg.h"
#include "CharacterDlg.h"
#include "BattleDlg.h"
#include "TeamDlg.h"
#include "../game/NPC.h"
#include "../game/Player.h"



aObject_Inheritance(MapDisplay,aDisplay);


extern char *unitTypes[];
extern char *unitActions[];



MapDisplay::MapDisplay() : Map(),aDisplay(),stats(0) {
app.printf("MapDisplay Init...");
	setScreen(0,14,g.getScreenWidth(),g.getScreenHeight()-34);
app.printf("2");
	setMousePoint(0,0);
app.printf("done");
}

MapDisplay::~MapDisplay() {
app.printf("MapDisplay::~MapDisplay(%x)",this);
}



void MapDisplay::start() {
app.printf("MapDisplay::start()");
	aComponent::add(app.panel);
	app.mapdlg->setMapDisplay(this);
	aComponent::add(app.mapdlg);
	aComponent::add(app.incdlg);
	aComponent::add(app.chrdlg);
	aComponent::add(app.btldlg);
	aComponent::add(app.teamdlg);
	aDisplay::start();
}

void MapDisplay::stop() {
	aDisplay::stop();
	aComponent::remove(app.panel);
	aComponent::remove(app.mapdlg);
	aComponent::remove(app.incdlg);
	aComponent::remove(app.chrdlg);
	aComponent::remove(app.btldlg);
	aComponent::remove(app.teamdlg);
}


void MapDisplay::paint(time_t time) {
//app.printf("MapDisplay::paint(time=%d,stats=%d,map=%p)",time,stats,this);

//app.printf("MapDisplay::paint(3,stats=%d,active=%p,active.st=%d)",stats,app.getPlayer(),app.getPlayer()? app.getPlayer()->st : 0);

	if((stats&1) && app.getPlayer() && app.getPlayer()->isIncarnated()) {
//app.printf("MapDisplay::paint(1)");
		g.setClip(screenX(),screenY(),screenX()+screenW(),screenY()+screenH());
//app.printf("MapDisplay::paint(2)");
		if(app.getLeader()->map==this) setViewPoint(*app.getLeader());
		viewpoint vp = v;
//g.fillRect(screenX(),screenY(),screenX()+screenW(),screenY()+screenH(),g.mapRGB(0x000000));
//app.printf("MapDisplay::paint(3)");
		paintGroundLayer(vp);
//app.printf("MapDisplay::paint(4)");
		paintUnitLayer(vp);

//app.printf("MapDisplay::paint(5)");
if(!(stats&16)) {
		paintCloudLayer(vp);
}

//app.printf("MapDisplay::paint(6)");
		g.resetClip();
	} else g.fillRect(screenX(),screenY(),screenX()+screenW(),screenY()+screenH(),g.mapRGB(0x330000));

//app.printf("MapDisplay::paint(7)");
	paintChat();

//app.printf("MapDisplay::paint(8)");
if(!(stats&32)) {
	paintAll(time);

	app.mainFrameImage->draw(0,0,1);
	paintFrame();
//app.printf("MapDisplay::paint(9)");
}

	if((stats&1) && app.getPlayer() && app.getPlayer()->isIncarnated()) {
		paintPlayerFrame(*app.getPlayer(),5,17);
		if(app.getFocus() && app.getFocus()->instanceOf(Character::getClass()))
			paintNPCFrame(*(Character *)app.getFocus(),screenX()+screenW()-app.gui[1]->getCell(2)->w-5,17);
	}

//app.printf("MapDisplay::paint(done)");
}

void MapDisplay::paintGroundLayer(viewpoint &v) {
//app.printf("MapDisplay::paintGroundLayer(1)");
	int i;
	short s,xm,ym;
	long x,y;
	aFont &f = *app.fonts[FONT_WHITE];
	maplocation *ml;
//app.printf("MapDisplay::paintGroundLayer(1.0.1)");
if(!(stats&4)) {
	for(y=-v.ty-Coordinate::t2,ym=v.my-1; y<g.getScreenHeight()+Coordinate::t3x3; y+=Coordinate::t2,ym++) {
//app.printf("MapDisplay::paintGroundLayer(1.0.2)");
		if(ym<0) ym += mh;else if(ym>=mh) ym -= mh;
//app.printf("MapDisplay::paintGroundLayer(1.0.3)");
		for(s=((ym&1)? Coordinate::t1 : 0),x=-v.tx-Coordinate::tw+s,xm=v.mx-1;
				x<g.getScreenWidth()+Coordinate::t2x3; x+=Coordinate::tw,xm++) {
//app.printf("MapDisplay::paintGroundLayer(1.0.4)");
			if(xm<0) xm += mw;else if(xm>=mw) xm -= mw;
//app.printf("MapDisplay::paintGroundLayer(1.0.5)");
			ml = &map[xm+ym*mw];
//app.printf("MapDisplay::paintGroundLayer(1.1,xm=%d,ym=%d,gv=%d)",xm,ym,ml->gv);
			if(ml->gv) app.terrainImages[0][ml->gv>>5]->draw(x+Coordinate::t1,y+Coordinate::t3,ml->gv&0x1f);
//app.printf("MapDisplay::paintGroundLayer(1.2)");
		}
	}
}
//app.printf("MapDisplay::paintGroundLayer(2)");
if(!(stats&8)) {
	for(y=-v.ty-Coordinate::t2,ym=v.my-1; y<g.getScreenHeight()+Coordinate::t3x3; y+=Coordinate::t2,ym++) {
		if(ym<0) ym += mh;else if(ym>=mh) ym -= mh;
		for(s=((ym&1)? Coordinate::t1 : 0),x=-v.tx-Coordinate::tw+s,xm=v.mx-1;
				x<g.getScreenWidth()+Coordinate::t2x3; x+=Coordinate::tw,xm++) {
			if(xm<0) xm += mw;else if(xm>=mw) xm -= mw;
			for(i=0,ml=&map[xm+ym*mw]; i<TERRAIN_TYPES-1; i++)
				if(ml->tv[i]) app.terrainImages[1][i+1]->draw(x+Coordinate::t1,y+Coordinate::t3,ml->tv[i]);
			if(stats&2) f.printf(x+Coordinate::t2,y+Coordinate::t2,"(%d,%d)",xm,ym);
		}
	}
}
//app.printf("MapDisplay::paintGroundLayer(3)");
}

void MapDisplay::paintCloudLayer(viewpoint &v) {
	short s,xm,ym;
	long x,y;
	Cloud *c;
	for(y=-v.ty-Coordinate::t2,ym=v.my-1; y<g.getScreenHeight()+Coordinate::t3x3; y+=Coordinate::t2,ym++) {
		if(ym<0) ym += mh;else if(ym>=mh) ym -= mh;
		for(s=((ym&1)? Coordinate::t1 : 0),x=-v.tx-Coordinate::tw+s,xm=v.mx-1;
				x<g.getScreenWidth()+Coordinate::t2x3; x+=Coordinate::tw,xm++) {
			if(xm<0) xm += mw;else if(xm>=mw) xm -= mw;
			for(c=map[xm+ym*mw].clouds; c; c=c->next) c->paint(v);
		}
	}
}

void MapDisplay::paintChat() {
	if(app.chatFirst) {
		int i,y = g.getScreenHeight()-39;
		Message *msg;
		SDL_Rect r = {0,0,0,0};
		for(i=0,msg=app.chatFirst; i<10 && msg; i++,y-=msg->font->getLineSkip(),msg=msg->next) {
			if(msg->time.ticks+300000<app.time.ticks) break;
			else {
				r.w = msg->font->stringWidth(msg->msg)+2,r.h = msg->font->getLineSkip();
				app.gui[8]->draw(5,y-r.h,r);
				msg->font->print(5+1,y+msg->font->getDescent(),msg->msg);
			}
		}
	}
}

void MapDisplay::paintPlayerFrame(Character &ch,int x,int y) {
	int v;
	SDL_Rect r;
	char str[16];
	app.faces[0]->draw(x+10,y+5,ch.getIcon());
	app.gui[1]->draw(x,y,1);
	app.fonts[FONT_WHITE_B]->print(x+103-app.fonts[FONT_WHITE_B]->stringWidth(ch.getName())/2,y+13,ch.getName());
	sprintf(str,"%d",ch.lvl);
	app.fonts[FONT_WHITE]->printf(x+23-app.fonts[FONT_WHITE]->stringWidth(str)/2,y+47,str);
	if(!ch.isDead()) {
		aFont &fm = *app.fonts[FONT_MINI];
		const Character::attribute &hp = ch.getAttribute(ATT_HP);
		if(hp.h.temp>0 && (v= hp.h.temp*100/hp.h.sum)>0) {
			r = *app.gui[1]->getCell(3),r.w -= 100-v;
			app.gui[1]->draw(x+56,y+19,r);
		}
		sprintf(str,"%d/%d",hp.h.temp,hp.h.sum);
		fm.print(x+103-fm.stringWidth(str)/2,y+25,str);

		const Character::attribute &mp = ch.getAttribute(ATT_LP);
		if(mp.h.temp>0 && (v=mp.h.temp*100/mp.h.sum)>0) {
			r = *app.gui[1]->getCell(4),r.w -= 100-v;
			app.gui[1]->draw(x+56,y+28,r);
		}
		sprintf(str,"%d/%d",mp.h.temp,mp.h.sum);
		fm.print(x+103-fm.stringWidth(str)/2,y+34,str);

		const Character::attribute &sta = ch.getAttribute(ATT_SP);
		if(sta.h.temp>0 && (v=sta.h.temp*20/sta.h.sum)>0) {
			r = *app.gui[1]->getCell(5),r.y += 20-v,r.h -= 20-v;
			app.gui[1]->draw(x+47,y+25-v,r);
		}
//		gui[1]->draw(x+43,y+5,pl.isBusy()? 7 : 6);
	}
}

void MapDisplay::paintNPCFrame(Character &ch,int x,int y) {
	int v;
	SDL_Rect r;
	char str[16];
	app.faces[0]->draw(x+109,y+5,ch.getIcon());
	app.gui[1]->draw(x,y,2);
	aFont &f = *app.fonts[FONT_WHITE_B+ch.getColour(*app.getPlayer())];
	f.print(x+53-f.stringWidth(ch.getName())/2,y+13,ch.getName());
	sprintf(str,"%d",ch.lvl);
	app.fonts[FONT_WHITE]->printf(x+128-app.fonts[FONT_WHITE]->stringWidth(str)/2,y+47,str);
	if(!ch.isDead()) {
		aFont &fsm = *app.fonts[FONT_MINI];
		char str[16];
		const Character::attribute &hp = ch.getAttribute(ATT_HP);
		if(hp.h.temp>0 && (v= hp.h.temp*100/hp.h.sum)>0) {
			r = *app.gui[1]->getCell(3),r.x += 100-v,r.w -= 100-v;
			app.gui[1]->draw(x+103-v,y+19,r);
		}
		sprintf(str,"%d/%d",hp.h.temp,hp.h.sum);
		fsm.print(x+53-fsm.stringWidth(str)/2,y+25,str);

		const Character::attribute &mp = ch.getAttribute(ATT_LP);
		if(mp.h.temp>0 && (v=mp.h.temp*100/mp.h.sum)>0) {
			r = *app.gui[1]->getCell(4),r.x += 100-v,r.w -= 100-v;
			app.gui[1]->draw(x+103-v,y+28,r);
		}
		sprintf(str,"%d/%d",mp.h.temp,mp.h.sum);
		fsm.print(x+53-fsm.stringWidth(str)/2,y+34,str);
	}
}

char *MapDisplay::tohms(double d,int r) {
	static char str[2][11];
	int h,m,s;
	if(d<1) h = 0;
	else h = (int)d,d -= h;
	m = (int)(d*60);
	s = (int)round(((d*60)-m)*60);
	sprintf(str[r],"%dº%02d'%02d\"",h,m,s);
	return str[r];
}


bool MapDisplay::keyDown(aKeyEvent &ke) {
	if(ke.mod&KMOD_CTRL) switch(ke.sym) {
		case SDLK_m:app.command(CMD_SHOW_HIDE_MAP);return true;
		case SDLK_c:app.command(CMD_CHARACTER_SHEET);return true;
		case SDLK_t:app.command(CMD_SHOW_HIDE_TEAM);return true;
		case SDLK_j:app.command(CMD_JOIN_TEAM);return true;
		case SDLK_d:app.command(CMD_DISBAND_TEAM);return true;
		default:return false;
	}
app.printf("MapDisplay::keyEvent(c=%c,code=%c)",(char)ke.unicode,ke.sym);
	switch(ke.sym) {
		case SDLK_F5:stats ^= 2;return true;
		case SDLK_F6:stats ^= 4;return true;
		case SDLK_F7:stats ^= 8;return true;
		case SDLK_F8:stats ^= 16;return true;
		case SDLK_F9:stats ^= 32;return true;
		case SDLK_F10:stats ^= 64;return true;
		case SDLK_RETURN:
app.printf("MapDisplay::keyEvent(SDLK_RETURN)");
			if(app.panel->getChatTextLength()>0) {
				app.msg(0,FONT_YELLOW,app.panel->getChatText());
				app.panel->clearChatText();
			}
			return true;
		case SDLK_ESCAPE:
			if(app.panel->getChatTextLength()>0) {
				app.panel->clearChatText();
				return true;
			} else return false;
		default:return app.panel->chatKeyDown(ke);
	}
}

bool MapDisplay::mouseDown(aMouseEvent &me) {
	return false;
}

bool MapDisplay::mouseUp(aMouseEvent &me) {
	return false;
}

bool MapDisplay::mouseMove(aMouseMotionEvent &mme) {
	setMousePoint(mme.x,mme.y);
	app.setHover(getItem(mouse.px,mouse.py));
	return false;
}

bool MapDisplay::mouseDrag(aMouseMotionEvent &mme) {
	return false;
}


void MapDisplay::setViewPoint(Coordinate &c) {
	setViewPoint(c.px,c.py);
}

void MapDisplay::setViewPoint(int32_t px,int32_t py) {
	v.px = px-g.getScreenWidth()/2,v.py = py-g.getScreenHeight()/2;
	if(wrap&WRAP_HORIZ) { if(v.px<0) v.px += pw;else if(v.px>=pw) v.px -= pw; }
	else {
		if(v.px<Coordinate::t1) v.px = Coordinate::t1;
		else if(v.px>=pw-g.getScreenWidth()-Coordinate::t1) v.px = pw-g.getScreenWidth();
	}
	if(wrap&WRAP_VERT) { if(v.py<0) v.py += ph;else if(v.py>=ph) v.py -= ph; }
	else {
		if(v.py<Coordinate::t3) v.py = Coordinate::t3;
		else if(v.py>=ph-g.getScreenHeight()-Coordinate::t3) v.py = ph-g.getScreenHeight();
	}
	Coordinate::pntToMap(v.px,v.py,v.mx,v.my);
	int32_t x1,y1;
	Coordinate::mapToPnt(v.mx,v.my,x1,y1);
	v.tx = v.px-(x1-Coordinate::t1)+((v.my&1)? Coordinate::t1 : 0),v.ty = v.py-(y1-Coordinate::t2);
}

void MapDisplay::setViewPoint(int16_t mx,int16_t my) {
	int32_t x,y;
	Coordinate::mapToPnt(mx,my,x,y);
	setViewPoint(x,y);
}

void MapDisplay::setMousePoint(int x,int y) {
	mouse.px = v.px+x,mouse.py = v.py+y;
	if(wrap&WRAP_HORIZ) { if(mouse.px<0) mouse.px += pw;else if(mouse.px>=pw) mouse.px -= pw; }
	if(wrap&WRAP_VERT) { if(mouse.py<0) mouse.py += ph;else if(mouse.py>=ph) mouse.py -= ph; }
	Coordinate::pntToMap(mouse.px,mouse.py,mouse.mx,mouse.my);
}





