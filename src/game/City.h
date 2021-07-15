#ifndef _GAME_CITY_H
#define _GAME_CITY_H

#include <stdio.h>
#include "Location.h"

class aRandom;
class Country;

class City : public Location {
aObject_Instance(City)

protected:
	uint32_t population;
	uint16_t work;
	uint8_t size;
	uint8_t produce;
	Country *country;

	void setPopulation(int p);
	void setSize(int s);

public:
	City(Map *map,uint32_t id,int mx,int my);
	virtual ~City();

	int getSize() { return size; }

	void build(aRandom &r);
	void setProduction(int p);

#ifdef WOTCLIENT
	void paint(int x,int y);
#endif
};


#endif /* _GAME_CITY_H */
