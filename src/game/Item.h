#ifndef _GAME_ITEM_H
#define _GAME_ITEM_H

#include "Enum.h"

enum {
	ITEM_W_TUNIC,
	ITEM_W_DRESS,
	ITEM_W_HOSES,
	ITEM_L_SHOES,
	ITEM_W_GUGEL,
	ITEM_W_CLOAK,
	ITEM_L_CLOAK,
	ITEM_S_CLOAK,
	ITEM_DAGGER,
};

struct item_prototype {
	const char *name;							/** < Item name */
	uint16_t icon;								/** < Icon index */
	uint16_t spr[2];							/** < Sprite used for avatar if worn; 0=Female, 1=Male */

	int8_t slot;								/** < Equip to slot, <0 can't equip */

	/** Required properties for using or wearing item. */
	struct {
		uint8_t lvl;							/** < Level required for this item */
		int8_t gen;								/** < Gender, <0 either gender can equip */
		uint8_t atts[1+ATT_BOD];			/** < Required attributes to use item PRE-BOD */
		int16_t skill;							/** < Required skill to use item, <0 no skill required */
		uint8_t sklvl;							/** < Required skill level to use item, 0=Skill is added on equip */
	} req;

	uint8_t type;								/** < Type of item */
	uint32_t mat;								/** < Material */
	uint8_t qty;								/** < Quality: 0=Broken,
																1=Poor, 2=Common, 3=Artisan, 4=Master, 5=Superior,
																6=Sacred, 7=Epic, 8=Legendary, 9=Mythical, 10=Divine */
	uint8_t dura;								/** < Item durability */
	uint32_t weight;							/** < Weight in gram */
	uint8_t speed;								/** < Time it takes to use item, game cycles */
	uint8_t cool;								/** < Time it takes for item to cool, seconds */
	int8_t atts[1+ATT_RES];					/** < Attribute raises, PRE-RES */
	uint8_t attmax[1+ATT_RES];				/** < Can't raise attribute above this value, or below if att is <0 */

	/** Effects */
	struct {
		int16_t index;							/** < Effect index */
		uint8_t lvl;							/** < Effect level */
	} eff[5];

	const char *notes[5];					/** < Notes that describe the item, last shoud be 0 */
};


struct item {
	int16_t index;								/** < Itemdata index, <0 not an item */
	uint16_t qnt;								/** < Quantity or charges */
	uint8_t dura;								/** < Item durability while wearing down */
	uint16_t cool;								/** < Item cooldown after used */
};

#define sizeof_item 7

extern const item_prototype itemdata[];


#endif /* _GAME_ITEM_H */
