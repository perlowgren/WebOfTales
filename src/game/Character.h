#ifndef _GAME_CHARACTER_H
#define _GAME_CHARACTER_H

#include <stdio.h>
#include <libamanita/aPath.h>
#include "../command.h"
#include "Map.h"
#include "Enum.h"
#include "Item.h"
#include "Skill.h"
#include "Effect.h"


#define N_SPRITES 4

class Location;


/** The sprite struct contains information for character-sprites' animations and dimensions etc. */
struct sprite {
	int length;								/** < Number of frames in the animation. */
	int w,h;									/** < Dimension of the sprite's images. */
	int basex,basey;						/** < Center location of the sprite in relation to upper left. */
	int l,t,r,b;							/** < Dimension of the area that can be touched by the mouse on the screen. */
	int frames[32];						/** < Frames in animation, values are the index rects in the image. */
	/** Shadow */
	struct {
		int x,y,shape;						/** < Location and shape index in image of the sprite-shadow. */
	} shadow;
};


class Character : public Coordinate {
friend class Map;
friend class MapDisplay;
friend class CharacterDlg;
friend class BattleDlg;

aObject_Instance(Character)

public:
	static const sprite sprites[];	/** < All sprites used in the game */

	uint16_t key;							/** < Key, used in Map for the hashtable */
	Character *next;						/** < Next item in the slot in the hashmap */

private:
	bool contains(int l,int t,int r,int b,int x,int y) { return l<=x && t<=y && r>=x && b>=y; }
	bool intersects(int l1,int t1,int r1,int b1,int l2,int t2,int r2,int b2) { return (l1<=r2 && r1>=l2 && t1<=b2 && b1>=t2); }

protected:
	/** An enemy to the character, defined by amount of aggro. */
	struct enemy {
		Character *target;				/** < Target Character */
		uint16_t dmg;						/** < Damage caused to this Character by target Character */
		uint16_t aggro;					/** < Aggro based on amount of damage caused by target */
	};

	/** Character Attribute, union */
	union attribute {
		/** Basic Character Attribute */
		struct {
			uint8_t value;					/** < Basic value of the attribute */
			uint8_t el;						/** < Elemental points added  */
			uint8_t sum;					/** < Sum of attribute and modifiers */
			uint8_t mind;					/** < Sum of attribute and modifiers, restricted by mind attribute */
			uint8_t body;					/** < Sum of attribute and modifiers, restricted by body attribute */
		} c;
		/** Battle Attribute */
		struct {
			int16_t value;					/** < Basic value of the attribute */
			int16_t sum;					/** < Sum of attribute and modifiers */
		} b;
		/** Health Attribute */
		struct {
			int16_t value;					/** < Basic value of the attribute */
			int16_t sum;					/** < Sum of attribute and modifiers */
			int16_t temp;					/** < Temporary value */
			int16_t reg;					/** < Regeneration */
		} h;
	} atts[ATTRIBUTES];

	uint32_t id;							/** < ID of the item */
	char *name;								/** < Name of the item */
	uint8_t namelen;						/** < Length of name */
	uint8_t icon;							/** < Icon index */

	uint16_t nitm;							/** < Number of Items */
	item *itm;								/** < Items */
	Location *loc;							/** < Location container is at */

	uint32_t xp;							/** < Experience */
	uint8_t lvl;							/** < Level */
	uint32_t st;							/** < Status of the Character, also condition */

	uint8_t nskills;						/** < Number of skills */
	skill *skills;							/** < Skills */
	uint8_t sk;								/** < Skill to use */
	uint8_t attack;						/** < Melee or ranged attack skill */
	uint8_t defend;						/** < Melee defend skill */

	uint8_t neff;							/** < Number of effects */
	effect *eff;							/** < Effects */

	uint8_t spr[N_SPRITES];				/** < Sprites 0=Standing, 1=Walking, 2=Attacking, 3=Dead */
	uint8_t sprtm;							/** < Sprite time, used for animating synchronized with game cycles */
	uint8_t act;							/** < Action */
	int16_t timer;							/** < Timer for present action */
	uint8_t dir;							/** < Direction character is facing */
	aTrail *trail;							/** < aTrail of steps returned by the path maker */
	Coordinate dest;						/** < Destination for moving */
	int32_t dx,dy;							/** < Destination x,y - Next step in the trail */
	Coordinate *target;					/** < Target character is focusing on */
	enemy *enms;							/** < Enemies */
	uint8_t nenms;							/** < Number of enemies */

	virtual bool setCharacter(uint16_t index,uint32_t v1,uint32_t v2,const uint8_t *data=0,size_t len=0);

public:
	uint8_t spri;								/** < Sprite index */


	Character(Map *m,uint32_t id);
	virtual ~Character();


	void setID(uint32_t i) { id = i; }
	uint32_t getID() { return id; }
	void setName(const char *nm);
	const char *getName() { return name; }
	int getNameLength() { return namelen; }
	void setIcon(int i) { icon = i; }
	int getIcon() { return icon; }

#ifdef WOTCLIENT
	int getColour(Character &c);
	virtual void paint(viewpoint &v);
#endif

	void setStatus(uint32_t s) { st |= s; }
	uint32_t getStatus() { return st; }
	void removeStatus(uint32_t s) { st |= s,st ^= s; }
	bool hasStatus(uint32_t s) { return (st&s)==s; }
	bool isUpToDate() { return st&ST_UPTODATE; }
	bool isRequesting() { return st&ST_REQUESTING; }
	bool isIncarnated() { return st&ST_INCARNATED; }
	bool isDead() { return st&ST_DEAD; }
	bool isUndead() { return st&ST_UNDEAD; }
	bool isUnconscious() { return st&ST_UNCONSCIOUS; }
	bool isDiseased() { return st&ST_DISEASED; }
	bool isPoisoned() { return st&ST_POISONED; }
	bool isParalyzed() { return st&ST_PARALYZED; }
	bool isCursed() { return st&ST_CURSED; }
	bool isPossessed() { return st&ST_POSSESSED; }

	int getLevel() { return lvl; }
	const attribute &getAttribute(int a) { return atts[a]; }
	int getXP() { return xp; }
	int getXP(int n) { return n*(xp&0xfffff)/0x100000; }
	int getAttackXP() { return skills[attack].xp; }
	int getAttackXP(int n) { return n*(skills[attack].xp&0xfffff)/0x100000; }
	int getHP() { return atts[ATT_HP].h.temp; }
	int getHP(int n) { return n*atts[ATT_HP].h.temp/atts[ATT_HP].h.sum; }
	int getLP() { return atts[ATT_LP].h.temp; }
	int getLP(int n) { return n*atts[ATT_LP].h.temp/atts[ATT_LP].h.sum; }
	int getSP() { return atts[ATT_SP].h.temp; }
	int getSP(int n) { return n*atts[ATT_SP].h.temp/atts[ATT_SP].h.sum; }

	void createItem(const item &i);
	void createItems(const item *i,int n);
	void addItem(const item &i);
	void addItems(const item *i,int n);
	item &getItem(int i) { return itm[i]; }
	item *getItems() { return itm; }
	int items() { return nitm; }
	void swapItems(int i1,int i2);
	void deleteItem(int i);
	void deleteItems();

	void addSkill(skill &s);
	int getSkills() { return nskills; }

	int getEffects() { return neff; }

	void setLocation(Location *l);

	bool contains(int x2,int y2);
	bool intersects(Character &c);
	bool intersects(int l,int t,int r,int b);

#ifdef WOTSERVER
	int stand() { setAction(ACT_STAND,(px<<16)|py,0,0);return 1; }
	int battle(Character &c) { setAction(ACT_BATTLE,(px<<16)|py,0,c.getID());return 1; }
	int talk(Character &c) { setAction(ACT_TALK,(px<<16)|py,0,c.getID());return 1; }
	int teleport(uint32_t locid,int32_t x,int32_t y) { setAction(ACT_TELEPORT,locid,(x<<16)|y,0);return 1; }
	int suicide() { setAction(ACT_SUICIDE,(px<<16)|py,0,0);return 1; }
	int setAction(int a) { setAction(a,(px<<16)|py,0,0);return 1; }
	int setAction(int a,Character &c) { setAction(a,(px<<16)|py,0,c.getID());return 1; }
#endif
	int getAction() { return act; }
	bool isBusy() { return timer>0; }
	bool isReady() { return timer<=0; }

	bool moveTo(uint32_t id,int32_t x,int32_t y);
	int velocity(int x,int y);
	static int pathMoveCost(int x,int y,int c,void *map,void *obj);

	Coordinate *getDest() { return &dest; }
	Coordinate *getTarget() { return target; }
	aTrail *getTrail() { return trail; }

	virtual void pushSkillQue(uint8_t s);
	virtual void shiftSkillQue();
#ifdef WOTSERVER
	int useSkill(time_t cycles);
	int skillChance(int s);

	void enterLocation(Location *l);
#endif
	void animate();

	virtual item *getWeapon() { return 0; }
	virtual int getDamage() { return 0; }

	int calculateXP(int l1,int l2);
	int calculateSkillXP(int lvl,int eff,int dif);
	virtual void addXP(int xp,int sk=-1);
	void addAggro(Character &ch,int dmg);

	virtual void update();
#ifdef WOTSERVER
	void updateStatus(time_t cycles);
#endif
	void recharge();
	virtual void cycle(time_t cycles);
	void kill();
	virtual void die();

	void setAction(uint8_t a,uint32_t p1,uint32_t p2,uint32_t p3);
	void setEffect(uint16_t index,uint8_t lvl,uint8_t value,uint16_t time,uint8_t v1=0,uint8_t v2=0,uint16_t v3=0);

	static void receiveActionPack(uint8_t *data);
#ifdef WOTCLIENT
	static void receiveBattlePack(uint8_t *data);
#endif
	static void receiveCharacterPack(uint8_t *data);
	static void receiveEffectPack(uint8_t *data);
	static void receiveLocationPack(uint8_t *data);

	void sendActionPack(uint8_t to,uint8_t a,uint32_t p1,uint32_t p2,uint32_t p3);
	void sendCharacterPack(uint8_t to,uint16_t index,uint32_t v1,uint32_t v2,uint8_t *data=0,size_t len=0);
	void sendLocationPack(uint8_t to,Location *l);
#ifdef WOTSERVER
	void sendBattlePack(uint8_t to,Character &c,Character &ct,uint8_t type,uint16_t effect,int8_t timer);
	void sendCommandPack(uint8_t to,uint8_t cmd,uint32_t v=0);
	void sendEffectPack(uint8_t to,uint16_t index,uint8_t lvl,uint8_t value,uint16_t time,uint8_t v1,uint8_t v2,uint16_t v3);
#endif

#ifdef WOTSERVER
	void getData(uint8_t **data);
#endif
	void setData(uint8_t **data);
};




#endif /* _GAME_CHARACTER_H */
