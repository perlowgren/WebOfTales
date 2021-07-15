#ifndef _SERVER_WEBOFTALES_H
#define _SERVER_WEBOFTALES_H

#include <stdio.h>
#include <libamanita/aApplication.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aThread.h>
#include <libamanita/net/aServer.h>


#ifdef WOT_GTK
#include <gtk/gtk.h>

void ShowMessage(GtkWidget *window,const gchar *msg,const gchar *cap);
#endif /* WOT_GTK */

class WoTFrame;
class ServerWorld;
class Character;
class Player;
class Team;


enum {
	ST_WOT_SERVER_SWITCH					= 0x0000,
	ST_WOT_SERVER_START					= 0x0001,
	ST_WOT_SERVER_STOP					= 0x0002,
	ST_WOT_SERVER_LOAD_HOSTS			= 0x0004,

	ST_WOT_SERVER_RUNNING				= 0x0010,
	ST_WOT_SERVER_WAS_RUNNING			= 0x0011,
};


class WebOfTales : public aApplication {
private:
	struct queuenode {
		aConnection client;
		int cmd;
		queuenode *next;
	};
	int queuesz;
	queuenode *queuefirst;
	queuenode *queuelast;

	int status;

	aServer *server;

	aThread runThread;				// aThread for running the server online world
	aThread sendqueueThread;		// Queue for handling server messages and sending of files

	void setStatus(int s,bool set=true) { status |= s;if(!set) status ^= s; }

public:
	char local_user[33];
	char local_password[33];
	char local_char_name[33];
	uint32_t local_char_id;

	ServerWorld *world;
	WoTFrame *mainFrame;


	WebOfTales();
	~WebOfTales();

#ifdef WOT_GTK
	bool init(int argc,char *argv[]);
	GdkPixbuf *createPixbuf(const gchar *filename);
#endif /* WOT_GTK */
	void init();
	void exit();
	bool isServerRunning() { return status&ST_WOT_SERVER_RUNNING; }

	static void _run(void *data) { ((WebOfTales *)data)->run(); }
	void run();
	void setServerState(int st=0);
	void switchServerState() { setServerState(ST_WOT_SERVER_SWITCH|ST_WOT_SERVER_LOAD_HOSTS); }
	void startServer() { setServerState(ST_WOT_SERVER_START|ST_WOT_SERVER_LOAD_HOSTS); }
	void stopServer() { setServerState(ST_WOT_SERVER_STOP|ST_WOT_SERVER_LOAD_HOSTS); }
	void killServer() { setServerState(ST_WOT_SERVER_STOP); }

	aServer *getServer() { return server; }

	static void _sendqueue(void *data) { ((WebOfTales *)data)->sendqueue(); }
	void sendqueue();
	void pushClientToQueue(aConnection client,int cmd);
	void removeClientFromQueue(aConnection client);
	aConnection popClientFromQue(int &cmd);
	void removeSendqueue();
	void fixNick(char **nm);
	void command(aConnection client,uint8_t *data,size_t len);
	static uint32_t serverHandler(aSocket *srv,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);

	aConnection getClient(uint32_t id) { return server? server->getClient(id) : 0; }
	const char *TeamToChannel(Team &t,char *ch);

	void deleteChannel(Team &ch);
	void deleteChannel(const char *ch) { if(server) server->deleteChannel(ch); }
	void joinChannel(Team &ch,aConnection c);
	void joinChannel(const char *ch,aConnection c) { if(server) server->joinChannel(ch,c); }
	void leaveChannel(Team &ch,aConnection c);
	void leaveChannel(const char *ch,aConnection c) { if(server) server->leaveChannel(ch,c); }

	void msgf(Player &to,uint8_t font,const char *format, ...);
	void msgf(uint32_t to,uint8_t font,const char *format, ...);
	void msgf(Team &to,uint8_t font,const char *format, ...);
	void msgf(const char *to,uint8_t font,const char *format, ...);

	void msg(Player &to,uint8_t font,const char *msg);
	void msg(uint32_t to,uint8_t font,const char *msg);
	void msg(Team &to,uint8_t font,const char *msg);
	void msg(const char *to,uint8_t font,const char *msg);

	void send(uint8_t to,Character *c,uint8_t *data,size_t len);
	void send(Player &to,uint8_t *data,size_t len);
	void send(uint32_t to,uint8_t *data,size_t len);
	void send(Team &to,uint8_t *data,size_t len);
	void send(aConnection client,uint8_t *data,size_t len);
	void send(const char *to,uint8_t *data,size_t len) { if(server) server->send(server->getChannel(to),data,len); }
	void send(uint8_t *data,size_t len) { if(server) server->send(data,len); }

	void consolef(int color,const char *format, ...);
};

extern WebOfTales app;


#endif /* _SERVER_WEBOFTALES_H */


