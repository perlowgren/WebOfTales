#ifndef _CLIENT_MAPDLG_H
#define _CLIENT_MAPDLG_H

#include <libamanita/sdl/aWindow.h>
#include "../game/City.h"

class MapDisplay;

class MapDlg : public aWindow,public aMouseMotionListener {
aObject_Instance(MapDlg)

private:
	MapDisplay *map;
	aImage *mapimage;
	int mw,mh,xp,yp,res;
	double longitude,latitude;
	char caption[32],coords[32];

public:
	static const int Width;
	static const int Height;

	MapDlg();
	virtual ~MapDlg();

	void setCaption(const char *c);
	void setCoordinates(double lon,double lat);
	void paint(time_t time);
	void setMapDisplay(MapDisplay *m);
	void drawMap();
	void setMapSize(int w,int h);
	bool mouseDown(aMouseEvent &me);
	bool mouseDrag(aMouseMotionEvent &mme);
	aImage *getMapImage() { return mapimage; }
	int mapX() { return xp; }
	int mapY() { return yp; }
	int mapWidth() { return mw; }
	int mapHeight() { return mh; }
	int resolution() { return res; }
};



#endif
