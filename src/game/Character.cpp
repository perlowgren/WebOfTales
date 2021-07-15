
#include "../config.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <libamanita/aMath.h>
#include "../Main.h"

#ifdef WOTSERVER
	#include <libamanita/aRandom.h>
	#include <libamanita/net/aServer.h>
	#include "../server/WebOfTales.h"
#elif defined WOTCLIENT
	#include <libamanita/net/aClient.h>
	#include <libamanita/sdl/aGraphics.h>
	#include <libamanita/sdl/aFont.h>
	#include "../client/WoTClient.h"
	#include "../client/IncarnateDlg.h"
	#include "../client/BattleDlg.h"
	#include "../client/CityDisplay.h"
#endif

#include "Map.h"
#include "Player.h"
#include "NPC.h"
#include "Item.h"
#include "City.h"



aObject_Inheritance(Character,Coordinate);


enum {
	CYCLES_RESTING		= GAME_FPS*3,
	CYCLES_ACTIVE		= GAME_FPS*7,
	CYCLES_ENEMIES		= GAME_FPS,
};


const sprite Character::sprites[] = {
	{ 1,35,60,18,54,9,5,26,55,{ 0 },							{ -7,-7,1 } },			// Human Stand
	{ 8,35,60,18,54,9,5,26,55,{ 0,1,2,1,0,3,4,3 },		{ -7,-7,1 } },			// Human Walk

	{ 1,45,29,23,22,0,0,45,29,{ 0 },							{ -10,-5,2 } },		// Wolf
	{ 1,45,25,26,10,0,0,45,25,{ 1 },							{ 0,0,0 } },			// Wolf Dead

	{ 28,48,36,23,31,12,0,31,36,
		{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3 },
			{ -7,-7,1 } },																		// Goblin Stand
	{ 9,48,36,23,31,12,0,31,36,{ 2,2,2,2,3,3,4,4,4 },	{ -7,-7,1 } },			// Goblin Battle
	{ 1,38,35,19,22,0,12,38,35,{ 5 },						{ 0,0,0 } },			// Goblin Dead
};


Character::Character(Map *m,uint32_t id) : Coordinate(m),id(id),name(0),namelen(0) {
	key = 0,next = 0,spri = 0,itm = 0,nitm = 0,loc = 0;
	nskills = 0,skills = 0,sk = 0,attack = 0,defend = 0,neff = 0,eff = 0;
	sprtm = 0,icon = 1,xp = 0,lvl = 0,st = 0;
	act = ACT_STAND,dir = SE,timer = 0,target = 0,trail = 0,nenms = 0,enms = 0;
}

Character::~Character() {
	if(name) { free(name);name = 0; }
	if(itm) { free(itm);itm = 0; }
	if(skills) { free(skills);nskills = 0,skills = 0; }
	if(eff) { free(eff);neff = 0,eff = 0; }
	if(trail) { delete trail;trail = 0; }
	if(enms) { free(enms);nenms = 0,enms = 0; }
	loc = 0;
}


void Character::setName(const char *nm) {
	if(name) { free(name);name = 0,namelen = 0; }
	if(nm && *nm) name = strdup(nm),namelen = strlen(name);
}

#ifdef WOTCLIENT
int Character::getColour(Character &c) {
	static const int cols[] = { COL_GREY,COL_GREEN,COL_YELLOW,COL_WHITE,COL_BLUE,COL_MAGENTA,COL_RED,COL_BLACK };
	int i = 3+(getLevel()-c.getLevel())/(1+c.getLevel()/10);
	return cols[i<0? 0 : (i>7? 7 : i)];
}

void Character::paint(viewpoint &v) {
	const sprite &sp = sprites[spri];
	int xp = px-v.px,yp = py-v.py;
	if(xp<0) xp += map->pw;else if(xp>=map->pw) xp -= map->pw;
	if(yp<0) yp += map->ph;else if(yp>=map->ph) yp -= map->ph;
	if(sp.shadow.shape>0) app.shadows->draw(xp+sp.shadow.x,yp+sp.shadow.y,sp.shadow.shape);
	xp -= sp.basex,yp -= sp.basey;
	app.npcspr[0]->draw(xp,yp,1+sp.frames[(app.time.frames+mx+my)%sp.length]);
	if(this==app.getHover() || this==app.getFocus()) {
		aFont *f = app.fonts[FONT_WHITE+getColour(*app.getPlayer())];
		f->print(xp+sp.basex-f->stringWidth(getName())/2,yp+sp.t-1,getName());
	}
}
#endif


void Character::createItem(const item &i) {
app.printf("Character::createItem(index=%d,nitm=%d,itm=%p)",i.index,nitm,itm);
	item *it = itm;
	itm = (item *)malloc(sizeof(item)*(nitm+1));
	if(it) {
		memcpy(itm,it,sizeof(item)*nitm);
		free(it);
	}
	itm[nitm].index = i.index;
	itm[nitm].qnt = i.qnt;
	itm[nitm].dura = itemdata[i.index].dura;
	itm[nitm].cool = 0;
	nitm++;
}

void Character::createItems(const item *i,int n) {
app.printf("Character::createItems(n=%d,nitm=%d)",n,nitm);
	if(!i || n<=0) return;
	item *it = itm;
	itm = (item *)malloc(sizeof(item)*(nitm+n));
	if(it) {
		memcpy(itm,it,sizeof(item)*nitm);
		free(it);
	}
	nitm += n;
	for(item *i1=itm; n>0; n--,i++,i1++) {
		i1->index = i->index;
		i1->qnt = i->qnt;
		i1->dura = itemdata[i1->index].dura;
		i1->cool = 0;
	}
}

void Character::addItem(const item &i) {
app.printf("Character::addItem(index=%d,nitm=%d,itm=%p)",i.index,nitm,itm);
	item *it = itm;
	itm = (item *)malloc(sizeof(item)*(nitm+1));
	if(it) {
		memcpy(itm,it,sizeof(item)*nitm);
		free(it);
	}
	itm[nitm++] = i;
}

void Character::addItems(const item *i,int n) {
app.printf("Character::addItems(n=%d,nitm=%d)",n,nitm);
	if(!i || n==0) return;
	item *it = itm;
	itm = (item *)malloc(sizeof(item)*(nitm+n));
	if(it) {
		memcpy(itm,it,sizeof(item)*nitm);
		free(it);
	}
	memcpy(&itm[nitm],i,sizeof(item)*n);
	nitm += n;
}

void Character::swapItems(int i1,int i2) {
	if(i1==i2 || i1<0 || i1>=nitm || i2<0 || i2>=nitm) return;
	item i = itm[i1];
	itm[i1] = itm[i2],itm[i2] = i;
}

void Character::deleteItem(int i) {
	if(i<0 || i>=nitm) return;
	if(nitm==1) {
		free(itm);
		itm = 0,nitm = 0;
	} else {
		item *it = itm;
		itm = (item *)malloc(sizeof(item)*(nitm-1));
		if(i>0) memcpy(itm,it,sizeof(item)*i);
		if(i<nitm-1) memcpy(&itm[i],&it[i+1],sizeof(item)*(nitm-1-i));
		free(it);
		nitm--;
	}
}

void Character::deleteItems() {
	if(itm) free(itm);
	itm = 0,nitm = 0;
}


void Character::addSkill(skill &s) {
	if(skills) for(int i=0; i<nskills; i++) if(skills[i].index==s.index) return;
	skill *sk = skills;
	skills = (skill *)malloc(sizeof(skill)*(nskills+1));
	if(sk) {
		memcpy(skills,sk,sizeof(skill)*nskills);
		free(sk);
	}
	skills[nskills++] = s;
}

void Character::setLocation(Location *l) {
	loc = l;
	if(loc) {
		setMap(loc->mx,loc->my);
		if(map) map->removeItem(*this);
	} else {
		if(map) map->placeItem(*this);
	}
}

bool Character::contains(int x2,int y2) {
	const sprite &s = sprites[spri];
	int x1 = px-s.basex,y1 = py-s.basey;
	return contains(x1+s.l,y1+s.t,x1+s.r,y1+s.b,x2,y2);
}

bool Character::intersects(Character &c) {
	const sprite &s1 = sprites[spri],&s2 = sprites[c.spri];
	int x1 = px-s1.basex,y1 = py-s1.basey,x2 = c.px-s2.basex,y2 = c.py-s2.basey;
	return intersects(x1+s1.l,y1+s1.t,x1+s1.r,y1+s1.b,x2+s2.l,y2+s2.t,x2+s2.r,y2+s2.b);
}

bool Character::intersects(int l,int t,int r,int b) {
	const sprite &s = sprites[spri];
	int x1 = px-s.basex,y1 = py-s.basey;
	return intersects(x1+s.l,y1+s.t,x1+s.r,y1+s.b,l,t,r,b);
}


void Character::setAction(uint8_t a,uint32_t p1,uint32_t p2,uint32_t p3) {
app.printf("Character::setAction(action=%d,id=%d,x=%d,y=%d,ap.p1=%d,ap.p2=%d,ap.p3=%d)",
			act,getID(),px,py,p1,p2,p3);
#ifdef WOTSERVER
	switch(a) {
		case ACT_MOVE:
			if(!moveTo(p3,p2>>16,p2&0xffff)) stand();
			else {
				act = ACT_MOVE;
				sendActionPack(SEND_TO_ALL,ACT_MOVE,(px<<16)|py,(target->px<<16)|target->py,p3);
			}
			break;

		case ACT_TELEPORT:
			//Location *l = ap.p1? Map::getLocation(ap.p1) : 0;
			setMap(p2>>16,p2&0xffff);
			if(map) map->placeItem(*this);
			act = ACT_STAND;
			sendActionPack(SEND_TO_ALL,ACT_STAND,p1,p2,p3);
			break;

		case ACT_BATTLE:
		case ACT_TALK:
			target = Map::getItem(p3);
		case ACT_STAND:
			act = a;
			sendActionPack(SEND_TO_ALL,act,p1,p2,p3);
			break;

		case ACT_SKILL:
			pushSkillQue(p1);
			break;

		case ACT_SUICIDE:
			target = 0;
			kill();
			act = a;
			sendActionPack(SEND_TO_ALL,act,p1,p2,p3);
			break;
	}
#elif defined WOTCLIENT
	switch(a) {
		case ACT_MOVE:
			setPoint(p1>>16,p1&0xffff);
			if(moveTo(p3,p2>>16,p2&0xffff)) {
				if(act!=a) act = ACT_MOVE,spri = spr[1],sprtm = app.time.frames%sprites[spri].length;
			} else act = ACT_STAND,spri = spr[0];
			break;

		case ACT_TELEPORT:
			setMap(p2>>16,p2&0xffff);
			if(map) map->placeItem(*this);
			act = ACT_STAND;
			break;

		case ACT_STAND:
			setPoint(p1>>16,p1&0xffff);
			act = a,spri = spr[0],target = 0;
			break;

		case ACT_BATTLE:
			setPoint(p1>>16,p1&0xffff);
			act = a,spri = spr[2],target = Map::getItem(p3);
			if(app.getPlayer() && target) {
				if(this==app.getPlayer()) app.btldlg->setOpponents(this,(Character *)target);
				else if(target==app.getPlayer()) app.btldlg->setOpponents((Player *)target,this);
			}
			break;

		case ACT_TALK:
			setPoint(p1>>16,p1&0xffff);
			act = a,spri = spr[2],target = Map::getItem(p3);
			break;

		case ACT_SUICIDE:
			target = 0;
			kill();
			break;
	}
#endif
}


bool Character::moveTo(uint32_t id,int32_t x,int32_t y) {
app.printf("Character::moveTo(px=%d,py=%d,id=%lu,x=%d,y=%d)",px,py,id,x,y);
	target = id? Map::getItem(id) : 0;
	if(target && target!=this) dest.set(*target);
	else {
		dest.setPoint(map,x,y);
		target = &dest;
	}

	if(!target) return false;
	if(trail) { delete trail;trail = 0; }
app.printf("Character::moveTo(mx=%d,my=%d,dest.mx=%d,dest.my=%d)",mx,my,dest.mx,dest.my);
	if(mx!=dest.mx || my!=dest.my) {
		aPath pt(map->mw,map->mh,map->wrap,map,this,Map::pathCompareArea,Map::pathTerrainType,pathMoveCost);
		trail = pt.search(mx,my,dest.mx,dest.my);

while(trail->hasMoreSteps()) {
app.printf("Character::moveTo(trail.x=%d,trail.y=%d)",trail->getX(),trail->getY());
trail->next();
}
trail->first();

		trail->next();
		mapToPnt(trail->getX(),trail->getY(),dx,dy);
	} else dx = dest.px,dy = dest.py;
	return true;
}

int Character::velocity(int x,int y) {
	static int vel[GROUND_TYPES] = {
	// oscean=O, plain=P, desert=D, tundra=T
	// 0=O  1=P  2=D  3=T
		512, 768, 768, // Human
	};
	Map::maplocation &t = map->map[x+y*map->mw];
	int n = vel[t.g];
//	if(n>0 && t.type2) n += vel[t.type2];
	return n;
}

int Character::pathMoveCost(int x,int y,int c,void *map,void *obj) {
	static int cost[2*GROUND_TYPES] = {
	// oscean=O, plain=P, desert=D, tundra=T
	// 0=O 1=P 2=D 3=T
		 0,  2,  1, // Land
		 1,  0,  0, // Sea
	};
	Map::maplocation &t = ((Map *)map)->map[x+y*((Map *)map)->mw];
	int n = cost[c*GROUND_TYPES+t.g];
	if(n==0) return PATH_MOVE_COST_CANNOT_MOVE;
//	if(t.t) n += cost[c+t.type2];
	return n;
}

void Character::pushSkillQue(uint8_t s) { sk = s; }
void Character::shiftSkillQue() { sk = attack; }



#ifdef WOTSERVER
/*
struct skill_prototype {
	const char *name;
	long type;								// Skill type
	struct skill_level {
		unsigned short time;				// Time in gamecycles it takes to perform skill, there are 6 cycles per second.
		unsigned short cool;				// Time in seconds it takes for skill to cool down, e.g. for spells
		struct {
			short index;					// Effect index
			unsigned char lvl;			// Effect level
		} eff[5];							// Effects
	} lvl[11];
};
struct skill {
	unsigned short index;
	unsigned long xp;
	unsigned char lvl;					// Skill Level: 0=Beginner,
												// 1=Layman, 2=Apprentice, 3=Journeyman, 4=Master, 5=Grandmaster
												// 6=Hero, 7=Epic, 8=Legend, 9=Deity, 10=Supreme
	unsigned char div;
	unsigned char sum;
	unsigned char temp;
	unsigned short cool;
};
*/
/*
	Skill chance table
	  1  -  300			= 0		30.0%
	301  -  500			= 1		20.0%
	501  -  650			= 2		15.0%
	651  -  750			= 3		10.0%
	751  -  830			= 4		 8.0%
	831  -  890			= 5		 6.0%
	891  -  930			= 6		 4.5%
	931  -  960			= 7		 3.0%
	961  -  985			= 8		 2.0%
	986  -  995			= 9		 1.0%
	996  - 1000			= 10		 0.5%
*/
char skill_chance[1000] = {
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,	1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,	2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,	3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,	4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,	5,5,5,5,5,5,5,5,5,5,	5,5,5,5,5,5,5,5,5,5,	5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,	5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,	6,6,6,6,6,6,6,6,6,6,	6,6,6,6,6,6,6,6,6,6,	6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,	7,7,7,7,7,7,7,7,7,7,	7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,	8,8,8,8,8,8,8,8,8,8,
	9,9,9,9,9,9,9,9,9,9,
	10,10,10,10,10,
};
int skill_fumble[11] = { 150,100,75,50,25,15,7,5,3,1,0 }; // Risk in promille of fumble for skill lvl

int Character::useSkill(time_t cycles) {
//if(instanceOf(Player::getClass())) app.printf("Character::useSkill(id=%d,timer=%d,sk=%d,attack=%d)",getID(),timer,sk,attack);
	if(isReady()) shiftSkillQue();
	if(isBusy() || (act!=ACT_BATTLE && sk==attack)) return 0;
	skill &s = skills[sk];
app.printf("Character::useSkill(id=%d,sk=%d,s.index=%d,s.sum=%d)",getID(),sk,s.index,s.sum);
	const skill_prototype &p = skilldata[s.index];
	int a = skillChance(s.sum);
	timer += p.lvl[s.lvl].time;
	if(timer<0) timer = 0;
	switch(p.type) {
		case SK_MELEE:
		{
			if(atts[ATT_HP].h.temp==0 || act!=ACT_BATTLE || !target || !target->instanceOf(Character::getClass())) return stand();
			Character &ct = *(Character *)target;
			int hp = ct.atts[ATT_HP].h.temp;
			if(hp==0) return stand();
			char msg[128];
			//battlepack bp = {0,0,0,0,0,0};
			Player *pl = (Player *)(instanceOf(Player::getClass())? this : (ct.instanceOf(Player::getClass())? &ct : 0));
			if(a>=0) {
				if(a<=s.sum) {
					int d = 0;
					skill &cts = ct.skills[ct.defend];
					if(ct.isReady()) {
						d = ct.skillChance(cts.lvl);
						ct.timer = skilldata[cts.index].lvl[cts.lvl].time;
						ct.timer = d<0? ct.timer*2 : ct.timer/2;
						if(d>cts.lvl) d = 0;
					}
					if(a>=d) {
						int xp = calculateSkillXP(s.lvl,a-d,cts.sum-s.lvl);
						if(xp>0) addXP(xp,sk);
						int n = a-d+getDamage(),dmg = atts[ATT_ATK].b.sum - ct.atts[ATT_DEF].b.sum;
						if(dmg>1) dmg = 1+rnd.uint32(dmg);
						if(n>0) dmg += rnd.oeD4(n);
						if(dmg>0) {
							if(dmg>=hp) hp = 0;
							else {
								ct.addAggro(*this,dmg);
								hp -= dmg;
							}
app.printf("Character::useSkill(dmg=%d,hp=%d,ct.enms=%p,ct.nenms=%d)",dmg,hp,ct.enms,ct.nenms);
							if(hp==0) ct.target = this;
							ct.sendEffectPack(SEND_TO_ALL,0,0,0,0,CH_STAT,ATT_HP,hp);
							if(pl) {
								sprintf(msg,"%s hit %s for %d HP! (att=%d,def=%d,rolls=%d)",getName(),ct.getName(),dmg,a,d,n);
								app.msg(*pl,FONT_WHITE,msg);
								pl->sendBattlePack(SEND_TO_CHANNEL,*this,ct,1,d,timer*4);
							}
						} else if(pl) pl->sendBattlePack(SEND_TO_CHANNEL,*this,ct,2,a,timer*4); // Armor
					} else if(pl) pl->sendBattlePack(SEND_TO_CHANNEL,*this,ct,3,d-a,timer*4); // Block
				} else if(pl) pl->sendBattlePack(SEND_TO_CHANNEL,*this,ct,4,a,timer*4); // Miss
			} else { // Fumble
				timer += p.lvl[s.lvl].time;
				if(pl) pl->sendBattlePack(SEND_TO_CHANNEL,*this,ct,5,0,timer*2);
			}
//app.printf("Character::useSkill(bp.type=%d)",bp.type);
			//if(bp.type>0 && pl) app.send(*pl,&bp,sizeof(battlepack));
			if(ct.act!=ACT_BATTLE || !ct.target) ct.battle(*this);
			break;
		}
	}
	if(p.lvl[s.lvl].cool>0) s.cool = p.lvl[s.lvl].cool;
app.printf("Character::useSkill(done)");
	return 1;
}

int Character::skillChance(int s) {
	int c = rnd.uint32(1000);
	if(c<skill_fumble[s]) return -1;
	return skill_chance[c];
}


void Character::enterLocation(Location *l) {
	if(trail) { delete trail;trail = 0; }
	stand();
	sendLocationPack(SEND_TO_ALL,l);
}
#endif


void Character::animate() {
	if(act==ACT_MOVE) {
// app.printf("Character::animate(px=%d,py=%d,dx=%d,dy=%d)",px,py,dx,dy);
		Coordinate c(map,dx,dy);

		if(mx==0 && c.mx>=map->mw-1) c.moveX(-map->mw);
		else if(mx==map->mw-1 && c.mx<=0) c.moveX(map->mw);
		if(my==0 && c.my>=map->mh-1) c.moveY(-map->mh);
		else if(my==map->mh-1 && c.my<=0) c.moveY(map->mh);

		int xv = velocity(mx,my),yv = xv/2,x1 = px,y1 = py,x2 = c.px,y2 = c.py,distx,disty;
		if(map->wrap&WRAP_HORIZ) { if(x1+map->pw-x2<x2-x1) x1 += map->pw;else if(x2+map->pw-x1<x1-x2) x2 += map->pw; }
		if(map->wrap&WRAP_VERT) { if(y1+map->ph-y2<y2-y1) y1 += map->ph;else if(y2+map->ph-y1<y1-y2) y2 += map->ph; }

		if(target!=&dest) {
			if(distance(*target)<32) {
app.printf("Character::animate(x1=%d,y1=%d,x2=%d,y2=%d,distx=%d,disty=%d)",x1,y1,x2,y2,abs(x1-x2),abs(y1-y2));
				Character &ct = *(Character *)target;
				if(!ct.isDead()) {
					if(trail) { delete trail;trail = 0; }
#ifdef WOTSERVER
					if(ct.instanceOf(NPC::getClass())) battle(ct);
					else talk(ct);
#endif
					return;
				}
			}
#ifdef WOTSERVER
		} else if(mx==dest.mx && my==dest.my && map->map[dest.mx+dest.my*map->mw].c==CITY) {
			Location *l = map->getLocation(mx,my);
			if(l && l->instanceOf(City::getClass())) enterLocation(l);
#endif
		}

		x1 = x1*256+fx,y1 = y1*256+fy,x2 = x2*256+c.fx,y2 = y2*256+c.fy,distx = abs(x1-x2),disty = abs(y1-y2);
		if(distx>xv+512 || disty>yv+256) {
			disty <<= 1;
			dir = abs(disty-distx)<xv+256? (y2<y1? (x2<x1? NW : NE) : (x2<x1? SW : SE)) :
				(disty<distx? (y2<y1? (x2<x1? SW : SE) : (x2<x1? NW : NE)) : (y2<y1? (x2<x1? NE : NW) : (x2<x1? SE : SW)));
			x1 += (dir==NE || dir==SE? xv : -xv),y1 += (dir==SE || dir==SW? yv : -yv);
			setPoint(x1>>8,y1>>8);
			fx = x1&0xff,fy = y1&0xff;
			map->placeItem(*this);
			//if(instanceOf(Player::getClass()) && ((Player *)this)->getTeam()) ((Player *)this)->getTeam()->move();

			if(trail && mx==trail->getX() && my==trail->getY()) {
				trail->next();
				if(trail->steps()>1) c.setMap(trail->getX(),trail->getY());
				else { c.set(dest);delete trail;trail = 0; }
				dx = c.px,dy = c.py;
			}
		} else {
			target = 0;
#ifdef WOTSERVER
			stand();
#endif
		}
	}
}

void Character::setEffect(uint16_t index,uint8_t lvl,uint8_t value,uint16_t time,uint8_t v1,uint8_t v2,uint16_t v3) {
	const effect_prototype &epr = effectdata[index];
	int n = 0;
	if(epr.type&EFF_INSTANT) {
		n = epr.type&0x0ff0;
		if(n==EFF_HEAL || n==EFF_DAMAGE) {
			if(v1==CH_STAT && (v2>=ATT_HP && v2<=ATT_SP)) atts[v2].h.temp = v3;
		} else if(n==EFF_NEGATE) for(n=0; n<neff; n++) if(eff[n].index==v3) eff[n].time = 0;
	} else {
		if(eff) {
			if(epr.type&EFF_STACKABLE) n = neff;
			else for(; n<neff; n++) if(eff[n].index==index) break;
			if(n==neff) {
				effect *e = eff;
				eff = (effect *)malloc(sizeof(effect)*(neff+1));
				memcpy(eff,e,sizeof(effect)*neff);
				free(e);
				eff[neff++] = (effect){ index,0,0,0 };
			}
		} else eff = (effect *)malloc(sizeof(effect)),eff[0] = (effect){ index,0,0,0 },neff = 1;
		if(eff[n].lvl<=lvl) {
			eff[n].lvl = lvl;
			if(eff[n].value<value) eff[n].value = value;
			if(eff[n].time<time) eff[n].time = time;
		}
	}
}

int Character::calculateXP(int l1,int l2) {
	int xp = 0x40000;
	if(l1>l2) l2 = 1+l1-l2,xp = xp/(l1*l2);
	else if(l1<l2) l2 = 1+l2-l1,xp = xp*l2/l1;
	else xp = xp/l1;
	return xp;
}

int Character::calculateSkillXP(int lvl,int eff,int dif) {
	eff = 5-lvl+eff;
	if(eff<=0) return 0;
	lvl = aMath::fibonacci[2+lvl];
	int xp = 0x1000*eff*eff/(lvl*lvl);
	if(dif==0) return xp;
	else if(dif<0) return xp/(1-dif); // Note: dif is negative here so it's actually "eff/(1+dif)"
	if(dif>3) dif = 3;
	return xp*(1+dif);
}

void Character::addXP(int xp,int sk) {
	bool u = false;
	if(sk>=0 && sk<nskills) {
		skills[sk].xp += xp;
app.printf("Character::addXP(xp=%d,sk=%d,skills[sk].xp=%d,pct=%d)",xp,sk,skills[sk].xp,getAttackXP(100));
		if((int)(skills[sk].xp>>20)>=(int)skills[sk].lvl+1) {
			skills[sk].xp = (skills[sk].lvl+1)<<20;
			if(skills[sk].lvl<=5) skills[sk].lvl++,u = true;
		}
		xp = xp/lvl;
	}
	this->xp += xp;
	if((this->xp>>20)>=lvl) this->xp = lvl<<20,u = true;
	if(u) update();
}

void Character::addAggro(Character &ch,int dmg) {
	int n;
	if(enms) {
		for(n=0; n<nenms; n++) if(enms[n].target==this) break;
		if(n==nenms) {
			enemy *e = enms;
			enms = (enemy *)malloc(sizeof(enemy)*(nenms+1));
			memcpy(enms,e,sizeof(enemy)*nenms);
			free(e);
			enms[nenms++] = (enemy){ this,0,0 };
		}
	} else enms = (enemy *)malloc(sizeof(enemy)),enms[0] = (enemy){ this,0,0 },nenms = 1,n = 0;
	enms[n].dmg += dmg,enms[n].aggro += dmg*60/atts[ATT_HP].h.sum;
}

bool Character::setCharacter(uint16_t index,uint32_t v1,uint32_t v2,const uint8_t *data,size_t len) {
	switch(index) {
		case CH_XP:addXP(v1,v2);return true;
		case CH_KILL:kill();return true;
	}
	return false;
}

void Character::update() {
	int i;
	for(i=ATT_PRE; i<=ATT_BOD; i++) {
		atts[i].c.sum += atts[i].c.value+atts[i].c.el;
		if(atts[i].c.sum>10) atts[i].c.sum = 10;
	}
	for(i=ATT_PRE; i<=ATT_BOD; i++) {
		atts[i].c.mind = atts[i].c.sum>atts[ATT_MIN].c.sum? atts[ATT_MIN].c.sum : atts[i].c.sum;
		atts[i].c.body = atts[i].c.sum>atts[ATT_BOD].c.sum? atts[ATT_BOD].c.sum : atts[i].c.sum;
	}
	for(i=ATT_ATK; i<=ATT_RES; i++) atts[i].b.sum += atts[i].b.value;
	atts[ATT_ATK].b.sum += lvl+aMath::fibonacci[1+atts[ATT_PER].c.body]+aMath::fibonacci[1+atts[ATT_AGI].c.body]+
										aMath::fibonacci[1+atts[ATT_PRE].c.mind+atts[ATT_POW].c.sum];
	atts[ATT_DEF].b.sum += lvl+aMath::fibonacci[1+atts[ATT_PER].c.body]+
										aMath::fibonacci[1+atts[ATT_PRE].c.mind+atts[ATT_AGI].c.body];
	atts[ATT_RES].b.sum += lvl+aMath::fibonacci[1+atts[ATT_PER].c.mind]+
										aMath::fibonacci[1+atts[ATT_PRE].c.mind+atts[ATT_POW].c.mind];

	atts[ATT_HP].h.reg += 1+lvl/3;
	atts[ATT_HP].h.sum += atts[ATT_HP].h.value+lvl*3+
										aMath::fibonacci[2+atts[ATT_CON].c.mind+atts[ATT_CON].c.body]*3;
	atts[ATT_LP].h.reg += 1+(lvl+1)/3;
	atts[ATT_LP].h.sum += atts[ATT_LP].h.value+lvl*3+
										aMath::fibonacci[2+atts[ATT_PRE].c.mind+atts[ATT_PER].c.mind]*3;
	atts[ATT_SP].h.reg += 1+(lvl+2)/3;
	atts[ATT_SP].h.sum += atts[ATT_SP].h.value+lvl*3+
										aMath::fibonacci[2+atts[ATT_POW].c.mind+atts[ATT_CON].c.body]*3;
}

#ifdef WOTSERVER
void Character::updateStatus(time_t cycles) {
	if(isDead()) return;
	if(atts[ATT_HP].h.temp==0) {
app.printf("Character::updateStatus(hp=0)");
		kill();
		return;
	}
	cycle(cycles);
}
#endif

void Character::recharge() {
	atts[ATT_HP].h.temp = atts[ATT_HP].h.sum;
	atts[ATT_LP].h.temp = atts[ATT_LP].h.sum;
	atts[ATT_SP].h.temp = atts[ATT_SP].h.sum;
}

void Character::cycle(time_t cycles) {
// app.printf("Character::cycle(id=%d,HPmax=%d,HPtemp=%d)",getID(),atts[ATT_HP].h.sum,atts[ATT_HP].h.temp);
	if(itm && (cycles%GAME_FPS)==0) for(int i=0; i<nitm; i++) if(itm[i].cool>0) itm[i].cool--;
	if(timer>0) timer--;
	if(isDead()) return;
	int i,n;
	if((act==ACT_STAND && (cycles%CYCLES_RESTING)==0) || (act!=ACT_BATTLE && (cycles%CYCLES_ACTIVE)==0)) {
		for(i=ATT_HP; i<=ATT_SP; i++)
			if(atts[i].h.temp<atts[i].h.sum && atts[i].h.reg>0) {
				atts[i].h.temp += atts[i].h.reg;
				if(atts[i].h.temp>atts[i].h.sum) atts[i].h.temp = atts[i].h.sum;
			}
		if(skills) for(i=0; i<nskills; i++) if(skills[i].cool>0) skills[i].cool--;
	}
	if(enms && (cycles%CYCLES_ENEMIES)==0) {
app.printf("Character::cycle(id=%d,nenms=%d,target=%p,aggro=%d)",getID(),nenms,enms[nenms-1].target,enms[nenms-1].aggro);
		for(i=0,n=nenms; i<nenms; i++) {
			if(enms[i].aggro>0) enms[i].aggro--;
			if(enms[i].target->isDead() || enms[i].aggro==0) n--;
		}
		if(n==0) { free(enms);enms = 0,nenms = 0; }
		else if(n<nenms) {
			enemy *e = enms;
			enms = (enemy *)malloc(sizeof(enemy)*n);
			for(i=0,n=0; i<nenms; i++) if(!e[i].target->isDead() && e[i].aggro>0) enms[n++] = e[i];
			free(e);
			nenms = n;
		}
	}
	if(eff) {
		for(i=0,n=neff; i<neff; i++) {
			if(eff[i].time>0) eff[i].time--;
			if(eff[i].time==0) n--;
		}
		if(n==0) { free(eff);eff = 0,neff = 0; }
		else if(n<neff) {
			effect *e = eff;
			eff = (effect *)malloc(sizeof(effect)*n);
			for(i=0,n=0; i<neff; i++) if(e[i].time==0) eff[n++] = e[i];
			free(e);
			neff = n;
		}
	}
// app.printf("Character::cycle(done)");
}


void Character::kill() {
#ifdef WOTSERVER
	Character *ct = target && target->instanceOf(Character::getClass())? (Character *)target : 0;
app.printf("Character::kill(id=%d,kid=%d)",getID(),ct? ct->getID() : 0);
	sendCharacterPack(SEND_TO_CLIENTS,CH_KILL,0,0);
	Player *pl = (Player *)(instanceOf(Player::getClass())? this : (ct->instanceOf(Player::getClass())? ct : 0));
	if(ct && pl) {
		app.msgf(*pl,FONT_BLUE,"%s killed %s!",ct->getName(),getName());
		ct->addXP(calculateXP(ct->getLevel(),lvl));
		pl->sendBattlePack(SEND_TO_CHANNEL,*ct,*this,6,0,-1);
	}
	die();
#elif defined WOTCLIENT
	die();
	if(getID()==app.getID() && !isIncarnated()) {
		app.incdlg->setPlayer((Player *)this);
		app.incdlg->setVisible(true);
	}
#endif
}


void Character::die() {
app.printf("Character::die(id=%d)",getID());
	setStatus(ST_DEAD);
	removeStatus(ST_UPTODATE|ST_INCARNATED);
	spri = spr[3],atts[ATT_HP].h.temp = 0,atts[ATT_LP].h.temp = 0,atts[ATT_SP].h.temp = 0;
	act = 0,target = 0;
	if(enms) { free(enms);enms = 0,nenms = 0; }
}


void Character::receiveActionPack(uint8_t *data) {
	uint8_t to,a;
	uint32_t id,p1,p2,p3;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	unpack_uint32(&data,id);
	unpack_uint8(&data,a);
	unpack_uint32(&data,p1);
	unpack_uint32(&data,p2);
	unpack_uint32(&data,p3);
	Character *c = (Character *)Map::getItem(id);
	if(!c) return;
	c->setAction(a,p1,p2,p3);
/*#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) c->sendActionPack(to,a,p1,p2,p3);
#endif*/
}

#ifdef WOTCLIENT
void Character::receiveBattlePack(uint8_t *data) {
	uint32_t id1,id2;
	uint8_t to,type;
	uint16_t effect;
	int8_t timer;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	unpack_uint32(&data,id1);
	unpack_uint32(&data,id2);
	unpack_uint8(&data,type);
	unpack_uint16(&data,effect);
	unpack_int8(&data,timer);
	app.btldlg->handleEvent(id1,id2,type,effect,timer);
}
#endif

void Character::receiveCharacterPack(uint8_t *data) {
	uint8_t *p = data,cmd,to;
	uint16_t index;
	uint32_t len,v1,v2;
	unpack_header(&p,cmd,len);
	unpack_uint8(&p,to);
	Character *c = (Character *)Map::getItem(get_unpack_uint32(p));
	if(!c) return;
	p += 4;
	unpack_uint16(&p,index);
	unpack_uint32(&p,v1);
	unpack_uint32(&p,v2);
	len -= (p-data);
	c->setCharacter(index,v1,v2,p,len);
#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) c->sendCharacterPack(to,index,v1,v2,p,len);
#endif
}

void Character::receiveEffectPack(uint8_t *data) {
	uint16_t index,time,v3;
	uint8_t to,lvl,value,v1,v2;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	Character *c = (Character *)Map::getItem(get_unpack_uint32(data));
	if(!c) return;
	data += 4;
	unpack_uint16(&data,index);
	unpack_uint8(&data,lvl);
	unpack_uint8(&data,value);
	unpack_uint16(&data,time);
	unpack_uint8(&data,v1);
	unpack_uint8(&data,v2);
	unpack_uint16(&data,v3);
	c->setEffect(index,lvl,value,time,v1,v2,v3);
#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) c->sendEffectPack(to,index,lvl,value,time,v1,v2,v3);
#endif
}

void Character::receiveLocationPack(uint8_t *data) {
	uint8_t to;
	uint32_t locid;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	Character *c = (Character *)Map::getItem(get_unpack_uint32(data));
	if(!c) return;
	Location *l = 0;
	data += 4;
	unpack_uint32(&data,locid);
	if(locid>0) l = Map::getLocation(locid);
	c->setLocation(l);
#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) c->sendLocationPack(to,l);
#else
	if(app.getLeader()==c) {
		if(l) {
			app.cityDisplay->enterCity((City *)l);
			app.command(CMD_ENTER,app.cityDisplay);
		} else app.command(CMD_ENTER,app.worldDisplay);
	}
#endif
}


void Character::sendActionPack(uint8_t to,uint8_t a,uint32_t p1,uint32_t p2,uint32_t p3) {
	uint8_t d[SOCKET_HEADER+18],*p = d;
	pack_header(&p,CMD_ACTION);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint8(&p,a);
	pack_uint32(&p,p1);
	pack_uint32(&p,p2);
	pack_uint32(&p,p3);
#ifdef WOTSERVER
	app.send(to,this,d,sizeof(d));
#else
	app.send(d,sizeof(d));
#endif
	// Is handled locally in setAction().
}

void Character::sendCharacterPack(uint8_t to,uint16_t index,uint32_t v1,uint32_t v2,uint8_t *data,size_t len) {
	uint8_t d[SOCKET_HEADER+15+len],*p = d;
	pack_header(&p,CMD_CHARACTER);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint16(&p,index);
	pack_uint32(&p,v1);
	pack_uint32(&p,v2);
	if(len>0) memcpy(p,data,len);
#ifdef WOTSERVER
	app.send(to,this,d,sizeof(d));
	if(to&SEND_TO_SERVER) setCharacter(index,v1,v2,data,len);
#else
	app.send(d,sizeof(d));
#endif
}

void Character::sendLocationPack(uint8_t to,Location *l) {
	uint8_t d[SOCKET_HEADER+9],*p = d;
	pack_header(&p,CMD_ENTER);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint32(&p,l? l->getID() : 0);
#ifdef WOTSERVER
	app.send(to,this,d,sizeof(d));
	if(to&SEND_TO_SERVER) setLocation(l);
#else
	app.send(d,sizeof(d));
#endif
}

#ifdef WOTSERVER
void Character::sendBattlePack(uint8_t to,Character &c,Character &ct,uint8_t type,uint16_t effect,int8_t timer) {
	uint8_t d[SOCKET_HEADER+13],*p = d;
	pack_header(&p,CMD_BATTLE);
	pack_uint8(&p,to);
	pack_uint32(&p,c.getID());
	pack_uint32(&p,ct.getID());
	pack_uint8(&p,type);
	pack_uint16(&p,effect);
	pack_int8(&p,timer);
	app.send(to,this,d,sizeof(d));
	// Will only be processed by BattleDlg that is only compiled for the Client.
}

void Character::sendCommandPack(uint8_t to,uint8_t cmd,uint32_t v) {
	uint8_t d[SOCKET_HEADER+9],*p = d;
	pack_header(&p,cmd);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint32(&p,v);
	app.send(to,this,d,sizeof(d));
	// Should be processed locally where called.
}

void Character::sendEffectPack(uint8_t to,uint16_t index,uint8_t lvl,uint8_t value,uint16_t time,uint8_t v1,uint8_t v2,uint16_t v3) {
	uint8_t d[SOCKET_HEADER+15],*p = d;
	pack_header(&p,CMD_EFFECT);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint16(&p,index);
	pack_uint8(&p,lvl);
	pack_uint8(&p,value);
	pack_uint16(&p,time);
	pack_uint8(&p,v1);
	pack_uint8(&p,v2);
	pack_uint16(&p,v3);
	app.send(to,this,d,sizeof(d));
	if(to&SEND_TO_SERVER) setEffect(index,lvl,value,time,v1,v2,v3);
}


void Character::getData(uint8_t **data) {
	int i;
	memcpy(*data,name,namelen+1);
	*data += namelen+1;
	if(nitm) for(i=0; i<nitm; i++) {
		pack_int16(data,itm[i].index);
		pack_uint16(data,itm[i].qnt);
		pack_uint8(data,itm[i].dura);
		pack_uint16(data,itm[i].cool);
	}
	if(nskills) for(i=0; i<nskills; i++) {
		pack_uint16(data,skills[i].index);
		pack_uint32(data,skills[i].xp);
		pack_uint8(data,skills[i].lvl);
		pack_uint8(data,skills[i].div);
		pack_uint8(data,skills[i].sum);
		pack_uint16(data,skills[i].cool);
	}
	if(neff) for(i=0; i<neff; i++) {
		pack_uint16(data,eff[i].index);
		pack_uint8(data,eff[i].lvl);
		pack_uint8(data,eff[i].value);
		pack_int16(data,eff[i].time);
	}
}
#endif

void Character::setData(uint8_t **data) {
	int i,n;
	setName((char *)*data);
	*data += namelen+1;

	if(itm) free(itm);
	if(nitm) {
		itm = (item *)malloc(n=sizeof(item)*nitm);
		for(i=0; i<nitm; i++) {
			unpack_int16(data,itm[i].index);
			unpack_uint16(data,itm[i].qnt);
			unpack_uint8(data,itm[i].dura);
			unpack_uint16(data,itm[i].cool);
		}
	} else itm = 0;
	if(skills) free(skills);
	if(nskills) {
		skills = (skill *)malloc(n=sizeof(skill)*nskills);
		for(i=0; i<nskills; i++) {
			unpack_uint16(data,skills[i].index);
			unpack_uint32(data,skills[i].xp);
			unpack_uint8(data,skills[i].lvl);
			unpack_uint8(data,skills[i].div);
			unpack_uint8(data,skills[i].sum);
			unpack_uint16(data,skills[i].cool);
		}
	} else skills = 0;
	if(eff) free(eff);
	if(neff) {
		eff = (effect *)malloc(n=sizeof(effect)*neff);
		if(neff) for(i=0; i<neff; i++) {
			unpack_uint16(data,eff[i].index);
			unpack_uint8(data,eff[i].lvl);
			unpack_uint8(data,eff[i].value);
			unpack_int16(data,eff[i].time);
		}
	} else eff = 0;
}



