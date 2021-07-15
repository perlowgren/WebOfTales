
#include "../config.h"
#include <stdlib.h>
#include <SDL/SDL_endian.h>

#ifdef WOTSERVER
#	include "../server/WebOfTales.h"
#elif defined WOTCLIENT
#	include <libamanita/net/aSocket.h>
#	include "../client/WoTClient.h"
#	include "../client/TeamDlg.h"
#endif

#include "Team.h"
#include "Player.h"


#ifdef WOTSERVER
uint32_t Team::index = 0;
#endif


Team::Team(uint32_t id) : id(id) {
	cap = 2;
	sz = 0;
	team = (Player **)malloc(cap*sizeof(Player *));
}

Team::Team(uint8_t **data) {
app.printf("Team::Team()");
	unpack_uint32(data,id);
app.printf("Team::Team(id=%lu)",id);
	unpack_uint16(data,sz);
	cap = sz;
app.printf("Team::Team(sz=%lu)",sz);
	team = (Player **)malloc(cap*sizeof(Player *));
	uint32_t id;
	for(int i=sz-1; i>=0; i--) {
		unpack_uint32(data,id);
app.printf("Team::Team(pl%d.id=%lu)",i,id);
		team[i] = (Player *)Map::getItem(id);
		team[i]->team = this;
app.printf("Team::Team(pl%d=%p)",i,team[i]);
	}
}

Team::~Team() {
	if(team) {
		for(size_t i=0; i<sz; i++) team[i]->team = 0;
		free(team);team = 0;
	}
}

void Team::handleEvent(uint32_t id,Player *pl,uint8_t evt) {
app.printf("Team::handleEvent(id=%lu,evt=%d)",id,evt);
	Team *t;
	if(evt==TEVT_CREATE) {
		if(pl)  {
#ifdef WOTSERVER
			id = ++index;
#endif
			t = new Team(id);
			Map::addTeam(*t);
			t->join(pl);
		}
	} else if(id && (t=Map::getTeam(id))) {
		if(evt==TEVT_DISBAND) Map::deleteTeam(*t);
		else if(pl) switch(evt) {
			case TEVT_JOIN:t->join(pl);break;
			case TEVT_LEAVE:t->leave(pl);break;
			case TEVT_SETLEADER:t->setLeader(pl);break;
		}
	}
}

void Team::disband() {
	if(sz==0) return;
	size_t i;
	team[0]->team = 0;
	for(i=1; i<sz; i++) {
		team[i]->team = 0;
		team[i]->set(*team[0]);
		team[0]->map->placeItem(*team[i]);
	}
#ifdef WOTSERVER
	app.deleteChannel(*this);
#endif
}

void Team::join(Player *pl) {
	if(sz==cap) {
		cap *= 2;
		team = (Player **)realloc(team,cap*sizeof(Player *));
	}
	team[sz++] = pl;
	pl->team = this;
	if(sz>1) {
		if(pl->map) pl->map->removeItem(*pl);
#ifdef WOTSERVER
		team[0]->stand();
		pl->stand();
#endif
	}
#ifdef WOTSERVER
	app.joinChannel(*this,app.getClient(pl->getID()));
#elif defined WOTCLIENT
	if(pl==app.getPlayer()) app.teamdlg->setTeam(this);
#endif
}

void Team::leave(Player *pl) {
	size_t i;
	for(i=0; i<sz; i++) if(team[i]==pl) break;
	if(i==sz) return;
	if(i==0 && sz>1) pl->map->replaceItem(*pl,*team[1]);
	for(sz--; i<sz; i++) team[i] = team[i+1];
	pl->team = 0;
	if(sz==0) Map::deleteTeam(*this);
	else {
#ifdef WOTSERVER
		app.leaveChannel(*this,app.getClient(pl->getID()));
#elif defined WOTCLIENT
		if(app.teamdlg->getTeam()==this && pl==app.getPlayer()) app.teamdlg->setTeam(0);
#endif
		pl->set(*team[0]);
		team[0]->map->placeItem(*pl);
	}
}

void Team::setLeader(Player *pl) {
	size_t i,n;
	for(i=0; i<sz; i++) if(team[i]==pl) break;
	if(i==0 || i==sz) return;
	Player *l = team[0];
	for(n=i; n>0; i--) team[i] = team[i-1];
	team[0] = pl;
	l->map->replaceItem(*l,*pl);
}

int Team::indexOf(Player &pl) {
	for(size_t i=0; i<sz; i++) if(team[i]==&pl) return i;
	return -1;
}

void Team::move(Coordinate &c) {
	for(size_t i=1; i<sz; i++) team[i]->set(c);
}



