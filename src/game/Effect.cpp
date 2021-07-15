
#include "../config.h"
#include <SDL/SDL_stdinc.h>
#include "Enum.h"
#include "Effect.h"


/*
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
*/
const effect_prototype effectdata[] = {
	{ "ATT_HP Damage",		0,	EFF_INSTANT|EFF_DAMAGE,			CH_STAT,ATT_HP,	{} },
	{ "ATT_HP Drain",			0,	EFF_PERSISTENT|EFF_DAMAGE,		CH_STAT,ATT_HP,	{} },
};

