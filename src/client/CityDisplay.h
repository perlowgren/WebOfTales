#ifndef _CLIENT_CITYSCREEN_H
#define _CLIENT_CITYSCREEN_H

#include <libamanita/sdl/aImage.h>
#include <libamanita/sdl/aDisplay.h>
#include "MapDisplay.h"
#include "../game/Coordinate.h"
#include "../game/City.h"

class aRandom;

class CityDisplay : public MapDisplay {
aObject_Instance(CityDisplay)

private:
	bool createMap();
	void buildCityWalls();
	void buildWall(int x1,int y1,int x2,int y2);
	void fillAlign(int *m,int t,int a);
	void fillRect(int *m,int t,int x,int y,int w,int h);
	void fillEllipse(int *m,int t,int x,int y,int w,int h);
	void erosion(aRandom &r,int *m,int e,int f);
	void erosion(aRandom &r,int *m,int x1,int y1,int x2,int y2,int e,int f);

public:
	CityDisplay();
	virtual ~CityDisplay();

	void enterCity(City *c);

	void paintUnitLayer(viewpoint &v);
	void paintFrame();
	bool keyDown(aKeyEvent &ke);
	bool mouseDown(aMouseEvent &me);
//	bool mouseUp(aMouseEvent &me);
//	bool mouseMove(aMouseMotionEvent &mme);
	bool mouseDrag(aMouseMotionEvent &mme);
};

#endif

