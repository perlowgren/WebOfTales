#ifndef _CLIENT_WOTCLIENT_H
#define _CLIENT_WOTCLIENT_H

#include <stdio.h>
#include <libamanita/aApplication.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aThread.h>
#include "../game/Map.h"


class aFont;
class aImage;
class aSocket;
class aClient;
class MapDlg;
class IncarnateDlg;
class CharacterDlg;
class BattleDlg;
class TeamDlg;
class WorldDisplay;
class CityDisplay;
class Panel;
class Message;
class Character;


struct set { int index,count; };


class WoTClient : public aApplication {
private:
	aClient *client;
	int mx,my;							// Mouse Point coordinate, where mouse is pointing

public:
	static const float version;

	struct gametime {
		uint32_t frames;
		uint32_t ticks;
	};

	aFont *fonts[32];
	aImage *mainFrameImage;
	aImage **terrainImages[2];
	aImage **constructImages[CONSTRUCT_TYPES];
	aImage *cloudsImage,*gui[16],*icons,*btlicons,*markers;
	aImage *npcspr[2],**plspr[10],*shadows,*faces[2],*items[2];

	aHashtable db;

	Panel *panel;
	MapDlg *mapdlg;
	IncarnateDlg *incdlg;
	CharacterDlg *chrdlg;
	BattleDlg *btldlg;
	TeamDlg *teamdlg;
	WorldDisplay *worldDisplay;
	CityDisplay *cityDisplay;

	gametime time;

	aThread runThread;

	Message *chatFirst,*chatLast;
	Player *player;
	Character *hover,*focus;


	WoTClient();
	~WoTClient();

	int init(int argc,char *argv[]);
	void events();
	void exit();

	uint32_t getID();

	void setPlayer(Player *pl) { player = pl; }
	Player *getPlayer() { return player; }
	Player *getLeader();

	void setHover(Character *c) { hover = c; }
	Character *getHover() { return hover; }
	void setFocus(Character *c) { focus = c; }
	Character *getFocus() { return focus; }

	bool load();
	bool loadImages(const char *p,const char *fn);
   void loadImageSet(aImage **images[],set list,const char **names,const set *parts);
	aHashtable &getDB() { return db; }

	void startThreads();
	void stopThreads();
	static void _run(void *data) { ((WoTClient *)data)->run(); }
	void run();

	bool consolef(const char *format, ...);
	bool consolef(aFont *f,const char *format, ...);
	bool console(const char *msg) { return console(0,msg); }
	bool console(aFont *f,const char *msg);

	void command(int cmd,void *param=0);

	static uint32_t clientHandler(aSocket *client,uint32_t state,intptr_t p1,intptr_t p2,intptr_t p3);
	void receive(uint8_t *data,size_t len);

	void msgf(uint32_t to,uint8_t font,const char *format, ...);
	void msg(uint32_t to,uint8_t font,const char *msg);
	void send(uint8_t *data,size_t len);

	void sendRequest(uint8_t request,uint32_t id=0);
};

extern WoTClient app;



#endif

