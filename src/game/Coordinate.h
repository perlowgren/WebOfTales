#ifndef _GAME_COORDINATE_H
#define _GAME_COORDINATE_H

#include <stdio.h>



class Map;


enum DIRECTION {
	N					= 7,
	NE					= 0,
	E					= 4,
	SE					= 1,
	S					= 5,
	SW					= 2,
	W					= 6,
	NW					= 3
};


class Coordinate : public aObject {
friend class Map;

aObject_Instance(Coordinate)

protected:
	Map *map;

public:
	static const int tw;
	static const int t1;
	static const int t2;
	static const int t3;
	static const int t2m1;
	static const int t2x3;
	static const int t3x3;

	int32_t px,py;
	uint8_t fx,fy;
	int16_t mx,my;

	Coordinate(Map *m=0) : aObject(),map(m),px(0),py(0),fx(0),fy(0),mx(0),my(0) {}
	Coordinate(Map *m,int32_t px,int32_t py);
	Coordinate(Map *m,int16_t mx,int16_t my);

	Map *getMap() { return map; }

	static void pntToMap(int32_t xp,int32_t yp,int16_t &xm,int16_t &ym);
	static void mapToPnt(int16_t xm,int16_t ym,int32_t &xp,int32_t &yp);

	void set(Coordinate &c) { map=c.map,px=c.px,py=c.py,fx=c.fx,fy=c.fy,mx=c.mx,my=c.my; }
	void setPoint(Map *m,int32_t x,int32_t y) { map = m;setPoint(x,y); }
	void setPoint(int32_t x,int32_t y);
	void setMap(Map *m,int16_t x,int16_t y) { map = m;setMap(x,y); }
	void setMap(int16_t x,int16_t y);
	void moveX(int x) { this->px += px*tw,mx += x; }
	void moveY(int y) { this->py += py*t2,my += y; }

	int distance(Coordinate &c);

	void write(FILE *fp);
	void read(FILE *fp);
};




#endif /* _GAME_COORDINATE_H */
