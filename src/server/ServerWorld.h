#ifndef _SERVER_SERVERWORLD_H
#define _SERVER_SERVERWORLD_H

#include <libamanita/aMath.h>
#include <libamanita/net/aServer.h>
#include "../game/Map.h"


class ServerWorld : public Map {
private:
	struct creationinfo {
		int continents;
		float landmass;
		float mountains;
		float islands;
		float forest;
		float cities;
		float technology;
	};
	struct spawnpoint {
		unsigned char rate;			// Time it takes until a new spawn
		unsigned char index;			// NPC index
		unsigned char strength;		// Strength of NPC
		unsigned char timer;			// Timer until next spawn
	};
	struct worldlocation {
		float temperature;
		float altitude;
		float sea;
		float environment;
		float vegetation;
		float life;
		float population;
		unsigned short culture;
		unsigned char inland;
		spawnpoint spawn;
	};
	enum WORLDLOCATION {
		TEMPERATURE			= 0,
		ALTITUDE				= 4,
		SEA					= 8,
		ENVIRONMENT			= 12,
		VEGETATION			= 16,
		LIFE					= 20,
		POPULATION			= 24,
		CULTURE				= 28,
		INLAND				= 30
	};

	uint32_t nameseed;
	float seaLevel;
	float seaLevelMultiply;
	float equator;
	int cycle,cycles;
	int era;
	int eraPresent;
	const char *eraDescription;
	bool completed;
	creationinfo ci;
	creationinfo result;
	worldlocation *world;
	size_t _mapsz;
	uint8_t *_map;				/** < This map is only used for non-big-endian systems to avoid swapping all data
													every time it is sent to new clients. */

	mapcolor getColor(int i,int filter);

public:
	ServerWorld(int w,int h);
	~ServerWorld();

	void create(time_t time);

	void run(time_t time);
	void sendWorldMap(aConnection client);
	void sendClouds(aConnection client);
	void sendItems(aConnection client);
	void sendTeams(aConnection client);
	void sendNPC(aConnection client,NPC &np,int type=1);
	bool uploadPlayer(Player &pl);
	bool downloadPlayer(Player &pl);
	void sendPlayer(aConnection client,Player &pl,int type=1);

	void writeMap();
	void openMap();
	void closeMap();
	void createSpawnPoints();

	bool saveWorld();
	bool loadWorld(const char *nm);

	bool createWorld(const char *nm,int con,int lnd,int mnt,int isl,int fst,int ppl,int tec);
	void nextEra();

	void cool();
	void oscean();
	void climate(float change);
	int biology(float change);
	void populate();

	void impact(int x,int y,WORLDLOCATION member,float radius,float value,bool add);
	void erosion(int i,WORLDLOCATION member,float change);
	int getTime() { return cycle; }
	int getEra() { return era; }
	const char *getEraDescription(int era);
	const creationinfo &getResult() { return result; }
	bool isCompleted() { return completed; }

	float adjustValue(float value) { return value<0.0f? 0.0f : (value>1.0f? 1.0f : value); }
	float belowSeaLevel(float value) { return seaLevel-value; }
	float aboveSeaLevel(float value) { return value-seaLevel; }

	mapcolor getColor(int x,int y,int filter) { return getColor(x+y*mw,filter); }
	static mapcolor irColor(float value) { return value<1.0f? glacierColor(value) : (value>1.6f? magmaColor(value-1.6f) : vegetationColor(1.0f-(value-1.0f)*(1.0f/.6f))); }
	static mapcolor environmentColor(float value);
	static mapcolor magmaColor(float value);
	static mapcolor glacierColor(float value);
	static mapcolor mountainColor(float value);
	static mapcolor osceanColor(float value);
	static mapcolor desertColor(float value);
	static mapcolor planctonColor(float value);
	static mapcolor vegetationColor(float value);
	static mapcolor cityColor(float value);
};

#endif

