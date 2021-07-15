#ifndef _GAME_TEAM_H
#define _GAME_TEAM_H


enum {
	TEVT_CREATE,
	TEVT_DISBAND,
	TEVT_JOIN,
	TEVT_LEAVE,
	TEVT_SETLEADER,
};


#define sizeof_team 5


class Character;
class Player;
class Coordinate;


class Team {
friend class TeamDlg;

private:
#ifdef WOTSERVER
	static uint32_t index;
#endif

	uint32_t id;
	Player **team;
	uint16_t sz;
	uint16_t cap;

public:
	static void handleEvent(uint32_t id,Player *pl,uint8_t evt);

	Team(uint32_t id=0);
	Team(uint8_t **data);
	~Team();

	void setID(uint32_t i) { id = i; }
	uint32_t getID() { return id; }

	void disband();
	void join(Player *pl);
	void leave(Player *pl);
	void setLeader(Player *pl);
	int members() { return sz; }
	Player *getMember(int i) { return i>=0 && i<(int)sz? team[i] : 0; }
	Player *getLeader() { return sz>0? team[0] : 0; }
	bool isLeader(Player &pl) { return team && team[0]==&pl; }
	int indexOf(Player &pl);
	bool contains(Player &pl) { return indexOf(pl)>=0; }
	void move(Coordinate &c);
};

#endif /* _GAME_TEAM_H */

