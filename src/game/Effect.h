#ifndef _GAME_EFFECT_H
#define _GAME_EFFECT_H

enum {
	EFF_INSTANT			= 0x0001,		// Hits instantly and then disappears
	EFF_PERSISTENT		= 0x0002,		// Remains over a certain time period
	EFF_CONSTANT		= 0x0004,		// Remains until canceled

	EFF_STACKABLE		= 0x0008,		// Several of the same effect can remain at the same time

	EFF_BUFF				= 0x0010,		// Increase a certain attribute or stat
	EFF_DEBUFF			= 0x0020,		// Decrease a ceratin attribute or stat
	EFF_HEAL				= 0x0030,		// Heals a certain stat
	EFF_DAMAGE			= 0x0040,		// Damage a ceratin stat
	EFF_GUARANTEE		= 0x0060,		// Guarantee the success of a certain skill
	EFF_NEGATE			= 0x0070,		// Remove an effect
	EFF_CONDITION		= 0x0080,		// Set a certain condition to character status
};

enum {
	EFF_HP_DAMAGE,							// Effect does instant damage
	EFF_HP_DRAIN,							// Effect does drain damage over a specific time
};

struct effect_prototype {
	const char *name;
	uint8_t icon;
	uint32_t type;
	uint8_t v1;
	uint8_t v2;
	uint16_t lvl[11];
};


struct effect {
	uint16_t index;
	uint8_t lvl;
	uint8_t value;
	int16_t time;
};

#define sizeof_effect 6

extern const effect_prototype effectdata[];


#endif /* _GAME_EFFECT_H */
