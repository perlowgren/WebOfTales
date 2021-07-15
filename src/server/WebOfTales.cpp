
#include "../config.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <libamanita/aMath.h>
#include <libamanita/aVector.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aString.h>
#include <libamanita/aThread.h>
#include <libamanita/aWord.h>
#include <libamanita/net/aServer.h>
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "../command.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "InstallDlg.h"
#include "LoginDlg.h"
#include "ServerWorld.h"
#include "../game/Cloud.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Team.h"
#include "../game/Location.h"


extern FILE *out;


WebOfTales app;


#ifdef WOT_GTK
int main(int argc,char *argv[]) {
	gchar rcfile[256];
	gchar *rc_files[] = { rcfile,NULL };
	sprintf(rcfile,"%stheme/gtkrc",app.getHomeDir());
	gtk_rc_set_default_files(rc_files);

	gtk_init(&argc,&argv);
	app.init(argc,argv);
	gtk_main();
	return 0;
}

void ShowMessage(GtkWidget *window,const gchar *msg,const gchar *cap) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,msg,"title");
	gtk_window_set_title(GTK_WINDOW(dialog),cap);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
#endif /* WOT_GTK */


WebOfTales::WebOfTales() : aApplication(APP_PROJECT,"server") {
	queuesz = 0;
	queuefirst = 0;
	queuelast = 0;
	status = 0;
	server = 0;
	world = 0;
	mainFrame = 0;
printf("WebOfTales::WebOfTales()");
}

WebOfTales::~WebOfTales() {
printf("WebOfTales::~WebOfTales()");
}

#ifdef WOT_GTK
bool WebOfTales::init(int argc,char *argv[]) {
printf("WebOfTales::init()");
	const char *file;

	aHttp http(app);
	file = http.getf(WEB_HOST,WEB_PATH "files.php?t=%" PRIu64,(uint64_t)(time(NULL)-app_last_access));
app.printf("WebOfTales::init(last_access=%" PRIu64 ",difftime=%" PRIu64 ")",
					(uint64_t)app_last_access,(uint64_t)(time(NULL)-app_last_access));
	if(file && *file) {
fprintf(stderr,"%s",file);
fflush(stderr);
		aVector *files = new aVector();
		files->split(file,"\n");
		new InstallDlg(0,"Downloading files...",*this,*files);
	} else this->init();
	return true;
}

GdkPixbuf *WebOfTales::createPixbuf(const gchar *filename) {
   GdkPixbuf *pixbuf;
   GError *error = 0;
   pixbuf = gdk_pixbuf_new_from_file(filename,&error);
   if(!pixbuf) {
      fprintf(stderr,"%s\n",error->message);
      g_error_free(error);
   }
   return pixbuf;
}
#endif /* WOT_GTK */

void WebOfTales::init() {
	const char *p;

	aApplication::init();
	setUserAgent(APP_USER_AGENT);

	if((p=getProperty("user"))) strcpy(local_user,p);
	else *local_user = '\0';
	if((p=getProperty("password"))) strcpy(local_password,p);
	else *local_password = '\0';
	*local_char_name = '\0';
	local_char_id = 0;

printf("world 1");
	world = new ServerWorld(128,128);
printf("world 2");
	world->loadWorld(getProperty("world"));
printf("world 3, world=%s",getProperty("world"));

	mainFrame = new WoTFrame(APP_TITLE);
	new LoginDlg(GTK_WINDOW(mainFrame->gtk.window),"Login");
}

void WebOfTales::exit() {
printf("WebOfTales::OnExit()");
	app.killServer();
	if(world) {
		Location *l;
		Team *t;
		Character *c;
		aHashtable::iterator iter;
		Map::deleteAllClouds();
		iter = Map::locationids.iterate();
		while((l=(Location *)iter.next())) delete l;
		iter = Map::teamids.iterate();
		while((t=(Team *)iter.next())) delete t;
		iter = Map::itemids.iterate();
		while((c=(Character *)iter.next())) delete c;
		delete world;
		world = 0;
	}

	if(mainFrame) {
		delete mainFrame;
		mainFrame = 0;
	}

	aApplication::exit();
}


void WebOfTales::run() {
	int frames;
	for(frames=0; 1; frames++) {
		world->run(frames);
		runThread.pauseFPS(GAME_FPS);
	}
}

void WebOfTales::setServerState(int st) {
printf("WebOfTales::startStopServer()");
	if(world->isCompleted()) {
		bool start = !server,stop = !start;
		if((st&ST_WOT_SERVER_START) && server) start = false;
		if((st&ST_WOT_SERVER_STOP) && !server) stop = false;
		if(start || stop) {
			aHttp http(app);
			http.setFormValue("u","%s",getProperty("user"));
			http.setFormValue("p","%s",getProperty("password"));
			http.setFormValue("wid","%" PRIu32,world->getID());
			http.setFormValue("e","%s",start? "start" : "stop");
			const char *file = http.post(WEB_HOST,WEB_PATH "server.php");
			if(!file || !*file) consolef(TXT_RED,get("login_err"));
			else if(!strncmp(file,"[ERROR]",7)) consolef(TXT_RED,get(&file[7]));
			else if(start) {
				consolef(TXT_GREEN,"%s [%s]",
						get("server_starting"),getProperty("local_addr"));
				server = new aServer(serverHandler);
				if(server->start(getProperty("local_addr"))) {
#ifdef WOT_GTK
					gtk_button_set_label(GTK_BUTTON(mainFrame->gtk.serverbutton1),get("server_stop"));
					gtk_button_set_label(GTK_BUTTON(mainFrame->gtk.serverbutton2),get("server_stop"));
#endif /* WOT_GTK */
					consolef(TXT_GREEN,get("server_running"));
				} else consolef(TXT_RED,get("server_err_start"));
#ifdef WOT_GTK
				if(!GTK_WIDGET_IS_SENSITIVE(mainFrame->gtk.serverbutton1))
					gtk_widget_set_sensitive(mainFrame->gtk.serverbutton1,TRUE);
				if(!GTK_WIDGET_IS_SENSITIVE(mainFrame->gtk.serverbutton2))
					gtk_widget_set_sensitive(mainFrame->gtk.serverbutton2,TRUE);
#endif /* WOT_GTK */
				runThread.start(_run,this);
				mainFrame->startUpdatingMap();//rotateThread.start(_rotate,this);
				sendqueueThread.start(_sendqueue,this);
			} else if(stop) {
				runThread.kill();
				mainFrame->stopUpdatingMap();//rotateThread.kill();
				sendqueueThread.kill();
				server->stop();
				delete server;
				server = 0;
#ifdef WOT_GTK
				gtk_button_set_label(GTK_BUTTON(mainFrame->gtk.serverbutton1),get("server_start"));
				gtk_button_set_label(GTK_BUTTON(mainFrame->gtk.serverbutton2),get("server_start"));
#endif /* WOT_GTK */
				consolef(TXT_GREEN,get("server_stopped"));
				removeSendqueue();
			}
			setStatus(ST_WOT_SERVER_RUNNING,server!=0);
		}
	} else consolef(TXT_RED,get("server_no_world"));
	if(st&ST_WOT_SERVER_LOAD_HOSTS) mainFrame->loadHosts();
}

const char *WebOfTales::TeamToChannel(Team &t,char *ch) {
	sprintf(ch,"t#%" PRIx32,t.getID());
	return ch;
}

void WebOfTales::deleteChannel(Team &ch) {
	if(!server) return;
	char str[20];
	server->deleteChannel(TeamToChannel(ch,str));
}

void WebOfTales::joinChannel(Team &ch,aConnection c) {
	if(!server) return;
	char str[20];
	server->joinChannel(TeamToChannel(ch,str),c);
}

void WebOfTales::leaveChannel(Team &ch,aConnection c) {
	if(!server) return;
	char str[20];
	if(ch.members()==0) server->deleteChannel(TeamToChannel(ch,str));
	else server->leaveChannel(TeamToChannel(ch,str),c);
}

void WebOfTales::msgf(Player &to,uint8_t font,const char *format, ...) {
	if(!server) return;
	char buf[101];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,100,format,args);
   va_end(args);
	msg(to,font,buf);
}

void WebOfTales::msgf(uint32_t to,uint8_t font,const char *format, ...) {
	if(!server) return;
	char buf[101];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,100,format,args);
   va_end(args);
	msg(to,font,buf);
}

void WebOfTales::msgf(Team &to,uint8_t font,const char *format, ...) {
	if(!server) return;
	char buf[101];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,100,format,args);
   va_end(args);
	msg(to,font,buf);
}

void WebOfTales::msgf(const char *to,uint8_t font,const char *format, ...) {
	if(!server) return;
	char buf[101];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,100,format,args);
   va_end(args);
	msg(to,font,buf);
}

void WebOfTales::msg(Player &to,uint8_t font,const char *m) {
	to.getTeam()? msg(*to.getTeam(),font,m) : msg(to.getID(),font,m);
}

void WebOfTales::msg(uint32_t to,uint8_t font,const char *msg) {
	if(!server) return;
	int l = strlen(msg)+1;
printf("WebOfTales::msg(to=%d,msg=%s,l=%d)",to,msg,l);
	uint8_t d[sizeof_messageheader+l],*p = d;
	packMessageHeader(&p,0,to,font);
	memcpy(p,msg,l);
	if(to==0) {
		consolef(TXT_BLUE,"%s",msg);
		server->send(d,sizeof(d));
	} else {
		aConnection client = server->getClient(to);
		if(client) {
			server->lock();
			server->send(client,d,sizeof(d));
			server->unlock();
		}
	}
}

void WebOfTales::msg(Team &to,uint8_t font,const char *m) {
	char str[20];
	msg(TeamToChannel(to,str),font,m);
}

void WebOfTales::msg(const char *to,uint8_t font,const char *msg) {
	if(!server) return;
	int l = strlen(msg)+1;
printf("WebOfTales::msg(to=%s,msg=%s,l=%d)",to,msg,l);
	uint8_t d[sizeof_messageheader+l],*p = d;
	packMessageHeader(&p,0,0,font);
	memcpy(p,msg,l);
	server->send(server->getChannel(to),d,sizeof(d));
}

void WebOfTales::send(uint8_t to,Character *c,uint8_t *data,size_t len) {
	if(to&SEND_TO_CLIENTS) send(data,len);
	else if(c) {
		if(to&SEND_TO_CHANNEL) send(*(Player *)c,data,len);
		else if(to&SEND_TO_CLIENT) send(c->getID(),data,len);
	}
}

void WebOfTales::send(Player &to,uint8_t *data,size_t len) {
	to.getTeam()? send(*to.getTeam(),data,len) : send(to.getID(),data,len);
}

void WebOfTales::send(uint32_t to,uint8_t *data,size_t len) {
	if(!server || !data || !len) return;
	server->lock();
	aConnection client = server->getClient(to);
	if(client) server->send(client,data,len);
	server->unlock();
}

void WebOfTales::send(Team &to,uint8_t *data,size_t len) {
	char str[20];
	send(TeamToChannel(to,str),data,len);
}

void WebOfTales::send(aConnection client,uint8_t *data,size_t len) {
	if(server && client && data && len>0) server->send(client,data,len);
}


void WebOfTales::consolef(int color,const char *format, ...) {
	char msg[1026];
	va_list args;
   va_start(args,format);
	vsnprintf(msg+1,1024,format,args);
   va_end(args);

printf("%s",msg+1);

	if(mainFrame) {
		*msg = '\n';
		mainFrame->console(color,msg);
	}
}



