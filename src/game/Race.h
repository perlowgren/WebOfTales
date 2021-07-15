#ifndef _GAME_RACE_H
#define _GAME_RACE_H


#include "Item.h"

/** Races, first part are the races numerically, second part bitmasks. */
enum RACE {
	SETHIAN,				/** < Human */
	CAINITE,				/** < Human */
	NEPHILIM,			/** < Half Human, Half Angel */
	DUERG,				/** < Dwarf */
	ALF,					/** < Elf */
	LJOSALF,				/** < Light Elf */
	SVARTALF,			/** < Dark Elf */
	THURS,				/** < Troll */
	MOROI,				/** < Witch */
	STRIGOI,				/** < Vampire */
	VARCOLAC,			/** < Werewolf */
	LICHE,				/** < Undead */
	DAIMON,				/** < Demon */
	DJINN,				/** < Genius */
	MARID,				/** <  */
	IFRIT,				/** < Efreet */
	RACES,

	_ALL_RACES			= 0xffffffff,
	_ELVES				= 0x00000001,
	_DEMONS				= 0x00000002,
};

struct race {
	const char *name;				/** < Race name. */
	int div;							/** < Divinity invested in this race that correlate to the given number
												of attribute points. */
	int reqrace;					/** < Can only reincarnate from this race. */
	int reqel[ELEMENTS];			/** < Require these elemental values to gain access. */
	int reqskill;					/** < Require this skill to gain access. */
	int reqali;						/** < Required alignment of the character soul. */
	int attpts;						/** < Attribute points, given attributes are subtracted from this value. */
	int atts[ATTRIBUTES];		/** < Attributes given by default. */
	int skills[5];					/** < Skills given by this race. Skill level is Unskilled so it has to be trained. */
	int skin[11];					/** < Skin colours this race have, same colour for both genders. */
	struct gender {
		int icon[33];				/** < Icons this race can use. */
		int hair[33];				/** < Hairs this race can have. */
		int face[33];				/** < Faces this race can have. */
		int nitm;					/** < Number of items this race incarnate with. */
		item itm[10];				/** < Items this race start with after incarnation. */
	} g[2];

	const char *notes[5];		/** < Description of the race. */
};

extern const race racedata[];


#endif /* _GAME_RACE_H */
