
#include "../config.h"

#ifdef WOTSERVER
	#include "../server/WebOfTales.h"
#elif defined WOTCLIENT
	#include <libamanita/sdl/aImage.h>
	#include <libamanita/sdl/aFont.h>
	#include "../Main.h"
	#include "../client/WoTClient.h"
#endif

#include "Location.h"


aObject_Inheritance(Location,aObject);


Location::Location(Map *map,uint32_t id,int mx,int my)
		: aObject(),map(map),id(id),name(0),mx(mx),my(my) {
	sprite = 0x00;
}

Location::~Location() {
app.printf("~Location(id=%d)",id);
	if(name) { free(name);name = 0; }
}


void Location::setName(const char *nm) {
	if(name) { free(name);name = 0; }
	if(nm && *nm) name = strdup(nm);
}

#ifdef WOTCLIENT
void Location::paint(int x,int y) {
	aImage &img = *app.constructImages[CITY][sprite>>4];
	SDL_Rect &r = *img.getCell(sprite&0xf);
	img.draw(x-r.w/2,y-r.h-16,r);
}
#endif

