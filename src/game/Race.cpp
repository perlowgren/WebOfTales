
#include "../config.h"
#include <SDL/SDL_stdinc.h>
#include "Enum.h"
#include "Race.h"


// struct race {
// 	const char *name;				/** < Race name. */
// 	int lvl;							/** < Level in the race hierarchy. */
// 	int cost;						/** < Divinity cost for incarnating as. */
// 	int reqrace;					/** < Can only reincarnate from this race. */
// 	int reqel[5];					/** < Require these elemental values to gain access. */
// 	int reqskill;					/** < Require this skill to gain access. */
// 	int reqali;						/** < Required alignment of the character soul. */
// 	int attpts;						/** < Attribute points, given attributes are subtracted from this value. */
// 	int atts[12];					/** < Attributes given by default. */
// 	int skills[5];					/** < Skills given by this race. Skill level is Unskilled so it has to be trained. */
// 	struct gender {
// 		int icon[33];				/** < Icons this race can use. */
// 		int skin[11];				/** < Skin colours this race have. */
// 		int hair[33];				/** < Hairs this race can have. */
// 		int face[33];				/** < Faces this race can have. */
// 		int nitm;					/** < Number of items this race incarnate with. */
// 		item itm[10];				/** < Items this race start with after incarnation. */
// 	} g[2];
// 
// 	const char *notes[5];		/** < Description of the race. */
// };
// enum {
// 	ITEM_W_TUNIC,
// 	ITEM_W_DRESS,
// 	ITEM_W_HOSES,
// 	ITEM_L_SHOES,
// 	ITEM_W_GUGEL,
// 	ITEM_W_CLOAK,
// 	ITEM_DAGGER,
// };

const race racedata[] = {
	{ "Sethian",1,-1,{ 0,0,0,0,0 },0,0,18,{ 2,1,2,1,1,2,1,0,0,0,6,6,6 },{ 0,0,0,0,0 },{1,2,3,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Cainite",1,-1,{ 0,0,0,0,0 },0,0,18,{ 1,2,1,1,2,1,2,0,0,0,6,6,6 },{ 0,0,0,0,0 },{1,2,3,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_L_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_L_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Nephilim",100,CAINITE,{ 6,4,3,4,3 },0,0,44,{ 4,2,4,2,3,4,4,0,0,0,12,12,12 },{ 0,0,0,0,0 },{5,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Duerg",2,-1,{ 0,0,0,0,0 },0,0,19,{ 1,2,1,1,3,1,3,0,0,0,0,0,0 },{ 0,0,0,0,0 },{4,6,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Alf",2,-1,{ 0,0,0,0,0 },0,0,19,{ 3,1,2,2,1,3,1,0,0,0,0,0,0 },{ 0,0,0,0,0 },{5,-1},{
		{ {1,2,3,-1},{0x51,0x52,0x50,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x51,0x52,0x50,-1},{1,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Ljosalf",100,ALF,{ 0,0,0,0,0 },0,0,32,{ 4,1,3,3,1,4,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{5,-1},{
		{ {1,2,3,-1},{0x51,0x52,0x50,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x51,0x52,0x50,-1},{1,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Svartalf",10,ALF,{ 0,0,0,0,0 },0,0,32,{ 3,2,3,3,2,3,3,0,0,0,12,12,12 },{ 0,0,0,0,0 },{6,-1},{
		{ {1,2,3,-1},{0x61,0x62,0x60,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x61,0x62,0x60,-1},{1,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_S_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Thurs",100,-1,{ 0,0,0,0,0 },0,0,20,{ 1,2,1,1,3,1,3,0,0,0,12,12,12 },{ 0,0,0,0,0 },{6,4,9,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Moroi",100,-1,{ 0,0,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{1,2,3,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Strigoi",100,MOROI,{ 0,0,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{4,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Varcolac",100,MOROI,{ 0,0,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{6,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{1,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{1,41,42,43,44,45,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Liche",100,-1,{ 0,0,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{4,-1},{
		{ {1,2,3,-1},{0,-1},{0,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0,-1},{0,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},

	{ "Daimon",100,-1,{ 0,0,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{7,-1},{
		{ {1,2,3,-1},{0x71,0x72,0x70,-1},{31,32,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x70,-1},{71,72,73,74,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Djinn",100,DAIMON,{ 0,1,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{8,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{31,32,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{71,72,73,74,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Marid",100,DJINN,{ 0,3,0,1,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{8,-1},{
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0x14,0x15,0,-1},{31,32,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x10,0x11,0x12,0x13,0,-1},{71,72,73,74,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
	{ "Ifrit",100,DJINN,{ 0,5,0,0,0 },0,0,8,{ 1,1,1,1,1,1,1,0,0,0,12,12,12 },{ 0,0,0,0,0 },{7,-1},{
		{ {1,2,3,-1},{0x71,0x72,0x70,-1},{31,32,-1},
			5,{{ITEM_W_DRESS,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} },
		{ {1,2,3,-1},{0x70,-1},{71,72,73,74,-1},
			5,{{ITEM_W_TUNIC,1},{ITEM_W_HOSES,1},{ITEM_L_SHOES,1},{ITEM_W_CLOAK,1},{ITEM_DAGGER,1}} }
	},{"",0}},
};



