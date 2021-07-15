#ifndef _GAME_SKILL_H
#define _GAME_SKILL_H


enum {
	SKILL_UNARMED,
	SKILL_DAGGER,
};


struct skill_prototype {
	const char *name;
	uint32_t type;							/** < Skill type */
	struct skill_level {
		uint16_t time;						/** < Time in gamecycles it takes to perform skill, there are 6 cycles per second. */
		uint16_t cool;						/** < Time in seconds it takes for skill to cool down, e.g. for spells. */
		/** Effects */
		struct {
			uint16_t index;				/** < Effect index */
			uint8_t lvl;					/** < Effect level */
		} eff[5];
	} lvl[11];
};

struct skill {
	uint16_t index;
	uint32_t xp;
	uint8_t lvl;							/** < Skill Level: 0=Unskilled,
														1=Layman, 2=Apprentice, 3=Journeyman, 4=Master, 5=Grandmaster */
	uint8_t div;							/** < Divinity levels:
														6=Hero, 7=Epic Hero, 8=Legend, 9=Deity, 10=Supreme Deity */
	uint8_t sum;							/** < Sum of lvl+div plus effects of other skills or items */
	uint16_t cool;							/** < Time for skill to cool to be used again. */
};

#define sizeof_skill 11

extern const skill_prototype skilldata[];
extern const char *skill_level_names[11];


#endif /* _GAME_SKILL_H */

