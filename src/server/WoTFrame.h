#ifndef _SERVER_WOTFRAME_H
#define _SERVER_WOTFRAME_H


#include <stdio.h>

#ifdef WOT_GTK
#	include <gtk/gtk.h>
#endif /* WOT_GTK */


enum {
	ROTATION,
	NO_ROTATION,
	RESET_ROTATION,
};


class WoTFrame {
friend class WebOfTales;
private:
	struct hostinfo {
		char id[13];
		char name[33];
		char ip[16];
		char players[13];
		char online[13];
	};
	struct charinfo {
		uint32_t id;
		char name[33];
		player_data *data;
	};

#ifdef WOT_GTK
	static const gchar *coltags[];

	struct {
		GtkWidget *window;
		GtkWidget *notebook;
		GtkWidget *charlabel;
		GtkWidget *charimage;
		GtkWidget *charscombo;
		GtkWidget *hostlist;
		GtkWidget *serverbutton1;
		GtkWidget *serverbutton2;
		GtkWidget *maparea;
		GtkWidget *chattextscroll;
		GtkWidget *chattextview;
		GtkWidget *chatentry;
		GtkWidget *mapviewcombo;
		GtkWidget *playerlist;
		GdkPixmap *maparea_map;
		GdkPixmap *maparea_buffer;
		GdkFont *font;
		GtkTextBuffer *chattextbuffer;
		GtkTextIter chattextiter;
	} gtk;

	int mapview;
	int maparea_paint;
	guint maparea_id;

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void charscombo_changed_callback(GtkComboBox *widget,gpointer data);
	static void createchar_clicked_callback(GtkWidget *widget,gpointer data);
	static void deletechar_clicked_callback(GtkWidget *widget,gpointer data);
	static void updateservlist_clicked_callback(GtkWidget *widget,gpointer data);
	static void serverbutton_clicked_callback(GtkWidget *widget,gpointer data);
	static void clientbutton_clicked_callback(GtkWidget *widget,gpointer data);
	static gboolean maparea_configure_event_callback(GtkWidget *widget,GdkEventConfigure *event);
	static gboolean maparea_expose_event_callback(GtkWidget *widget,GdkEventExpose *event);
	static gboolean maparea_timeout_callback(gpointer data);
	static void chatmsg_clicked_callback(GtkWidget *widget,gpointer data);
	static void createworld_clicked_callback(GtkWidget *widget,gpointer data);
	static void openworld_clicked_callback(GtkWidget *widget,gpointer data);
	static void serversettings_clicked_callback(GtkWidget *widget,gpointer data);
	static void mapview_changed_callback(GtkComboBox *widget,gpointer data);


#endif /* WOT_GTK */

	hostinfo *hosts;
	charinfo *chars;
	int nchars;

	unsigned long charImageID;

public:
	WoTFrame(const char *caption);
	~WoTFrame();

	void startClient();
	void sendChatMessage(const char *msg);

	void loadHosts();
	void loadChar(const char *c);
	void loadChar(int n);
	void loadChars(const char *sel=0);

	void startUpdatingMap();
	void stopUpdatingMap();
	void repaintMap();
	void updateMap();
	void setMapView(int v);

	void insertClient(aConnection client);
	void removeClient(aConnection client);

	void console(int col,const char *msg);
};


#endif /* _SERVER_WOTFRAME_H */


