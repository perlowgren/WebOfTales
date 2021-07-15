
#include "../config.h"
#include <math.h>
#include <string.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include "../Main.h"
#include "WoTClient.h"
#include "WorldDisplay.h"
#include "MapDlg.h"


aObject_Inheritance(MapDlg,aWindow);


const int MapDlg::Width = 270;
const int MapDlg::Height = 160;


MapDlg::MapDlg() : aWindow(0,0,MapDlg::Width,MapDlg::Height,TOP_CAPTION|BOTTOM_CAPTION|CLOSE_BUTTON) {
app.printf("MapDlg::MapDlg(1)");
	moveToCenter();
	map = 0;
	mapimage = new aImage(MapDlg::Width-fr.l->w-fr.r->w,MapDlg::Height-fr.t->h-fr.b->h);
	setBackground(mapimage);
app.printf("MapDlg::MapDlg(2)");
	*caption = 0;
	setCoordinates(0.0,0.0);
	aComponent *c = new aComponent(fr.l->w,fr.t->h,mapimage->getWidth(),mapimage->getHeight());
	c->setMouseListener(this);
	c->setMouseMotionListener(this);
	add(c);
}

MapDlg::~MapDlg() {
	delete mapimage;
}

void MapDlg::setCaption(const char *c) {
	if(c) strncpy(caption,c,31);
	else *caption = 0;
	setTopCaption(caption,app.fonts[FONT_WHITE]);
}

void MapDlg::setCoordinates(double lon,double lat) {
	longitude = lon,latitude = lat;
	sprintf(coords,"%s%c - %s%c",MapDisplay::tohms(fabs(lon),0),lon<0? 'W' : 'E',MapDisplay::tohms(fabs(lat),1),lat<0? 'N' : 'S');
	setBottomCaption(coords,app.fonts[FONT_WHITE]);
}

void MapDlg::paint(time_t time) {
	aWindow::paint(time);
	int x = map->v.mx*res,y = (map->v.my/2)*res;
	int w = (g.getScreenWidth()/128)*res,h = (g.getScreenHeight()/64)*res;
	int x1 = getX()+fr.l->w+xp+x,y1 = getY()+fr.t->h+yp+y,r = 8;
	g.setClip(getX()+fr.l->w+xp,getY()+fr.t->h+yp,mw,mh);
	app.markers->draw(x1-4,y1,1);
	app.markers->draw(x+w>mw? x1+w-mw-r+4 : x1+w-r+4,y1,2);
	app.markers->draw(x1-4,y+h>mh? y1+h-mh-r : y1+h-r,3);
	app.markers->draw(x+w>mw? x1+w-mw-r+4 : x1+w-r+4,y+h>mh? y1+h-mh-r : y1+h-r,4);
	g.resetClip();
	region *rgn = map->getRegion((Coordinate &)*app.getPlayer());
	if(rgn && rgn->name) app.fonts[FONT_WHITE]->print(getX()+fr.l->w+2,getY()+fr.t->h+12,rgn->name);
	else {
		area *a = map->getArea((Coordinate &)*app.getPlayer());
		if(a && a->name) app.fonts[FONT_WHITE]->print(getX()+fr.l->w+2,getY()+fr.t->h+12,a->name);
	}
}

void MapDlg::setMapDisplay(MapDisplay *m) {
	map = m;
	setKeyListener(map);
	if(!map) return;
	setTopCaption(map->getName(),app.fonts[FONT_WHITE]);
	drawMap();
}

void MapDlg::drawMap() {
app.printf("MapDlg::drawMap(map=%p,map->map=%p,mw=%d,mh=%d)",map,map->map,map->mw,map->mh);
	if(map->mw==0 || map->mh==0) return;
	setMapSize(map->mw,map->mh);
	aGraphics g(*mapimage);
	g.fillRect(0,0,mapimage->getWidth(),mapimage->getHeight(),g.mapRGB(0x000000));
	int xres = res,yres = xres/2;
	Map::mapcolor c;
	g.setClip(xp,yp,xp+mw*xres,yp+mh*yres);
	for(int y=0,i=0; i<map->mh; y+=yres,i++) 
		for(int x=-xres,j=-1,n=((i&1)? (xres/2) : 0); j<=map->mw; x+=xres,j++) {
			c = map->getColor((j<0? j+map->mw : (j>=map->mw? j-map->mw : j))+i*map->mw,0);
			g.fillRect(xp+x+n,yp+y,xres,yres,g.mapRGB(c.r,c.g,c.b));
		}
	g.resetClip();
//	g.drawRect(xp-1,yp-1,mw*xres+2,mh*yres+2,0);
}

void MapDlg::setMapSize(int w,int h) {
	mw = w,mh = h/2;
	res = (256/mw)<(128/mh)? (256/mw) : (128/mh);
	if(!res) res = 1;
	else {
//		if(res>1 && !(mw*res==256 && mh*res==128)) res--;
		if(res>3 && (res&1)) res--;
		if(res>1) mw *= res,mh *= res;
	}
	xp = mw<256? (256-mw)/2 : 0;
	yp = mh<128? (128-mh)/2 : 0;
}

bool MapDlg::mouseDown(aMouseEvent &me) {
	if(aWindow::mouseDown(me)) return true;
	me.x -= getX()+fr.l->w+xp,me.y -= getY()+fr.t->h+yp;
	if(!map->instanceOf(WorldDisplay::getClass()) && me.x<mapimage->getWidth() && me.y<mapimage->getHeight()) {
		map->setViewPoint((short)(me.x/res),(short)(me.y/(res/2)));
		return true;
	}
	return false;
}

bool MapDlg::mouseDrag(aMouseMotionEvent &mme) {
	if(!map->instanceOf(WorldDisplay::getClass())) {
		map->setViewPoint((short)((mme.x-(getX()+fr.l->w+xp))/res),(short)((mme.y-(getY()+fr.t->h+yp))/(res/2)));
		return true;
	}
	return false;
}


