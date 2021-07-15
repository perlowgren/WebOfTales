#ifndef _CLIENT_WORLDFRAME_H
#define _CLIENT_WORLDFRAME_H

#include <libamanita/sdl/aImage.h>
#include "MapDisplay.h"


class WorldDisplay : public MapDisplay {
aObject_Instance(WorldDisplay)

public:
	WorldDisplay();
	virtual ~WorldDisplay();

	void paint(time_t time);
	void paintUnitLayer(viewpoint &v);
	void paintFrame();

	bool keyDown(aKeyEvent &ke);
	bool mouseDown(aMouseEvent &me);
};

#endif

