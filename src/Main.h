#ifndef _MAIN_H
#define _MAIN_H


#define APP_PROJECT				"weboftales"
#define APP_TITLE					"A Web of Tales v0.1"
#define APP_USER_AGENT			"WebOfTales"

#define WEB_HOST					"mantichora.net"
#define WEB_PATH					"/wotnet/"

/*
#define DAT_PATH "./data/"
#define LOG_PATH "./"
#define SERVER_PATH "./data/server/"
#define CLIENT_PATH "./data/client/"
#define CHARS_PATH "./data/server/chars/"
#define AUDIO_PATH "./data/audio/"
#define IMAGES_PATH "./data/images/"
#define FONTS_PATH "./data/fonts/"
#define LANGUAGES_PATH "./data/lang/"
*/

#define NICK_CHARS				"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ|_=+.,:;/\\?!@#$%^&*()~`"

enum {
	TXT_BLACK,
	TXT_RED,
	TXT_ORANGE,
	TXT_YELLOW,
	TXT_GREEN,
	TXT_CYAN,
	TXT_BLUE,
	TXT_PURPLE,
	TXT_MAGENTA,
};

enum COLOUR {
	COL_WHITE,
	COL_GREY,
	COL_BLACK,
	COL_RED,
	COL_ORANGE,
	COL_YELLOW,
	COL_GREEN,
	COL_CYAN,
	COL_BLUE,
	COL_PURPLE,
	COL_MAGENTA,
};

enum FONT {
	FONT_MINI,
// 	FONT_SMALL,
// 	FONT_SMALL_SERIF,
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
// 	FONT_WHITE_I,
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
// 	FONT_SERIF,
	FONT_LARGE,
// 	FONT_LARGE_SERIF
};


#define GAME_FPS 12


#endif

