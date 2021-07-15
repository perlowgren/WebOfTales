#ifndef _GAME_MAP_H
#define _GAME_MAP_H

#include <stdio.h>
#include <libamanita/aVector.h>
#include <libamanita/aHashtable.h>
#include "Coordinate.h"



#define ALIGN_HEX_EQ(a,m,v)		((a[0]->m==v)|((a[1]->m==v)<<1)|((a[2]->m==v)<<2)|((a[3]->m==v)<<3))
#define ALIGN_HEX_NE(a,m,v)		((a[0]->m!=v)|((a[1]->m!=v)<<1)|((a[2]->m!=v)<<2)|((a[3]->m!=v)<<3))
#define ALIGN_HEX_LT(a,m,v)		((a[0]->m<v)|((a[1]->m<v)<<1)|((a[2]->m<v)<<2)|((a[3]->m<v)<<3))
#define ALIGN_HEX_LE(a,m,v)		((a[0]->m<=v)|((a[1]->m<=v)<<1)|((a[2]->m<=v)<<2)|((a[3]->m<=v)<<3))
#define ALIGN_HEX_GT(a,m,v)		((a[0]->m>v)|((a[1]->m>v)<<1)|((a[2]->m>v)<<2)|((a[3]->m>v)<<3))
#define ALIGN_HEX_GE(a,m,v)		((a[0]->m>=v)|((a[1]->m>=v)<<1)|((a[2]->m>=v)<<2)|((a[3]->m>=v)<<3))

#define ALIGN_HEX_BT(a,m,v1,v2)	((a[0]->m>v1 && a[0]->m<v2)|((a[1]->m>v1 && a[1]->m<v2)<<1)|((a[2]->m>v1 && a[2]->m<v2)<<2)|((a[3]->m>v1 && a[3]->m<v2)<<3))

#define SUM_HEX_EQ(a,m,v)			((a[0]->m==v)+(a[1]->m==v)+(a[2]->m==v)+(a[3]->m==v))

class aWord;
class aRandom;
class Cloud;
class Location;
class Character;
class NPC;
class Player;
class Team;

enum WRAP {
	WRAP_NONE	= 0,
	WRAP_HORIZ	= 1,
	WRAP_VERT	= 2,
	WRAP_BOTH	= 3
};

enum GROUND {
	OCEAN							= 0,
	PLAIN							= 1,
	DESERT						= 2,
	GROUND_TYPES				= 3,
};
enum TERRAIN {
	TERRAIN_NONE				= 0,
	MOUNTAIN						= 1,
	SWAMP							= 2,
	FOREST						= 3,
	DJUNGLE						= 4,
	TERRAIN_TYPES				= 5
};
enum CONSTRUCT {
	CONSTRUCT_NONE				= 0,
	ROAD							= 1,
	CITY							= 2,
	WALL							= 3,
	HOUSE							= 4,
	CONSTRUCT_TYPES			= 5
};

enum {
	AREA_OCEAN					= 0,
	AREA_SEA						= 1,
	AREA_LAKE					= 2,
	AREA_POND					= 3,
	AREA_CONTINENT				= 4,
	AREA_ISLAND					= 5,
	AREA_ISLET					= 6,
	AREA_REGION					= 7
};

struct viewpoint {
	int32_t px,py;
	int16_t mx,my,tx,ty;
};

struct area {
	char *name;
	uint8_t type;
	uint16_t index,x,y,sz,terrain[7];
};

struct region {
	char *name;
	uint8_t type;
	uint16_t index,x,y,area,sz,terrain[7];
};

#define sizeof_mapheader 13

class Map {
private:
	void setBounds(int w,int h);

	void putCloud(Cloud &c);
	void removeCloud(Cloud &c);

	void putItem(Character &c);
	Character *getItem(int32_t px,int32_t py,aClass *c);

protected:
	struct mapheader {
		uint32_t ver;
		uint32_t id;
		uint16_t w;
		uint16_t h;
		uint8_t wrap;
	};
	uint32_t id,seed;
	char *name;

	Location *location;

/*
	void writeHeader(FILE *fp);
	void readHeader(FILE *fp,mapheader *h=0);
*/

public:
	struct mapcolor {
		uint8_t r,g,b,a;
	};
	struct maplocation {
		uint8_t g;								/** < Ground */
		uint8_t gi;							/** < Ground index */
		uint8_t gv;							/** < Ground value */
		uint8_t ga;							/** < Ground align */
		uint8_t t;								/** < Terrain */
		uint8_t tv[TERRAIN_TYPES-1];		/** < Terrain values */
		uint8_t c;								/** < construct */
		uint8_t cv;							/** < construct value */
		uint16_t a;							/** < area index */
		uint16_t r;							/** < region index */
		uint16_t p;							/** < population */
		Cloud *clouds;						/** < Clouds in this maplocation */
		Character *items;					/** < Items in this maplocation */
		Location *loc;
	};

	static const uint32_t ver;
	static aVector cloudids;
	static aHashtable locationids;
	static aHashtable itemids;
	static aHashtable teamids;

	int32_t mw,mh,pw,ph;
	size_t size;
	uint8_t wrap;
	maplocation *map;
	int nareas;
	area *areas;
	int nregions;
	region *regions;

	Map();
	Map(int w,int h);
	~Map();

	void setID(uint32_t i) { id = i; }
	uint32_t getID() { return id; }
	void setName(const char *nm);
	const char *getName() { return name; }

	int getPoint(int x,int y) { return ((x>=mw? x-mw : (x<0? x+mw : x))+(y>=mh? y-mh : (y<0? y+mh : y))*mw); }

	void createMap(int w,int h);
	void clearMap();

	bool loadMap(uint8_t *data);
	void makeTerrain(aRandom &r);
	void makeClouds(int n);
	void makeAreas(aWord &w);
	void makeCities(aRandom &r,aWord &w);
	void makeRegions(aWord &w);

	static int pathCompareArea(int x1,int y1,int x2,int y2,void *map);
	static int pathTerrainType(int x,int y,void *map);
	static int pathMoveCost(int x,int y,int c,void *map,void *obj);

	int getDir(int x1,int y1,int x2,int y2);

	area *getArea(Coordinate &c) { return getArea(c.mx,c.my); }
	area *getArea(int x,int y) { return areas? &areas[map[x+y*mw].a] : 0; }
	region *getRegion(Coordinate &c) { return getRegion(c.mx,c.my); }
	region *getRegion(int x,int y) { return regions? &regions[map[x+y*mw].r] : 0; }

	void **getArea(int x,int y,int radius,void *map,int sz);
	void **getIsoArea(int x,int y,int radius,void *map,int sz);

	static void addLocation(Location *l);
	static Location *getLocation(uint32_t id) { return (Location *)locationids.get(id); }
	Location *getLocation(int x,int y) { return map[x+y*mw].loc; }
	void setLocation(Location *l) { location = l; }
	Location *getLocation() { return location; }
	uint32_t getLocationID();

#ifdef WOTCLIENT
	void createClouds(uint8_t *data);
#endif
	static void deleteAllClouds();
	void placeCloud(Cloud &c);

	void createItems(uint8_t *data);
	static void addItem(Character &c);
	static Character *getItem(uint32_t id) { return (Character *)itemids.get(id); }
	static void deleteAllItems();
	static void deleteItem(Character &c);
	Character *getItem(int32_t px,int32_t py) { return getItem(px,py,0); }
	void placeItem(Character &c);
	void removeItem(Character &c);
	void replaceItem(Character &c1,Character &c2);

	void createTeams(uint8_t *data);
	static void addTeam(Team &t);
	static Team *getTeam(uint32_t id) { return (Team *)teamids.get(id); }
	static void deleteAllTeams();
	static void deleteTeam(Team &t);

	Character *getCharacter(int32_t px,int32_t py);

	mapcolor getColor(int x,int y,int filter) { return getColor(y*mw+x,filter); }
	mapcolor getColor(int i,int filter);
};

#endif /* _GAME_MAP_H */

