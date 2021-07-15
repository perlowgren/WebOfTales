
#include "../config.h"
#include <SDL/SDL_stdinc.h>
#include "Enum.h"
#include "Skill.h"
#include "Effect.h"


/*
enum {
	SK_MELEE,
	SK_RANGED,
	SK_INSTANT,
	SK_CONSTANT,
	SK_PASSIVE,
};

enum {
	SKILL_UNARMED,
	SKILL_DAGGER,
};

struct skill_prototype {
	const char *name;
	uint32_t type;
	struct skill_level {
		uint16_t time;
		uint16_t cool;
		struct {
			uint16_t index;
			uint8_t lvl;
		} eff[5];
	} lvl[11];
};
*/
const skill_prototype skilldata[] = {
	{ "Unarmed",SK_MELEE,{
		{ 12,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{ 10,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  9,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  8,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  7,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  6,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  5,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  4,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  3,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  2,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  1,0,{{EFF_HP_DAMAGE,0},{-1}} }
	}},
	{ "Dagger",SK_MELEE,{
		{ 12,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{ 10,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  9,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  8,0,{{EFF_HP_DAMAGE,0},{-1}} },
		{  7,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,0},{-1}} },
		{  6,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,1},{-1}} },
		{  5,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,2},{-1}} },
		{  4,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,3},{-1}} },
		{  3,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,4},{-1}} },
		{  2,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,5},{-1}} },
		{  1,0,{{EFF_HP_DAMAGE,0},{EFF_HP_DRAIN,6},{-1}} }
	}},
};


const char *skill_level_names[11] = {
	"Unskilled","Layman","Apprentice","Journeyman","Master","Grandmaster","Hero","Epic Hero","Legend","Deity","Supreme Deity"
};


