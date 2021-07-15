
#include "../config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL_stdinc.h>
#include <libamanita/aWord.h>
#include <libamanita/aRandom.h>
#include <libamanita/aString.h>
#include "../Main.h"
#include "../command.h"
#include "WebOfTales.h"
#include "ServerWorld.h"
#include "../game/Cloud.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Team.h"


const float OCEAN_LEVEL				= 0.33f;
const float COAST_LEVEL				= 0.0f;
const float MAGMA_LEVEL				= 2.1f;
const float TUNDRA_LEVEL			= 0.9f;
const float DESERT_LEVEL			= 1.295f;
const float FOREST_LEVEL			= 0.55f;
const float MOUNTAIN_LEVEL			= 0.75f;
const float CITY_LEVEL				= 0.5f;



ServerWorld::ServerWorld(int w,int h) : Map(w,h) {
app.printf("ServerWorld::ServerWorld(1)");
	name = 0;
	nameseed = 0;
	seaLevel = .5f;
	seaLevelMultiply = 1.0f/seaLevel;
	equator = (float)mh/2.0f;

	era = 0;
	eraPresent = 0;
	eraDescription = 0;

	world = (worldlocation *)malloc(sizeof(worldlocation)*size);
	memset(&result,0,sizeof(creationinfo));
	_map = 0;

	completed = false;
app.printf("ServerWorld::ServerWorld(2)");
}

ServerWorld::~ServerWorld() {
app.printf("ServerWorld::~ServerWorld(%p)",this);
	closeMap();
	if(world) { free(world);world = 0; }
}

void ServerWorld::create(time_t time) {
	if(cycle==0 || cycle==cycles) nextEra();
	cycle++;
	switch(era) {
		case 1:cool();break;
		case 2:
			climate(.1f);
			oscean();
			break;
		case 3:
			if(biology(.05f)==0) cycle = 0;
			break;
		case 4:
			populate();
			break;
	}
}


void ServerWorld::run(time_t time) {
	app.lock();
// app.printf("ServerWorld::run(1)\n");

	if(cloudids.size()) for(int i=cloudids.size()-1; i>=0; i--) ((Cloud *)cloudids[i])->animate();

	Character *c;
	aHashtable::iterator iter;
	if((time&1)==0) {
		Character *c;
		iter = itemids.iterate();
		while((c=(Character *)iter.next())) c->updateStatus(time);
		iter = itemids.iterate();
		while((c=(Character *)iter.next())) c->useSkill(time);
	}
	iter = itemids.iterate();
	while((c=(Character *)iter.next())) c->animate();

	/*TODO!!!
	if((time%GAME_FPS)==0) { // Every second
		spawnpoint *sp;
		uint32_t i,n = 0,t = 0;
		for(i=0; i<size; i++) {
			sp = &world[i].spawn;
			if(sp->rate>0) {
				if(sp->timer==1) n++;
				if(sp->timer>0) t++;
			}
		}
		if(n>0) {
			uint32_t l = 8+(n*sizeof(npc1));
			unsigned char buf[l];
			memcpy(buf,"SPN ",4);
			*((uint32_t *)&buf[4]) = n;
			npc1 *n1 = (npc1 *)&buf[8];
			for(i=0; i<size; i++) {
				sp = &world[i].spawn;
				if(sp->rate>0 && sp->timer==1) *n1++ = (npc1){ i%mw,i/mw,sp->index,sp->strength };
			}
			app.send((void *)buf,l);
		}
		if(t>0) for(i=0,n=0; i<size; i++) {
			sp = &world[i].spawn;
			if(sp->rate>0 && sp->timer>0) sp->timer--;
		}
	}*/
// app.printf("ServerWorld::run(2)\n");
	app.unlock();
}

void ServerWorld::sendWorldMap(aConnection client) {
app.printf("ServerWorld::sendWorldMap(name=\"%s\")",name);
	if(!_map || !_mapsz) return;
	app.send(client,_map,_mapsz);
app.printf("ServerWorld::sendWorldMap(client=%p,clientid=%d,_mapsz=%d)",client,client? client->getID() : 0,_mapsz);
}

void ServerWorld::sendClouds(aConnection client) {
	uint32_t i,sz = cloudids.size();
	uint8_t d[sizeof_sendheader+4+sz*sizeof_cloud],*p = d;
	packSendHeader(&p,SND_CLOUDS);
	pack_uint32(&p,sz);
	for(i=0; i<sz; i++) ((Cloud *)cloudids[i])->pack(&p);
	app.send(client,d,sizeof(d));
app.printf("ServerWorld::sendClounds(sz=%d,sizeof(d)=%d)\n",sz,sizeof(d));
}

void ServerWorld::sendItems(aConnection client) {
	aHashtable::iterator iter = itemids.iterate();
	Character *c;
	uint16_t npn = 0,pln = 0,nml = 0;
	while((c=(Character *)iter.next()))
		if(c->instanceOf(NPC::getClass())) npn++;
		else if(c->instanceOf(Player::getClass())) pln++,nml += c->getNameLength()+1;
	uint8_t d[sizeof_sendheader+4+sizeof_npc*npn+sizeof_player*pln+nml],*p = d;
	packSendHeader(&p,SND_ITEMS);
	pack_uint16(&p,npn);
	pack_uint16(&p,pln);
	if(npn>0) for(iter=itemids.iterate(); (c=(Character *)iter.next()); )
		if(c->instanceOf(NPC::getClass())) ((NPC *)c)->pack(&p);
	if(pln>0) for(iter=itemids.iterate(); (c=(Character *)iter.next()); )
		if(c->instanceOf(Player::getClass())) ((Player *)c)->pack(&p);
	app.send(client,d,sizeof(d));
app.printf("ServerWorld::sendItems(npn=%d,pln=%d,sizeof(d)=%d)\n",npn,pln,sizeof(d));
}

void ServerWorld::sendTeams(aConnection client) {
	Team *t;
	int i,n = 0;
	if(teamids.size()) {
		aHashtable::iterator iter = teamids.iterate();
		while((t=(Team *)iter.next())) n += sizeof_team+t->members()*4;
	}
	uint8_t d[sizeof_sendheader+4+n],*p = d;
	packSendHeader(&p,SND_TEAMS);
	pack_uint32(&p,teamids.size());
	if(teamids.size()) {
		aHashtable::iterator iter = teamids.iterate();
		while((t=(Team *)iter.next())) {
			pack_uint32(&p,t->getID());
			pack_uint16(&p,t->members());
			for(i=t->members()-1; i>=0; i--) {
				pack_uint32(&p,t->getMember(i)->getID());
app.printf("ServerWorld::sendTeams(member%d.id=%lu)\n",i,swap_be_32(*(uint32_t *)(p-4)));
			}
		}
	}
	app.send(client,d,sizeof(d));
app.printf("ServerWorld::sendTeams(n=%d,sizeof(d)=%d)\n",n,sizeof(d));
}


void ServerWorld::sendNPC(aConnection client,NPC &np,int type) {
	int n = type==1? sizeof_npc : np.getNPCSize();
	uint8_t d[sizeof_sendheader+n],*p = d;
	packSendHeader(&p,SND_NPC,type);
	if(type==1) np.pack(&p);
	else np.packNPC(&p);
	if(client) app.send(client,d,sizeof(d));
	else app.send(d,sizeof(d));
}

bool ServerWorld::uploadPlayer(Player &pl) {
app.printf("ServerWorld::uploadPlayer(id=%d,name=%s)",pl.getID(),pl.getName());
	aString data;
	pl.getPlayerFile(data);
	aHttp http(app);
	http.setFormValue("c","save");
	http.setFormValue("u","%s",getProperty("user"));
	http.setFormValue("p","%s",getProperty("password"));
	http.setFormValue("cid","%" PRIu32,pl.getID());
	http.setFormValue("nm","%s",pl.getName());
	http.setFormValue("data","%s",data.toCharArray());
	const char *file = http.post(WEB_HOST,WEB_PATH "char.php");
	char fn[128];
	sprintf(fn,"%sserver/char_%" PRIu32 ".txt",app.getHomeDir(),pl.getID());
	FILE *fp = fopen(fn,"w");
	if(fp==0) return false;
	size_t i = fwrite(data.toCharArray(),data.length(),1,fp);
	fclose(fp);
	return true;
}

bool ServerWorld::downloadPlayer(Player &pl) {
	aHttp http(app);
	http.setFormValue("u","%s",getProperty("user"));
	http.setFormValue("p","%s",getProperty("password"));
	http.setFormValue("cid","%" PRIu32,pl.getID());
	const char *file = http.post(WEB_HOST,WEB_PATH "char.php");
app.printf("ServerWorld::downloadPlayer()\n%s",file);
	player_data *pd = Player::parsePlayerFile(file);
	if(pd) {
		pl.setPlayerData(pd);
		Player::deletePlayerData(pd);
	}
	return true;
}

void ServerWorld::sendPlayer(aConnection client,Player &pl,int type) {
app.printf("ServerWorld::sendPlayer(type=%d,id=%d,name=%s)",type,pl.getID(),pl.getName());
	int n = type==1? sizeof_player+pl.getNameLength()+1 : pl.getPlayerSize();
	uint8_t d[sizeof_sendheader+n],*p = d;
	packSendHeader(&p,SND_PLAYER,type);
	if(type==1) pl.pack(&p);
	else pl.packPlayer(&p);
	if(client) app.send(client,d,sizeof(d));
	else app.send(d,sizeof(d));
app.printf("ServerWorld::sendPlayer(n=%d,sizeof(d)=%d)",n,sizeof(d));
}



void ServerWorld::writeMap() {
app.consolef(TXT_BLACK,"\n%s",app.get("srvwrld_write_map"));
	int lm = 0,mnt = 0,fst = 0,cit = 0;
	size_t i;
	for(i=0; i<size; i++) {
		worldlocation &p = world[i];
		maplocation &t = map[i];
		t.g = OCEAN,t.t = TERRAIN_NONE,t.c = CONSTRUCT_NONE;
		if(p.sea>COAST_LEVEL) continue;
		if(p.temperature>DESERT_LEVEL) t.g = DESERT;
		else t.g = PLAIN;
	}
	for(i=0; i<size; i++) {
		worldlocation &p = world[i];
		maplocation &t = map[i];
		if(t.g==OCEAN) continue;
		maplocation **l = (maplocation **)getIsoArea(i%mw,i/mw,2,map,sizeof(maplocation));
		if(p.population>=CITY_LEVEL) t.c = CITY;
		else {
			if(l[1] && l[1]->g!=OCEAN && l[2] && l[2]->g!=OCEAN &&
					l[3] && l[3]->g!=OCEAN && l[4] && l[4]->g!=OCEAN) {
				if(p.altitude>MOUNTAIN_LEVEL) t.t = MOUNTAIN;
			}
			if(t.t==TERRAIN_NONE) {
				if(p.vegetation>FOREST_LEVEL) t.g = PLAIN,t.t = FOREST;
			}
		}
	}
	for(i=0; i<size; i++) {
		maplocation &t = map[i];
		if(t.g==OCEAN) continue;
		lm++;
		if(t.c==CITY) cit++;
		else if(t.t==MOUNTAIN) mnt++;
		else if(t.t==FOREST) fst++;
	}
	result.landmass = (float)((double)lm/(double)size);
	result.mountains = (float)((double)mnt/(double)lm);
	result.forest = (float)((double)fst/(double)lm);

	aRandom r(seed);
	aWord w;
	w.setSeed(id);
app.consolef(TXT_BLACK,app.get("srvwrld_make_clds"));
	makeClouds(300);
//consolef(TXT_BLACK,"Create Countries");
//	makeCountries();
//	removeRegions();
app.consolef(TXT_BLACK,app.get("srvwrld_make_ter"));
	makeTerrain(r);
app.consolef(TXT_BLACK,app.get("srvwrld_make_areas"));
	makeAreas(w);
app.consolef(TXT_BLACK,app.get("srvwrld_make_cities"));
	makeCities(r,w);
	makeRegions(w);
app.consolef(TXT_BLACK,app.get("srvwrld_create_spwn"));
	createSpawnPoints();
app.consolef(TXT_BLACK,"%s\n",app.get("srvwrld_map_finished"));
}

void ServerWorld::openMap() {
	if(!name || *name=='\0') return;
	if(_map) free(_map);
	int n = strlen(name)+1;
	_mapsz = sizeof_sendheader+sizeof_mapheader+n+size*2;
	_map = (uint8_t *)malloc(_mapsz);
	uint8_t *p = _map;
	packSendHeader(&p,SND_WORLDMAP,0);
	pack_uint32(&p,ver);
	pack_uint32(&p,id);
	pack_uint16(&p,mw);
	pack_uint16(&p,mh);
	pack_uint8(&p,wrap);
	memcpy(p,name,n);
	p += n;
	maplocation *l = map;
	for(size_t i=0; i<size; i++,l++) pack_uint16(&p,l->g|(l->t<<4)|(l->c<<8));
/*	if(!name || !*name) return;
	char *path = SERVER_PATH;
	char file[64];
app.printf("ServerWorld::loadWorld(saveMap)...");
	sprintf(file,"%smap.dat",path);
	saveMap(file);*/
}

void ServerWorld::closeMap() {
	if(_map) free(_map);
	_map = 0,_mapsz = 0;
/*	if(!name || !*name) return;
	char *path = SERVER_PATH;
	char file[64];
	sprintf(file,"%smap.dat",path);
	::remove(file);*/
}

/*
enum {
	NPC_GOBLIN,
	NPC_GOBLIN_WARRIOR,
	NPC_GOBLIN_SHAMAN,
	NPC_GOBLIN_CHEIFTAIN,
	NPC_WOLF_CUB,
	NPC_WOLF,
	NPC_MONSTERS,
};
struct spawnpoint {
	unsigned char rate;
	unsigned char index;
	unsigned char strength;
	unsigned char timer;
};
*/
void ServerWorld::createSpawnPoints() {
	/*unsigned long monsters[7][1+NPC_MONSTERS] = {
		{   1, 0, 0, 0, 0, 0, 0 },	// 0. OCEAN
		{ 100,10, 5, 3, 2,15, 5 },	// 1. PLAIN
		{ 100,10, 5, 3, 2,15, 5 },	// 2. DESERT
		{ 100,10, 5, 3, 2,15, 5 },	// 3. MOUNTAIN
		{ 100,10, 5, 3, 2,15, 5 },	// 4. SWAMP
		{ 100,10, 5, 3, 2,15, 5 },	// 5. FOREST
		{ 100,10, 5, 3, 2,15, 5 },	// 6. DJUNGLE
	};*/
	static const int distribution[7] = { 20,30,20,20,20,50,50 };
	int sum[7],monsters[7][1+NPC_MONSTERS];
	int n,t;
	size_t i;
	aRandom r(seed);

app.printf("ServerWorld::createSpawnPoints(1)");
	for(i=0; i<7; i++) {
		for(n=0; n<NPC_MONSTERS; n++) monsters[i][1+n] = NPC::npcdata[n].distribution[i];
		sum[i] = aMath::sum(&monsters[i][1],NPC_MONSTERS);
		monsters[i][0] = (100*sum[i])/distribution[i]-sum[i],sum[i] += monsters[i][0];
	}
	for(i=0,n=0; i<size; i++) {
		maplocation &ml = map[i];
		if(ml.g!=OCEAN && ml.c==CONSTRUCT_NONE) {
			if(ml.t!=TERRAIN_NONE) t = DESERT+ml.t;
			else t = ml.g;
			t = r.rollTable(monsters[t],sum[t]);
			if(t>0) { world[i].spawn = (spawnpoint){ 60,t-1,1,0 },n++; }
		}
	}
app.printf("ServerWorld::createSpawnPoints(2,n=%d)",n);

	uint8_t d[4+n*sizeof_npc],*p = d;
app.printf("ServerWorld::createSpawnPoints(2.1,n=%d)",n);
	pack_uint16(&p,n);
	pack_uint16(&p,0);
app.printf("ServerWorld::createSpawnPoints(2.2,n=%d)",n);
	for(i=0; i<size; i++) if(world[i].spawn.rate>0) {
		pack_uint16(&p,world[i].spawn.index);
		pack_uint16(&p,i%mw);
		pack_uint16(&p,i/mw);
		pack_uint32(&p,0);
	}
app.printf("ServerWorld::createSpawnPoints(2.3,n=%d)",n);
	createItems(d);
app.printf("ServerWorld::createSpawnPoints(3,n=%d,sizeof(d)=%d)",n,sizeof(d));
}


bool ServerWorld::saveWorld() {
app.printf("\nServerWorld::saveWorld(%s)",name);
	if(name==NULL || *name=='\0') return false;
	char file[64];
	size_t i;
	uint16_t l = strlen(name)+1;
	FILE *fp;

	sprintf(file,"%sserver/worlds/%s.dat",app.getHomeDir(),name);
	if(!(fp=fopen(file,"wb"))) return false;

	mapheader h = { ver,id,mw,mh,wrap };
	i = fwrite(&h,sizeof(mapheader),1,fp);
	i = fwrite(&l,sizeof(l),1,fp);
	i = fwrite(name,l,1,fp);

	i = fwrite(world,sizeof(worldlocation)*size,1,fp);
	fclose(fp);

	openMap();

	completed = true;
app.printf("ServerWorld::saveWorld() done\n");
	return true;
}

bool ServerWorld::loadWorld(const char *nm) {
app.printf("\nServerWorld::loadWorld(%s)",nm);
	if(nm==NULL || *nm=='\0') return false;
	char file[64];
	size_t i;
	uint16_t l;
	FILE *fp;

	completed = false;
	sprintf(file,"%sserver/worlds/%s.dat",app.getHomeDir(),nm);
	if(!(fp=fopen(file,"rb"))) return false;

	mapheader h;
	i = fread(&h,sizeof(mapheader),1,fp);
	id = h.id;
	wrap = (WRAP)h.wrap;
	if(h.w!=mw || h.h!=mh) {
		createMap(h.w,h.h);
		free(world);
		world = (worldlocation *)malloc(sizeof(worldlocation)*size);
	}
	i = fread(&l,sizeof(l),1,fp);
	char n[l];
	i = fread(n,l,1,fp);
	setName(n);
	i = fread(world,sizeof(worldlocation)*size,1,fp);
	fclose(fp);

	writeMap();
	openMap();

	completed = true;
app.printf("ServerWorld::loadWorld() done\n");
	return true;
}


bool ServerWorld::createWorld(const char *nm,int con,int lnd,int mnt,int isl,int fst,int ppl,int tec) {
app.printf("ServerWorld::createWorld(1)");
	size_t i,n;
	int x,x1,y1,w;
	float sz;
	cycle = 0;
	era = 0;
	eraPresent = 0;
	eraDescription = 0;
	ci = (creationinfo){
		con,
		(float)lnd/100.0f,
		(float)mnt/100.0f,
		(float)isl/100.0f,
		(float)fst/100.0f,
		(float)ppl/100.0f,
		(float)tec/100.0f
	};

	setName(nm);
	aRandom r(seed);

	deleteAllItems();
	deleteAllTeams();

app.printf("ServerWorld::createWorld(2)");
	memset(world,0,sizeof(worldlocation)*size);

app.printf("ServerWorld::createWorld(3)");
	for(i=1,n=ci.continents,x=r.uint32(mw),w=mw/n; i<=n; i++,x+=w) {
		x1 = x+r.uint32(w),y1 = mh/n+r.uint32(mh-mh*2/n);
		if(x1>=mw) x1 -= mw;
		sz = (float)n/20.0f;
		sz = (1.0f-sz)+r.real64()*sz*2.0f;
		sz = (.1f+ci.landmass*1.0f+.5f*(n/5.0f))*w*(1.5f-ci.mountains*.7f)*sz;
app.printf("ServerWorld::createWorld(x=%d,x1=%d,y1=%d,sz=%f)",x,x1,y1,sz);
		impact(x1,y1,TEMPERATURE,sz,.9f,false);
	}
app.printf("ServerWorld::createWorld(4)");
	for(i=1,n=50; i<=n; i++)
		impact(r.uint32(mw),r.uint32(mh),TEMPERATURE,
			(mw/20+r.uint32(mw/10))*(.5f+ci.mountains),r.real64()*ci.mountains*.6f-.2f,true);
app.printf("ServerWorld::createWorld(5)");

	for(i=0,sz=ci.islands; i<size; i++) {
		worldlocation &p = world[i];
		p.temperature += r.real64()*sz*2.0f-sz;
		p.temperature = 3.0f-p.temperature;
		if(p.temperature<2.0f) p.temperature = 2.0f+r.real64()*.2f;
		else if(p.temperature>3.0f) p.temperature = 3.0f-r.real64()*.1f;
		p.altitude = 1.0f;
	}
app.printf("ServerWorld::createWorld(6)");
	completed = false;
	return true;
}

void ServerWorld::nextEra() {
	switch(++era) {
		case 1:
			cycles = 1000;
//			for(int i=0; i<size; i++) world[i].sea = -1.0f;
			break;
		case 2:
			cycles = 100;
			break;
		case 3:
		{
			cycles = 100;
			float t,c;
			for(size_t i=0; i<size; i++) {
				worldlocation &p = world[i];

				t = belowSeaLevel(p.altitude);
				if(t>0) t = pow(1.0f-(t/seaLevel),2);
				else t = pow(1.0f-(fabs(t)/(1.0f-seaLevel)),2);

				c = p.temperature>=.7f? (p.temperature-.7f)/.67f : .0f;
				if(c>1.0f) c -= (c-1.0f)*3.0f;

				p.environment = t*c;
				if(p.environment<.0f) p.environment = .0f;
			}
			break;
		}
		case 4:
		{
			cycles = 30+(int)(ci.cities*10.0f);
//			worldlocation **a;
//			int i,j,sz = sizeof(worldlocation);
//			float l;
//			for(i=0,j; i<size; i++) {
//				worldlocation &p = world[i];
//				a = (worldlocation **)getArea(i%mw,i/mw,4,world,sz);
//				for(j=0,l=.0f; j<9; j++) if(a[j]) l += a[j]->life*(j==0? 1.0f : (j<5? .3f : (j<10? .1f : .03f)));
//				p.population = l;
//			}
			break;
		}
		case 5:
			completed = true;
			break;
	}
	cycle = 0;
}



void ServerWorld::cool() {
	float change = .01f;
	int e = 0;
	for(size_t i=0; i<size; i++) {
		worldlocation &p = world[i];
		if(p.temperature>2.0f) {
			p.temperature -= change;
			p.altitude -= change;
			e++;
		}
		erosion(i,TEMPERATURE,.005f);
	}
	if(e==0) cycle = 0;
}


void ServerWorld::climate(float change) {
	float lat;
	int x,y,i;
	for(y=0; y<mh; y++) {
		lat = 1.0f-pow(fabs(equator-y)/equator,3);
		for(x=0; x<mw; x++) {
			worldlocation &p = world[i=x+y*mw];
			if(p.sea<.05f) {
				if(p.temperature>(.8f+.5f*(/*((1.0f-p.altitude)/(1.0f-seaLevel))**/lat))) p.temperature -= change;
			} else {
				if(p.temperature>(.8f+.2f*(p.altitude/seaLevel)+.2f*lat)) p.temperature -= change;
				if(p.altitude<.0f) p.temperature += change;
			}
			erosion(i,TEMPERATURE,p.sea>.05? .05f : .005f);
			if(p.altitude+p.sea>=seaLevel) p.sea = seaLevel-p.altitude;
			else p.sea += .01f;
		}
	}
}

void ServerWorld::oscean() {
	worldlocation **a;
	float as1,as2,*s1,s2;
	size_t j,sz = sizeof(worldlocation);
	for(size_t i=0; i<size; i++) {
		worldlocation &p = world[i];
		erosion(i,ALTITUDE,(p.sea>.0f? (1.0f-p.sea)*.001f :
			(p.temperature<.8f? .1f*(1.0f-p.temperature) : .0005f*p.temperature)));
		as1 = p.altitude+p.sea;
		a = (worldlocation **)getArea(i%mw,i/mw,2,world,sz);
		for(j=1; j<9; j++) if(a[j]) {
			s1 = &a[j]->sea,as2 = a[j]->altitude+*s1;
			if(as2>as1) {
				s2 = (as2-as1)/2.0f;
				if(s2>*s1) s2 = *s1;
				p.sea += s2,*s1 -=s2;
			}
		}
	}
}


int ServerWorld::biology(float change) {
	int e = 0;
	for(size_t i=0; i<size; i++) {
		worldlocation &p = world[i];
		if(p.temperature<DESERT_LEVEL || p.vegetation>.1f) {
			if(p.life>=p.environment) p.life = p.environment;
			else p.life += p.environment*change,e++;
			if(p.life>p.vegetation) p.vegetation += p.life*(ci.forest*2.0f)*.2;
			erosion(i,VEGETATION,.001f);
		}
	}
	return e;
}


void ServerWorld::populate() {
	worldlocation **a,*n;
	size_t j,sz = sizeof(worldlocation);
	float l,v;
	biology(.005f);
	for(size_t i=0; i<size; i++) {
		worldlocation &p = world[i];
		a = (worldlocation **)getArea(i%mw,i/mw,4,world,sz);
		for(j=0,l=.0f; j<9; j++) if(a[j]) l += a[j]->life*(j==0? 1.0f : (j<5? .3f : .1f));
		if(p.population>l) p.population = l;
		else {
			p.population += l*.001f;
			for(j=1; j<21; j++) if((n=a[j]) && n->population>.0f && p.population>n->population)
					v = (j<5? .005f : (j<10? .003f : .001f)),p.population += v,n->population -= v;
		}
		for(j=0; j<9; j++) if(a[j]) {
			v = (j==0? .01f : (j<5? .003f : .001f));
			a[j]->life -= p.population*v;
			a[j]->vegetation -= p.population*v*.01f;
		}
	}
}


void ServerWorld::impact(int x,int y,WORLDLOCATION member,float radius,float value,bool add) {
	int n,s = aMath::round(radius);
	float d;
	float *p,v;
	for(int i=x-s; i<x+s*2; i++)
		for(int j=y-s*2; j<y+s*4; j++)
			if(((wrap&WRAP_HORIZ) || (i>=0 && i<mw)) && ((wrap&WRAP_VERT) || (j>=0 && j<mh))) {
				d = aMath::distance((long)(x+(mw<<1)),(y>>1)+(mh<<1),i+(mw<<1),(j>>1)+(mh<<1));
				if(d<=radius) {
					n = (i<0? mw+i : (i>=mw? i-mw : i))+(j<0? mh+j : (j>=mh? j-mh : j))*mw;
					p = (float *)(((char *)&world[n])+member);
					v = (float)(1.-(d/radius))*value;
					if(add) *p += v;
					else if(*p<v) *p = v;
				}
			}
}

void ServerWorld::erosion(int i,WORLDLOCATION member,float change) {
	worldlocation **a = (worldlocation **)getArea(i%mw,i/mw,2,world,sizeof(worldlocation));
	float n,*n1 = (float *)(((char *)a[0])+member),*n2;
	for(int j=1; j<9; j++) if(a[j]) {
		n2 = (float *)(((char *)a[j])+member);
		if(*n2<=.0f) continue;
		n = (((*n2+1.0f)/(*n1+1.0f))-1.0f)*change;
		*n1 += n,*n2 -= n;
	}
}


const char *ServerWorld::getEraDescription(int era) {
	if(era<1 || era>5) era = 1;
	if(era!=eraPresent) {
		eraPresent = era;
		eraDescription = app.getf("srvwrld_era_%d",era);
	}
	return eraDescription;
}


Map::mapcolor ServerWorld::getColor(int i,int filter) {
	if(filter>2) return Map::getColor(i,filter-3);
	worldlocation &p = world[i];
	if(filter==1) return irColor(p.temperature);
	else if(filter==2) return environmentColor(p.environment);
	else {
		if(p.temperature>MAGMA_LEVEL) return magmaColor(adjustValue(p.temperature-2.0f));
		else if(p.sea>COAST_LEVEL) {
			/*if(filter==3 && p.population>CITY_LEVEL) return cityColor(adjustValue(p.population-CITY_LEVEL));
			else if(filter==4 && p.life>.5f) return planctonColor(adjustValue((p.life-.5f)*2.0f));
			else if(filter==5) return mountainColor(adjustValue((p.altitude)));
			else*/ return osceanColor(adjustValue(p.sea));
		} else {
			if(p.population>CITY_LEVEL) return cityColor(adjustValue(p.population-CITY_LEVEL));
			else if(p.vegetation>FOREST_LEVEL) return vegetationColor(adjustValue((p.vegetation-.5f)*2.0f));
			else if(p.life>FOREST_LEVEL) return vegetationColor(adjustValue((p.life-.5f)*2.0f));
			else if(p.altitude>MOUNTAIN_LEVEL) return mountainColor(adjustValue((p.altitude-seaLevel)*seaLevelMultiply));
			else return desertColor(adjustValue((p.altitude-seaLevel)*seaLevelMultiply));
		}
	}
	return (mapcolor){0,0,0,0};
}


inline Map::mapcolor ServerWorld::environmentColor(float c) { return c>=0.99f? (mapcolor){0xff,0xff,0xff} : (mapcolor){0xff,(int)(221.0f*c),0}; }
inline Map::mapcolor ServerWorld::magmaColor(float c) { return (mapcolor){0xff,(int)(221.0f*c),0}; }
inline Map::mapcolor ServerWorld::glacierColor(float c) { return (mapcolor){0xff-(int)(153.0f*c),0xff-(int)(102.0f*c),0xff}; }
inline Map::mapcolor ServerWorld::mountainColor(float c) { return (mapcolor){(int)(255.0f*c),(int)(221.0f*c),(int)(187.0f*c)}; }
inline Map::mapcolor ServerWorld::osceanColor(float c) { return (mapcolor){0,0xcc-(int)(204.0f*c),0xff-(int)(204.0f*c)}; }
inline Map::mapcolor ServerWorld::desertColor(float c) { return (mapcolor){0xcc+(int)(51.0f*c),0xcc+(int)(51.0f*c),0x33+(int)(153.0f*c)}; }
inline Map::mapcolor ServerWorld::planctonColor(float c) { return (mapcolor){0,0xff-(int)(204.0f*c),0xcc-(int)(204.0f*c)}; }
inline Map::mapcolor ServerWorld::vegetationColor(float c) { return (mapcolor){0x33-(int)(51.0f*c),0xcc-(int)(102.0f*c),0x33-(int)(51.0f*c)}; }
inline Map::mapcolor ServerWorld::cityColor(float c) { return (mapcolor){0x99+(int)(102.0f*c),0,0}; }



