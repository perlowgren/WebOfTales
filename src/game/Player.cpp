
#include "../config.h"
#include <math.h>
#include <string.h>
#include <libamanita/aMath.h>
#include <libamanita/aString.h>
#include <libamanita/aRandom.h>
#include "../Main.h"
#include "../command.h"

#ifdef WOTSERVER
	#include <libamanita/net/aServer.h>
	#include "../server/WebOfTales.h"
#elif defined WOTCLIENT
	#include <libamanita/net/aSocket.h>
	#include <libamanita/sdl/aGraphics.h>
	#include <libamanita/sdl/aFont.h>
	#include "../client/WoTClient.h"
	#include "../client/MapDisplay.h"
#endif

#include "Location.h"
#include "Player.h"
#include "Team.h"



aObject_Inheritance(Player,Character);


const int Player::avatarSpritePaintOrder[4][11] = {
	{ AVT_RHAND,AVT_LHAND,AVT_SKIN,AVT_LEGS,AVT_FEET,AVT_CHEST,AVT_BACK,AVT_FACE,AVT_HAIR,AVT_HEAD,-1 },
	{ AVT_BACK,AVT_SKIN,AVT_LEGS,AVT_FEET,AVT_CHEST,AVT_BACK,AVT_HAIR,AVT_HEAD,AVT_FACE,AVT_RHAND,AVT_LHAND, },
	{ AVT_BACK,AVT_SKIN,AVT_LEGS,AVT_FEET,AVT_CHEST,AVT_BACK,AVT_HAIR,AVT_HEAD,AVT_FACE,AVT_RHAND,AVT_LHAND, },
	{ AVT_RHAND,AVT_LHAND,AVT_SKIN,AVT_LEGS,AVT_FEET,AVT_CHEST,AVT_BACK,AVT_FACE,AVT_HAIR,AVT_HEAD,-1 },
};
/*
enum {
	AVT_SKIN					= 0,
	AVT_HAIR					= 1,
	AVT_FACE					= 2,
	AVT_CHEST				= 3,
	AVT_LEGS					= 4,
	AVT_FEET					= 5,
	AVT_HEAD					= 6,
	AVT_BACK					= 7,
	AVT_RHAND				= 8,
	AVT_LHAND				= 9,
};
*/
const int Player::avatarSpriteHorizontalAdjust[4][10] = {
	{ 5,9,9,5,5,5,9,5,0,0 },{ 5,9,9,5,5,5,9,5,0,0 },
	{ 4,8,8,4,4,4,8,4,0,0 },{ 4,8,8,4,4,4,8,4,0,0 },
};
const int Player::avatarSpriteVerticalAdjust[10] = { 3,0,0,3,26,-2,0,3,0,0 };
const int Player::avatarFrameVerticalAdjust[5] = { 0,1,2,1,2 };
const int Player::avatarHeadCoverHair[121] = {0,
	1,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,
};


Player::Player(Map *m,uint32_t id,const char *nm,uint32_t locid,int16_t px,int16_t py,uint32_t st,uint8_t icon) : Character(m,0) {
	setID(id);
	setName(nm);
	setPoint(px,py);
	setStatus(st);
	setIcon(icon);
	reset();
}

Player::Player(Map *m,uint8_t **data) : Character(m,0) {
	unpack(data);
}

Player::~Player() {
app.printf("Player::~Player(id=%lu)",getID());
	team = 0;
}


/*#ifdef WOTSERVER
int Player::battle(Character &c) {
	if(team && team->members()>1) {
		for(int i=0; i<team->members(); i++) team->getMember(i)->setAction(ACT_BATTLE,c);
		return true;
	} else return setAction(ACT_BATTLE,c);
}

#elif defined WOTCLIENT*/
#ifdef WOTCLIENT
void Player::setAction(int x,int y) {
	if(team && !team->isLeader(*this)) return;
	act = ACT_STAND,spri = spr[0];
	target = 0;
	Character *c = map->getItem(x,y);
	sendActionPack(SEND_TO_ALL,ACT_MOVE,0,(x<<16)|y,c? c->getID() : 0);
}

void Player::paint(viewpoint &v) {
	static const int actionicons[] = { 0,0,0,2,3,0,0 };
	const sprite &sp = sprites[spri];
	if(!isUpToDate()) {
		if(!isRequesting()) {
			setStatus(ST_REQUESTING);
			app.sendRequest(REQ_PLAYER,getID());
		}
		return;
	}
	int xp = px-v.px,yp = py-v.py;
	if(xp<0) xp += map->pw;else if(xp>=map->pw) xp -= map->pw;
	if(yp<0) yp += map->ph;else if(yp>=map->ph) yp -= map->ph;
	paintAvatarImage(xp,yp,dir,sp.frames[(app.time.frames-sprtm)%sp.length]);
	xp -= sp.basex,yp -= sp.basey;

	//if(this==app.getHover() || this==app.getFocus())
		app.fonts[FONT_WHITE]->print(xp+sp.basex-app.fonts[FONT_WHITE]->stringWidth(getName())/2,yp,getName());

	if(actionicons[act]) {
		if(target && target->instanceOf(Character::getClass())) {
			Character &c = *(Character *)target;
			const sprite &s2 = sprites[c.spri];
			xp = (px<c.px? px : c.px)+abs(px-c.px)/2-v.px-16;
			yp = (py<c.py? py : c.py)+abs(py-c.py)/2-v.py-32-(sp.basey>s2.basey? sp.basey : s2.basey);
		} else xp = px-v.px-16,yp = py-sp.basey-v.py-32;
		if(xp<0) xp += map->pw;else if(xp>=map->pw) xp -= map->pw;
		if(yp<0) yp += map->ph;else if(yp>=map->ph) yp -= map->ph;
		app.icons->draw(xp,yp-abs(((app.time.frames>>1)%8)-4),actionicons[act]);
	}
}


int Player::setIcon(int i) {
	int n;
	const int *icon = racedata[inc.race].g[inc.gen].icon;
	for(n=0; n!=i && icon[n]!=-1; n++);
	if(n!=i || icon[n]==-1) i = 0;
	this->icon = icon[i];
	updateAvatar();
	return i;
}

int Player::setSkin(int s) {
	int n;
	const int *skin = racedata[inc.race].skin;
	for(n=0; n!=s && skin[n]!=-1; n++);
	if(n!=s || skin[n]==-1) s = 0;
	avt.skin = skin[s];
	updateAvatar();
	return s;
}

int Player::setHair(int h) {
	int n;
	const int *hair = racedata[inc.race].g[inc.gen].hair;
	for(n=0; n!=h && hair[n]!=-1; n++);
	if(n!=h || hair[n]==-1) h = 0;
	avt.hair = hair[h];
	updateAvatar();
	return h;
}

int Player::setFace(int f) {
	int n;
	const int *face = racedata[inc.race].g[inc.gen].face;
	for(n=0; n!=f && face[n]!=-1; n++);
	if(n!=f || face[n]==-1) f = 0;
	avt.face = face[f];
	updateAvatar();
	return f;
}

/*
enum {
	AVT_SKIN					= 0,
	AVT_HAIR					= 1,
	AVT_FACE					= 2,
	AVT_CHEST				= 3,
	AVT_LEGS					= 4,
	AVT_FEET					= 5,
	AVT_HEAD					= 6,
	AVT_BACK					= 7,
	AVT_RHAND				= 8,
	AVT_LHAND				= 9,
};

enum {
	SLOT_CHEST				= 0,
	SLOT_LEGS				= 1,
	SLOT_WAIST				= 2,
	SLOT_FEET				= 3,
	SLOT_HEAD				= 4,
	SLOT_BACK				= 5,
	SLOT_RHAND				= 6,
	SLOT_LHAND				= 7,
};
*/
void Player::updateAvatar() {
app.printf("Player::updateAvatar(slot[SLOT_CHEST]=%d,slot[SLOT_LEGS]=%d,slot[SLOT_FEET]=%d)",
			slot[SLOT_CHEST].index,slot[SLOT_LEGS].index,slot[SLOT_FEET].index);
	avt.spr[AVT_BACK]		= slot[SLOT_BACK].index>=0?	itemdata[slot[SLOT_BACK].index].spr[inc.gen]		: 0;
	avt.spr[AVT_HEAD]		= slot[SLOT_HEAD].index>=0?	itemdata[slot[SLOT_HEAD].index].spr[inc.gen]		: 0;
	avt.spr[AVT_CHEST]	= slot[SLOT_CHEST].index>=0?	itemdata[slot[SLOT_CHEST].index].spr[inc.gen]	: 0;
	avt.spr[AVT_LEGS]		= slot[SLOT_LEGS].index>=0?	itemdata[slot[SLOT_LEGS].index].spr[inc.gen]		: 0;
	avt.spr[AVT_FEET]		= slot[SLOT_FEET].index>=0?	itemdata[slot[SLOT_FEET].index].spr[inc.gen]		: 0;
	avt.spr[AVT_RHAND]	= slot[SLOT_RHAND].index>=0?	itemdata[slot[SLOT_RHAND].index].spr[inc.gen]	: 0;
	avt.spr[AVT_LHAND]	= slot[SLOT_LHAND].index>=0?	itemdata[slot[SLOT_LHAND].index].spr[inc.gen]	: 0;
app.printf("Player::updateAvatar(avt[SLOT_CHEST]=%d,avt[SLOT_LEGS]=%d,avt[SLOT_FEET]=%d)",
			avt.spr[SLOT_CHEST],avt.spr[SLOT_LEGS],avt.spr[SLOT_FEET]);

	avt.spr[AVT_SKIN]		= avt.skin;
	avt.spr[AVT_HAIR]		= avatarHeadCoverHair[avt.spr[AVT_HEAD]]? 0 : avt.hair;
	avt.spr[AVT_FACE]		= avt.face;
}


bool Player::saveAvatarImage() {
	if(!isUpToDate()) return false;
app.printf("Character::saveAvatarImage(1)");
	char file[32];
	aImage img(35,60);
	g.setCanvas(img);
	g.fillRect(0,0,35,60,g.mapRGB(0x660000));
	const sprite &sp = sprites[spr[0]];
	paintAvatarImage(sp.basex,sp.basey,SW,0);
	sprintf(file,"%simages/avatars/%" PRIu32 ".png",app.getHomeDir(),getID());
	bool b = img.save(file);
app.printf("Character::saveAvatarImage(2,b=%d)",b);
	g.resetCanvas();
	return b;
}

void Player::paintAvatarImage(int x,int y,int dir,int frame) {
	const sprite &sp = sprites[spr[0]];
// app.printf("Player::paintAvatarImage(1)");
	if(sp.shadow.shape>0) app.shadows->draw(x+sp.shadow.x,y+sp.shadow.y,sp.shadow.shape);
	const int *o = avatarSpritePaintOrder[dir],*ha = avatarSpriteHorizontalAdjust[dir];
	int i,n,p,f = 0,ax,ay,va = avatarFrameVerticalAdjust[frame];
	aImage *s;
	SDL_Rect *r;
	x -= sp.basex,y -= sp.basey,frame += 1+dir*5;
	for(i=0; i<11; i++) {
		if((n=o[i])!=-1 && (p=avt.spr[n])>0) {
// app.printf("Player::paintAvatarImage(n=%d,x=%d,y=%d)",n,x,y);
			ax = x+ha[n],ay = y+avatarSpriteVerticalAdjust[n];
			if(n==AVT_HAIR) app.plspr[n][(p>>4)-1]->draw(ax,ay+va,1+inc.gen*32+(p&0xf)*4+dir);
			else if(n==AVT_FACE || n==AVT_HEAD)
				app.plspr[n][0]->draw(ax,ay+va,1+(p-1)*4+dir);
			else if(n==AVT_BACK) app.plspr[n][p-1]->draw(ax,ay,(f&(1<<n)? 15 : 0)+frame);
			else if(n==AVT_FEET) {
				s = app.plspr[n][0],r = s->getCell((p-1)*20+frame);
				s->draw(ax,ay+sp.h-r->h,*r);
			} else if(n==AVT_SKIN) app.plspr[n][p-1]->draw(ax,ay,frame+inc.gen*20);
			else app.plspr[n][0]->draw(ax,ay,(p-1)*20+frame);
			f |= 1<<n;
// app.printf("Player::paintAvatarImage(n=%d,done)",n);
		}
	}
// app.printf("Player::paintAvatarImage(done)");
}
#endif


void Player::pushSkillQue(unsigned char s) {
	for(int i=0; i<4; i++) if(que[i]==0) que[i] = s+1;
}
void Player::shiftSkillQue() {
	if(*que==0) sk = attack;
	else {
		sk = *que-1;
		for(int i=0; i<3; i++) que[i] = que[i+1];
		que[3] = 0;
	}
// app.printf("Player::shiftSkillQue(sk=%d)",sk);
}



#ifdef WOTSERVER
void Player::addXP(int xp,int sk) {
	int l1 = lvl,l2 = 0;
	if(sk>=0) l2 = skills[sk].lvl;
	Character::addXP(xp,sk);
	sendCharacterPack(SEND_TO_CLIENT,CH_XP,xp,sk);
	if(lvl>l1) app.msgf(getID(),FONT_ORANGE_B,"%s gained a new level!",getName());
	if(sk>=0 && skills[sk].lvl>l2)
		app.msgf(getID(),FONT_ORANGE_B,"%s is now %s in the %s skill!",
			getName(),skill_level_names[skills[sk].lvl],skilldata[skills[sk].index].name);
}
#endif


bool Player::setCharacter(uint16_t index,uint32_t v1,uint32_t v2,const uint8_t *data,size_t len) {
	if(!isUpToDate()) return false;
	if(Character::setCharacter(index,v1,v2,data)) return true;
	switch(index) {
		case CH_UPGRADE_ELEM:
			if(div.el[v1]+v2<=10 && div.num>=aMath::fibonacci[3+div.el[v1]+v2]) {
				div.el[v1] += v2;
				update();
			}
			return true;

		case CH_EQUIP_ITEM:
			equipItem(v1);
			update();
#ifdef WOTCLIENT
			updateAvatar();
#endif
			return true;
		case CH_UNEQUIP_ITEM:
			unequipItem(v1);
			update();
#ifdef WOTCLIENT
			updateAvatar();
#endif
			return true;
	}
	return false;
}

void Player::update() {
	int i;
	lvl = (xp>>20)+1;
	div.num = lvl*5;
	if(isIncarnated()) div.num -= racedata[inc.race].div;
	for(i=SPIRIT; i<=EARTH; i++) if(div.el[i]>0) div.num -= aMath::fibonacci[5+div.el[i]]-5;
app.printf("Player::update(div.num=%d)",div.num);

	for(i=ATT_PRE; i<=ATT_BOD; i++) atts[i].c.sum = 0;
	atts[ATT_PRE].c.el = (div.el[SPIRIT])/2;
	atts[ATT_POW].c.el = (div.el[FIRE])/2;
	atts[ATT_PER].c.el = (div.el[AIR])/2;
	atts[ATT_AGI].c.el = (div.el[WATER])/2;
	atts[ATT_CON].c.el = (div.el[EARTH])/2;
	atts[ATT_MIN].c.el = (div.el[AIR]+1)/2;
	atts[ATT_BOD].c.el = (div.el[EARTH]+1)/2;
	atts[ATT_ATK].b.sum = (div.el[FIRE]+1)/2;
	atts[ATT_DEF].b.sum = (div.el[WATER]+1)/2;
	atts[ATT_RES].b.sum = (div.el[SPIRIT]+1)/2;
	for(i=ATT_HP; i<=ATT_SP; i++) atts[i].h.sum = 0,atts[i].h.reg = 0;

	attack = defend = 0;
	for(i=0; i<SLOTS; i++) if(slot[i].index>=0) {
		const item_prototype &p = itemdata[slot[i].index];
app.printf("Player::update(name=%s,arm=%d)",p.name,p.atts[ATT_DEF]);
		if(p.type&ITEM_ARMOR) atts[ATT_DEF].b.sum += p.atts[ATT_DEF];
		if((i==SLOT_RHAND || i==SLOT_LHAND) && p.req.skill>0) {
			if((p.type&ITEM_MELEE) || (p.type&ITEM_RANGED)) attack = p.req.skill;
			else if(p.type&ITEM_DEFENCE) defend = p.req.skill;
		}
	}
	for(i=0; i<nskills; i++) skills[i].sum = skills[i].lvl+skills[i].div;
	if(defend==0 && attack>0) defend = attack;
app.printf("Player::update(atts[ATT_DEF].sum=%d)",atts[ATT_DEF].b.sum);
	Character::update();
app.printf("Player::update(done)");
}

int Player::canEquipItem(item &i) {
	const item_prototype &p = itemdata[i.index];
	if(p.slot<0) return 1;
	if(p.req.gen>=0 && p.req.gen!=inc.gen) return 2;
	int n;
	for(n=ATT_PRE; n<=ATT_BOD; n++) if(p.req.atts[n]>atts[n].c.sum) return 3;
	if(p.req.skill>=0 && p.req.sklvl>0) {
		for(n=0; n<nskills; n++) if(skills[n].index==p.req.skill) {
			if(skills[n].lvl<p.req.sklvl) n = nskills;
			break;
		}
		if(n==nskills) return 4;
	}
	return 0;
}

int Player::equipItem(int n) {
	if(n<0 || n>=nitm) return -1;
	int i = canEquipItem(itm[n]);
	if(i!=0) return i;
	const item_prototype &p = itemdata[itm[n].index];
app.printf("Player::equipItem(n=%d,p.name=\"%s\",p.slot=%d)",n,p.name,p.slot);
	if(slot[(int)p.slot].index>=0) unequipItem(p.slot);
	slot[(int)p.slot] = itm[n];
	deleteItem(n);
	if(p.req.skill>=0 && p.req.sklvl==0) {
		for(i=0; i<nskills; i++) if(skills[i].index==p.req.skill) break;
		if(i==nskills) {
			skill s = (skill){ p.req.skill,0,0,0,0,0 };
			addSkill(s);
		}
	}
	return 0;
}

int Player::unequipItem(int n) {
app.printf("Player::unequipItem(n=%d)",n);
	if(n<0 || n>=SLOTS) return -1;
	if(slot[n].index<0) return 1;
	const item_prototype &p = itemdata[slot[n].index];
	addItem(slot[n]);
	slot[n].index = -1;
	if((p.slot==SLOT_RHAND || p.slot==SLOT_LHAND) && p.req.skill>0) {
		if(p.req.skill==attack) attack = 0;
		if(p.req.skill==defend) defend = 0;
	}
app.printf("Player::unequipItem(done)");
	return 0;
}

void Player::deleteEquipment() {
	for(int i=0; i<SLOTS; i++) slot[i].index = -1;
}

item *Player::getWeapon() { return slot[SLOT_RHAND].index>=0? &slot[SLOT_RHAND] : 0;}

int Player::getDamage() {
	item *w = getWeapon();
	return w? itemdata[w->index].atts[ATT_ATK] : 0;
}


void Player::cycle(time_t cycles) {
// fprintf(out,"Player::cycle(id=%d)\n",getID());
// fflush(out);
	if((cycles%GAME_FPS)==0) for(int i=0; i<SLOTS; i++) if(slot[i].cool>0) slot[i].cool--;
	Character::cycle(cycles);
}

void Player::die() {
	if(team) team->leave(this);
	if(map) map->removeItem(*this);
	Character::die();
}

void Player::reset() {
	int i;
	removeStatus(ST_UPTODATE);

	div = (divinity){ 0,0,{ 0,0,0,0,0 } };
	inc = (incarnation){ 0,0,0,0 };
	for(i=ATT_PRE; i<=ATT_BOD; i++) atts[i].c.value = 0;
	for(i=ATT_ATK; i<=ATT_RES; i++) atts[i].b.value = 0;
	for(i=ATT_HP; i<=ATT_SP; i++) atts[i].h.value = 0;
	for(i=0; i<SLOTS; i++) slot[i].index = -1;

	if(itm) free(itm);
	nitm = 0,itm = 0;
	if(skills) free(skills);
	nskills = 1;
	skills = (skill *)malloc(sizeof(skill)*nskills);
	skills[0] = (skill){ SKILL_UNARMED,0,0,0,0,0 };
	if(eff) free(eff);
	neff = 0,eff = 0;

	que[0] = 0,que[1] = 0,que[2] = 0,que[3] = 0;
	spr[0] = 0,spr[1] = 1,spr[2] = 0,spr[3] = 0;
	spri = spr[0];

	team = 0;
	/*update();
	if(isDead()) kill();
	else recharge();*/
}


///** Incarnate Pack. */
//struct incarnatepack {
	//uint8_t cmd;								/** < Package command. All packages should have this */
	//uint16_t len;							/** < Package length in bytes. All packages should have this */
	//uint32_t id;								/** < Player ID */
	//uint32_t locid;							/** < Location ID */
	//int16_t px,py;							/** < Point X & Y */
	//uint32_t xp;								/** < XP increase to Player */
	//uint8_t icon;							/** < Player Icon */
	//uint8_t gen;								/** < Gender */
	//uint8_t race;							/** < Race */
	//uint16_t age;							/** < Age */
	//uint8_t skin;							/** < Incarnation skin */
	//uint8_t hair;							/** < Incarnation hair */
	//uint8_t face;							/** < Incarnation face */
	//int8_t atts[1+ATT_BOD];				/** < Attributes, PRE-BOD */
//};
void Player::receiveIncarnatePack(uint8_t *data) {
	uint8_t to;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	Player *pl = (Player *)Map::getItem(get_unpack_uint32(data));
	if(!pl) return;
	int i;
	data += 8;
	pl->st = ST_INCARNATED|ST_UPTODATE;

#ifdef WOTSERVER
	{
		uint32_t index = rnd.uint32(Map::locationids.size()),locid;
		type_t type;
		Location *l = (Location *)Map::locationids.getByIndex(index,type);
		Coordinate c(0,(int16_t)l->mx,(int16_t)l->my);
		locid = l->getID();
		pl->px = c.px,pl->py = c.py,data += 4;
	}
#else
	unpack_int32(&data,pl->px);
	unpack_int32(&data,pl->py);
#endif

	pl->setPoint(pl->px,pl->py);
	unpack_uint32(&data,pl->xp);
	unpack_uint8(&data,pl->icon);
app.printf("Player::receiveIncarnatePack(st=%d,mx=%d,my=%d,px=%d,py=%d)",pl->st,pl->mx,pl->my,pl->px,pl->py);
	pl->inc.num++;
	unpack_uint8(&data,pl->inc.gen);
	unpack_uint8(&data,pl->inc.race);
	unpack_uint16(&data,pl->inc.age);
	unpack_uint8(&data,pl->avt.skin);
	unpack_uint8(&data,pl->avt.hair);
	unpack_uint8(&data,pl->avt.face);
	for(i=ATT_PRE; i<=ATT_BOD; i++) unpack_uint8(&data,pl->atts[i].c.value);
	for(i=ATT_ATK; i<=ATT_RES; i++) pl->atts[i].b.value = 0;
	for(i=ATT_HP; i<=ATT_SP; i++) pl->atts[i].h.value = 0;

	const race &r = racedata[pl->inc.race];
	pl->deleteItems();
	pl->deleteEquipment();
	pl->createItems(r.g[pl->inc.gen].itm,r.g[pl->inc.gen].nitm);
	for(i=pl->items()-1; i>=0; i--) pl->equipItem(i);

app.printf("Player::receiveIncarnatePack(id=%lu) -> update",pl->getID());
	pl->update();
app.printf("Player::receiveIncarnatePack(id=%lu) -> recharge",pl->getID());
	pl->recharge();
app.printf("Player::receiveIncarnatePack(id=%lu) -> placeItem",pl->getID());
	pl->map->placeItem(*pl);
app.printf("Player::receiveIncarnatePack(id=%lu) -> done",pl->getID());
#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) pl->sendIncarnatePack(to);
#endif
}
/*
	pack_uint8(p,CMD_INCARNATE);
	p += 2;
	pack_uint8(p,to);
	pack_uint32(p,getID());
	pack_uint32(p,0);
	pack_int16(p,px);
	pack_int16(p,py);
	pack_uint32(p,xp);
	pack_uint8(p,icon);
app.printf("Player::sendIncarnatePack(st=%d,mx=%d,my=%d,px=%d,py=%d)",st,mx,my,px,py);
	pack_uint8(p,inc.gen);
	pack_uint8(p,inc.race);
	pack_uint16(p,inc.age);
	pack_uint8(p,avt.skin);
	pack_uint8(p,avt.hair);
	pack_uint8(p,avt.face);
	for(i=ATT_PRE; i<=ATT_BOD; i++) pack_uint8(p,atts[i].c.value);
	app.send(d,sizeof(d));
	// Is processed locally in IncarnateDlg.
*/

void Player::receiveTeamPack(uint8_t *data) {
	uint32_t cid,tid;
	uint8_t to,evt;
	data += SOCKET_HEADER;
	unpack_uint8(&data,to);
	unpack_uint32(&data,cid);
	unpack_uint32(&data,tid);
	unpack_uint8(&data,evt);
	Player *pl = (Player *)Map::getItem(cid);
	if(!pl) return;
	Team::handleEvent(tid,pl,evt);
#ifdef WOTSERVER
	if(to!=SEND_TO_SERVER) pl->sendTeamPack(to,tid,evt);
#endif
}

void Player::sendIncarnatePack(uint8_t to) {
	uint8_t d[SOCKET_HEADER+29+(1+ATT_BOD-ATT_PRE)],*p = d;
	int i;
	pack_header(&p,CMD_INCARNATE);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint32(&p,0);
	pack_int32(&p,px);
	pack_int32(&p,py);
	pack_uint32(&p,xp);
	pack_uint8(&p,icon);
app.printf("Player::sendIncarnatePack(st=%d,mx=%d,my=%d,px=%d,py=%d)",st,mx,my,px,py);
	pack_uint8(&p,inc.gen);
	pack_uint8(&p,inc.race);
	pack_uint16(&p,inc.age);
	pack_uint8(&p,avt.skin);
	pack_uint8(&p,avt.hair);
	pack_uint8(&p,avt.face);
	for(i=ATT_PRE; i<=ATT_BOD; i++) pack_uint8(&p,atts[i].c.value);
	app.send(d,sizeof(d));
	// Is processed locally in IncarnateDlg.
}

void Player::sendTeamPack(uint8_t to,uint32_t id,uint8_t evt) {
	uint8_t d[SOCKET_HEADER+10],*p = d;
	pack_header(&p,CMD_TEAM);
	pack_uint8(&p,to);
	pack_uint32(&p,getID());
	pack_uint32(&p,id);
	pack_uint8(&p,evt);
#ifdef WOTSERVER
	app.send(to,this,d,sizeof(d));
	if(to&SEND_TO_SERVER) Team::handleEvent(id,this,evt);
#else
	app.send(d,sizeof(d));
#endif
}


#ifdef WOTSERVER
void Player::pack(uint8_t **data) {
	pack_uint32(data,id);
	pack_uint32(data,0);
	pack_int32(data,px);
	pack_int32(data,py);
	pack_uint32(data,st);
	pack_uint8(data,icon);
	memcpy(*data,name,namelen+1);
	*data += namelen+1;
}

int Player::getPlayerSize() {
	return 31+(1+ATT_BOD-ATT_PRE)+(1+ATT_SP-ATT_HP)*2+sizeof_divinity+sizeof_incarnation+sizeof_avatar+
			namelen+1+sizeof_item*(SLOTS+nitm)+sizeof_skill*nskills+sizeof_effect*neff;
}

void Player::packPlayer(uint8_t **data) {
	int i;
	pack_uint32(data,id);
	pack_uint32(data,0);
	pack_int32(data,px);
	pack_int32(data,py);
	pack_uint8(data,spri);
	for(i=0; i<4; i++) pack_uint8(data,spr[i]);
	pack_uint8(data,icon);
	pack_uint32(data,xp);
	pack_uint8(data,lvl);
	pack_uint32(data,st);

	// atts
	for(i=ATT_PRE; i<=ATT_BOD; i++) pack_uint8(data,atts[i].c.value);
	for(i=ATT_HP; i<=ATT_SP; i++) pack_int16(data,atts[i].h.temp);

	// divinity
	pack_int16(data,div.num);
	pack_int8(data,div.align);
	for(i=0; i<4; i++) pack_uint8(data,div.el[i]);

	// incarnation
	pack_uint16(data,inc.num);
	pack_uint16(data,inc.age);
	pack_uint8(data,inc.gen);
	pack_uint8(data,inc.race);

	// avatar
	pack_uint8(data,avt.skin);
	pack_uint8(data,avt.hair);
	pack_uint8(data,avt.face);

	pack_uint16(data,nitm);
	pack_uint8(data,nskills);
	pack_uint8(data,neff);

	for(i=0; i<SLOTS; i++) {
		pack_int16(data,slot[i].index);
		pack_uint16(data,slot[i].qnt);
		pack_uint8(data,slot[i].dura);
		pack_uint16(data,slot[i].cool);
	}

	getData(data);
}
#endif

void Player::unpack(uint8_t **data) {
	unpack_uint32(data,id);
	*data += 4;
	unpack_int32(data,px);
	unpack_int32(data,py);
	unpack_uint32(data,st);
	unpack_uint8(data,icon);
	setName((const char *)*data);
	*data += namelen+1;

	setPoint(px,py);
	reset();
app.printf("Player::unpack(hp=%d)",atts[ATT_HP].h.sum);
}

#ifdef WOTCLIENT
void Player::unpackPlayer(uint8_t **data) {
	int i;
	unpack_uint32(data,id);
	*data += 4;
	unpack_int32(data,px);
	unpack_int32(data,py);
	unpack_uint8(data,spri);
	for(i=0; i<4; i++) unpack_uint8(data,spr[i]);
	unpack_uint8(data,icon);
	unpack_uint32(data,xp);
	unpack_uint8(data,lvl);
	unpack_uint32(data,st);

	// atts
	for(i=ATT_PRE; i<=ATT_BOD; i++) unpack_uint8(data,atts[i].c.value);
	for(i=ATT_HP; i<=ATT_SP; i++) unpack_int16(data,atts[i].h.temp);

	// divinity
	unpack_int16(data,div.num);
	unpack_int8(data,div.align);
	for(i=0; i<4; i++) unpack_uint8(data,div.el[i]);

	// incarnation
	unpack_uint16(data,inc.num);
	unpack_uint16(data,inc.age);
	unpack_uint8(data,inc.gen);
	unpack_uint8(data,inc.race);

	// avatar
	unpack_uint8(data,avt.skin);
	unpack_uint8(data,avt.hair);
	unpack_uint8(data,avt.face);

	unpack_uint16(data,nitm);
	unpack_uint8(data,nskills);
	unpack_uint8(data,neff);

	for(i=0; i<SLOTS; i++) {
		unpack_int16(data,slot[i].index);
		unpack_uint16(data,slot[i].qnt);
		unpack_uint8(data,slot[i].dura);
		unpack_uint16(data,slot[i].cool);
	}

	setPoint(px,py);
	setData(data);
app.printf("Player::unpackPlayer(name=%s,xp=%d,lvl=%d)",name,xp,lvl);

	update();
	updateAvatar();
}
#endif


void Player::getPlayerFile(aString &s) {
	int i;
	s.append("/* Player File v.0.1 - ID(").append(getID()).append(") Name(").append(getName()).
			append(") */\n[ /* Character */\n\t").append(0).append(',').
			append(px).append(',').append(py).append(",\n\t").append((int)spri).append(",[");
	for(i=0; i<4; i++) s.append((int)spr[i]).append(i<3? ',' : ']');
	s.append(",\n\t").append((int)icon).append(',').append(xp).append(',').append((int)lvl).
			append(',').append(st).append(",\n\t[");
	for(i=ATT_PRE; i<=ATT_BOD; i++) s.append((int)atts[i].c.value).append(',');
	for(i=ATT_HP; i<=ATT_SP; i++) s.append((int)atts[i].h.temp).append(i<ATT_SP? ',' : ']');
	s.append(",\n\t").append(div.num).append(",").append((int)div.align).append(",[");
	for(i=SPIRIT; i<=EARTH; i++) s.append((int)div.el[i]).append(i<EARTH? ',' : ']');
	s.append(",\n\t[").append(inc.num).append(',').
			append(inc.age).append(',').append((int)inc.gen).append(',').append((int)inc.race).append("],\n\t").
			append((int)avt.skin).append(',').append((int)avt.hair).append(',').append((int)avt.face).append(",\n\t[");
	for(i=0; i<SLOTS; i++) {
		item &m = slot[i];
		s.append("\n\t\t[").append(m.index).append(',').append(m.qnt).append(',').
			append((int)m.dura).append(',').	append((int)m.cool).append("]").append(i<SLOTS-1? ',' : '\0');
		if(m.index!=-1) s.append(" /* ").append(itemdata[m.index].name).append(" */");
	}
	s.append("\n\t],\n\t").append(nitm).append(',').append((int)nskills).append(',').append(neff).
			append("\n];\n");
	if(nitm) {
		s.append("[ /* Items");
		for(i=0; i<nitm; i++) {
			item &m = itm[i];
			s.append(" */\n\t[").append(m.index).append(',').append(m.qnt).append(',').append((int)m.dura).
				append(',').append((int)m.cool).append(" ]").append(i<nitm-1? ',' : '\0').
				append(" /* ").append(itemdata[m.index].name);
		}
		s.append(" */\n];\n");
	}
	if(nskills) {
		s.append("[ /* Skills");
		for(i=0; i<nskills; i++) {
			skill &sk = skills[i];
			s.append(" */\n\t[").append(sk.index).append(',').append(sk.xp).append(',').append((int)sk.lvl).append(',').
				append((int)sk.div).append(',').append((int)sk.sum).append(',').append(sk.cool).append("]").
					append(i<nskills-1? ',' : '\0').append(" /* ").append(skilldata[sk.index].name);
		}
		s.append(" */\n];\n");
	}
	if(neff) {
		s.append("[ /* Effects");
		for(i=0; i<neff; i++) {
			effect &e = eff[i];
			s.append(" */\n\t[").append(e.index).append(',').append((int)e.lvl).append(',').
				append((int)e.value).append(',').append(e.time).append("]").
				append(i<neff-1? ',' : '\0').append(" /* ").append(effectdata[e.index].name);
		}
		s.append(" */\n];\n");
	}
}

player_data *Player::parsePlayerFile(const char *p) {
	if(!p || !*p) return;
	int n = strlen(p),s = 0,i = 0,v = 0;
	char buf[n+1],*p1,*p2,c;
	memcpy(buf,p,n+1);
	for(p1=buf,p2=buf,n=0; (c=*p1)!='\0'; p1++) {
		switch(s) {
			case 0:
				if(c=='/') s = 1,v = 0;
				else if(c==';' || c==',') *p2++ = ',',v = 0;
				else if(c=='-') *p2++ = c;
				else if(c>='0' && c<='9') { if(!v) v = 1,i++;*p2++ = c; }
				else v = 0;
				break;
			case 1:if(c=='/') s = 2;else if(c=='*') s = 3;else *p2++ = '/',*p2++ = c;break;
			case 2:if(c=='\n' || c=='\r') *p2++ = c,s = 0;break;
			case 3:if(c=='*') s = 4;break;
			case 4:if(c=='/') s = 0;else s = 3;break;
		}
	}
	*p2 = '\0',p1 = buf;
app.printf("%s",buf);
	long data[i],l = 0,*d = data;
	for(i=0,v=0,s=0; *p1!='\0'; p1++) {
putc(*p1,stderr);
		if(*p1>='0' && *p1<='9') l = l*10+(*p1-'0'),v |= 1;
		else if(*p1=='-') v |= 2;
		else if(*p1==',') { if(v&1) data[i++] = (v&2)? -l : l;l = 0,v = 0; }
	}
app.printf("\ndata:");
for(n=0; n<i; n++) app.printf("data[%d]=%ld",n,data[n]);
	player_data *pd = (player_data *)malloc(sizeof(player_data));
	d++;
	pd->px = *d++;
	pd->py = *d++;
	pd->spri = *d++;
	for(i=0; i<4; i++) pd->spr[i] = *d++;
	pd->icon = *d++;
	pd->xp = *d++;
	pd->lvl = *d++;
	pd->st = *d++;
	for(i=0; i<=ATTRIBUTES; i++) pd->atts[i] = *d++;
	pd->div = *d++;
	pd->align = *d++;
	for(i=0; i<ELEMENTS; i++) pd->el[i] = *d++;
	pd->inc = *d++;
	pd->age = *d++;
	pd->gen = *d++;
	pd->race = *d++;
	pd->skin = *d++;
	pd->hair = *d++;
	pd->face = *d++;
	for(i=0; i<SLOTS; i++) {
		item &m = pd->slot[i];
		m.index = *d++,m.qnt = *d++,m.dura = *d++,m.cool = *d++;
	}
	pd->nitm = *d++;
	pd->nskills = *d++;
	pd->neff = *d++;
	if(pd->nitm) {
		pd->itm = (item *)malloc(pd->nitm*sizeof(item));
		for(i=0; i<pd->nitm; i++) {
			item &m = pd->itm[i];
			m.index = *d++,m.qnt = *d++,m.dura = *d++,m.cool = *d++;
		}
	} else pd->itm = 0;
	if(pd->nskills) {
		pd->skills = (skill *)malloc(pd->nskills*sizeof(skill));
		for(i=0; i<pd->nskills; i++) {
			skill &sk = pd->skills[i];
			sk.index = *d++,sk.xp = *d++,sk.lvl = *d++,sk.div = *d++,sk.sum = *d++,sk.cool = *d++;
		}
	} else skills = 0;
	if(pd->neff) {
		pd->eff = (effect *)malloc(pd->neff*sizeof(effect));
		for(i=0; i<pd->neff; i++) {
			effect &e = pd->eff[i];
			e.index = *d++,e.lvl = *d++,e.value = *d++,e.time = *d++;
		}
	} else eff = 0;
	return pd;
}

void Player::deletePlayerData(player_data *pd) {
	if(!pd) return;
	if(pd->nitm) free(pd->itm);
	if(pd->nskills) free(pd->skills);
	if(pd->neff) free(pd->eff);
	free(pd);
}

void Player::setPlayerData(player_data *pd) {
	if(!pd) return;
	px = pd->px;
	py = pd->py;
	spri = pd->spri;
	setPoint(px,py);
	for(i=0; i<4; i++) spr[i] = pd->spr[i];
	icon = pd->icon;
	xp = pd->icon;
	lvl = pd->lvl;
	st = pd->st|ST_UPTODATE;
	for(i=ATT_PRE; i<=ATT_BOD; i++) atts[i].c.value = pd->atts[i];
	for(i=ATT_HP; i<=ATT_SP; i++) atts[i].h.temp = pd->atts[i];
	div.num = pd->div;
	div.align = pd->align;
	for(i=SPIRIT; i<=EARTH; i++) div.el[i] = pd->el[i];
	inc.num = pd->inc;
	inc.age = pd->age;
	inc.gen = pd->gen;
	inc.race = pd->race;
	avt.skin = pd->skin;
	avt.hair = pd->hair;
	avt.face = pd->face;
	for(i=0; i<SLOTS; i++) slot[i] = pd->slot[i];
	nitm = pd->nitm;
	nskills = pd->nskills;
	neff = pd->neff;
	if(itm) free(itm);
	if(nitm) {
		itm = (item *)malloc(nitm*sizeof(item));
		for(i=0; i<nitm; i++) itm[i] = pd->itm[i];
	} else itm = 0;
	if(skills) free(skills);
	if(nskills) {
		skills = (skill *)malloc(nskills*sizeof(skill));
		for(i=0; i<nskills; i++) skills[i] = pd->skills[i];
	} else skills = 0;
	if(eff) free(eff);
	if(neff) {
		eff = (effect *)malloc(neff*sizeof(effect));
		for(i=0; i<neff; i++) eff[i] = pd->eff[i];
	} else eff = 0;
	update();
#ifdef WOTCLIENT
	updateAvatar();
#endif
}


