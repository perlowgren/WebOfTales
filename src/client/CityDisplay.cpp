
#include "../config.h"
#include <stdio.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aTextfield.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "CityDisplay.h"
#include "MapDlg.h"
#include "../game/Player.h"



aObject_Inheritance(CityDisplay,MapDisplay);


CityDisplay::CityDisplay() : MapDisplay() {
app.printf("CityDisplay Init...");
	wrap = WRAP_NONE;
	Map::createMap(64,64);
}

CityDisplay::~CityDisplay() {
}


void CityDisplay::paintUnitLayer(viewpoint &v) {
	short s,xm,ym;
	long x,y;
	aImage *img;
	SDL_Rect *r;
	for(y=-v.ty-Coordinate::t2,ym=v.my-1; y<g.getScreenHeight()+Coordinate::t3x3; y+=Coordinate::t2,ym++) {
		if(ym<0) ym += mh;else if(ym>=mh) ym -= mh;
		for(s=((ym&1)? Coordinate::t1 : 0),x=-v.tx-Coordinate::tw+s,xm=v.mx-1;
				x<g.getScreenWidth()+Coordinate::t2x3; x+=Coordinate::tw,xm++) {
			if(xm<0) xm += mw;else if(xm>=mw) xm -= mw;
			maplocation &t = map[xm+ym*mw];
			if(t.c!=CONSTRUCT_NONE) {
				img = app.constructImages[t.c][0];
				r = img->getCell(t.cv);
				img->draw(x,y+Coordinate::t1-r->h,*r);
			}
		}
	}
}

void CityDisplay::paintFrame() {
	static uint32_t t1 = 0;
	aFont &f = *app.fonts[FONT_MINI];
	int h = f.getAscent()-1;
	if(app.time.frames) f.printf(5,h,"FPS: %d",1000/(app.time.ticks-t1));
	f.printf(70,h,"Frames: %d",app.time.frames);
	Player *pl = app.getPlayer();
	if(pl) {
		f.printf(200,h,"Player:  px=%d  py=%d  mx=%d  my=%d",pl->px,pl->py,pl->mx,pl->my);
		if(pl->getTrail()) f.printf(450,h,"aTrail:  x=%d  y=%d",pl->getTrail()->getX(),pl->getTrail()->getY());
		if(pl->getDest()) f.printf(600,h,"Dest:  px=%d  py=%d  mx=%d  my=%d",
			pl->getDest()->px,pl->getDest()->py,pl->getDest()->mx,pl->getDest()->my);
	} else {
		f.printf(200,h,"Mouse:  px=%d  py=%d  mx=%d  my=%d",mouse.px,mouse.py,mouse.mx,mouse.my);
	}
	t1 = app.time.ticks;
}


bool CityDisplay::keyDown(aKeyEvent &ke) {
	if(MapDisplay::keyDown(ke)) return true;
	switch(ke.sym) {
		case SDLK_ESCAPE:
		{
			if(app.getPlayer() && app.getLeader()==app.getPlayer())
				app.getPlayer()->sendLocationPack(SEND_TO_ALL,0);
			return true;
		}
		default:return false;
	}
}


bool CityDisplay::mouseDown(aMouseEvent &me) {
	if(MapDisplay::mouseDown(me)) return true;
	return false;
}

bool CityDisplay::mouseDrag(aMouseMotionEvent &mme) {
	if(MapDisplay::mouseDrag(mme)) return true;
	return false;
}


void CityDisplay::enterCity(City *c) {
	if(stats&1) stats ^= 1;
	location = c;
	setName(c->getName());
	app.mapdlg->setCoordinates(c->mx/c->getMap()->mw*360.-180.,c->my/c->getMap()->mh*180.-90.);
	createMap();
	setViewPoint((short)(mw/2),(short)(mh/2));
	stats |= 1;
}

bool CityDisplay::createMap() {
	if(!location) return false;
	unsigned long i;
	int n,a = 0,s,b,hw = mw/2,hh = mh/2;
	aRandom r(seed);
	maplocation **l = (maplocation **)location->getMap()->getIsoArea(location->mx,location->my,2,location->getMap()->map,sizeof(maplocation));
	int m1[size],m2[size],t = l[0]->g;
	for(i=0; i<size; i++) m1[i] = t,m2[i] = CONSTRUCT_NONE;
	for(n=OCEAN; n<GROUND_TYPES; n++) if(n!=t) {
		for(i=1,a=0,b=1; i<=8; i++,b<<=1) if(l[i] && l[i]->g==n) a |= b;
app.printf("CityDisplay::createMap(a=%d)",a);
		if(a) fillAlign(m1,n,a);
	}
	erosion(r,m1,3,1);
	erosion(r,m1,2,1,mw-2,mh-1,1,2);
	fillEllipse(m1,t,hw-4,hh-4,8,8);

	if(location->instanceOf(City::getClass())) {
		City &city = *(City *)location;
		s = city.getSize(),a = 2+s*s/6;
		fillEllipse(m2,1,hw-a,hh-a,a*2,a*2);
		if(s>5) {
			a = 2+s*s/12;
			fillEllipse(m2,2,hw-a,hh-a,a*2,a*2);
			if(s>10) {
				a = s*s/24;
				fillEllipse(m2,3,hw-a,hh-a,a*2,a*2);
			}
		}
		erosion(r,m2,1,1);
	}

//	buildCityWalls();

app.printf("CityDisplay::createMap(1)",a);
	clearMap();
	int sum[3];
	int houseTable[3][7] = {
		{ 100,100, 30,  0,  5,  0,  0 },
		{  30, 30,100,  1,  5, 10,  5 },
		{   0,  0,  0,  1,  1,100, 50 }
	};
app.printf("CityDisplay::createMap(1.1)",a);
	for(i=0; i<3; i++) sum[i] = aMath::sum(houseTable[i],7);
app.printf("CityDisplay::createMap(1.2)",a);
	for(i=0; i<size; i++) {
		t = m2[i];
		maplocation &m = map[i];
		m.g = m1[i];
		m.t = 0;
		for(n=0; n<TERRAIN_TYPES-1; n++) m.tv[n] = 0;
		n = m.g==OCEAN? 0 : m2[i];
		m.c = n? HOUSE : CONSTRUCT_NONE;
		m.cv = n? 1+r.rollTable(houseTable[n-1],sum[n-1]) : 0;
	}
app.printf("CityDisplay::createMap(2)",a);
	makeTerrain(r);
	return true;
}

void CityDisplay::fillAlign(int *m,int t,int a) {
	int hw = mw/2,hh = mh/2,hw2 = hw/2,hh2 = hh/2,hw3 = hw2/2,hh3 = hh2/2;
	if(a&0x1) {
		fillRect(m,t,mw-hw2,0,hw2,hh2);
		fillRect(m,t,mw-hw2-hw3,0,hw3,hh3);
		fillRect(m,t,mw-hw3,hh2,hw3,hh3);
		fillRect(m,t,mw-hw2-hw3,hh2,hw3,hh3);
	}
	if(a&0x2) {
		fillRect(m,t,mw-hw2,mh-hh2,hw2,hh2);
		fillRect(m,t,mw-hw2-hw3,mh-hh3,hw3,hh3);
		fillRect(m,t,mw-hw3,mh-hh2-hh3,hw3,hh3);
		fillRect(m,t,mw-hw2-hw3,mh-hh2-hh3,hw3,hh3);
	}
	if(a&0x4) {
		fillRect(m,t,0,mh-hh2,hw2,hh2);
		fillRect(m,t,hw2,mh-hh3,hw3,hh3);
		fillRect(m,t,0,mh-hh2-hh3,hw3,hh3);
		fillRect(m,t,hw2,mh-hh2-hh3,hw3,hh3);
	}
	if(a&0x8) {
		fillRect(m,t,0,0,hw2,hh2);
		fillRect(m,t,hw2,0,hw3,hh3);
		fillRect(m,t,0,hh2,hw3,hh3);
		fillRect(m,t,hw2,hh2,hw3,hh3);
	}
	if(a&0x10) fillRect(m,t,mw-hw3,hh2+hh3,hw3,hh2);
	if(a&0x20) fillRect(m,t,hw2+hw3,mh-hh3,hw2,hh3);
	if(a&0x40) fillRect(m,t,0,hh2+hh3,hw3,hh2);
	if(a&0x80) fillRect(m,t,hw2+hw3,0,hw2,hh3);
}

void CityDisplay::fillRect(int *m,int t,int x,int y,int w,int h) {
	if(w<0 || h<0 || x+w<0 || y+h<0 || x>=mw || y>=mw) return;
	int i,j;
	for(i=x; i<x+w; i++) for(j=y; j<y+h; j++)
		if(i>=0 && i<mw && j>=0 && j<mh) m[i+j*mw] = t;
}

void CityDisplay::fillEllipse(int *m,int t,int x,int y,int w,int h) {
	int i,rx = w/2,ry = h/2,cx = x+rx,cy = y+ry,nx = 0,ny = ry,x1,y1,x2,y2;
	if(cx>=0 && cx<mw && cy-ny>=0 && cy-ny<mh) m[cx+(cy-ny)*mw] = t;
	if(cx>=0 && cx<mw && cy-ny>=0 && cy-ny<mh) m[cx+(cy+ny)*mw] = t;
	while(ry*ry*nx<rx*rx*ny) {
		nx++,x1 = nx*2,y1 = ny*2-1,x2 = rx*2,y2 = ry*2,x1 *= x1,y1 *= y1,x2 *= x2,y2 *= y2;
		if(y2*x1+x2*y1-x2*y2>=0) ny--;
		for(i=cx-nx; i<=cx+nx; i++) {
			if(i>=0 && i<mw && cy-ny>=0 && cy-ny<mh) m[i+(cy-ny)*mw] = t;
			if(i>=0 && i<mw && cy-ny>=0 && cy-ny<mh) m[i+(cy+ny)*mw] = t;
		}
	}
	while(ny!=0) {
		ny--,x1 = nx*2+1,y1 = ny*2,x2 = rx*2,y2 = ry*2,x1 *= x1,y1 *= y1,x2 *= x2,y2 *= y2;
		if(y2*x1+x2*y1-x2*y2<=0) nx++;
		for(i=cx-nx; i<=cx+nx; i++) {
			if(i>=0 && i<mw && cy-ny>=0 && cy-ny<mh) m[i+(cy-ny)*mw] = t;
			if(i>=0 && i<mw && cy-ny>=0 && cy-ny<mh) m[i+(cy+ny)*mw] = t;
		}
	}
}

void CityDisplay::erosion(aRandom &r,int *m,int e,int f) {
	int n,hw = mw/2,hh = mh/2;
	for(n=0; n<e; n++) {
		erosion(r,m,1,1,hw,hh,1,f);
		erosion(r,m,mw-2,1,hw,hh,1,f);
		erosion(r,m,1,mh-2,hw,hh,1,f);
		erosion(r,m,mw-2,mh-2,hw,hh,1,f);
	}
}

void CityDisplay::erosion(aRandom &r,int *m,int x1,int y1,int x2,int y2,int e,int f) {
	int n,x,y,i,j,types[5],c,r2,nx = x1<x2? 1 : -1,ny = y1<y2? 1 : -1;
	int **l;
	x2 += nx,y2 += ny;
	for(n=0; n<e; n++) {
		for(y=y1; y!=y2; y+=ny) {
			for(x=x1; x!=x2; x+=nx) {
				l = (int **)getIsoArea(x,y,1,m,sizeof(int));
				for(i=1,types[0]=0; i<=4; i++) if(l[i]) for(j=0,types[i]=0; j<=i; j++)
					if(l[j] && *l[j]==*l[i]) { types[j]++;break; }
				if((c=types[0])<4) {
					if(f==0) r2 = 4;
					else r2 = 16,c = c*c;
					if(!c || r.roll(r2)>c) {
						i = types[0]<3? r.roll(4-types[0]) : 1;
						for(j=1; j<=4; j++) if((i-=types[j])<=0) break;
						if(f==2) { if(*l[0]>*l[j]) *l[0] = *l[j]; }
						else if(f==3) { if(*l[0]<*l[j]) *l[0] = *l[j]; }
						else *l[0] = *l[j];
					}
				}
			}
		}
	}
}

void CityDisplay::buildCityWalls() {
/*	static const int walls[65] = { 22,
		1,
		7,2,
		12,3,8,3,
		16,4,9,4,13,4,9,4,
		19,5,10,2,14,3,8,3,17,5,10,5,10,5,10,5,
		21,6,11,11,15,15,15,15,18,18,18,18,18,18,18,18,20,6,10,10,15,15,15,15,18,18,11,11,15,15,15,15 };*/
	int x = mw>>1;
	//unsigned short **l;
	buildWall(0,0,x+1,mh-1);
	buildWall(mw-1,0,x-1,mh-1);
//	for(i=0; i<size; i++) if(map[i].building && (map[i].terrain<=COAST || map[i].terrain>=8)) map[i].building = 0;
//	for(y=0,i=0; y<mapHeight(); y++) for(x=0; x<mapWidth(); x++,i++) if(map[i].building) {
//		l = (unsigned short **)getIsoArea(x,y,1,bld,2,0,0xf);
//		align = walls[(*l[0]>0)|((*l[1]>0)<<1)|((*l[2]>0)<<2)|((*l[3]>0)<<3)];
//		map[i].terrain = DESERT;
//		map[i].building = (align<<8)|WALL;
//	}
}

void CityDisplay::buildWall(int x1,int y1,int x2,int y2) {
/*	int i,m,x,y,nx = x1<x2? 1 : -1,p;
	x2 += nx;
	for(y=y1; y<=y2; y++)
		for(x=x1,p=0; x!=x2; x+=nx) {
			if((y<y2 && (map[x+(nx==1? (y&1? nx : 0) : (y&1? 0 : nx))+(y+1)*mw].terrain.type1)>=GROUND_TYPES) ||
				(y>0 && (map[x+(nx==1? (y&1? nx : 0) : (y&1? 0 : nx))+(y-1)*mw].terrain.type1)>=8)) p = 1;
			if((map[(x+nx)+y*mw].terrain.g)>=8) p = 2;
			if(p && (m=(map[i=(x+y*mw)].terrain.type1))>COAST && m<8) map[i].building = 1;
			if(p==2) break;
		}
	for(y=y1; y<=y2; y++)
		for(x=x1,p=0; x!=x2; x+=nx) {
			if(map[x+y*mw].building) break;
			if(!(p&1) && y>0 && map[i=x+(nx==1? (y&1? nx : 0) : (y&1? 0 : nx))+(y-1)*mw].building) p |= 1;
			if(!(p&2) && y<y2 && map[i=x+(nx==1? (y&1? nx : 0) : (y&1? 0 : nx))+(y+1)*mw].building) p |= 2;
			if(p==3) map[x+y*mw].building = 1;
		}
	for(y=y1; y<=y2; y++)
		for(x=x1,p=0; x!=x2; x+=nx) {
			if(!(p&1) && y>0 && map[i=x-(nx==1? (y&1? 0 : nx) : (y&1? nx : 0))+(y-1)*mw].building) p |= 1;
			if(!(p&2) && y<y2 && map[i=x-(nx==1? (y&1? 0 : nx) : (y&1? nx : 0))+(y+1)*mw].building) p |= 2;
			if(p==7) {
				map[x+y*mw].building = 0;
				if(!map[x+nx+y*mw].building) break;
			}
			if(!(p&4) && map[x+y*mw].building) p |= 4;
		}*/
}

