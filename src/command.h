#ifndef _COMMAND_H
#define _COMMAND_H


enum {
	SEND_TO_NONE		= 0,				/** < Don't send anywhere, or handle. */
	SEND_TO_SERVER		= 1,				/** < Send to server. */
	SEND_TO_CLIENT		= 2,				/** < Send to a client specified elsewhere in the package. */
	SEND_TO_CHANNEL	= 4,				/** < Send to a channel of clients spceified elsewhere in the package. */
	SEND_TO_CLIENTS	= 8,				/** < Send to all clients. */
	SEND_TO_ALL			= 15,				/** < Send to all clients and server. */
};


enum {										/** < Commands to communicate between Server and Client. */
	CMD_ADD					= 11,			/** < Add new Client */
	CMD_DELETE				= 12,			/** < Delete Client */

	CMD_JOIN					= 21,			/** < Join channel */

	CMD_REQUEST				= 31,			/** < Request data */
	CMD_SEND					= 32,			/** < Sending data */

	CMD_CLOUD				= 41,			/** < Sending Cloud update data */
	CMD_SPAWN				= 42,			/** < Spawn npc */

	CMD_ENTER				= 51,			/** < Enter Display */
	CMD_EXIT					= 52,			/** < Exit Display or exit game */

	CMD_ACTION				= 101,		/** < Character action */
	CMD_EFFECT				= 102,		/** < Character effect */
	CMD_KILL					= 104,		/** < Kill Character */
	CMD_INCARNATE			= 105,		/** < Incarnate Character */
	CMD_CHARACTER			= 107,		/** < Update Character */
	CMD_BATTLE				= 108,		/** < Battle event, sent to the BattleDlg */
	CMD_TEAM					= 109,		/** < Team event */

	CMD_MESSAGE				= 201,		/** < Message */
	CMD_REMOTE				= 202,		/** < Remote Pack */

	CMD_SETTINGS			= 1001,		/** < Client command: Settings */
	CMD_SHOW_HIDE_MAP		= 1002,		/** < Client command: Show/Hide Map (Ctrl-M) */
	CMD_INVENTORY			= 1003,		/** < Client command: Show/Hide Inventory (Ctrl-I) */
	CMD_CHARACTER_SHEET	= 1004,		/** < Client command: Show/Hide Character Sheet (Ctrl-C) */
	CMD_SHOW_HIDE_TEAM	= 1051,		/** < Client command: Create/Show/Hide Team (Ctrl-T) */
	CMD_JOIN_TEAM			= 1052,		/** < Client command: Join Team (Ctrl-J) */
	CMD_DISBAND_TEAM		= 1053,		/** < Client command: Disband Team (Ctrl-D) */
};


//struct messagepack {
	//uint8_t cmd;							/** < cmd must be included with every pack as the first char. */
	//uint16_t len;							/** < len must be in every pack as a short following cmd. Should not be set. */
	//uint32_t from;
	//uint32_t to;
	//uint8_t font;
//};
//#define getMessagePack(from,to,font) {CMD_MESSAGE,0,SDL_SwapBE32(from),SDL_SwapBE32(to),(font)}
//#define setMessagePack(p,from,to,font) ((*(messagepack *)(p)) = (messagepack)getMessagePack(from,to,font))

/** Request commands */
enum {
	REQ_CLOUDS			= 11,				/** < Request Clouds */
	REQ_ITEMS			= 12,				/** < Request Items */
	REQ_TEAMS			= 13,				/** < Request Teams */
	REQ_PLAYER			= 14,				/** < Request a Player */
};

/** requestpack is sent from a Client to the Server to request data. */
//struct requestpack {
	//uint8_t cmd;							/** < cmd must be included with every pack as the first char. */
	//uint16_t len;							/** < len must be in every pack as a short following cmd. Should not be set. */
	//uint8_t data;							/** < data contains one of the request commands. */
	//uint32_t id;							/** < ID of data that is requested. */
//};
//#define getRequestPack(data,id) {CMD_REQUEST,0,(data),SDL_SwapBE32(id)}
//#define setRequestPack(p,data) ((*(requestpack *)(p)) = (requestpack)getRequestPack(data,id))

/** Send commands */
enum {
	SND_WORLDMAP		= 11,
	SND_WORLDMAPDONE	= 12,
	SND_CLOUDS			= 31,
	SND_ITEMS			= 32,
	SND_TEAMS			= 33,
	SND_NPC				= 41,
	SND_PLAYER			= 42,
};

///** sendpack is sent from the Server to a Client containing data. */
//struct sendpack {
	//uint8_t cmd;							/** < cmd must be included with every pack as the first char. */
	//uint16_t len;							/** < len must be in every pack as a short following cmd. Should not be set. */
	//uint8_t data;							/** < data contains one of the send commands. */
	//uint8_t value;							/** < value can be any value to define the contained data. */
//};
//#define getSendPack(data,value) {CMD_SEND,0,(data),(value)}
//#define setSendPack(p,data,value) ((*(sendpack *)(p)) = (sendpack)getSendPack(data,value))

/** Commands to control Server from Server. */
enum {
	SRV_PRINT			= 11,				/** <	"/print player [id]" - Print a player's character information. */
	SRV_SUICIDE			= 101,			/** < "/suicide [id]" - Force a player to commit suicide. */
	SRV_TELEPORT		= 201,			/** < "/teleport [id] [x],[y]" - Teleport player to coordinate on map. */
};

/** Commands to remote control Server from Client or Server. */
enum {
	RMT_XP				= 21,				/** <	Give XP to Player */
	RMT_SUICIDE			= 31,				/** <	Kill character to reincarnate. */
};


#define sizeof_messageheader (SOCKET_HEADER+9)
#define sizeof_requestheader (SOCKET_HEADER+5)
#define sizeof_sendheader (SOCKET_HEADER+2)

class aDictionary;
extern aDictionary commands;
extern aDictionary server_commands;
extern aDictionary remote_commands;

extern void packMessageHeader(uint8_t **data,uint32_t from,uint32_t to,uint8_t font);
extern void packRequestHeader(uint8_t **data,uint8_t request,uint32_t id);
extern void packSendHeader(uint8_t **data,uint8_t send,uint8_t value=0);


#endif



