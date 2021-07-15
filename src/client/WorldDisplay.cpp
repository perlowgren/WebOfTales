
#include "../config.h"
#include <stdio.h>
#include <libamanita/aMath.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aTextfield.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "WorldDisplay.h"
#include "CityDisplay.h"
#include "MapDlg.h"
#include "CharacterDlg.h"
#include "BattleDlg.h"
#include "../game/Cloud.h"
#include "../game/Character.h"


aObject_Inheritance(WorldDisplay,MapDisplay);


WorldDisplay::WorldDisplay() : MapDisplay() {
app.printf("WorldDisplay Init...");
app.printf("1 app.getDB(%x)",&app.getDB());
	wrap = WRAP_HORIZ;
app.printf("3");
}

WorldDisplay::~WorldDisplay() {
}


void WorldDisplay::paint(time_t time) {
	if(app.getPlayer()) setViewPoint(*app.getPlayer());
	MapDisplay::paint(time);
}

void WorldDisplay::paintUnitLayer(viewpoint &v) {
	if(app.getPlayer()) app.mapdlg->setCoordinates(app.getPlayer()->px*360./pw-180.,app.getPlayer()->py*180./ph-90.);
	else app.mapdlg->setCoordinates(mouse.px*360./pw-180.,mouse.py*180./ph-90.);

	Location *l;
	Character *c;
	int16_t s,xm,ym;
	int32_t x,y,xp,yp;
	if(app.getPlayer() && app.getPlayer()->getDest()) {
		if(app.getPlayer()->getTrail() && app.getPlayer()->getTrail()->hasMoreSteps()) {
			aTrail &t = *app.getPlayer()->getTrail();
			size_t i = t.index(),n = t.length()-1;
			int xm,ym;
			do {
				Coordinate::mapToPnt((int16_t)t.getX(),(int16_t)t.getY(),xp,yp);
				xp -= v.px,yp -= v.py,xm = t.getX(),ym = t.getY();
				if(xp<0) xp += pw;else if(xp>=pw) xp -= pw;
				if(yp<0) yp += ph;else if(yp>=ph) yp -= ph;
				t.next();
				app.markers->draw(xp-8,yp-8,5+getDir(xm,ym,t.getX(),t.getY()));
			} while(t.index()<n);
			t.setIndex(i);
		}
		if(app.getPlayer()->getDest()==app.getPlayer()->getTarget()) {
			xp = app.getPlayer()->getDest()->px-v.px,yp = app.getPlayer()->getDest()->py-v.py;
			if(xp<0) xp += pw;else if(xp>=pw) xp -= pw;
			if(yp<0) yp += ph;else if(yp>=ph) yp -= ph;
			app.markers->draw(xp-8,yp-8,9);
		}
	}
	if(!(stats&2)) for(y=-v.ty-Coordinate::t2,ym=v.my-1; y<g.getScreenHeight()+Coordinate::t3x3; y+=Coordinate::t2,ym++) {
		if(ym<0) ym += mh;else if(ym>=mh) ym -= mh;
		for(s=((ym&1)? Coordinate::t1 : 0),x=-v.tx-Coordinate::tw+s,xm=v.mx-1;
				x<g.getScreenWidth()+Coordinate::t2x3; x+=Coordinate::tw,xm++) {
			if(xm<0) xm += mw;else if(xm>=mw) xm -= mw;
			maplocation &t = map[xm+ym*mw];
			if((l=t.loc)) {
				if((c=t.items)) do {
					if(((c->py-((ym&1)? Coordinate::t2 : 0))%Coordinate::t1)<Coordinate::t2) c->paint(v);
				} while((c=c->next));
				l->paint(x+Coordinate::t1,y+Coordinate::t1);
				if((c=t.items)) do {
					if(((c->py-((ym&1)? Coordinate::t2 : 0))%Coordinate::t1)>=Coordinate::t2) c->paint(v);
				} while((c=c->next));
			} else if((c=t.items)) do c->paint(v);while((c=c->next));
		}
	}
//app.printf("done");
}

void WorldDisplay::paintFrame() {
	static uint32_t t1 = 0;
	aFont &f = *app.fonts[FONT_MINI];
	int h = f.getAscent()-1;
	if(app.time.frames) f.printf(5,h,"FPS: %d",1000/(app.time.ticks-t1));
	f.printf(70,h,"Frames: %d",app.time.frames);
	if(!app.getPlayer())
		f.printf(200,h,"Mouse:  px=%d  py=%d  mx=%d  my=%d",mouse.px,mouse.py,mouse.mx,mouse.my);
	else {
		Player &pl = *app.getPlayer();
		f.printf(200,h,"Character:  px=%d  py=%d  mx=%d  my=%d",pl.px,pl.py,pl.mx,pl.my);
		if(pl.getTrail()) f.printf(450,h,"aTrail:  x=%d  y=%d",pl.getTrail()->getX(),pl.getTrail()->getY());
		if(pl.getDest())
			f.printf(600,h,"Dest:  px=%d  py=%d  mx=%d  my=%d",
				pl.getDest()->px,pl.getDest()->py,pl.getDest()->mx,pl.getDest()->my);
		else if(app.getFocus())
			f.printf(600,h,"Focus:  px=%d  py=%d  mx=%d  my=%d",
				app.getFocus()->px,app.getFocus()->py,app.getFocus()->mx,app.getFocus()->my);
		h += f.getAscent();
		f.printf(200,h,"Mouse:  px=%d  py=%d  mx=%d  my=%d",mouse.px,mouse.py,mouse.mx,mouse.my);
		if(app.getHover()) f.printf(450,h,"Hover:  %s",app.getHover()->getName());
	}
	t1 = app.time.ticks;
}

bool WorldDisplay::keyDown(aKeyEvent &ke) {
	if(MapDisplay::keyDown(ke)) return true;
	switch(ke.sym) {
		default:return false;
	}
}


bool WorldDisplay::mouseDown(aMouseEvent &me) {
	if(MapDisplay::mouseDown(me)) return true;
	if(me.x>=screenX() && me.y>=screenY() && me.x<screenX()+screenW() && me.y<screenY()+screenH()) {
app.printf("WorldDisplay::mouseDown(x=%d,y=%d)",me.x,me.y);
		if(!app.btldlg->isVisible()) {
			setMousePoint(me.x,me.y);
			app.setFocus(getItem(mouse.px,mouse.py));
			if(me.button==SDL_BUTTON_LEFT && app.getPlayer()) app.getPlayer()->setAction(mouse.px,mouse.py);
			if(me.button==SDL_BUTTON_RIGHT && app.chrdlg->isVisible())
				app.chrdlg->setCharacter(app.getFocus() && app.getFocus()->instanceOf(Character::getClass())?
					(Character *)app.getFocus() : app.getPlayer());

/*			Location *l = getLocation(mouse.mx,mouse.my);
			if(l && l->instanceOf(City::getClass())) {
				app.cityDisplay->enterCity((City *)l);
				app.command(CMD_ENTER,app.cityDisplay);
			}*/
		}
	}
	return false;
}

