
#include "config.h"
#include <stdlib.h>
#include <libamanita/aDictionary.h>
#include <libamanita/net/aSocket.h>
#include "command.h"


aDictionary::word cmd_words[] = {
	{ "add",				CMD_ADD },
	{ "delete",			CMD_DELETE },

	{ "join",			CMD_JOIN },

	{ "request",		CMD_REQUEST },
	{ "send",			CMD_SEND },

	{ "cloud",			CMD_CLOUD },
	{ "spawn",			CMD_SPAWN },

	{ "enter",			CMD_ENTER },
	{ "exit",			CMD_EXIT },

	{ "action",			CMD_ACTION },
	{ "effect",			CMD_EFFECT },
	{ "kill",			CMD_KILL },
	{ "incarnate",		CMD_INCARNATE },
	{ "character",		CMD_CHARACTER },
	{ "battle",			CMD_BATTLE },
	{ "team",			CMD_TEAM },

	{ "message",		CMD_MESSAGE },
	{ "remote",			CMD_REMOTE },
{0,0}};
aDictionary commands(cmd_words);


aDictionary::word srv_words[] = {
	{ "print",			SRV_PRINT },
	{ "suicide",		SRV_SUICIDE },
	{ "teleport",		SRV_TELEPORT },
{0,0}};
aDictionary server_commands(srv_words);


aDictionary::word rmt_words[] = {
	{ "xp",				RMT_XP },
	{ "suicide",		RMT_SUICIDE },
{0,0}};
aDictionary remote_commands(rmt_words);





void packSendHeader(uint8_t **data,uint8_t send,uint8_t value) {
	pack_header(data,CMD_SEND);
	pack_uint8(data,send);
	pack_uint8(data,value);
}

void packRequestHeader(uint8_t **data,uint8_t request,uint32_t id) {
	pack_header(data,CMD_REQUEST);
	pack_uint8(data,request);
	pack_uint32(data,id);
}

void packMessageHeader(uint8_t **data,uint32_t from,uint32_t to,uint8_t font) {
	pack_header(data,CMD_MESSAGE);
	pack_uint32(data,from);
	pack_uint32(data,to);
	pack_uint8(data,font);
}


