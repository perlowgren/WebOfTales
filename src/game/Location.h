#ifndef _GAME_LOCATION_H
#define _GAME_LOCATION_H

#include <stdio.h>
#include "Map.h"

class Location : public aObject {
aObject_Instance(Location)

protected:
	Map *map;
	uint32_t id;
	char *name;

public:
	uint16_t mx,my,sprite;

	Location(Map *map,uint32_t id,int mx,int my);
	virtual ~Location();

	Map *getMap() { return map; }
	uint32_t getID() { return id; }
	void setName(const char *nm);
	const char *getName() { return name; }

#ifdef WOTCLIENT
	virtual void paint(int x,int y);
#endif
};


#endif /* _GAME_LOCATION_H */
