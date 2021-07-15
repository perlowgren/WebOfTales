#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H


#include "Character.h"
#include "Race.h"
#include "Team.h"

class aString;


/** Player Divinity. */
struct divinity {
	int16_t num;							/** < Unspent Divinity points. */
	int8_t align;							/** < Alignment af the soul. */
	uint8_t el[ELEMENTS];				/** < Elements */
};


/** Player Incarnation. */
struct incarnation {
	uint16_t num;							/** < Number of Incarnations */
	uint16_t age;							/** < Incarnation Age */
	uint8_t gen;							/** < Gender */
	uint8_t race;							/** < Race */
};


/** Player Avatar. */
struct avatar {
	uint8_t skin;							/** < Incarnation skin */
	uint8_t hair;							/** < Incarnation hair */
	uint8_t face;							/** < Incarnation face */
	uint8_t spr[AVT_SPRITES];			/** < Sprites */
};

#define sizeof_divinity (3+ELEMENTS)
#define sizeof_incarnation 6
#define sizeof_avatar 3
#define sizeof_player 17

struct player_data {
	long px,py;
	long spri;
	long spr[4];
	long icon;
	long xp;
	long lvl;
	long st;
	long atts[ATTRIBUTES];
	long div;
	long align;
	long el[ELEMENTS];
	long inc;
	long age;
	long gen;
	long race;
	long skin;
	long hair;
	long face;
	item *slot[SLOTS]
	long nitm;
	long nskills;
	long neff;
	item **itm;
	skill **skills;
	effect **eff;
};

/** The Player class.
 * This class represents the players character and thus extends the Character class.
 */
class Player : public Character {
friend class Team;
friend class Map;
friend class IncarnateDlg;
friend class CharacterDlg;
friend class ServerWorld;

aObject_Instance(Player)

public:
	static const int avatarSpritePaintOrder[4][11];
	static const int avatarSpriteHorizontalAdjust[4][10];
	static const int avatarSpriteVerticalAdjust[10];
	static const int avatarFrameVerticalAdjust[5];
	static const int avatarHeadCoverHair[121];

protected:
	divinity div;							/** < Divinity & Elements */
	incarnation inc;						/** < Incarnation */
	avatar avt;								/** < Avatar */
	item slot[SLOTS];						/** < Item Slots. */
	unsigned char que[4];				/** < Action que */
	Team *team;								/** < Team that character belongs to */

	void reset();
	bool setCharacter(uint16_t index,uint32_t v1,uint32_t v2,const uint8_t *data=0,size_t len=0);

public:
	Player(Map *m,uint32_t id,const char *nm,uint32_t locid,int16_t px,int16_t py,uint32_t st,uint8_t icon);
	Player(Map *m,uint8_t **data);
	virtual ~Player();

#ifdef WOTCLIENT
	void setAction(int x,int y);

	void paint(viewpoint &v);
#endif

	void setAvatar(avatar &a) { avt = a; }
	avatar &getAvatar() { return avt; }
#ifdef WOTCLIENT
	int setIcon(int i);
	int setSkin(int s);
	int setHair(int h);
	int setFace(int f);
	void updateAvatar();
	bool saveAvatarImage();
	void paintAvatarImage(int x,int y,int dir,int frame);
#endif

	void pushSkillQue(unsigned char s);
	void shiftSkillQue();

#ifdef WOTSERVER
	void addXP(int xp,int sk=-1);
#endif
	void update();

	int canEquipItem(item &i);
	int equipItem(int n);
	int unequipItem(int n);
	void deleteEquipment();

	item *getWeapon();
	int getDamage();


	void createTeam() { if(!team) sendTeamPack(SEND_TO_ALL,0,TEVT_CREATE); }
	void disbandTeam() { if(team) sendTeamPack(SEND_TO_ALL,team->getID(),TEVT_DISBAND); }
	void joinTeam(Player &pl) { if(isTeamLeader()) pl.joinTeam(*team); }
	void joinTeam(Team &t) { if(!team) sendTeamPack(SEND_TO_ALL,t.getID(),TEVT_JOIN); }
	void leaveTeam() { if(team) sendTeamPack(SEND_TO_ALL,team->getID(),TEVT_LEAVE); }
	bool isTeamLeader() { return team && team->isLeader(*this); }
	void setTeamLeader() { if(team) sendTeamPack(SEND_TO_ALL,team->getID(),TEVT_SETLEADER); }
	Team *getTeam() { return team; }

	void cycle(time_t cycles);
	void die();

	static void receiveIncarnatePack(uint8_t *data);
	static void receiveTeamPack(uint8_t *data);

	void sendIncarnatePack(uint8_t to);
	void sendTeamPack(uint8_t to,uint32_t id,uint8_t evt);

#ifdef WOTSERVER
	void pack(uint8_t **data);
	int getPlayerSize();
	void packPlayer(uint8_t **data);
#endif

	void unpack(uint8_t **data);
#ifdef WOTCLIENT
	void unpackPlayer(uint8_t **data);
#endif
	void getPlayerFile(aString &s);
	static player_data *parsePlayerFile(const char *p);
	static void deletePlayerData();
	void setPlayerData(player_data *pd);
};




#endif /* _GAME_PLAYER_H */
