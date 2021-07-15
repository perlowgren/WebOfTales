
#include "../config.h"
#include <math.h>
#include <string.h>
#include <libamanita/net/aSocket.h>
#include "NPC.h"


aObject_Inheritance(NPC,Character);


/** NPC Prototypes */
// struct npc_prototype {
// 	const char *name;					/** < NPCs Name */
// 	unsigned char icon;				/** < Face icon */
// 	unsigned char xp;					/** < Experience */
// 	unsigned char lvl;				/** < Level */
// 	short atts[ATTRIBUTES];			/** < Attributes */
// 	unsigned char spr[4];			/** < Sprites */
// 	unsigned char nskills;			/** < Number of Skills */
// 	/** NPC Skills */
// 	struct {
// 		unsigned short index;		/** < Skill index */
// 		unsigned char lvl;			/** < Skill level */
// 	} skills[5];
// 	unsigned char attack;			/** < Attack skill index */
// 	unsigned char defend;			/** < Defend skill index */
// };
const npc_prototype NPC::npcdata[] = {
	{ "Jackal",			5,1, 1,{ 2,1,2,3,1,1,3, 0, 0, 0,12, 0,12 },{ 2,2,2,3 },
							1,{{SKILL_UNARMED,1}},0,0,1,{ 0,10,15, 5, 0, 0, 5 } },

	{ "Coyote",			5,1, 2,{ 2,2,3,3,1,2,3, 0, 0, 0,12, 0,12 },{ 2,2,2,3 },
							1,{{SKILL_UNARMED,1}},0,0,1,{ 0,15,10, 5, 0, 0, 5 } },

	{ "Vaetti",			4,1, 3,{ 2,2,2,3,2,2,3, 8, 8, 8,12, 0,12 },{ 4,4,5,6 },
							1,{{SKILL_DAGGER ,2}},0,0,2,{ 0, 5, 5,15, 0, 5, 0 } },

	{ "Hobgoblin",		4,1, 4,{ 2,3,2,3,2,2,3, 7, 7, 7,12, 0,12 },{ 4,4,5,6 },
							1,{{SKILL_DAGGER ,2}},0,0,2,{ 0, 5, 5,15, 0, 5, 0 } },

	{ "Wolf",			5,1, 5,{ 2,2,3,3,2,2,3,12,12,12,12, 0,12 },{ 2,2,2,3 },
							1,{{SKILL_UNARMED,2}},0,0,2,{ 0,10, 5,10, 0,15, 5 } },

	{ "Goblin",			4,1, 6,{ 2,3,2,3,2,2,3,12,12,12,12, 0,12 },{ 4,4,5,6 },
							1,{{SKILL_DAGGER ,3}},0,0,3,{ 0, 5, 5,15, 5,10, 5 } },

	{ "Timber Wolf",	5,1, 7,{ 2,3,3,3,3,2,3,10,10,10,12, 0,12 },{ 2,2,2,3 },
							1,{{SKILL_UNARMED,3}},0,0,3,{ 0,10, 0,15, 0,15, 0 } },

	{ "Kobold",			4,1, 8,{ 3,3,2,3,2,3,3,12,12,12,18,18,18 },{ 4,4,5,6 },
							1,{{SKILL_DAGGER ,3}},0,0,3,{ 0, 0, 0,10, 5, 5, 5 } },

	{ "Tikoloshe",		4,1, 9,{ 3,3,2,3,2,3,3,15,15,15,24,24,24 },{ 4,4,5,6 },
							1,{{SKILL_DAGGER ,3}},0,0,3,{ 0, 0, 5, 0, 5, 0, 5 } },

	{ "Dire Wolf",		5,1,10,{ 2,3,2,3,4,2,4,10,10,10,24, 0,24 },{ 2,2,2,3 },
							1,{{SKILL_UNARMED,4}},0,0,4,{ 0, 0, 0, 5, 0, 5, 0 } },
};


NPC::NPC(Map *m,uint8_t **data) : Character(m,0) {
	unpack(data);
}

NPC::~NPC() {}


void NPC::update() {
	int i;
	for(i=ATT_PRE; i<=ATT_BOD; i++) atts[i].c.el = atts[i].c.sum = 0;
	for(i=ATT_ATK; i<=ATT_RES; i++) atts[i].b.sum = 0;
	for(i=ATT_HP; i<=ATT_SP; i++) atts[i].h.sum = 0,atts[i].h.reg = 0;
	Character::update();
}


int NPC::getDamage() {
	return npcdata[index].damage;
}


#ifdef WOTSERVER
void NPC::pack(uint8_t **data) {
	pack_uint16(data,index);
	pack_int16(data,mx);
	pack_int16(data,my);
	pack_uint32(data,st);
}

//struct npc2 {
	//uint32_t id;						/** < NPC ID */
	//uint16_t index;					/** < NPC Prototype index */
	//uint16_t px,py;					/** < Point X & Y */
	//uint8_t spri;						/** < Sprite index */
	//uint16_t spr[4];					/** < Sprites */
	//uint16_t icon;					/** < Icon */
	//uint32_t xp;						/** < XP */
	//uint8_t lvl;						/** < Lvl */
	//uint16_t st;						/** < Status */
	//uint16_t nitm;					/** < Items */
	//uint8_t nskills;					/** < Skills */
	//uint8_t neff;						/** < Effects */
//};
int NPC::getNPCSize() {
	return 32+(1+ATT_BOD-ATT_PRE)+(1+ATT_SP-ATT_HP)*2+
			namelen+1+sizeof_item*nitm+sizeof_skill*nskills+sizeof_effect*neff;
}

void NPC::packNPC(uint8_t **data) {
	int i;
	pack_uint32(data,id);
	pack_uint16(data,index);
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

	pack_uint16(data,nitm);
	pack_uint8(data,nskills);
	pack_uint8(data,neff);

	getData(data);
}
#endif

void NPC::unpack(uint8_t **data) {
	int i;
	unpack_uint16(data,index);
	unpack_int16(data,mx);
	unpack_int16(data,my);
	unpack_uint32(data,st);

	setID(0x80000000+mx+my*map->mw);
	setMap(mx,my);
	const npc_prototype &n = npcdata[index];
	setName(n.name);
	icon = n.icon,xp = n.xp,lvl = n.lvl;
	for(i=ATT_PRE; i<=ATT_BOD; i++) atts[i].c.value = n.atts[i];
	for(i=ATT_ATK; i<ATT_RES; i++) atts[i].b.value = n.atts[i];
	for(i=ATT_HP; i<ATT_SP; i++) atts[i].h.value = n.atts[i];
	nskills = n.nskills;
	skills = (skill *)malloc(sizeof(skill)*nskills);
	for(i=0; i<nskills; i++) skills[0] = (skill){ n.skills[i].index,0,n.skills[i].lvl,0,0,0 };
	for(i=0; i<N_SPRITES; i++) spr[i] = n.spr[i];
	spri = spr[0];
	update();
	if(isDead()) kill();
	else recharge();
}

#ifdef WOTCLIENT
void NPC::unpackNPC(uint8_t **data) {
	int i;
	*data += 4;
	unpack_uint16(data,index);
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

	unpack_uint16(data,nitm);
	unpack_uint8(data,nskills);
	unpack_uint8(data,neff);

	setPoint(px,py);

	setData(data);
	update();
}
#endif

