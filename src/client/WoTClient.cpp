
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>
#include <libamanita/net/aClient.h>
#include <libamanita/sdl/aGraphics.h>
#include <libamanita/sdl/aFont.h>
#include <libamanita/sdl/aButton.h>
#include <libamanita/sdl/aIconButton.h>
#include <libamanita/sdl/aScroll.h>
#include <libamanita/sdl/aTabset.h>
#include <libamanita/sdl/aTextbox.h>
#include <libamanita/sdl/aTooltip.h>
#include "../Main.h"
#include "../command.h"
#include "WoTClient.h"
#include "Message.h"
#include "WorldDisplay.h"
#include "CityDisplay.h"
#include "Panel.h"
#include "MapDlg.h"
#include "IncarnateDlg.h"
#include "CharacterDlg.h"
#include "BattleDlg.h"
#include "TeamDlg.h"
#include "../game/Cloud.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Team.h"



WoTClient app;


aMouseEvent me;
aMouseMotionEvent mme;
aKeyEvent ke;
aTooltipEvent tte;


const float WoTClient::version = 0.1f;


int main(int argc,char *argv[]) {
	return app.init(argc,argv);
}


WoTClient::WoTClient() : aApplication(APP_PROJECT,"client"),client(0),player(0),hover(0),focus(0) {
printf("WoTClient::WoTClient(1)");
	panel = 0;
	chatFirst = 0;
	chatLast = 0;
printf("WoTClient::WoTClient(2)");
}

WoTClient::~WoTClient() {
printf("WoTClient::~WoTClient() almost done");
//	settings.save(DAT_PATH "settings.dat");
printf("aImage::created=%d\nImage::deleted=%d",aImage::created,aImage::deleted);
//printf("aObject::created=%d\nObject::deleted=%d",aObject::created,aObject::deleted);
//printf("Character::created=%d\Character::deleted=%d",Character::createdCharacter::deleted);
printf("aPath::created=%d\nPath::deleted=%d",aPath::created,aPath::deleted);
}

int WoTClient::init(int argc,char *argv[]) {
printf("init(1)");
	char host[256],s[256];

	aApplication::init();
	setUserAgent(APP_USER_AGENT);

	for(int i=0; i<argc; i++) printf("argv[%d]=%s",i,argv[i]);
	if(argv[1] && *argv[1]) sprintf(host,"%s %s",argv[1],argv[2]);
	else strcpy(host,"127.0.0.1:4444 1|Nobody");

	if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)<0) {
printf("Unable to init SDL: %s",SDL_GetError());
		::exit(1);
	}
	if(SDLNet_Init()==-1) {
printf("SDLNet_Init: %s",SDLNet_GetError());
		::exit(2);
	}
	if(TTF_Init()==-1) {
printf("TTF_Init: %s",TTF_GetError());
		::exit(3);
	}

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

/*int audio_rate = 22050;
uint16_t audio_format = AUDIO_S16; // 16-bit stereo
int audio_channels = 2;
int audio_buffers = 4096;*/
/* This is where we open up our audio device.  Mix_OpenAudio takes
as its parameters the audio format we'd /like/ to have. */
/*if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
printf("Unable to open audio!\n");
exit(1);
}
atexit(Mix_CloseAudio);*/

/* If we actually care about what we got, we can ask here.  In this
program we don't, but I'm showing the function call here anyway
in case we'd want to know later. */
/*Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
music = Mix_LoadMUS("audio/Multiverse.mp3");
Mix_PlayMusic(music,-1);*/
//Mix_HookMusicFinished(musicDone);

printf("init(2,host=%s)",host);

	const char *app_title = APP_TITLE;

	setProperty("connect_host",host);
	setProperty("caption",app_title);

	sprintf(s,"%simages/icons/icon32.bmp",app.getHomeDir());
	aGraphics::init(app_title,s,800,600,0,SDL_SWSURFACE|SDL_ANYFORMAT);
//		aGraphics::init(SDL_INIT_AUDIO|SDL_INIT_VIDEO,1024,768,16,SDL_HWSURFACE/*SDL_SWSURFACE*/|SDL_FULLSCREEN);

	load();
printf("init(3)");

	command(CMD_ENTER,worldDisplay);
	startThreads();

printf("Starting client...");
	client = new aClient(clientHandler);
	client->start(host);
	setLocalID(client->getID()/*atoi(getProperty("id"))*/);
/*
	char file[256];
	sprintf(file,LOG_PATH "client%d.log",client->getID());
	freopen(file,"w",stderr);
*/
	events();
	exit();
	return 0;
}


void WoTClient::events() {
	int t1 = 0,t2 = 0,dx = 0,dy = 0,count = 0;
	SDL_Event event;
	aDisplay *f;
	while(SDL_WaitEvent(&event)) {
// printf("WoTClient::handleEvents(type=%d)",event.type);
		f = aDisplay::getActiveDisplay();
		if(f && f->isVisible()) switch(event.type) {
			case SDL_KEYDOWN:
			{
				lock();
printf("WoTClient::handleEvents(code=%c,sym=%c)",(char)event.key.keysym.unicode,event.key.keysym.sym);
				aKeyEvent ke = { 0,event.key.keysym.sym,event.key.keysym.mod,event.key.keysym.unicode };
				f->handleKeyDown(ke);
				unlock();
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				lock();
				t2 = SDL_GetTicks();
				if(t2-t1>300) count = 0;
				aMouseEvent me = { 0,event.button.x,event.button.y,event.button.button,++count };
				dx = event.button.x,dy = event.button.y,t1 = t2;
				f->handleMouseDown(me);
				unlock();
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				lock();
				aMouseEvent me = { 0,event.button.x,event.button.y,event.button.button,count };
				dx = dy = 0;
				f->handleMouseUp(me);
				unlock();
				break;
			}
			case SDL_MOUSEMOTION:
			{
				lock();
				aMouseMotionEvent mme = { 0,mx=event.motion.x,my=event.motion.y,dx,dy };
				if(event.motion.state) f->handleMouseDrag(mme);
				else f->handleMouseMove(mme);
				unlock();
				break;
			}
			case SDL_QUIT:return;
		}
// printf("WoTClient::handleEvents(done)");
	}
}


void WoTClient::exit() {
printf("WoTClient::exit(1)");

	char str[256];
	aDisplay *f1 = aDisplay::getActiveDisplay();
printf("WoTClient::exit(f1=%p)",f1);
	if(f1) f1->stop();
printf("WoTClient::exit(2)");
	Player *pl;
	if((pl=(Player *)Map::getItem(client->getID()))) pl->saveAvatarImage();
printf("WoTClient::exit(3)");
	stopThreads();
printf("WoTClient::exit(4)");
	client->stop();
printf("WoTClient::exit(5)");
	aGraphics::close();
printf("WoTClient::exit(6)");
	sprintf(str,"%sclient/map.dat",app.getHomeDir());
	::remove(str);
	sprintf(str,"%sclient/cld.dat",app.getHomeDir());
	::remove(str);
printf("WoTClient::exit(7)");

/*Mix_HaltMusic();
Mix_FreeMusic(music);
music = 0;*/

	int i;
	for(i=0; i<32; i++) if(fonts[i]) {
printf("WoTClient::exit(fonts[%d]=%s)",i,fonts[i]->getName());
		delete fonts[i];fonts[i] = 0;
	}
printf("WoTClient::exit(8)");
	if(cityDisplay) { delete cityDisplay;cityDisplay = 0; }
printf("WoTClient::exit(9)");
	if(worldDisplay) { delete worldDisplay;worldDisplay = 0; }

printf("WoTClient::exit(10)");
	if(panel) { delete panel;panel = 0; }
printf("WoTClient::exit(11)");
	Message *msg;
	while(chatFirst) { msg = chatFirst->next;delete chatFirst;chatFirst = msg; }

printf("WoTClient::exit(12)");
	if(client) { delete client;client = 0; }
printf("WoTClient::exit(13)");
	{
		aImage *img;
		Location *l;
		Team *t;
		Character *c;
		aHashtable::iterator iter = db.iterate();
		while((img=(aImage *)iter.next())) delete img;

		Map::deleteAllClouds();
		iter = Map::locationids.iterate();
		while((l=(Location *)iter.next())) delete l;
		iter = Map::teamids.iterate();
		while((t=(Team *)iter.next())) delete t;
		iter = Map::itemids.iterate();
		while((c=(Character *)iter.next())) delete c;
	}

	TTF_Quit();
	SDLNet_Quit();
	SDL_Quit();

	for(i=0; i<10; i++) free(plspr[i]);

	aApplication::exit();

printf("WoTClient::exit(done)");
}

uint32_t WoTClient::getID() {
	return client? client->getID() : 0;
}

Player *WoTClient::getLeader() {
	return player? (player->getTeam()? player->getTeam()->getLeader() : player) : 0;
}

bool WoTClient::load() {
	char s[256];
setLocalTime(SDL_GetTicks());
printf("WoTClient::load(sizeof(Character)=%d,sizeof(Player)=%d)",sizeof(Character),sizeof(Player));
printf("WoTClient::load(1,fonts=%d)",sizeof(fonts));
	memset(fonts,0,sizeof(fonts));
/*
enum FONT {
	FONT_MINI,
	FONT_SMALL,
	FONT_SMALL_SERIF,
	FONT_WHITE,
	FONT_GREY,
	FONT_BLACK,
	FONT_RED,
	FONT_ORANGE,
	FONT_YELLOW,
	FONT_GREEN,
	FONT_CYAN,
	FONT_BLUE,
	FONT_PURPLE,
	FONT_MAGENTA,
	FONT_WHITE_I,
	FONT_WHITE_B,
	FONT_GREY_B,
	FONT_BLACK_B,
	FONT_RED_B,
	FONT_ORANGE_B,
	FONT_YELLOW_B,
	FONT_GREEN_B,
	FONT_CYAN_B,
	FONT_BLUE_B,
	FONT_PURPLE_B,
	FONT_MAGENTA_B,
	FONT_SERIF,
	FONT_LARGE,
	FONT_LARGE_SERIF
};
*/
	sprintf(s,"%sProFont.ttf",getFontsDir());
	fonts[FONT_MINI]				= new aFont(s,9,TTF_STYLE_NORMAL,0xffffff,2);
	fonts[FONT_MINI]->setSpacing(1);
	fonts[FONT_WHITE]				= new aFont(s,12,TTF_STYLE_NORMAL,0xffffff,0);
	fonts[FONT_GREY]				= new aFont(s,12,TTF_STYLE_NORMAL,0x999999,0);
	fonts[FONT_BLACK]				= new aFont(s,12,TTF_STYLE_NORMAL,0x000000,0);
	fonts[FONT_RED]				= new aFont(s,12,TTF_STYLE_NORMAL,0xff6666,0);
	fonts[FONT_ORANGE]			= new aFont(s,12,TTF_STYLE_NORMAL,0xe69900,0);
	fonts[FONT_YELLOW]			= new aFont(s,12,TTF_STYLE_NORMAL,0xffff66,0);
	fonts[FONT_GREEN]				= new aFont(s,12,TTF_STYLE_NORMAL,0x66ff66,0);
	fonts[FONT_CYAN]				= new aFont(s,12,TTF_STYLE_NORMAL,0x66ffff,0);
	fonts[FONT_BLUE]				= new aFont(s,12,TTF_STYLE_NORMAL,0x6666ff,0);
	fonts[FONT_PURPLE]			= new aFont(s,12,TTF_STYLE_NORMAL,0x9966ff,0);
	fonts[FONT_MAGENTA]			= new aFont(s,12,TTF_STYLE_NORMAL,0xff66ff,0);
	sprintf(s,"%sBodoni_Bold.ttf",getFontsDir());
	fonts[FONT_WHITE_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0xffffff,2);
	fonts[FONT_GREY_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x999999,2);
	fonts[FONT_BLACK_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x000000,2);
	fonts[FONT_RED_B]				= new aFont(s,11,TTF_STYLE_NORMAL,0xff6666,2);
	fonts[FONT_ORANGE_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0xe69900,2);
	fonts[FONT_YELLOW_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0xffff66,2);
	fonts[FONT_GREEN_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x66ff66,2);
	fonts[FONT_CYAN_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x66ffff,2);
	fonts[FONT_BLUE_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x6666ff,2);
	fonts[FONT_PURPLE_B]			= new aFont(s,11,TTF_STYLE_NORMAL,0x9966ff,2);
	fonts[FONT_MAGENTA_B]		= new aFont(s,11,TTF_STYLE_NORMAL,0xff66ff,2);
	sprintf(s,"%sVinque.ttf",getFontsDir());
	fonts[FONT_LARGE]				= new aFont(s,13,TTF_STYLE_NORMAL,0xffffff,2);
setLocalTime(SDL_GetTicks());


printf("WoTClient::load(2)");
console(fonts[FONT_GREY],"Loading Images");
	loadImages(getHomeDir(),"images.xim");

printf("WoTClient::load(3)");

setLocalTime(SDL_GetTicks());
	mainFrameImage								= (aImage *)db.get("MainFrame");
	cloudsImage									= (aImage *)db.get("Clouds");

	static const char *terrainImageNames[2] = { "Ground","Terrain" };
	static const set terrainImageLen[2] = { {OCEAN,3},{MOUNTAIN,4} };
	loadImageSet(terrainImages,(set){0,2},terrainImageNames,terrainImageLen);

	static const char *constructImageNames[4] = { "Roads","Cities","Walls","Houses" };
	static const set constructImageLen[4] = { {0,1},{0,3},{0,1},{0,1} };
	loadImageSet(constructImages,(set){ROAD,4},constructImageNames,constructImageLen);

	npcspr[0]									= (aImage *)db.get("NPC");
	npcspr[1]									= 0;

	static const char *plsprImageNames[8] = { "Skin","Hair","Face","Chest","Legs","Feet","Head","Back" };
	static const set plsprImageLen[8] = { {0,10},{0,7},{0,1},{0,1},{0,1},{0,1},{0,1},{0,3} };
	loadImageSet(plspr,(set){AVT_SKIN,8},plsprImageNames,plsprImageLen);

	/*for(n=AVT_SKIN; n<=AVT_LHAND; n++) {
		plspr[n] = (aImage **)malloc(sizeof(aImage **)*plsprImages[n]);
		if(plsprImageNames[n]) for(i=plsprImages[n]-1; i>=0; i--) {
			sprintf(str,"%s%d",plsprImageNames[n],i+1);
			plspr[n][i] = (aImage *)db.get(str);
		}
	}*/
	shadows										= (aImage *)db.get("Shadows");
	faces[0]										= (aImage *)db.get("FaceIcons32");
	faces[1]										= (aImage *)db.get("FaceIcons16");
	items[0]										= (aImage *)db.get("ItemIcons32");
	items[1]										= (aImage *)db.get("ItemIcons16");
	gui[0]										= (aImage *)db.get("GUI");
	gui[1]										= (aImage *)db.get("ChrGui");
	gui[2]										= (aImage *)db.get("IncDlg");
	gui[3]										= (aImage *)db.get("ChrDlg");
	gui[4]										= (aImage *)db.get("BtlDlg");
	gui[5]										= 0;
	gui[6]										= 0;
	gui[7]										= 0;
	gui[8]										= (aImage *)db.get("ChatBg");
	gui[9]										= 0;
	gui[10]										= (aImage *)db.get("Buttons");
	gui[11]										= 0;
	gui[12]										= 0;
	gui[13]										= 0;
	gui[14]										= 0;
	gui[15]										= 0;
	icons											= (aImage *)db.get("Icons");
	btlicons										= (aImage *)db.get("BattleIcons");
	markers										= (aImage *)db.get("Markers");

setLocalTime(SDL_GetTicks());
printf("WoTClient::load(4)");
	aFont *f = fonts[FONT_WHITE];
	uint32_t buttondata[36] = {
		22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
		40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57
	};
	uint32_t iconbuttondata[4] = { 58,59,60,61 };
	uint32_t windowdata[21] = { 0x642b2b,1,2,3,4,5,6,7,8,9,10,11,12,13,14,3,3,15,16,17,15 };
	uint32_t scrolldata[22] = { 62,63,64,62,65,66,67,65,68,69,70,68,71,72,73,71,74,75,76,74,77,78, };
	uint32_t tabsetdata[26] = { 79,80,81,82,83,84,85,86,87,0,5,0,5,88,89,90,91,92,93,94,95,96,0,5,0,5 };
	uint32_t tooltipdata[13] = { 97,98,99,100,101,102,103,104,105,2,0,2,0 };
	aButton::setDefaultSettings(gui[0],f,buttondata);
	aIconButton::setDefaultSettings(gui[0],iconbuttondata);
	aWindow::setDefaultSettings(gui[0],0,f,windowdata);
	aScroll::setDefaultSettings(gui[0],scrolldata);
	aTabset::setDefaultSettings(gui[0],f,f,tabsetdata);
	aTextfield::setDefaultSettings(f);
	aTooltip::setDefaultSettings(gui[0],f,tooltipdata);

	strcpy(aTextbox::yes,get("yes"));
	strcpy(aTextbox::no,get("no"));
	strcpy(aTextbox::ok,get("ok"));
	strcpy(aTextbox::cancel,get("cancel"));

setLocalTime(SDL_GetTicks());
printf("WoTClient::load(8)");
	panel = new Panel();

console(fonts[FONT_GREY],"Creating Interface");
	mapdlg = new MapDlg();
	mapdlg->setVisible(false);
	incdlg = new IncarnateDlg();
	incdlg->setVisible(false);
	chrdlg = new CharacterDlg();
	chrdlg->setVisible(false);
	btldlg = new BattleDlg();
	btldlg->setVisible(false);
	teamdlg = new TeamDlg();
	teamdlg->setVisible(false);
setLocalTime(SDL_GetTicks());
printf("WoTClient::load(worldDisplay)");
	worldDisplay = new WorldDisplay();
setLocalTime(SDL_GetTicks());
printf("WoTClient::load(worldDisplay=%p)",worldDisplay);
printf("WoTClient::load(cityDisplay)");
	cityDisplay = new CityDisplay();
setLocalTime(SDL_GetTicks());
printf("WoTClient::load(cityDisplay=%p)",cityDisplay);
	return true;
}

bool WoTClient::loadImages(const char *p,const char *fn) {
	int n,i;
	setLocalTime(SDL_GetTicks());
printf("Load xim file: %s",fn);

	aImage **images = aImage::readXIM(p,fn,n);
	setLocalTime(SDL_GetTicks());
printf("images(n=%d)",n);
	if(images && n>0) for(i=0; i<n; i++) {
		db.put(images[i]->getName(),(aObject *)images[i]);
printf("image(%s)",images[i]->getName());
	}
	delete[] images;
printf("Finished reading...");
	return true;
}

void WoTClient::loadImageSet(aImage **images[],set list,const char **names,const set *parts) {
	int n,i;
	char str[32];
	if(list.index>0) for(n=0; n<list.index; n++) images[n] = 0;
	for(n=0; n<list.count; n++) if(parts[n].count>0) {
		images[list.index+n] = (aImage **)malloc(sizeof(aImage **)*(parts[n].index+parts[n].count));
		for(i=parts[n].count-1; i>=0; i--) {
			sprintf(str,"%s%d",names[n],i+1);
			images[list.index+n][parts[n].index+i] = (aImage *)db.get(str);
		}
	}
}

void WoTClient::startThreads() {
console(fonts[FONT_GREY],"Starting aClient");
	runThread.start(_run,this);
}

void WoTClient::stopThreads() {
console(fonts[FONT_GREY],"Stopping aClient");
	runThread.kill();
}


void WoTClient::run() {
printf("WoTClient::run(1)");
	int i,n;
	Character *c;
	aDisplay *f;
	aHashtable::iterator iter;
	aTooltipEvent tte = { 0,0,0,GAME_FPS/2,GAME_FPS*7 };
	for(time.frames=0; 1; time.frames++) {
		time.ticks = SDL_GetTicks();
		setLocalTime(time.ticks);
		lock();
// printf("WoTClient::run(frames=%d)",time.frames);

		for(i=0,n=Map::cloudids.size(); i<n; i++) ((Cloud *)Map::cloudids[i])->animate();

		if((time.frames&1)==0) {
			iter = Map::itemids.iterate();
			while((c=(Character *)iter.next())) c->cycle(time.frames);
		}
		iter = Map::itemids.iterate();
		while((c=(Character *)iter.next())) c->animate();

		f = aDisplay::getActiveDisplay();
		if(f) {
			f->paint(time.frames);
			tte.x = mx,tte.y = my;
			f->handleToolTip(tte);
			g.flip();
		}

// printf("WoTClient::run(done)");
		unlock();
		runThread.pauseFPS(GAME_FPS);
	}
}



bool WoTClient::consolef(const char *format, ...) {
	char msg[129];
	va_list list;
	va_start(list,format);
	vsnprintf(msg,128,format,list);
	va_end(list);
	return console(0,msg);
}

bool WoTClient::consolef(aFont *f,const char *format, ...) {
	char msg[129];
	va_list list;
	va_start(list,format);
	vsnprintf(msg,128,format,list);
	va_end(list);
	return console(f,msg);
}

bool WoTClient::console(aFont *f,const char *msg) {
printf("Console: %s",msg);
	WoTClient::chatFirst = new Message(0,msg,f,WoTClient::chatFirst);
	if(WoTClient::chatLast==NULL) WoTClient::chatLast = WoTClient::chatFirst;
	return true;
}


void WoTClient::msgf(uint32_t to,uint8_t font,const char *format, ...) {
	if(!client) return;
	char buf[101];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,100,format,args);
   va_end(args);
	msg(to,font,buf);
}

void WoTClient::msg(uint32_t to,uint8_t font,const char *msg) {
	if(!client) return;
printf("WoTClient::msg(to=%d,font=%d,msg=\"%s\")",to,font,msg);
	int l = strlen(msg)+1;
	uint8_t d[sizeof_messageheader+l],*p = d;
	packMessageHeader(&p,client->getID(),to,font);
	memcpy(p,msg,l);
	client->send(d,sizeof(d));
}

void WoTClient::send(uint8_t *data,size_t len) {
	if(client && data && len>0) client->send(data,len);
}

void WoTClient::sendRequest(uint8_t request,uint32_t id) {
	if(!client) return;
	uint8_t d[sizeof_requestheader],*p = d;
	packRequestHeader(&p,request,id);
	client->send(d,sizeof(d));
}



