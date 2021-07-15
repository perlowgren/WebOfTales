
#include "../config.h"
#include <SDL/SDL_stdinc.h>
#include "Enum.h"
#include "Item.h"
#include "Skill.h"



const item_prototype itemdata[] = {
	{ "Wool Tunic",1,{0,2},SLOT_CHEST,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_WOOL,ITEM_QTY_COMMON,20,1000,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical tunic, worn by","most commoners.",0} },

	{ "Wool Dress",2,{1,0},SLOT_CHEST,{0,FEMALE,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_WOOL,ITEM_QTY_COMMON,20,2000,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical dress, worn by","most common females.",0} },

	{ "Wool Hoses",3,{1,2},SLOT_LEGS,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_WOOL,ITEM_QTY_COMMON,20,1000,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical hoses, worn by","most commoners.",0} },

	{ "Leather Shoes",4,{1,2},SLOT_FEET,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_LEATHER,ITEM_QTY_COMMON,20,500,0,0,
			{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical shoes, worn by","most commoners.",0} },

	{ "Wool Gugel",5,{1,41},SLOT_HEAD,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_WOOL,ITEM_QTY_COMMON,20,500,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical hat, worn by","most commoners.",0} },

	{ "Wool Cloak",5,{1,1},SLOT_BACK,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_WOOL,ITEM_QTY_COMMON,20,3000,0,0,
			{0,0,0,0,0,0,0,0,2,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical cloak, worn by","most commoners.",0} },

	{ "Linen Cloak",5,{2,2},SLOT_BACK,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_LINEN,ITEM_QTY_COMMON,20,3000,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical cloak, worn by","most commoners.",0} },

	{ "Silk Cloak",5,{3,3},SLOT_BACK,{0,-1,{0,0,0,0,0,0,0},-1,0},
			ITEM_ARMOR,ITEM_MAT_SILK,ITEM_QTY_ARTISAN,20,3000,0,0,
			{0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"Typical cloak, worn by","most commoners.",0} },

	{ "Dagger",6,{0,0},SLOT_RHAND,{0,-1,{0,0,0,0,0,0,0},SKILL_DAGGER,0},
			ITEM_MELEE,ITEM_MAT_STEEL,ITEM_QTY_COMMON,20,200,1,0,
			{0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,0,0},{{-1}},
			{"A normal dagger used mostly","as a tool in everyday","life, though sometimes","as a weapon.",0} },
};


