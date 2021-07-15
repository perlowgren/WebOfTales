
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>
#include <libamanita/aDictionary.h>
#include <libamanita/net/aClient.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "WorldDisplay.h"
#include "CityDisplay.h"
#include "MapDlg.h"
#include "IncarnateDlg.h"
#include "CharacterDlg.h"
#include "BattleDlg.h"
#include "TeamDlg.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Team.h"


extern aDictionary commands;


void WoTClient::command(int cmd,void *param) {
	switch(cmd) {
		case CMD_ENTER:
			if(param) {
				aDisplay *f1 = aDisplay::getActiveDisplay(),*f2 = (aDisplay *)param;
printf("ENTER f1=%p -> f2=%p",f1,f2);
				if(f1) f1->stop();
				f2->start();
				break;
			}
		case CMD_EXIT:
		{
			SDL_Event event = { SDL_QUIT };
			SDL_PushEvent(&event);
			break;
		}
		case CMD_MESSAGE:console((const char *)param);break;

		case CMD_SHOW_HIDE_MAP:mapdlg->setVisible(!mapdlg->isVisible());break;
		case CMD_CHARACTER_SHEET:
			if(chrdlg->isVisible()) chrdlg->setVisible(false);
			else if(getPlayer()) {
				chrdlg->setCharacter(getPlayer());
				chrdlg->setVisible(true);
			}
			break;
		case CMD_SHOW_HIDE_TEAM:
			if(getPlayer()) {
				Player &pl = *getPlayer();
				if(!pl.getTeam()) {
					pl.createTeam();
					console("create team!");
				} else teamdlg->setTeam(pl.getTeam());
			}
			break;
		case CMD_JOIN_TEAM:
			if(getPlayer()) {
				Player &pl = *getPlayer();
				consolef("join team! target=%p,act=%d",getPlayer()->getTarget(),getPlayer()->getAction());
				if(pl.isTeamLeader() && getPlayer()->getTarget() && getPlayer()->getAction()==ACT_TALK)
					((Player *)getPlayer()->getTarget())->joinTeam(*pl.getTeam());
			}
			break;
		case CMD_DISBAND_TEAM:if(getPlayer() && getPlayer()->isTeamLeader()) getPlayer()->disbandTeam();break;
	}
}



uint32_t WoTClient::clientHandler(aSocket *client,uint32_t state,intptr_t p1,intptr_t p2,intptr_t p3) {
// app.printf("WoTClient::clientHandler(state=%d)",state);
	switch(state) {
		case SM_ERR_ALLOC_SOCKETSET:app.printf("SM_ERR_ALLOC_SOCKETSET: %s",SDLNet_GetError());break;
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		{
			uint32_t ipaddr = client->getIP();
			app.printf("%s: \"%s\",host=\"%d.%d.%d.%d\",port=%d",
				state==SM_ERR_RESOLVE_HOST? "SM_ERR_RESOLVE_HOST" : "SM_ERR_OPEN_SOCKET",
				SDLNet_GetError(),
				ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,
				client->getPort()
			);
			break;
		}
		case SM_ERR_ADD_SOCKET:app.printf("SM_ERR_ADD_SOCKET: %s",SDLNet_GetError());break;
		case SM_STARTING_CLIENT:
		{
			uint32_t ipaddr = client->getIP();
			const char *host = client->getHost();
			app.printf("IP Address : %d.%d.%d.%d\nHostname   : %s\nPort       : %d",
				ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,
				host? host : "N/A",
				client->getPort()
			);
			break;
		}
		case SM_ERR_CHECK_SOCKETS:app.printf("SM_ERR_CHECK_SOCKETS: %s",SDLNet_GetError());break;
		case SM_ERR_PUT_MESSAGE:app.printf("SM_ERR_GET_MESSAGE: %s,\"%s\",%d",SDLNet_GetError(),(char *)p1,p2);break;
		case SM_GET_MESSAGE:app.receive((uint8_t *)p2,(size_t)p3);break;
	}
// app.printf("WoTClient::clientHandler(done)");
	return 0;
}

void WoTClient::receive(uint8_t *data,size_t len) {
app.printf("WoTClient::receive(data=%d,cmd=%s,len=%d)",*data,commands.getKey(*data),len);
	uint8_t *p = data,cmd;
	unpack_header(&p,cmd);
	lock();
	switch(cmd) {
		case CMD_ACTION:Character::receiveActionPack(data);break;

		case CMD_BATTLE:Character::receiveBattlePack(data);break;

		case CMD_CHARACTER:Character::receiveCharacterPack(data);break;

		case CMD_CLOUD:Cloud::receiveCloudPack(data);break;

		case CMD_DELETE:
		{
			Character *c = Map::getItem(get_unpack_uint32(p));
			if(c) Map::deleteItem(*c);
			break;
		}

		case CMD_EFFECT:Character::receiveEffectPack(data);break;

		case CMD_ENTER:Character::receiveLocationPack(data);break;

		case CMD_EXIT:command(CMD_EXIT);break;

		case CMD_INCARNATE:Player::receiveIncarnatePack(data);break;

		case CMD_MESSAGE:
		{
			uint32_t from,to;
			uint8_t font;
			unpack_uint32(&p,from);
			unpack_uint32(&p,to);
			unpack_uint8(&p,font);
 printf("Message: \"%s\"",(char *)p);
			if(from==0) console(fonts[font],(char *)p);
			else {
				Character *c = Map::getItem(from);
				if(c) consolef(fonts[font],"[%s]: %s",c->getName(),(char *)p);
			}
			break;
		}
		case CMD_SEND:
		{
			uint8_t send,value;
			unpack_uint8(&p,send);
			unpack_uint8(&p,value);
			switch(send) {
				case SND_WORLDMAP:
consolef(app.fonts[FONT_GREY],"Receive: Worldmap (size=%d)",len);
					if(worldDisplay->loadMap(p)) {
						char str[256];
						sprintf(str,"%s - %s",getProperty("caption"),worldDisplay->getName());
						worldDisplay->setViewPoint((int16_t)(worldDisplay->mw/2),(int16_t)(worldDisplay->mh/2));
						mapdlg->setMapDisplay(worldDisplay);
						worldDisplay->stats |= 1;
console(fonts[FONT_GREY],"Map loaded and running.");
					}
					break;

				case SND_WORLDMAPDONE:
consolef(app.fonts[FONT_GREY],"Receive: Worldmap done");
					sendRequest(REQ_CLOUDS);
					break;

				case SND_CLOUDS:
consolef(app.fonts[FONT_GREY],"Receive: Clouds (size=%d)",len);
					worldDisplay->createClouds(p);
					sendRequest(REQ_ITEMS);
					break;

				case SND_ITEMS:
consolef(app.fonts[FONT_GREY],"Receive: Items (size=%d)",len);
					setPlayer(0);
					worldDisplay->createItems(p);
					sendRequest(REQ_TEAMS);
					break;

				case SND_TEAMS:
consolef(app.fonts[FONT_GREY],"Receive: Teams (size=%d)",len);
					worldDisplay->createTeams(p);
					console(fonts[FONT_GREY],"NPCs and Player Characters loaded.");
					break;

				case SND_NPC:
consolef(app.fonts[FONT_GREY],"Receive: NPC %d (size=%d)",value,len);
					if(value==1) {
						NPC *ch = new NPC(worldDisplay,&p);
						Map::addItem(*ch);
						worldDisplay->placeItem(*ch);
					} else {
						NPC *c = (NPC *)Map::getItem(get_unpack_uint32(p));
						if(c) c->unpackNPC(&p);
					}
					break;

				case SND_PLAYER:
consolef(app.fonts[FONT_GREY],"Receive: Player %d (size=%d)",value,len);
					if(value==1) {
						Player *pl = new Player(worldDisplay,&p);
						Map::addItem(*pl);
						if(pl->isIncarnated()) worldDisplay->placeItem(*pl);
						else if(pl->getID()==client->getID()) {
							app.incdlg->setPlayer(pl);
							app.incdlg->setVisible(true);
						}
					} else {
						Player *pl = (Player *)Map::getItem(get_unpack_uint32(p));
						if(pl) {
							pl->unpackPlayer(&p);
							if(pl->getID()==client->getID()) {
								setPlayer(pl);
								if(!pl->isIncarnated()) {
									app.incdlg->setPlayer(pl);
									app.incdlg->setVisible(true);
								}
							}
							aString file;
							pl->getPlayerFile(file);
							char fn[128];
							size_t i;
							sprintf(fn,"%sclient/char_%" PRIu32 ".txt",app.getHomeDir(),pl->getID());
							FILE *fp = fopen(fn,"w");
							i = fwrite(file.toCharArray(),file.length(),1,fp);
							fclose(fp);
						}
					}
					break;
			}
			break;
		}

		case CMD_TEAM:Player::receiveTeamPack(data);break;
	}
	unlock();
}



