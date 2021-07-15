#ifndef _GAME_ENUM_H
#define _GAME_ENUM_H


/** Items Types */
enum ITEM_TYPE {
	ITEM_USELESS			= 0,		/** < Item cannot be used in any way in the game, may have some value though */
	ITEM_ARMOR				= 1,		/** <  Armor worn, increase the ARM attribute */
	ITEM_MELEE				= 2,		/** <  Meleeweapon */
	ITEM_RANGED				= 4,		/** <  Ranged weapon */
	ITEM_DEFENCE			= 8,		/** <  Defenceitem, such as a shield */
	ITEM_TOOL				= 16,		/** <  Tool of some kind */
	ITEM_RESOURCE			= 32,		/** <  Can be used as a resource, for creation of other items */
	ITEM_FOOD				= 64,		/** <  To be eaten */
	ITEM_QUEST				= 128,	/** <  Questitem, to be used in some quest */
};

/** Item Materials */
enum ITEM_MATERIAL {
	ITEM_MAT_HEMP			= 0x00000001,
	ITEM_MAT_LINEN			= 0x00000002,
	ITEM_MAT_WOOL			= 0x00000004,
	ITEM_MAT_SILK			= 0x00000008,
	ITEM_MAT_LEATHER		= 0x00000010,
	ITEM_MAT_RINGMAIL		= 0x00000020,
	ITEM_MAT_SCALEMAIL	= 0x00000040,
	ITEM_MAT_PLATE			= 0x00000080,

	ITEM_MAT_PAPER			= 0x00000100,
	ITEM_MAT_WOOD			= 0x00000200,
	ITEM_MAT_STONE			= 0x00000400,
	ITEM_MAT_CLAY			= 0x00000800,
	ITEM_MAT_PORCELAIN	= 0x00001000,
	ITEM_MAT_TIN			= 0x00002000,
	ITEM_MAT_COPPER		= 0x00004000,
	ITEM_MAT_BRASS			= 0x00008000,
	ITEM_MAT_IRON			= 0x00010000,
	ITEM_MAT_STEEL			= 0x00020000,
	ITEM_MAT_CRYSTAL		= 0x00040000,
	ITEM_MAT_PEARL			= 0x00080000,
	ITEM_MAT_RUBY			= 0x00100000,
	ITEM_MAT_DIAMOND		= 0x00200000,
};

/** Item Qualities */
enum ITEM_QUALITY {
	ITEM_QTY_BROKEN		= 0,
	ITEM_QTY_POOR,
	ITEM_QTY_COMMON,
	ITEM_QTY_ARTISAN,
	ITEM_QTY_MASTER,
	ITEM_QTY_SUPERIOR,
	ITEM_QTY_SACRED,
	ITEM_QTY_EPIC,
	ITEM_QTY_LEGENDARY,
	ITEM_QTY_MYTHICAL,
	ITEM_QTY_DIVINE,
};

/** Gender Types */
enum GENDER {
	FEMALE		= 0,	/** < Male */
	MALE			= 1,	/** < Female */
};

/** Character Statuses */
enum CHARACTER_STATUS {
	ST_UPTODATE			= 0x00000001,	/** < Used by aClient. Set when a Character is loaded completely from server */
	ST_REQUESTING		= 0x00000002,	/** < Used by aClient. Set when requested from server but not yet loaded, to avoid sending another request */

	ST_INCARNATED		= 0x00000100,	/** < Only used for Players */
	ST_DEAD				= 0x00000200,	/** < Dead, a ghost, has to reincarnate, turn undead or revive by magic */
	ST_UNDEAD			= 0x00000400,	/** < Undead, risen after death by magic or some disease */
	ST_UNCONSCIOUS		= 0x00000800,	/** < Unconscious, cannot make conscious actions until awakened */
	ST_DISEASED			= 0x00001000,	/** < Diseased, some kind of disease decreases attributes */
	ST_POISONED			= 0x00002000,	/** < Poisoned, some kind of poison decreases attributes */
	ST_DRUNK				= 0x00004000,	/** < Drunk from consuming alcohol */
	ST_TRANCE			= 0x00008000,	/** < In a trance state */
	ST_PSYCHEDELIC		= 0x00010000,	/** < In a psychedelic state of mind */
	ST_PARALYZED		= 0x00020000,	/** < Paralyzed, decrease attributes */
	ST_TERRORIZED		= 0x00040000,	/** < Very scared */
	ST_CURSED			= 0x00080000,	/** < Cursed, decrease attributes */
	ST_POSSESSED		= 0x00100000,	/** < Possessed by some kind of entity */
	ST_INSANE			= 0x00200000,	/** < Insane, mad, crazy */
	ST_DISARMED			= 0x00400000,	/** < Weapon is disarmed */
	ST_BERSERK			= 0x00800000,	/** < Turned berserk */
};

/** Used for determining what type of value a package contain, and how to handle it. */
enum {
	CH_ELEM					= 0,				/** < Elements */
	CH_ATT					= 1,				/** < Attributes */
	CH_STAT					= 2,				/** < Stats */
	CH_SKILL					= 3,				/** < Skills */

	CH_UPGRADE_ELEM		= 100,			/** < Upgrade Element, v1=Element, v2=Increase */

	CH_XP						= 200,			/** < Add Character XP, v1=Amount of XP, v2=Skill index, <0 Character XP */
	CH_UPDATE				= 201,			/** < Update Character */

	CH_EQUIP_ITEM			= 1000,			/** < Eqiup an item, v1=Item index, v2=Slot */
	CH_UNEQUIP_ITEM		= 1001,			/** < Unequip an item, v1=Slot, v2=Item index */

	CH_KILL					= 2000,			/** < Kill character */
};

/** Elements */
enum ELEMENT {
	SPIRIT,		/** < Spirit */
	FIRE,			/** < Fire */
	AIR,			/** < Air */
	WATER,		/** < Water */
	EARTH,		/** < Earth */
	ELEMENTS		/** < Number of Elements */
};

/** Character Attributes */
enum ATTRIBUTE {
	ATT_PRE,			/** < Presence				- Personality, charisma and artistry - Spirit */
	ATT_POW,			/** < Power					- Will and strength - Fire */
	ATT_PER,			/** < Perception			- Awareness, intelligence, alertness and wits - Air */
	ATT_AGI,			/** < Agility				- Dexterity and quickness, also psyche and social competence - Water */
	ATT_CON,			/** < Constitution		- Wellness, endurance and concentration - Earth */
	ATT_MIN,			/** < Mind,					- Mind and body generally determine how much of the previous attributes are.. */
	ATT_BOD,			/** < Body,						mental or physical. e.g. POW with high MIN means a strong will, while
															POW with high BOD means physical strength. */
	ATT_ATK,			/** < Attack				- Basic damage accumulated by attributes and level */
	ATT_DEF,			/** < Defence				- Defence value, ability to naturally avoid getting hit */
	ATT_RES,			/** < Resistance			- Resistance value, ability to naturally avoid magical influence */

	ATT_HP,			/** < Health Points		- Physical health, when it reaches zero the character becomes
															unconscious, when it goes below zero the character dies. */
	ATT_LP,			/** < Life Points			- Life energy, when it goes below zero the character become insane. */
	ATT_SP,			/** < Stamina Points		- Physical stamina, when it reaches zero the character
															becomes exhausted and cannot move, when it goes below zero
															the character becomes unconscious. */
	ATTRIBUTES		/** < Number of Attributes */
};

/** Character actions */
enum ACTION {
	ACT_STAND,			/** < Standing */
	ACT_MOVE,			/** < Moving */
	ACT_TELEPORT,		/** < Teleport to location */
	ACT_TALK,			/** < Talking (not used) */
	ACT_BATTLE,			/** < In a battle */
	ACT_SKILL,			/** < Use Skill */
	ACT_SUICIDE,		/** < Commit suicide */
};

/** Skill Types */
enum SKILL_TYPE {
	SK_MELEE,			/** < Skill used in melee combat */
	SK_RANGED,			/** < Skill used in ranged combat */
	SK_INSTANT,			/** < The skill has instant effect */
	SK_CONSTANT,		/** < The skill has constant effect for the time of skill cooldown */
	SK_PASSIVE,			/** < The skill is in passive effect always */
};

/** Sprite Animations */
enum SPRITE {
	SPRITE_STAND,		/** < Standing */
	SPRITE_WALK,		/** < Walking */
	SPRITE_ATTACK,		/** < Attacking (not used) */
	SPRITE_DEAD,		/** < Dead (only used by NPCs) */
};

/** Alignment */
enum ALIGNMENT {
	LAW,					/** < Lawful */
	LIFE,					/** < Life */
	GOOD,					/** < Good */
	NEUTRAL,				/** < Neutral */
	EVIL,					/** < Evil */
	DEATH,				/** < Death */
	CHAOS,				/** < Chaotic */
};

/** Player Avatar Layers */
enum AVATAR {
	AVT_SKIN,			/** < Body */
	AVT_HAIR,			/** < Hair */
	AVT_FACE,			/** < Face */
	AVT_CHEST,			/** < Chest */
	AVT_LEGS,			/** < Legs */
	AVT_FEET,			/** < Feet */
	AVT_HEAD,			/** < Head */
	AVT_BACK,			/** < Back */
	AVT_RHAND,			/** < First Hand */
	AVT_LHAND,			/** < Second Hand */
	AVT_SPRITES,
};

/** Player Slots */
enum SLOT {
	SLOT_CHEST,			/** < Chest */
	SLOT_LEGS,			/** < Legs */
	SLOT_WAIST,			/** < Waist */
	SLOT_FEET,			/** < Feet */
	SLOT_HEAD,			/** < Head */
	SLOT_BACK,			/** < Back */
	SLOT_RHAND,			/** < First Hand */
	SLOT_LHAND,			/** < Second Hand */
	SLOT_NECK1,			/** < Neck 1 */
	SLOT_NECK2,			/** < Neck 2 */
	SLOT_RWRIST,		/** < First Wrist */
	SLOT_LWRIST,		/** < Second Wrist */
	SLOT_FINGER1,		/** < Finger 1 */
	SLOT_FINGER2,		/** < Finger 2 */
	SLOT_FINGER3,		/** < Finger 3 */
	SLOT_FINGER4,		/** < Finger 4 */
	SLOTS,				/** < Number of Slots */
};



#endif /* _GAME_ENUM_H */
