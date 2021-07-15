
#include "../config.h"
#include <stdio.h>
#include <libamanita/sdl/aFont.h>
#include "../Main.h"
#include "Message.h"



Message::Message(int type,const char *m,aFont *f,Message *n) : type(type),next(n),prev(0) {
	time = app.time;
	if(n) n->prev = this;
	msg = strdup(m);
	font = f? f : app.fonts[FONT_WHITE];
}

Message::~Message() {
	if(msg) { free(msg);msg = 0; }
}


