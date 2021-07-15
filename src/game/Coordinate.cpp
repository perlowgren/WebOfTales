
#include "../config.h"
#include <SDL/SDL_stdinc.h>
#include <libamanita/aMath.h>
#include "Map.h"
#include "Coordinate.h"


aObject_Inheritance(Coordinate,aObject);

const int Coordinate::tw = 128;
const int Coordinate::t1 = (Coordinate::tw>>1); // 64
const int Coordinate::t2 = (Coordinate::tw>>2); // 32
const int Coordinate::t3 = (Coordinate::tw>>3); // 16
const int Coordinate::t2m1 = Coordinate::t2-1; // 31
const int Coordinate::t2x3 = Coordinate::t2*3; // 96
const int Coordinate::t3x3 = Coordinate::t3*3; // 48


Coordinate::Coordinate(Map *m,int32_t px,int32_t py) : aObject(),map(m),px(px),py(py),fx(0),fy(0) {
	pntToMap(px,py,mx,my);
}

Coordinate::Coordinate(Map *m,int16_t mx,int16_t my) : aObject(),map(m),fx(0),fy(0),mx(mx),my(my) {
	mapToPnt(mx,my,px,py);
}

void Coordinate::pntToMap(int32_t xp,int32_t yp,int16_t &xm,int16_t &ym) {
	ym = (yp+t3)/t2-1,xm = (xp+((ym&1)? t1 : 0))/tw-(ym&1);
	xp = (xp+((ym&1)? t1 : 0))%tw,yp = (yp+t3)%t2;
	if(xp<t2) {
		xp = (1+xp)/2;
		if(yp<t3) { if(xp+yp<t3) ym--,xm -= ym&1? 1 : 0; }
		else { if(xp+(t2m1-yp)<t3) ym++,xm -= ym&1? 1 : 0; }
	} else if(xp>=t2x3) {
		xp = (tw-xp)/2;
		if(yp<t3) { if(xp+yp<t3) ym--,xm += ym&1? 0 : 1; }
		else { if(xp+(t2m1-yp)<t3) ym++,xm += ym&1? 0 : 1; }
	}
}
void Coordinate::mapToPnt(int16_t xm,int16_t ym,int32_t &xp,int32_t &yp) {
	xp = xm*tw+((ym&1)? tw : t1),yp = ym*t2+t2;
}


void Coordinate::setPoint(int32_t x,int32_t y) {
	if(x<0) x += map->pw;else if(x>=map->pw) x -= map->pw;
	if(y<0) y += map->ph;else if(y>=map->ph) y -= map->ph;
	pntToMap(x,y,mx,my);
	if(mx<0) mx += map->mw;else if(mx>=map->mw) mx -= map->mw;
	if(my<0) my += map->mh;else if(my>=map->mh) my -= map->mh;
	px = x,py = y;
}
void Coordinate::setMap(int16_t x,int16_t y) {
	if(x<0) x += map->mw;else if(x>=map->mw) x -= map->mw;
	if(y<0) y += map->mh;else if(y>=map->mh) y -= map->mh;
	mx = x,my = y;
	mapToPnt(mx,my,px,py);
}


int Coordinate::distance(Coordinate &c) {
	int dx = abs(px-c.px),dy = abs(py-c.py);
	if(abs(dx-map->pw)<dx) dx = abs(dx-map->pw);
	if(abs(dy-map->ph)<dy) dy = abs(dy-map->ph);
	return dx>dy*2? dx : dy*2;
}

void Coordinate::write(FILE *fp) {
	size_t i;
	i = fwrite(&px,4,1,fp);
	i = fwrite(&py,4,1,fp);
	fputc(fx,fp);
	fputc(fy,fp);
}
void Coordinate::read(FILE *fp) {
	size_t i;
	int32_t x,y;
	i = fread(&x,4,1,fp);
	i = fread(&y,4,1,fp);
	fx = fgetc(fp);
	fy = fgetc(fp);
	setPoint(x,y);
}


