
#include "../config.h"
#include <stdio.h>
#include <math.h>
#include <libamanita/aRandom.h>

#ifdef WOTSERVER
	#include "../server/WebOfTales.h"
#elif defined WOTCLIENT
	#include <libamanita/sdl/aGraphics.h>
	#include <libamanita/sdl/aFont.h>
	#include <libamanita/sdl/aImage.h>
	#include "../Main.h"
	#include "../client/WoTClient.h"
#endif

#include "City.h"


aObject_Inheritance(City,Location);


City::City(Map *map,uint32_t id,int mx,int my)
		: Location(map,id,mx,my),population(0),country(0) {
	work = 0,size = 0,produce = 0;
}

City::~City() {
app.printf("~City(id=%d,name=%s)",id,name);
}

//enum GROUND {
//	OCEAN							= 0,
//	PLAIN							= 1,
//	DESERT						= 2,
//	GROUND_TYPES				= 3,
//};
//enum TERRAIN {
//	TERRAIN_NONE				= 0,
//	MOUNTAIN						= 1,
//	SWAMP							= 2,
//	FOREST						= 3,
//	DJUNGLE						= 4,
//	TERRAIN_TYPES				= 5
//};

void City::build(aRandom &r) {
	static const int terrainSupport[] = { 20000,50000,3000,5000,3000,10000,7000 };
	Map::maplocation **l = (Map::maplocation **)map->getIsoArea(mx,my,1,map->map,sizeof(Map::maplocation));
	int t,s = 1000;
	for(int i=0; i<=4; i++) if(l[i]) {
		t = terrainSupport[l[i]->t!=TERRAIN_NONE? GROUND_TYPES-1+l[i]->t : l[i]->g];
		s += t;//t/50+r.getUint32(t-t/50);
	}
	setPopulation(s/50+r.uint32(s-s/50));
}

void City::setProduction(int p) {
	if(p!=1) return;
	produce = p;
	work = 1;
}

void City::setPopulation(int p) {
	if(p>=1000000000) p = 1000000000;
	population = p;
	setSize(population<10000? 1+population*5/10000 :
		(population<100000? 6+(population-10000)*5/90000 : 6+(int)log10(population)));
}

void City::setSize(int s) {
	size = s;
	if(size<1) size = 1;
	else if(size>12) size = 12;
	sprite = 0x10*((size-1)/5);
app.printf("City Pop: %d, Size: %d, Sprite: %d",population,size,sprite);
}


#ifdef WOTCLIENT
void City::paint(int x,int y) {
	aImage &img = *app.constructImages[CITY][sprite>>4];
	SDL_Rect &r = *img.getCell(sprite&0xf);
	img.draw(x-r.w/2,y-r.h-16,r);
	aFont &f = *app.fonts[FONT_LARGE];
//	country->paintFlags(x-46,y);
//	MapDisplay::signs->draw(x-34,y,0,country->getID());
	f.print(x,y-r.h-18,ALIGN_CENTER,getName());
}
#endif

