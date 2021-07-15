#ifndef _GAME_NPC_H
#define _GAME_NPC_H

#include "Character.h"

/** NPC Types */
enum NPC_TYPE {
	NPC_JACKAL,
	NPC_COYOTE,
	NPC_VAETTI,
	NPC_HOBGOBLIN,
	NPC_WOLF,
	NPC_GOBLIN,
	NPC_TIMBER_WOLF,
	NPC_KOBOLD,
	NPC_DIRE_WOLF,
	NPC_MONSTERS,
};

/** NPC Prototypes */
struct npc_prototype {
	const char *name;						/** < NPCs Name */
	int icon;								/** < Face icon */
	int xp;									/** < Experience */
	int lvl;									/** < Level */
	int atts[ATTRIBUTES];				/** < Attributes */
	int spr[N_SPRITES];					/** < Sprites */
	int nskills;							/** < Number of Skills */
	/** NPC Skills */
	struct {
		int index;							/** < Skill index */
		int lvl;								/** < Skill level */
	} skills[5];
	int attack;								/** < Attack skill index */
	int defend;								/** < Defend skill index */
	int damage;								/** < Weapon damage. */
	int distribution[7];
};


#define sizeof_npc 10

/** Send NPC 2, send all player data. */
struct npc2 {
	uint32_t id;							/** < NPC ID */
	uint16_t index;						/** < NPC Prototype index */
	uint16_t px,py;						/** < Point X & Y */
	uint8_t spri;							/** < Sprite index */
	uint16_t spr[N_SPRITES];			/** < Sprites */
	uint16_t icon;							/** < Icon */
	uint32_t xp;							/** < XP */
	uint8_t lvl;							/** < Lvl */
	uint16_t st;							/** < Status */
	uint16_t nitm;							/** < Items */
	uint8_t nskills;						/** < Skills */
	uint8_t neff;							/** < Effects */
};


/** The NPC class.
 * This class represents the Non Player Characters in the game, and thus it extends the Character class.
 */
class NPC : public Character {
aObject_Instance(NPC)

public:
	static const npc_prototype npcdata[];

	uint16_t index;						/** < NPCdata index */

	NPC(Map *m,uint8_t **data);
	virtual ~NPC();

	void update();
	int getDamage();

#ifdef WOTSERVER
	void pack(uint8_t **data);
	int getNPCSize();
	void packNPC(uint8_t **data);
#endif

	void unpack(uint8_t **data);
#ifdef WOTCLIENT
	void unpackNPC(uint8_t **data);
#endif
};




#endif /* _GAME_NPC_H */
