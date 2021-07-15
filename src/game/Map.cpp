
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>
#include <libamanita/aWord.h>
#include <libamanita/aPath.h>
#include <libamanita/aFlood.h>

#ifdef WOTSERVER
#	include "../server/WebOfTales.h"
#elif defined WOTCLIENT
#	include <libamanita/net/aSocket.h>
#	include "../client/WoTClient.h"
#	include "../client/TeamDlg.h"
#endif

#include "Map.h"
#include "Cloud.h"
#include "City.h"
#include "Character.h"
#include "NPC.h"
#include "Player.h"
#include "Team.h"


const uint32_t Map::ver = 0x00000100;

aVector Map::cloudids;
aHashtable Map::locationids;
aHashtable Map::itemids;
aHashtable Map::teamids;


Map::Map() : name(0),wrap(WRAP_HORIZ) {
app.printf("Map::Map()");
	setBounds(0,0);
	map = 0,mw = 0,mh = 0,pw = 0,ph = 0,size = 0;
	nareas = 0,areas = 0,nregions = 0,regions = 0;
app.printf("Map::Map(done)");
}

Map::Map(int w,int h) : name(0),wrap(WRAP_HORIZ) {
app.printf("Map::Map(w=%d,h=%d)",w,h);
	setBounds(0,0);
	map = 0,mw = 0,mh = 0,pw = 0,ph = 0,size = 0;
	nareas = 0,areas = 0,nregions = 0,regions = 0;
	createMap(w,h);
	clearMap();
app.printf("Map::Map(done)");
}

Map::~Map() {
app.printf("Map::~Map(%x)",this);
	int i;
	area *a;
	region *r;
	if(name) { free(name);name = 0; }
	if(map) { free(map);map = 0; }
	if(areas) {
		for(i=0,a=areas; i<nareas; i++,a++) if(a->name) free(a->name);
		free(areas);
		areas = 0;
	}
	if(regions) {
		for(i=0,r=regions; i<nregions; i++,r++) if(r->name) free(r->name);
		free(regions);
		regions = 0;
	}
}

void Map::setName(const char *nm) {
	if(!nm || *nm=='\0') return;
	char s[4] = {0,0,0,0};
	if(name) free(name);
	name = strdup(nm);
	for(int i=0,n=strlen(nm); i<n; i++) s[3-(i%4)] ^= nm[i];
	seed = *(uint32_t *)((void *)s);
}

void Map::createMap(int w,int h) {
app.printf("Map::createMap(w=%d,h=%d)",w,h);
	if(mw==w && mh==h) return;
	setBounds(w,h);
	if(map) free(map);
	map = (maplocation *)malloc(size*sizeof(maplocation));
app.printf("Map::createMap(size*sizeof(*map)=%d,size*sizeof(maplocation)=%d,done)",size*sizeof(*map),size*sizeof(maplocation));
}

void Map::clearMap() {
	if(map) memset(map,0,size*sizeof(maplocation));
}

bool Map::loadMap(uint8_t *data) {
	if(!data) return false;

	uint32_t v;
	unpack_uint32(&data,v);
	if(v!=ver) {
		//TODO!!! Warn that application is of another version than the data.
		return false;
	}

	uint16_t w,h;
	unpack_uint32(&data,id);
	unpack_uint16(&data,w);
	unpack_uint16(&data,h);
	unpack_uint8(&data,wrap);

	createMap(w,h);

	setName((const char *)data);
	data += strlen(name)+1;

	uint16_t n;
	maplocation *l = map;
	for(size_t i=0; i<size; i++,l++) {
		unpack_uint16(&data,n);
		l->g = n&15,l->t = (n>>4)&15,l->c = (n>>8)&15;
	}

	aRandom r(seed);
	aWord word;
	word.setSeed(id);
	makeTerrain(r);
	makeAreas(word);
	makeCities(r,word);
	makeRegions(word);
	return true;
}



void Map::makeTerrain(aRandom &r) {
	int x,y,i,n,v,g = GROUND_TYPES,g2 = g*g;
	maplocation *t,**l;
	for(y=0; y<mh; y++) for(x=0; x<mw; x++) {
		i = x+y*mw,t = &map[i],l = (maplocation **)getIsoArea(x,y,2,map,sizeof(maplocation));
		t->gi = r.uint32(16);
		t->gv = 1+(l[0]? (l[0]->g<<5) : 0)+(l[1]? l[1]->g*g2 : 0)+(l[2]? l[2]->g*g : 0)+(l[5]? l[5]->g : 0);
		t->ga = (!l[1] || l[1]->g!=t->g) | ((!l[2] || l[2]->g!=t->g)<<1) |
					((!l[3] || l[3]->g!=t->g)<<2) | ((!l[4] || l[4]->g!=t->g)<<3) |
						((!l[5] || l[5]->g!=t->g)<<4) | ((!l[7] || l[7]->g!=t->g)<<5);
		for(n=0; n<TERRAIN_TYPES-1; n++) {
			v = (l[0] && l[0]->t==n+1? 8 : 0)+(l[1] && l[1]->t==n+1? 4 : 0)+
							(l[2] && l[2]->t==n+1? 2 : 0)+(l[5] && l[5]->t==n+1? 1 : 0);
			if(v==0xf) t->tv[n] = v+r.uint32(3),t->gv = 0;
			else t->tv[n] = v;
		}
		if(t->t && l[1] && l[1]->t==t->t && l[2] && l[2]->t==t->t && l[5] && l[5]->t==t->t) t->gv = 0;

		if(t->c!=CONSTRUCT_NONE) {
			if(t->c==ROAD) t->cv = 1+((l[1] && l[1]->c==t->c)|((l[2] && l[2]->c==t->c)<<1)|
					((l[3] && l[3]->c==t->c)<<2)|((l[4] && l[4]->c==t->c)<<3));
			else if(t->c==CITY) t->cv = 1;
		}
	}
}


void Map::makeClouds(int n) {
	deleteAllClouds();
	for(int i=0; i<n; i++) cloudids += new Cloud(this,i);
}



void Map::makeAreas(aWord &w) {
app.printf("Map::makeAreas()");
	static const char *areaTypeNames[] = { "Ocean","Sea","Lake","Pond","Continent","Island","Islet" };
	aFlood f(wrap);
	int d[size],s = size,i,t;
	char *p,name[128];
	maplocation *ml;
	area *a;
	for(i=0; i<s; i++) d[i] = map[i].g>OCEAN? -1 : -2;
	for(i=0,nareas=0; i<s; i++) if(d[i]<0) f.fill(d,mw,mh,i%mw,i/mw,nareas++);
	areas = (area *)malloc(sizeof(area)*nareas);
	memset(areas,0,sizeof(area)*nareas);
	for(i=0; i<s; i++) {
		ml = &map[i],t = ml->t!=TERRAIN_NONE? GROUND_TYPES-1+ml->t : ml->g;
		ml->a = d[i],a = &areas[ml->a];
		if(a->type==0) *a = (area){ 0,1,ml->a,i%mw,i/mw,0,{0,0,0,0,0,0,0} };
		a->sz++,a->terrain[t]++;
	}
	for(i=0; i<nareas; i++) {
		strcpy(name,w.generate(4,10));
		a = &areas[i],p = &name[strlen(name)-1];
		if(a->terrain[OCEAN]>=1) {
			if(a->sz>=5000) { a->type = AREA_OCEAN;strcat(name,*p=='i'? "c" : "ic"); }
			else a->type = a->sz>=500? AREA_SEA : (a->sz>=2? AREA_LAKE : AREA_POND);
		} else {
			if(a->sz>=300) { a->type = AREA_CONTINENT;strcat(name,*p=='a'? "n" : "an"); }
			else a->type = a->sz>=4? AREA_ISLAND : AREA_ISLET;
		}
		strcat(name," ");
		strcat(name,areaTypeNames[(int)a->type]);
		a->name = strdup(name);
	}
for(i=0; i<nareas; i++) {
a = &areas[i];
app.printf("Map::makeAreas(name=%s,x=%d,y=%d,sz=%d,terrain=(%d,%d,%d,%d,%d,%d,%d))",
	a->name,a->x,a->y,a->sz,a->terrain[0],a->terrain[1],a->terrain[2],a->terrain[3],a->terrain[4],
		a->terrain[5],a->terrain[6]);}
app.printf("Map::makeAreas(done)");
}


void Map::makeCities(aRandom &r,aWord &w) {
app.printf("Map::makeCities()");
	int s = size,i;
	char name[128];
	City *c;
	for(i=0; i<s; i++) if(map[i].c==CITY) {
		c = new City(this,i,i%mw,i/mw);
		c->build(r);
		strcpy(name,w.generate(4,10));
		strcat(name," City");
		c->setName(name);
		addLocation(c);
		map[i].loc = c;
	}
app.printf("Map::makeCities(done)");
}


void Map::makeRegions(aWord &w) {
app.printf("Map::makeRegions()");
	aFlood f(wrap);
	int d[size],s = size,i,j,n,t,**l;
	char name[128];
	maplocation *ml;
	area *a;
	region *r;
	for(i=0,nregions=1; i<s; i++) {
		if(map[i].loc) d[i] = nregions++;
		else d[i] = map[i].g==OCEAN? 0 : -1;
	}
	for(n=0; n<4; n++) {
		for(i=0; i<s; i++) if(d[i]>0 && d[i]<=0xffff) {
			l = (int **)getIsoArea(i%mw,i/mw,2,d,sizeof(int));
			for(j=1; j<9; j++) if(l[j] && *l[j]==-1) *l[j] = (d[i]<<16);
		}
		for(i=0; i<s; i++) if(d[i]>0xffff) d[i] >>= 16;
	}
	for(i=0; i<s; i++) if(d[i]==-1) f.fill(d,mw,mh,i%mw,i/mw,nregions++);
	regions = (region *)malloc(sizeof(region)*nregions);
	memset(regions,0,sizeof(region)*nregions);
	for(i=0; i<s; i++) {
		ml = &map[i],t = ml->t!=TERRAIN_NONE? GROUND_TYPES-1+ml->t : ml->g;
		ml->r = d[i],r = &regions[ml->r];
		if(r->type==0) *r = (region){ 0,1,ml->r,i%mw,i/mw,ml->a,0,{0,0,0,0,0,0,0} };
		r->sz++,r->terrain[t]++;
		if(ml->loc) r->name = strdup(((City *)ml->loc)->getName());
	}
	regions[0].name = 0;
	for(n=1; n<nregions; n++) {
		r = &regions[n],a = &areas[r->area];
		r->type = a->sz==r->sz || r->sz>=100? a->type : AREA_REGION;
		if(!r->name && r->type==AREA_REGION) {
			strcpy(name,w.generate(4,10));
			strcat(name," Region");
			r->name = strdup(name);
		}
	}
for(i=0; i<nregions; i++) {
r = &regions[i];
app.printf("Map::makeRegions(name=%s,x=%d,y=%d,sz=%d,terrain=(%d,%d,%d,%d,%d,%d,%d))",
	r->name,r->x,r->y,r->sz,r->terrain[0],r->terrain[1],r->terrain[2],r->terrain[3],r->terrain[4],
		r->terrain[5],r->terrain[6]);}
app.printf("Map::makeRegions(done)");
}


int Map::pathCompareArea(int x1,int y1,int x2,int y2,void *map) {
//ffprintf(stderr,out,"Map::pathCompareArea(map=%x)\n",map);
//fflush(out);
	if(!map) return 0;
	Map *m = (Map *)map;
	return m->map[x1+y1*m->mw].a-m->map[x2+y2*m->mw].a;
}
int Map::pathTerrainType(int x,int y,void *map) {
	if(!map) return 0;
	Map *m = (Map *)map;
	return (m->map[x+y*m->mw].g)>OCEAN? 0 : 1;
}
int Map::pathMoveCost(int x,int y,int c,void *map,void *obj) {
	static int ground[2*GROUND_TYPES] = {
	// oscean=O, plain=P, desert=D, tundra=T
	// 0=O 1=P 2=D 3=T
		 0,  2,  1, // BuildRoad Land
		 1,  0,  0, // BuildRoad Sea
	};
//	static int terrain[2*TERRAIN_TYPES] = {
//	// oscean=O, coast=C, desert=D, plain=P, tundra=T, forest=F, mountain=M, road=R, city=C, street=S, wall=W, house=H
//	// 0=O 1=C 2=D 3=P 4=T 5=F 6=M 7=R 8=C 9=S 10=W 11=H
//		  0,  0,  2,  1,  3,  1,  3, -2, -1, 1,   0,   0, // BuildRoad Land
//		  3,  1,  0,  0,  0,  0,  0, -2, -1, 1,   0,   0  // BuildRoad Sea
//	};
	Map::maplocation &t = ((Map *)map)->map[x+y*((Map *)map)->mw];
	int n = ground[c*GROUND_TYPES+t.g];
	if(n==0) return PATH_MOVE_COST_CANNOT_MOVE;
//	if(t.type2) n += cost[c+t.type2];
	return n;
}

int Map::getDir(int x1,int y1,int x2,int y2) {
	if(wrap&WRAP_HORIZ) { if(x1+mw-x2<x2-x1) x1 += mw;else if(x2+mw-x1<x1-x2) x2 += mw; }
	if(wrap&WRAP_VERT) { if(y1+mh-y2<y2-y1) y1 += mh;else if(y2+mh-y1<y1-y2) y2 += mh; }
	return y2<y1? (x2<x1+(y1&1)? 3 : 0) : (x2<x1+(y1&1)? 2 : 1);
}


#define RIGHT1(x) (x>=mw-1? x+1-mh : x+1)
#define BOTTOM1(y) (y>=mh-1? y+1-mh : y+1)
#define LEFT1(x) (x<1? x+mw-1 : x-1)
#define TOP1(y) (y<1? y+mh-1 : y-1)
#define RIGHT2(x) (x>=mw-2? x+2-mw : x+2)
#define BOTTOM2(y) (y>=mh-2? y+2-mh : y+2)
#define LEFT2(x) (x<=1? mw+x-2 : x-2)
#define TOP2(y) (y<=1? mh+y-2 : y-2)

void **Map::getArea(int x,int y,int radius,void *map,int sz) {
	static void *n[21];
	char *m = (char *)map,hw = (char)(wrap&WRAP_HORIZ),vw = (char)(wrap&WRAP_VERT);
	n[0] = m+(x+y*mw)*sz;
	if(radius>=1) {
		n[2] = vw || y<mh-1? m+(x+BOTTOM1(y)*mw)*sz : 0;
		n[4] = vw || y>0? m+(x+TOP1(y)*mw)*sz : 0;
		if(hw || x<mw-1) {
			n[1] = m+(RIGHT1(x)+y*mw)*sz;
			if(radius>=2) {
				n[5] = vw || y>0? m+(RIGHT1(x)+TOP1(y)*mw)*sz : 0;
				n[6] = vw || y<mh-1? m+(RIGHT1(x)+BOTTOM1(y)*mw)*sz : 0;
			}
		} else n[1] = n[5] = n[6] = 0;
		if(hw || x>0) {
			n[3] = m+(LEFT1(x)+y*mw)*sz;
			if(radius>=2) {
				n[7] = vw || y<mh-1? m+(LEFT1(x)+BOTTOM1(y)*mw)*sz : 0;
				n[8] = vw || y>0? m+(LEFT1(x)+TOP1(y)*mw)*sz : 0;
			}
		} else n[3] = n[7] = n[8] = 0;

		if(radius>=3) {
			if(hw || x<mw-2) {
				n[9] = m+(RIGHT2(x)+y*mw)*sz;
				if(radius>=4) {
					n[13] = vw || y>0? m+(RIGHT2(x)+TOP1(y)*mw)*sz : 0;
					n[14] = vw || y<mh-1? m+(RIGHT2(x)+BOTTOM1(y)*mw)*sz : 0;
				}
			} else n[9] = n[13] = n[14] = 0;

			if(vw || y<mh-2) {
				n[10] = m+(x+BOTTOM2(y)*mw)*sz;
				if(radius>=4) {
					n[15] = hw || x<mw-1? m+(RIGHT1(x)+BOTTOM2(y)*mw)*sz : 0;
					n[16] = hw || x>0? m+(LEFT1(x)+BOTTOM2(y)*mw)*sz : 0;
				}
			} else n[10] = n[15] = n[16] = 0;

			if(hw || x>1) {
				n[11] = m+(LEFT2(x)+y*mw)*sz;
				if(radius>=4) {
					n[17] = vw || y<mh-1? m+(LEFT2(x)+BOTTOM1(y)*mw)*sz : 0;
					n[18] = vw || y>0? m+(LEFT2(x)+TOP1(y)*mw)*sz : 0;
				}
			} else n[11] = n[17] = n[18] = 0;

			if(vw || y>1) {
				n[12] = m+(x+TOP2(y)*mw)*sz;
				if(radius>=4) {
					n[19] = hw || x>0? m+(LEFT1(x)+TOP2(y)*mw)*sz : 0;
					n[20] = hw || x<mw-1? m+(RIGHT1(x)+TOP2(y)*mw)*sz : 0;
				}
			} else n[12] = n[19] = n[20] = 0;
		}
	}
	return n;
}

void **Map::getIsoArea(int x,int y,int radius,void *map,int sz) {
	static void *n[21];
	char *m = (char *)map,hw = (wrap&1),vw = (wrap&2);
	n[0] = m+(x+y*mw)*sz;
	if(radius>=1) {
		if(radius>=2) {
			n[5] = hw || x<mw-1? m+(RIGHT1(x)+y*mw)*sz : 0;
			n[6] = vw || y<mh-2? m+(x+BOTTOM2(y)*mw)*sz : 0;
			n[7] = hw || x>0? m+(LEFT1(x)+y*mw)*sz : 0;
			n[8] = vw || y>1? m+(x+TOP2(y)*mw)*sz : 0;
		}

		if(y&1) x++;
		if(hw || x<mw) {
			if(x>=mw) x -= mw;
			n[1] = vw || y>0? m+(x+TOP1(y)*mw)*sz : 0;
			n[2] = vw || y<mh? m+(x+BOTTOM1(y)*mw)*sz : 0;
		} else n[1] = n[2] = 0;
		x--;
		if(hw || x>=0) {
			if(x<0) x += mw;
			n[3] = vw || y<mh? m+(x+BOTTOM1(y)*mw)*sz : 0;
			n[4] = vw || y>0? m+(x+TOP1(y)*mw)*sz : 0;
		} else n[3] = n[4] = 0;
	}
	return n;
}


void Map::addLocation(Location *l) { locationids.put(l->getID(),l); }
uint32_t Map::getLocationID() { return location? location->getID() : 0; }


void Map::setBounds(int w,int h) {
	mw = w,mh = h;
	pw = w*128,ph = h*32;
	size = w*h;
}


#ifdef WOTCLIENT
void Map::createClouds(uint8_t *data) {
app.printf("Map::createClouds(1)");
	unsigned long i;
	uint32_t sz;
	unpack_uint32(&data,sz);
	deleteAllClouds();
app.printf("Map::createClouds(2,sz=%d)",sz);
	for(i=0; i<sz; i++) cloudids += new Cloud(this,&data);
app.printf("Map::createClouds(3)");
}
#endif

void Map::deleteAllClouds() {
	if(cloudids.size()) {
		Cloud *c;
		for(int i=0,n=cloudids.size(); i<n; i++) {
			c = (Cloud *)cloudids[i];
			if(c->map->map) c->map->map[c->key].clouds = 0;
			delete c;
		}
		cloudids.clear();
	}
}
void Map::placeCloud(Cloud &c) {
	if(c.map) c.map->removeCloud(c);
	c.map = this;
	c.key = c.mx+c.my*mw;
	putCloud(c);
}
void Map::putCloud(Cloud &c) {
	if(!map) return;
	c.next = map[c.key].clouds,map[c.key].clouds = &c;
}
void Map::removeCloud(Cloud &c) {
	if(!map) return;
	Cloud *c1 = map[c.key].clouds;
	if(c1) {
		if(c1==&c) map[c.key].clouds = c1->next;
		else for(; c1->next; c1=c1->next) if(c1->next==&c) {
			c1->next = c.next;
			break;
		}
	}
	c.next = 0;
}


void Map::createItems(uint8_t *data) {
	uint16_t npn,pln;
	int i;
	Character *ch;
	unpack_uint16(&data,npn);
	unpack_uint16(&data,pln);
	for(i=npn; i>0; i--) {
		ch = new NPC(this,&data);
		addItem(*ch);
		placeItem(*ch);
	}
	for(i=pln; i>0; i--) {
		ch = new Player(this,&data);
		addItem(*ch);
		if(ch->isIncarnated()) placeItem(*ch);
	}
}

void Map::addItem(Character &c) {
	itemids.put(c.getID(),(void *)&c);
}
void Map::deleteAllItems() {
	if(!itemids.size()) return;
	Character *c;
	aHashtable::iterator iter = Map::itemids.iterate();
	while((c=(Character *)iter.next())) {
		c->map->removeItem(*c);
		delete c;
	}
	itemids.removeAll();
}
void Map::deleteItem(Character &c) {
app.printf("Map::deleteItem(id=%lu)",c.getID());
	if(c.map) c.map->removeItem(c);
	itemids.remove(c.getID());
#ifdef WOTSERVER
	Character *ct;
	aHashtable::iterator iter = Map::itemids.iterate();
	while((ct=(Character *)iter.next())) if(ct->target==&c) ct->stand();
#elif defined WOTCLIENT
	if(app.getPlayer()==&c) app.setPlayer(0);
	if(app.getHover()==&c) app.setHover(0);
	if(app.getFocus()==&c) app.setFocus(0);
#endif
	delete &c;
app.printf("Map::deleteItem(done)");
}

void Map::placeItem(Character &c) {
// app.printf("Map::placeItem(id=%lu)",c.getID());
	if(c.map) c.map->removeItem(c);
	c.map = this;
	c.key = c.mx+c.my*mw;
	putItem(c);
}
void Map::removeItem(Character &c) {
// app.printf("Map::removeItem(id=%lu)",c.getID());
	if(!map) return;
	Character *c1 = map[c.key].items;
	if(c1) {
		if(c1==&c) map[c.key].items = c1->next;
		else for(; c1->next; c1=c1->next) if(c1->next==&c) {
			c1->next = c.next;
			break;
		}
	}
	c.next = 0;
}
void Map::replaceItem(Character &c1,Character &c2) {
// app.printf("Map::placeItem(id=%lu)",c.getID());
	c1.map->removeItem(c1);
	if(c2.map) c2.map->removeItem(c2);
	c2.set(c1);
	c2.key = c2.mx+c2.my*mw;
	putItem(c2);
}
void Map::putItem(Character &c) {
	if(!map) return;
	Character *c1 = map[c.key].items;
	if(!c1 || c.py<=c1->py) c.next = c1,map[c.key].items = &c;
	else {
		while(c1->next && c.py>c1->next->py) c1 = c1->next;
		c.next = c1->next,c1->next = &c;
	}
}
Character *Map::getItem(int32_t px,int32_t py,aClass *c) {
	static const int xs[2][9] = { { 0,0,-1,0,1,-1,0,-1,0 }, { 0,1,0,0,1,-1,1,0,0 }, };
	static const int ys[9] = { 2,1,1,0,0,0,-1,-1,-2 };
	if(!map) return 0;
	int16_t xm,ym;
	Coordinate::pntToMap(px,py,xm,ym);
	Character *c1 = 0,*c2;
	for(int i=0; i<=8; i++) {
		c2 = map[getPoint(xm+xs[ym&1][i],ym+ys[i])].items;
		while(c2) {
			if(c2->contains(px,py) && (!c1 || c2->py>c1->py) && (!c || c2->instanceOf(c))) c1 = c2;
			c2 = c2->next;
		}
		if(c1) break;
	}
	return c1;
}



void Map::createTeams(uint8_t *data) {
	Team *t;
	uint32_t i,n;
	unpack_uint32(&data,n);
app.printf("Map::createTeams(n=%u)",n);
	for(i=0; i<n; i++) {
		t = new Team(&data);
		addTeam(*t);
	}
}

void Map::addTeam(Team &t) {
	teamids.put(t.getID(),(void *)&t);
}
void Map::deleteAllTeams() {
	if(!teamids.size()) return;
	Team *t;
	aHashtable::iterator iter = Map::teamids.iterate();
	while((t=(Team *)iter.next())) {
		t->disband();
		delete t;
	}
	teamids.removeAll();
}
void Map::deleteTeam(Team &t) {
#ifdef WOTCLIENT
	if(app.teamdlg->getTeam()==&t) app.teamdlg->setTeam(0);
#endif
	t.disband();
	teamids.remove(t.getID());
	delete &t;
}

Character *Map::getCharacter(int32_t px,int32_t py) { return (Character *)getItem(px,py,&Character::getClass()); }


Map::mapcolor Map::getColor(int i,int filter) {
	static mapcolor groundcolors[GROUND_TYPES][32] = {
		{		// OCEAN
			{0x00,0x00,0xd3},{0x00,0x00,0xcf},{0x00,0x00,0xcc},{0x00,0x00,0xc9},
			{0x00,0x00,0xc6},{0x00,0x00,0xc3},{0x00,0x00,0xc0},{0x00,0x00,0xbc},
			{0x00,0x00,0xd3},{0x00,0x00,0xcf},{0x00,0x00,0xcc},{0x00,0x00,0xc9},
			{0x00,0x00,0xc6},{0x00,0x00,0xc3},{0x00,0x00,0xc0},{0x00,0x00,0xbc},
			{0x00,0x69,0xcc},{0x00,0x66,0xcc},{0x00,0x63,0xcc},{0x00,0x60,0xcc},
			{0x00,0x69,0xcc},{0x00,0x66,0xcc},{0x00,0x63,0xcc},{0x00,0x60,0xcc},
			{0x00,0x69,0xcc},{0x00,0x66,0xcc},{0x00,0x63,0xcc},{0x00,0x60,0xcc},
			{0x00,0x69,0xcc},{0x00,0x66,0xcc},{0x00,0x63,0xcc},{0x00,0x60,0xcc},
		},{	// PLAIN
			{0x79,0xcc,0x39},{0x76,0xc9,0x36},{0x73,0xc6,0x33},{0x70,0xc3,0x30},
			{0x79,0xcc,0x39},{0x76,0xc9,0x36},{0x73,0xc6,0x33},{0x70,0xc3,0x30},
			{0x79,0xcc,0x39},{0x76,0xc9,0x36},{0x73,0xc6,0x33},{0x70,0xc3,0x30},
			{0x79,0xcc,0x39},{0x76,0xc9,0x36},{0x73,0xc6,0x33},{0x70,0xc3,0x30},
			{0x89,0xcc,0x39},{0x86,0xc9,0x36},{0x83,0xc6,0x33},{0x80,0xc3,0x30},
			{0x89,0xcc,0x39},{0x86,0xc9,0x36},{0x83,0xc6,0x33},{0x80,0xc3,0x30},
			{0x89,0xcc,0x39},{0x86,0xc9,0x36},{0x83,0xc6,0x33},{0x80,0xc3,0x30},
			{0x89,0xcc,0x39},{0x86,0xc9,0x36},{0x83,0xc6,0x33},{0x80,0xc3,0x30},
		},{	// DESERT
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
			{0xff,0xcc,0x99},{0xfc,0xc9,0x96},{0xf9,0xc6,0x93},{0xf6,0xc3,0x90},
		}
	};
	static mapcolor terraincolors[TERRAIN_TYPES][8] = { {{0,0,0}},
		{		// MOUNTAIN
			{0xa0,0xa0,0x90},{0xa3,0xa3,0x93},{0xa6,0xa6,0x96},{0xa9,0xa9,0x99},
			{0xc0,0xc0,0xd0},{0xc3,0xc3,0xd3},{0xc6,0xc6,0xd6},{0xc9,0xc9,0xd9},
		},{	// SWAMP
			{0xff,0xff,0xff},{0xfc,0xfc,0xfc},{0xf9,0xf9,0xf9},{0xf6,0xf6,0xf6},
			{0xf3,0xf3,0xf3},{0xdc,0xfc,0xcc},{0xd9,0xf9,0xc9},{0xd6,0xf6,0xc6},
		},{	// FOREST
			{0x00,0x8f,0x00},{0x00,0x89,0x00},{0x00,0x7f,0x00},{0x00,0x79,0x00},
		},{	// DJUNGLE
			{0x00,0x8f,0x00},{0x00,0x89,0x00},{0x00,0x7f,0x00},{0x00,0x79,0x00},
		}

	};
	static mapcolor constructcolors[CONSTRUCT_TYPES] = { {0},
		{0xff,0x99,0x00},	// ROAD
		{0xff,0x00,0x00},	// CITY
		{0x99,0x99,0x99},	// WALL
		{0x99,0x99,0x99}	// HOUSE
	};
	maplocation &t = map[i];
	if(filter==0) {
		if(t.c!=CONSTRUCT_NONE) return constructcolors[t.c];
		else if(t.t!=TERRAIN_NONE) return terraincolors[t.t][0];
		else return groundcolors[t.g][(t.ga? 16 : 0)+t.gi];
	} else if(filter==1) {
		int a = (map[i].a%0x7f)<<1;
		return (mapcolor){ (a<<6)|(a>>2),(a<<4)|(a>>4),(a<<2)|(a>>6) };
	} else if(filter==2) {
		int r = (map[i].r%0x7f)<<1;
		return (mapcolor){ (r<<6)|(r>>2),(r<<4)|(r>>4),(r<<2)|(r>>6) };
	}
	return (mapcolor){0,0,0};
}

