
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/aMath.h>
#include <libamanita/aString.h>
#include <libamanita/aDictionary.h>
#include <libamanita/aThread.h>
#include <libamanita/aVector.h>
#include <libamanita/net/aServer.h>
#include "../Main.h"
#include "../command.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "ServerWorld.h"
#include "../game/Player.h"
#include "../game/Team.h"
#include "../game/Location.h"


extern aDictionary commands;



void WebOfTales::pushClientToQueue(aConnection client,int cmd) {
	queuesz++;
	queuenode *qn = (queuenode *)malloc(sizeof(queuenode));
	*qn = (queuenode){ client,cmd,0 };
	if(queuelast) queuelast->next = qn,queuelast = qn;
	else queuefirst = queuelast = qn;
}

void WebOfTales::removeClientFromQueue(aConnection client) {
	if(queuefirst) {
		queuesz--;
		queuenode *qn = queuefirst;
		if(queuefirst->client==client) {
			if(queuefirst==queuelast) queuefirst = queuelast = 0;
			free(qn);
		} else while(qn->next) {
			if(qn->next->client==client) {
				queuenode *qn1 = qn->next;
				qn->next = qn->next->next;
				if(queuelast==qn1) queuelast = qn;
				free(qn1);
			}
			qn = qn->next;
		}
	}
}

aConnection WebOfTales::popClientFromQue(int &cmd) {
	aConnection c = 0;
	if(queuefirst) {
		queuesz--;
		queuenode *qn = queuefirst;
		c = queuefirst->client,cmd = queuefirst->cmd,queuefirst = queuefirst->next;
		if(!queuefirst) queuelast = 0;
		free(qn);
	}
	return c;
}

void WebOfTales::removeSendqueue() {
	int cmd;
	while(popClientFromQue(cmd));
}

void WebOfTales::sendqueue() {
	int cmd;
	aConnection client;
	while(1) {
		lock();
		client = popClientFromQue(cmd);
		unlock();
		if(client) {
printf("sendqueue(id=%d,nick=\"%s\")",client->getID(),client->getNick());
			world->sendWorldMap(client);
			uint8_t d[sizeof_sendheader],*p = d;
			packSendHeader(&p,SND_WORLDMAPDONE);
			server->lock();
			server->send(client,d,sizeof(d));
			server->unlock();
		}
		sendqueueThread.pause(2000);
	}
}


void WebOfTales::fixNick(char **nm) {
	unsigned int i;
	if((i=strspn(*nm,NICK_CHARS))!=strlen(*nm)) (*nm)[i] = '\0';
}


uint32_t WebOfTales::serverHandler(aSocket *srv,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	switch(st) {
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		{
			uint32_t ipaddr = app.server->getIP();
			app.consolef(TXT_RED,"%s: " /*"\"%s\","*/ "host=\"%d.%d.%d.%d\",port=%d",
				st==SM_ERR_RESOLVE_HOST? "SM_ERR_RESOLVE_HOST" : "SM_ERR_OPEN_SOCKET",
				//SDLNet_GetError(),
				ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,
				app.server->getPort()
			);
			break;
		}
		case SM_STARTING_SERVER:
		{
			uint32_t ipaddr = app.server->getIP();
			const char *host = app.server->getHost();
			app.consolef(TXT_PURPLE,"\t%s: %d.%d.%d.%d\n\t%s: %s\n\t%s: %d",
				app.get("server_ip"),ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,
				app.get("server_host"),host? host : "N/A",
				app.get("server_port"),app.server->getPort()
			);
			break;
		}
		case SM_ERR_ALLOC_SOCKETSET:app.consolef(TXT_RED,"SM_ERR_ALLOC_SOCKETSET" /*": %s",SDLNet_GetError()*/);break;
		case SM_ERR_CHECK_SOCKETS:app.consolef(TXT_RED,"SM_ERR_CHECK_SOCKETS" /*": %s",SDLNet_GetError()*/);break;
		case SM_CHECK_NICK:app.fixNick((char **)p1);break;
		case SM_DUPLICATE_ID:/*srv->send((TCPsocket)p1,"Duplicate ID...bye bye!");*/break;
		case SM_ADD_CLIENT:
		{
			uint8_t cmd = CMD_ADD;
			app.lock();
			app.command((aConnection)p1,&cmd,1);
			app.unlock();
			break;
		}
		case SM_KILL_CLIENT:
		{
			uint8_t cmd = CMD_DELETE;
			app.lock();
			app.command((aConnection )p1,&cmd,1);
			app.unlock();
			break;
		}
		case SM_ERR_PUT_MESSAGE:app.consolef(TXT_RED,"SM_ERR_GET_MESSAGE: " /*"%s,"*/ "\"%s\",%d"/*,SDLNet_GetError()*/,(char *)p1,p2);break;
		case SM_GET_MESSAGE:
		{
			aConnection client = (aConnection)p1;
			if(*(uint8_t *)p2==CMD_MESSAGE) {
				app.command(client,(uint8_t *)p2,(size_t)p3);
			} else {
				app.lock();
				app.command(client,(uint8_t *)p2,(size_t)p3);
				app.unlock();
			}
			break;
		}
	}
	return 0;
}


void WebOfTales::command(aConnection client,uint8_t *data,size_t len) {
printf("command(data=%d,cmd=%s,len=%d)",*data,commands.getKey(*data),len);
	uint8_t *p = data,cmd;
	unpack_header(&p,cmd);
	switch(*data) {
		case CMD_ACTION:Character::receiveActionPack(data);break;

		case CMD_ADD:
			msgf(client->getID(),FONT_BLUE,app.get("server_queue"),2+queuesz*2);
			mainFrame->insertClient(client);
			pushClientToQueue(client,0);
			break;

		case CMD_CHARACTER:Character::receiveCharacterPack(data);break;

		case CMD_DELETE:
		{
			mainFrame->removeClient(client);
			removeClientFromQueue(client);
			Player *pl = (Player *)Map::getItem(client->getID());
printf("command(CMD_DELETE,id=%d,pl=%p)",client->getID(),pl);
			if(pl) {
				if(pl->getTeam()) pl->leaveTeam();
				world->uploadPlayer(*pl);
				Map::deleteItem(*pl);
				pl->sendCommandPack(SEND_TO_ALL,CMD_DELETE);
			}
			break;
		}

		case CMD_ENTER:Character::receiveLocationPack(data);break;

		case CMD_INCARNATE:Player::receiveIncarnatePack(data);break;

		case CMD_MESSAGE:
		{
			uint32_t from,to;
			unpack_uint32(&p,from);
			unpack_uint32(&p,to);
			if(to) {
				aConnection c;
				server->lock();
				c = server->getClient(from);
				if(c) server->send(c,data,len);
				c = server->getClient(to);
				if(c) server->send(c,data,len);
				server->unlock();
			} else server->send(data,len);
			break;
		}
		case CMD_REQUEST:
		{
			uint8_t request;
			uint32_t id;
			unpack_uint8(&p,request);
			unpack_uint32(&p,id);
			switch(request) {
				case REQ_CLOUDS:world->sendClouds(client);break;
				case REQ_ITEMS:
				{
					Player *pl = (Player *)Map::getItem(client->getID());
					if(!pl) {
						pl = new Player(world,client->getID(),client->getNick(),0,0,0,0,1);
						Map::addItem(*pl);
						world->downloadPlayer(*pl);
						world->sendPlayer(0,*pl,1);
					}
					world->sendItems(client);
					world->sendPlayer(client,*pl,2);
					mainFrame->removeClient(client);
					client->setActive(true);
					mainFrame->insertClient(client);
					break;
				}
				case REQ_TEAMS:
					world->sendTeams(client);
					break;
				case REQ_PLAYER:
				{
					Player *pl = (Player *)Map::getItem(id);
					if(pl) world->sendPlayer(client,*pl,2);
					break;
				}
			}
			break;
		}

		case CMD_TEAM:Player::receiveTeamPack(data);break;

		default:msgf(client->getID(),FONT_BLUE,app.get("server_cmd"),(int)*data);
	}
}




