
#include "../config.h"
#include <math.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>

#ifdef WOTSERVER
	#include <libamanita/net/aServer.h>
	#include "../command.h"
	#include "../server/WebOfTales.h"
#elif defined WOTCLIENT
	#include <libamanita/net/aSocket.h>
	#include <libamanita/sdl/aGraphics.h>
	#include "../client/WoTClient.h"
#endif

#include "Map.h"
#include "Cloud.h"


aObject_Inheritance(Cloud,Coordinate);


int Cloud::dirsX[8] = { 2,2,2,0,-2,-2,-2,0 };
int Cloud::dirsY[8] = { -1,0,1,1,1,0,-1,-1 }; // 0=NE, 1=E, 2=SE, 3=S, 4=SW, 5=W, 6=NW, 7=N

Cloud::Cloud(Map *m,uint32_t id) : Coordinate(m) {
	setPoint(rnd.uint32(map->pw),rnd.uint32(map->ph));
	fx = 0,fy = 0;
	key = 0,next = 0;
	map->placeCloud(*this);
	cld.id = id;
	cld.seed = rnd.uint32();
	if(cld.seed>0xffff) cld.seed >>= cld.seed&0xf;
	cld.str = 64+rnd.uint32(192);
	cld.dir = rnd.uint32(8);
	setForce();
	setVelocity();
	setIndex();
// app.printf("Cloud::Cloud(seed=%d,force=%d,vel=%d,dir=%d)\n",cld.seed,cld.force,cld.vel,cld.dir);
}

Cloud::Cloud(Map *m,uint8_t **data) : Coordinate(m) {
	uint32_t x,y;
	unpack_uint32(data,x);
	unpack_uint32(data,y);
	unpack_uint32(data,cld.seed);
	unpack_uint16(data,cld.force);
	unpack_uint8(data,cld.str);
	unpack_uint8(data,cld.dir);

	setPoint(x>>8,y>>8);
	fx = x&0xff,fy = y&0xff;
	key = 0,next = 0;
	map->placeCloud(*this);

	setVelocity();
	setIndex();
// app.printf("Cloud::Cloud(seed=%d,force=%d,vel=%d,dir=%d)\n",cld.seed,cld.force,cld.vel,cld.dir);
}

void Cloud::update() {
// app.printf("Cloud::update()\n");
	setForce();
	setVelocity();
	cld.dir = (cld.dir+((cld.seed/7)%3)-1)%8;
	cld.seed >>= 2;
// app.printf("Cloud::update(done)\n");
}

void Cloud::animate() {
#ifdef WOTSERVER
	if(!cld.force--) {
		update();
		if(cld.seed<0x1000) {
app.printf("Cloud::animate(id=%d,force=%d,vel=%d,dir==%d)\n",cld.id,cld.force,cld.vel,cld.dir);
			cld.seed |= rnd.uint32()<<12;
			sendCloudPack();
		}
	}
#elif defined WOTCLIENT
	if(!cld.force--) update();
#endif
	move();
}

void Cloud::move() {
	long x = ((px<<8)|fx)+dirsX[cld.dir]*cld.vel,y = ((py<<8)|fx)+dirsY[cld.dir]*cld.vel;
	setPoint(x>>8,y>>8);
	fx = x&0xff,fy = y&0xff;
	map->placeCloud(*this);
}


#ifdef WOTCLIENT
void Cloud::paint(viewpoint &v) {
	SDL_Rect &r = *app.cloudsImage->getCell(cld.index);
	long x = px-v.px-r.w/2,y = py-v.py-r.h/2;
	if(x+r.w<0) x += map->pw;else if(x>=map->pw) x -= map->pw;
	if(y+r.h<0) y += map->ph;else if(y>=map->ph) y -= map->ph;
	app.cloudsImage->draw(x,y,r);
}
#endif


bool Cloud::receiveCloudPack(uint8_t *data) {
	data += SOCKET_HEADER;
	Cloud *cl = (Cloud *)Map::cloudids[get_unpack_uint32(data)];
	if(!cl) return false;
	uint32_t x,y;
	data += 4;
	unpack_uint32(&data,x);
	unpack_uint32(&data,y);
	unpack_uint32(&data,cl->cld.seed);
	unpack_uint16(&data,cl->cld.force);
	unpack_uint8(&data,cl->cld.str);
	unpack_uint8(&data,cl->cld.dir);

	cl->setPoint(x>>8,y>>8);
	cl->fx = x&0xff,cl->fy = y&0xff;
	cl->setVelocity();
	cl->setIndex();
	return true;
}

#ifdef WOTSERVER
void Cloud::sendCloudPack() {
	uint8_t d[SOCKET_HEADER+18],*p = d;
	pack_header(&p,CMD_CLOUD);
	pack_uint16(&p,cld.id);
	pack(&p);
	app.send(d,sizeof(d));
}

void Cloud::pack(uint8_t **data) {
	uint32_t x = (px<<8)|fx,y = (py<<8)|fy;
	pack_uint32(data,x);
	pack_uint32(data,y);
	pack_uint32(data,cld.seed);
	pack_uint16(data,cld.force);
	pack_uint8(data,cld.str);
	pack_uint8(data,cld.dir);
}
#endif

