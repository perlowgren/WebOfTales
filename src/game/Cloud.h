#ifndef _GAME_CLOUD_H
#define _GAME_CLOUD_H

#include "Map.h"
#include "Coordinate.h"


struct cloud {
	uint16_t id;
	uint32_t seed;
	uint16_t force;
	uint8_t str;
	uint8_t vel;
	uint8_t index;
	uint8_t dir;
};

#define sizeof_cloud 16

class Cloud : public Coordinate {
aObject_Instance(Cloud)

public:
	static int dirsX[8],dirsY[8];

	uint32_t key;
	Cloud *next;
	cloud cld;

	Cloud(Map *m,uint32_t id);
	Cloud(Map *m,uint8_t **data);
	void setForce() { cld.force = 720+(cld.seed%2880); }
	void setVelocity() { cld.vel = 32+((cld.seed/3)%96); }
	void setIndex() { cld.index = 1+(cld.str-64)/24; }
	void update();
	void animate();
	void move();
#ifdef WOTCLIENT
	void paint(viewpoint &v);
#endif

	static bool receiveCloudPack(uint8_t *data);

#ifdef WOTSERVER
	void sendCloudPack();

	void pack(uint8_t **data);
#endif
};



#endif /* _GAME_CLOUD_H */
