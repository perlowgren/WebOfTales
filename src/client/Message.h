#ifndef _CLIENT_MESSAGE_H
#define _CLIENT_MESSAGE_H

#include "WoTClient.h"


class aFont;

class Message {
public:
	int type;
	WoTClient::gametime time;
	char *msg;
	aFont *font;
	Message *next,*prev;

	Message(int type,const char *m,aFont *f,Message *n);
	~Message();
};



#endif

