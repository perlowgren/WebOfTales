#ifndef _CLIENT_MAPFRAME_H
#define _CLIENT_MAPFRAME_H

#include <libamanita/sdl/aImage.h>
#include <libamanita/sdl/aDisplay.h>
#include "WoTClient.h"
#include "../game/Map.h"
#include "../game/Cloud.h"


class Character;

struct mousepoint {
	int32_t px,py;
	int16_t mx,my;
};

class MapDisplay : public Map,public aDisplay {
aObject_Instance(MapDisplay)

public:
	viewpoint v;
	mousepoint mouse;
	int stats;

	MapDisplay();
	virtual ~MapDisplay();

	void start();
	void stop();
	virtual void paint(time_t time);
	virtual void paintGroundLayer(viewpoint &v);
	virtual void paintUnitLayer(viewpoint &v) {}
	virtual void paintCloudLayer(viewpoint &v);
	virtual void paintFrame() {}
	void paintChat();
	void paintPlayerFrame(Character &ch,int x,int y);
	void paintNPCFrame(Character &ch,int x,int y);
	static char *tohms(double d,int r);

	virtual bool keyDown(aKeyEvent &ke);
	virtual bool mouseDown(aMouseEvent &me);
	virtual bool mouseUp(aMouseEvent &me);
	virtual bool mouseMove(aMouseMotionEvent &mme);
	virtual bool mouseDrag(aMouseMotionEvent &mme);

	void setViewPoint(Coordinate &c);
	void setViewPoint(int32_t px,int32_t py);
	void setViewPoint(int16_t mx,int16_t my);
	void setMousePoint(int x,int y);
};

#endif

