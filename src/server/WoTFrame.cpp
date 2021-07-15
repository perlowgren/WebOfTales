
#include "../config.h"
#include <stdio.h>
#include <string.h>

#ifdef WOT_GTK
	#include <gtk/gtk.h>
#endif /* WOT_GTK */

#include <libamanita/aVector.h>
#include <libamanita/aDictionary.h>
#include <libamanita/aThread.h>
#include <libamanita/net/aServer.h>
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "../command.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "ServerWorld.h"
#include "CreateCharDlg.h"
#include "CreateWorldDlg.h"
#include "../game/Cloud.h"
#include "../game/NPC.h"
#include "../game/Player.h"
#include "../game/Race.h"


#ifdef WOT_GTK
enum {
	TAB_MAIN,
	TAB_SERVER,
	TAB_PATCHES
};

enum {
	HOSTLIST_WORLD,
	HOSTLIST_IP,
	HOSTLIST_PLAYERS,
	HOSTLIST_ONLINE,
	HOSTLIST_COLS
};

enum {
	PLAYERLIST_NAME,
	PLAYERLIST_COLS
};

const gchar *WoTFrame::coltags[] = {
	"black_fg","red_fg","orange_fg","yellow_fg","green_fg","cyan_fg","blue_fg","purple_fg","magenta_fg",
};


gboolean WoTFrame::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
//	WoTFrame *dlg = (WoTFrame *)data;
	app.exit();
	gtk_main_quit();
	return FALSE;
}

void WoTFrame::charscombo_changed_callback(GtkComboBox *widget,gpointer data) {
app.printf("charscombo_changed_callback(1)");
	WoTFrame *dlg = (WoTFrame *)data;
	gint n = gtk_combo_box_get_active(GTK_COMBO_BOX(dlg->gtk.charscombo));
	dlg->loadChar(n);
app.printf("charscombo_changed_callback(2)");
}

void WoTFrame::createchar_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("createchar_clicked_callback()");
	WoTFrame *dlg = (WoTFrame *)data;
	new CreateCharDlg(GTK_WINDOW(dlg->gtk.window),app.get("char_caption"));
}

void WoTFrame::deletechar_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("deletechar_clicked_callback()");
//	WoTFrame *dlg = (WoTFrame *)data;
}

void WoTFrame::updateservlist_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("reloadservlist_clicked_callback()");
	WoTFrame *dlg = (WoTFrame *)data;
	dlg->loadHosts();
}

void WoTFrame::serverbutton_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("serverbutton_clicked_callback()");
//	WoTFrame *dlg = (WoTFrame *)data;
	app.switchServerState();
}

void WoTFrame::clientbutton_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("clientbutton_clicked_callback()");
	WoTFrame *dlg = (WoTFrame *)data;
	dlg->startClient();
}

/* Create a new backing pixmap of the appropriate size */
gboolean WoTFrame::maparea_configure_event_callback(GtkWidget *widget,GdkEventConfigure *event) {
	if(app.mainFrame->gtk.maparea_map) gdk_pixmap_unref(app.mainFrame->gtk.maparea_map);
	app.mainFrame->gtk.maparea_map = gdk_pixmap_new(widget->window,widget->allocation.width,widget->allocation.height,-1);
	if(app.mainFrame->gtk.maparea_buffer) gdk_pixmap_unref(app.mainFrame->gtk.maparea_buffer);
	app.mainFrame->gtk.maparea_buffer = gdk_pixmap_new(widget->window,widget->allocation.width,widget->allocation.height,-1);
	app.mainFrame->maparea_paint |= 2;
	app.mainFrame->updateMap();
	return TRUE;
}

/* Redraw the screen from the backing pixmap */
gboolean WoTFrame::maparea_expose_event_callback(GtkWidget *widget,GdkEventExpose *event) {
	app.mainFrame->updateMap();
	gdk_draw_pixmap(widget->window,widget->style->fg_gc[GTK_WIDGET_STATE(widget)],app.mainFrame->gtk.maparea_buffer,
		event->area.x,event->area.y,event->area.x,event->area.y,event->area.width,event->area.height);
	return FALSE;
}

gboolean WoTFrame::maparea_timeout_callback(gpointer data) {
	WoTFrame *dlg = (WoTFrame *)data;
	dlg->repaintMap();
	return TRUE;
}


void WoTFrame::chatmsg_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("chatmsg_clicked_callback()");
	WoTFrame *dlg = (WoTFrame *)data;
	const char *msg = gtk_entry_get_text(GTK_ENTRY(dlg->gtk.chatentry));
	dlg->sendChatMessage(msg);
	gtk_entry_set_text(GTK_ENTRY(dlg->gtk.chatentry),"");
}

void WoTFrame::createworld_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("createworld_clicked_callback()");
	WoTFrame *dlg = (WoTFrame *)data;
	new CreateWorldDlg(GTK_WINDOW(dlg->gtk.window),app.get("cwd_caption"));
}

void WoTFrame::openworld_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("openworld_clicked_callback()");
//	WoTFrame *dlg = (WoTFrame *)data;
}

void WoTFrame::serversettings_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("serversettings_clicked_callback()");
//	WoTFrame *dlg = (WoTFrame *)data;
}

void WoTFrame::mapview_changed_callback(GtkComboBox *widget,gpointer data) {
app.printf("mapview_changed_callback(1)");
	WoTFrame *dlg = (WoTFrame *)data;
	gint n = gtk_combo_box_get_active(GTK_COMBO_BOX(dlg->gtk.mapviewcombo));
	dlg->setMapView(n);
app.printf("mapview_changed_callback(2)");
}
#endif /* WOT_GTK */


WoTFrame::WoTFrame(const char *caption) {
	char s[256];
	hosts = 0;
	chars = 0;
	nchars = 0;
	charImageID = 0;

#ifdef WOT_GTK
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *label;
	GtkWidget *scroll;
	GtkWidget *button;
	GtkWidget *box,*hbox,*vbox;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkListStore *store;
	size_t i;

	gtk.maparea = NULL;
	gtk.maparea_map = NULL;
	gtk.maparea_buffer = NULL;
	gtk.font = NULL;
	gtk.chattextbuffer = 0;
	mapview = 0;
	maparea_paint = 2;
	maparea_id = 0;

	gtk.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gtk.window),caption);
	//gtk_window_set_default_size(GTK_WINDOW(window),780,540);
	gtk_widget_set_size_request(gtk.window,780,540);
	gtk_window_set_resizable(GTK_WINDOW(gtk.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gtk.window),GTK_WIN_POS_CENTER);
	sprintf(s,"%simages/icons/icon16.png",app.getHomeDir());
	gtk_window_set_icon(GTK_WINDOW(gtk.window),app.createPixbuf(s));
	//gtk_container_set_border_width(GTK_CONTAINER(window),5);

	gtk.notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(gtk.notebook),GTK_POS_BOTTOM);
	gtk_container_add(GTK_CONTAINER(gtk.window),gtk.notebook);

	table = gtk_table_new(2,2,FALSE);
	{ /* Main Tab - Character Box */
		frame = gtk_frame_new(app.get("mainfrm_char_box"));
		box = gtk_vbox_new(FALSE,5);
		gtk_container_set_border_width(GTK_CONTAINER(box),5);
		gtk.charscombo = gtk_combo_box_new_text();
		g_signal_connect(G_OBJECT(gtk.charscombo),"changed",G_CALLBACK(WoTFrame::charscombo_changed_callback),this);
		gtk_box_pack_start(GTK_BOX(box),gtk.charscombo,FALSE,FALSE,5);
		GtkWidget *t1 = gtk_table_new(2,2,FALSE);
		gtk.charlabel = gtk_label_new("");
		gtk_label_set_justify(GTK_LABEL(gtk.charlabel),GTK_JUSTIFY_LEFT);
		gtk_table_attach(GTK_TABLE(t1),gtk.charlabel,0,1,0,1,
			(GtkAttachOptions)(GTK_FILL),(GtkAttachOptions)(GTK_FILL),5,5);
		gtk.charimage = gtk_image_new();
		gtk_table_attach(GTK_TABLE(t1),gtk.charimage,1,2,0,1,
			(GtkAttachOptions)(GTK_FILL),(GtkAttachOptions)(GTK_FILL),5,5);
		gtk_box_pack_start(GTK_BOX(box),t1,TRUE,TRUE,5);
		hbox = gtk_hbox_new(FALSE,5);
		button = gtk_button_new_with_label(app.get("char_create"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(createchar_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
		button = gtk_button_new_with_label(app.get("char_delete"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(deletechar_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,5);
		gtk_container_add(GTK_CONTAINER(frame),box);
		gtk_table_attach(GTK_TABLE(table),frame,0,1,0,2,
			(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);
	}
	{ /* Main Tab - Host List Box */
		gchar *titles[] = {
			(gchar *)app.get("mainfrm_hcol_1"),(gchar *)app.get("mainfrm_hcol_2"),
			(gchar *)app.get("mainfrm_hcol_3"),(gchar *)app.get("mainfrm_hcol_4")
		};
		gint widths[] = { 150,120,60,60 };
		//frame = gtk_frame_new(app.get("mainfrm_serv_box"));
		//box = gtk_hbox_new(FALSE,5);
		//gtk_container_set_border_width(GTK_CONTAINER(box),5);
		scroll = gtk_scrolled_window_new(NULL,NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),GTK_SHADOW_IN);

		gtk.hostlist = gtk_tree_view_new();
		gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(gtk.hostlist),TRUE);
		gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk.hostlist)),GTK_SELECTION_SINGLE);
		for(i=HOSTLIST_WORLD; i<=HOSTLIST_ONLINE; i++) {
			column = gtk_tree_view_column_new();
			gtk_tree_view_column_set_title(column,titles[i]);
			gtk_tree_view_column_set_sizing(column,GTK_TREE_VIEW_COLUMN_FIXED);
			gtk_tree_view_column_set_fixed_width(column,widths[i]);
			gtk_tree_view_append_column(GTK_TREE_VIEW(gtk.hostlist),column);
			renderer = gtk_cell_renderer_text_new();
			gtk_tree_view_column_pack_start(column,renderer,TRUE);
			gtk_tree_view_column_add_attribute(column,renderer,"text",i);
		}
		store = gtk_list_store_new(HOSTLIST_COLS+1,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_POINTER);
		gtk_tree_view_set_model(GTK_TREE_VIEW(gtk.hostlist),GTK_TREE_MODEL(store));
		g_object_unref(store);

		gtk_container_add(GTK_CONTAINER(scroll),gtk.hostlist);
		//gtk_container_add(GTK_CONTAINER(box),scroll);
		//gtk_container_add(GTK_CONTAINER(frame),box);
		gtk_table_attach(GTK_TABLE(table),scroll,1,2,0,1,
			(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);
	}
	{ /* Main Tab - Controls */
		frame = gtk_frame_new(app.get("mainfrm_ctrl_box"));
		box = gtk_hbox_new(FALSE,5);
		gtk_container_set_border_width(GTK_CONTAINER(box),5);
		button = gtk_button_new_with_label(app.get("mainfrm_update_serv_list"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(updateservlist_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(box),button,FALSE,FALSE,0);
		gtk.serverbutton1 = gtk_button_new_with_label(app.get("server_start"));
		g_signal_connect(G_OBJECT(gtk.serverbutton1),"clicked",G_CALLBACK(serverbutton_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(box),gtk.serverbutton1,FALSE,FALSE,0);
		button = gtk_button_new_with_label(app.get("client_start"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(clientbutton_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(box),button,FALSE,FALSE,0);
		gtk_container_add(GTK_CONTAINER(frame),box);
		gtk_table_attach(GTK_TABLE(table),frame,1,2,1,2,
			(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_SHRINK),5,5);
	}
	//gtk_widget_show(table);
	label = gtk_label_new(app.get("mainfrm_tab_main"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gtk.notebook),table,label);


	hbox = gtk_hbox_new(FALSE,0);
	vbox = gtk_vbox_new(FALSE,0);
	//table = gtk_table_new(2,4,FALSE);
	{ /* aServer Tab - World Frame */
		gtk.maparea = gtk_drawing_area_new();
		gtk.font = gdk_font_load("-misc-fixed-bold-r-normal--13-*-*-*-*-*-*");
		gtk_widget_set_size_request(gtk.maparea,512,256);
		g_signal_connect(G_OBJECT(gtk.maparea),"expose_event",G_CALLBACK(WoTFrame::maparea_expose_event_callback),NULL);
		g_signal_connect(G_OBJECT(gtk.maparea),"configure_event",G_CALLBACK(WoTFrame::maparea_configure_event_callback),NULL);
		gtk_box_pack_start(GTK_BOX(vbox),gtk.maparea,FALSE,FALSE,5);
	}
	{ /* aServer Tab - Chat */
		const gchar *fg = "foreground";
		gtk.chattextscroll = gtk_scrolled_window_new(NULL,NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(gtk.chattextscroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(gtk.chattextscroll),GTK_SHADOW_IN);
		gtk.chattextview = gtk_text_view_new();

		gtk.chattextbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk.chattextview));
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_BLACK],fg,"#000000",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_RED],fg,"#990000",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_ORANGE],fg,"#996600",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_YELLOW],fg,"#999900",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_GREEN],fg,"#009900",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_CYAN],fg,"#009999",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_BLUE],fg,"#000099",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_PURPLE],fg,"#660099",NULL);
		gtk_text_buffer_create_tag(gtk.chattextbuffer,coltags[TXT_MAGENTA],fg,"#990066",NULL);
		gtk_text_buffer_get_iter_at_offset(gtk.chattextbuffer,&gtk.chattextiter,0);
		gtk_text_buffer_insert_with_tags_by_name(gtk.chattextbuffer,&gtk.chattextiter,APP_TITLE,-1,coltags[TXT_BLACK],NULL);

		gtk_container_add(GTK_CONTAINER(gtk.chattextscroll),gtk.chattextview);
		gtk_box_pack_start(GTK_BOX(vbox),gtk.chattextscroll,TRUE,TRUE,5);
		gtk.chatentry = gtk_entry_new();
		g_signal_connect(G_OBJECT(gtk.chatentry),"activate",G_CALLBACK(chatmsg_clicked_callback),this);
		gtk_box_pack_start(GTK_BOX(vbox),gtk.chatentry,FALSE,FALSE,5);
	}
	gtk_box_pack_start(GTK_BOX(hbox),vbox,FALSE,FALSE,5);

	vbox = gtk_vbox_new(FALSE,0);
	{ /* aServer Tab - Select Map View */
		frame = gtk_frame_new(app.get("mainfrm_mapview_box"));
		box = gtk_hbox_new(FALSE,5);
		gtk_container_set_border_width(GTK_CONTAINER(box),5);
		aVector list;
		list.split(app.get("mainfrm_view_list"),";",true);
		gtk.mapviewcombo = gtk_combo_box_new_text();
		for(i=0; i<list.size(); i++)
			gtk_combo_box_append_text(GTK_COMBO_BOX(gtk.mapviewcombo),(gchar *)list[i]);
		gtk_combo_box_set_active(GTK_COMBO_BOX(gtk.mapviewcombo),0);
		g_signal_connect(G_OBJECT(gtk.mapviewcombo),"changed",G_CALLBACK(mapview_changed_callback),this);
		gtk_container_add(GTK_CONTAINER(box),gtk.mapviewcombo);
		gtk_container_add(GTK_CONTAINER(frame),box);
		gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,0);
	}
	{ /* aServer Tab - aServer Controls */
		GtkWidget *t1 = gtk_table_new(2,2,TRUE);
		gtk_container_set_border_width(GTK_CONTAINER(t1),5);
		button = gtk_button_new_with_label(app.get("mainfrm_create_wrld"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(createworld_clicked_callback),this);
		gtk_table_attach_defaults(GTK_TABLE(t1),button,0,1,0,1);
		button = gtk_button_new_with_label(app.get("mainfrm_open_wrld"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(openworld_clicked_callback),this);
		gtk_table_attach_defaults(GTK_TABLE(t1),button,1,2,0,1);
		button = gtk_button_new_with_label(app.get("server_settings"));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(serversettings_clicked_callback),this);
		gtk_table_attach_defaults(GTK_TABLE(t1),button,0,1,1,2);
		gtk.serverbutton2 = gtk_button_new_with_label(app.get("server_start"));
		g_signal_connect(G_OBJECT(gtk.serverbutton2),"clicked",G_CALLBACK(serverbutton_clicked_callback),this);
		gtk_table_attach_defaults(GTK_TABLE(t1),gtk.serverbutton2,1,2,1,2);
		gtk_table_set_col_spacings(GTK_TABLE(t1),5);
		gtk_table_set_row_spacings(GTK_TABLE(t1),5);
		gtk_box_pack_start(GTK_BOX(vbox),t1,FALSE,FALSE,0);
	}
	{ /* aServer Tab - Player List */
		//frame = gtk_frame_new(app.get("mainfrm_players"));
		//box = gtk_hbox_new(FALSE,5);
		//gtk_container_set_border_width(GTK_CONTAINER(box),5);
		scroll = gtk_scrolled_window_new(NULL,NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
		//gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),GTK_SHADOW_IN);

		gtk.playerlist = gtk_tree_view_new();
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(app.get("mainfrm_players"),renderer,"text",PLAYERLIST_NAME,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(gtk.playerlist),column);
		store = gtk_list_store_new(PLAYERLIST_COLS,G_TYPE_STRING);
		gtk_tree_view_set_model(GTK_TREE_VIEW(gtk.playerlist),GTK_TREE_MODEL(store));
		g_object_unref(store);
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),gtk.playerlist);

		/*GtkWidget *list = gtk_list_new();
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),list);*/
		//gtk_container_add(GTK_CONTAINER(box),scroll);
		//gtk_container_add(GTK_CONTAINER(frame),box);
		gtk_box_pack_start(GTK_BOX(vbox),scroll,TRUE,TRUE,5);
	}
	gtk_box_pack_start(GTK_BOX(hbox),vbox,TRUE,TRUE,5);
	//gtk_widget_show(table);
	label = gtk_label_new(app.get("mainfrm_tab_server"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gtk.notebook),hbox,label);

	frame = gtk_frame_new(app.get("mainfrm_tab_patch"));
	gtk_container_set_border_width(GTK_CONTAINER(frame),5);
	label = gtk_label_new(app.get("mainfrm_tab_patch"));
	gtk_notebook_append_page(GTK_NOTEBOOK(gtk.notebook),frame,label);

	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
#endif /* WOT_GTK */
}


WoTFrame::~WoTFrame() {
	if(hosts) { free(hosts);hosts = 0; }
	if(nchars) {
		for(int i=0; i<nchars; i++) Player::deletePlayerData(chars[i].data);
		free(chars);
		chars = 0;
		nchars = 0;
	}
}


void WoTFrame::startClient() {
#ifdef WOT_GTK
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk.hostlist));
	if(gtk_tree_selection_get_selected(selection,&model,&iter)) {
		hostinfo *h;
		gtk_tree_model_get(model,&iter,HOSTLIST_COLS,&h,-1);

		if(app.local_char_id) {
			char cmd[1024];
			GError *err = NULL;

			sprintf(cmd,
#ifdef _WIN32
				"wotclient.exe"
#else
				"wotclient"
#endif
				" %s:4444:%" PRIu32 ":%s",h->ip,app.local_char_id,app.local_char_name);
app.printf("host=%s,cmd=%s",h->name,cmd);

			g_spawn_command_line_async(cmd,&err);
			if(err!=NULL) {
				app.printf("Unable to launch client: %s\n", err->message);
				g_error_free(err);
			}
		} else ShowMessage(gtk.window,app.get("login_err_char"),app.get("login_err_capt"));

	} else ShowMessage(gtk.window,app.get("login_err_wrld"),app.get("login_err_capt"));
#endif /* WOT_GTK */
}


void WoTFrame::sendChatMessage(const char *msg) {
	char text[257];
	strncpy(text,msg,256);
	if(*text=='/' || *text=='-') {
		char *p1 = strchr(text,' '),*p2 = 0;
		if(p1) {
			*p1++ = '\0';while(*p1==' ') p1++;
			p2 = strchr(p1,' ');
			if(p2) { *p2++ = '\0';while(*p2==' ') p2++; }
		}
		uint32_t n;
		if((n=server_commands.getValue(text+1))) {
			switch(n) {
				case SRV_PRINT:
					if(p1 && p2 && (*p1=='p' || *p1=='P') && !strnicmp(p1,"player",6)) {
						Player *pl = (Player *)Map::getItem((uint32_t)atol(p2));
						if(pl && pl->instanceOf(Player::getClass())) {
							aString str;
							pl->getPlayerFile(str);
							app.consolef(TXT_BLUE,"%s",str.toCharArray());
						}
					} else if(p1 && (*p1=='c' || *p1=='C') && !strnicmp(p1,"channels",8)) {
						aConnection c = 0;
						if(p2) c = app.getClient((uint32_t)atol(p2));
						if(c) {
							app.consolef(TXT_BLUE,"Channels [%s]:",c->getNick());
							aVector &channels = c->getChannels();
							for(int i=0; i<(int)channels.size(); i++)
								app.consolef(TXT_BLUE,"* %s [%d]",((aChannel)channels[i])->getName(),((aChannel)channels[i])->size());
						} else {
							app.consolef(TXT_BLUE,"Channels:");
							aHashtable::iterator iter = app.getServer()->getChannels();
							aChannel ch;
							while((ch=(aChannel)iter.next()))
								app.consolef(TXT_BLUE,"* %s [%d]",ch->getName(),ch->size());
						}
					}
					break;
				case SRV_SUICIDE:
					if(p1) {
						Player *pl = (Player *)Map::getItem((uint32_t)atol(p1));
						if(pl) pl->suicide();
					}
					break;
				case SRV_TELEPORT:
					if(p1 && p2) {
						Player *pl = (Player *)Map::getItem((uint32_t)atol(p1));
						if(pl) {
							int32_t x = (int32_t)atol(p2),y;
							if((p2=strchr(p2,','))) {
								y = (int32_t)atol(p2+1);
								pl->teleport(0,x,y);
								app.consolef(TXT_BLUE,"Teleport %s to [%u,%u]",pl->getName(),x,y);
							}
						}
					}
					break;
			}
		}
	} else app.msg((uint32_t)0,FONT_BLUE,text);
}


void WoTFrame::loadHosts() {
app.printf("WoTFrame::loadHosts(1)");
	if(hosts) { free(hosts);hosts = 0; }
	aVector list;
	aHttp http(app);
	const char *file = http.get(WEB_HOST,WEB_PATH "hosts.txt");
	if(file && *file) {
app.printf("WoTFrame::loadHosts(2,file=\"%s\")",http.getFile());
		list.split(http.getFile(),"\n",true);
app.printf("WoTFrame::loadHosts(3)");
		if(list.size()) {
			hosts = (hostinfo *)malloc(sizeof(hostinfo)*list.size());
app.printf("WoTFrame::loadHosts(4)");
			char str[512],*p1,*p2 = 0;
			for(size_t i=0,j; i<list.size(); i++) {
				hostinfo &h = hosts[i];
				strcpy(str,(char *)list[i]);
app.printf("WoTFrame::loadHosts(str=%s)",str);
				for(j=0,p1=str; j<=4; j++,p1=p2) {
					if(j<4) p2 = strchr(p1,';'),*p2++ = '\0';
					switch(j) {
						case 0:strcpy(h.id,p1);break;
						case 1:strcpy(h.name,p1);break;
						case 2:strcpy(h.ip,p1);break;
						case 3:strcpy(h.players,p1);break;
						case 4:strcpy(h.online,p1);break;
					}
				}
			}
		}
	}
app.printf("WoTFrame::loadHosts(5)");
#ifdef WOT_GTK
	GtkListStore *store;
	GtkTreeIter iter;
	store = gtk_list_store_new(HOSTLIST_COLS+1,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_POINTER);
	if(hosts) {
		for(size_t i=0; i<list.size(); i++) {
			gtk_list_store_append(store,&iter);
			gtk_list_store_set(store,&iter,
				HOSTLIST_WORLD,hosts[i].name,
				HOSTLIST_IP,hosts[i].ip,
				HOSTLIST_PLAYERS,hosts[i].players,
				HOSTLIST_ONLINE,hosts[i].online,
				HOSTLIST_COLS,(gpointer)&hosts[i],-1);
		}
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW(gtk.hostlist),GTK_TREE_MODEL(store));
	g_object_unref(store);
#endif /* WOT_GTK */
app.printf("WoTFrame::loadHosts(done)");
}


void WoTFrame::loadChar(const char *c) {
	if(!chars) return;
app.printf("loadChar: %s",c);
	if(c && *c) {
		int n = 0;
		for(; n<nchars; n++) if(!strcmp(chars[n].name,c)) break;
		if(n<nchars) loadChar(n);
	}
}

void WoTFrame::loadChar(int n) {
	if(!chars) return;
	if(n<0) return;
	charinfo &ch = chars[n];
app.printf("loadChar: id=%d",ch.id);
	char str[1024];

	strcpy(app.local_char_name,ch.name);
	app.local_char_id = atoi(ch.id);

	sprintf(str,"%s\n%s: %s %s\n%s: %s\n%s: %s",
		ch.name,
		app.get("char_race"),racedata[ch.data->race].name,
		ch.data->gen? app.get("char_gender_1") : app.get("char_gender_0"),
		app.get("char_level"),ch.data->lvl,
		app.get("char_age"),ch.data->age);
#ifdef WOT_GTK
	gtk_label_set_text(GTK_LABEL(gtk.charlabel),str);
#endif /* WOT_GTK */
/*
	if(id>0) {
		sprintf(str,IMAGES_PATH "avatars/%d.png",id);
		if(fp=fopen(str,"r")) fclose(fp);
		else id = 0;
	}
app.printf("loadChar: charImageID=%d,id=%d",charImageID,id);
	if(charImageID!=id) {
		if(!id) sprintf(str,IMAGES_PATH "avatars/frame.png");
app.printf("loadChar: str=%s",str);
		charImage.release();
		charImage.load(str);
		charImageID = id;
		InvalidateRect(avatarStatic,0,false);
		UpdateWindow(avatarStatic);
	}*/
}

void WoTFrame::loadChars(const char *sel) {
app.printf("WoTFrame::loadChars(1)");
	if(nchars) {
		for(int i=0; i<nchars; i++) {
			Player::deletePlayerData(chars[i].data);
#ifdef WOT_GTK
			gtk_combo_box_remove_text(GTK_COMBO_BOX(gtk.charscombo),0);
#endif /* WOT_GTK */
		}
		free(chars);
		chars = 0;
		nchars = 0;
	}
	aHttp http(app);
	http.setFormValue("u","%s",app.local_user);
	http.setFormValue("p","%s",app.local_password);
	const char *file = http.post(WEB_HOST,WEB_PATH "char.php");
	if(file && *file) {
app.printf("WoTFrame::loadChars(2)");
		aVector list;
		list.split(file,"\n\n\n",true);
app.printf("WoTFrame::loadChars(3)");
		nchars = list.size();
		if(nchars>0) {
app.printf("WoTFrame::loadChars(4)");
			chars = (charinfo *)malloc(sizeof(charinfo)*nchars);
			char str[65536],*id,*name,*data;
app.printf("WoTFrame::loadChars(5)");
			for(size_t i=0,j; i<list.size(); i++) {
				charinfo &ch = chars[i];
				strcpy(str,(char *)list[i]);
				id = str;
				name = strchr(id,'\t'),*name++ = '\0';
				data = strchr(name,'\t'),*data++ = '\0';
				ch.id = atoi(id);
				strcpy(ch.name,name);
				ch.data = Player::parsePlayerFile(data);
app.printf("WoTFrame::loadChars(id=%s,name=%s)",id,name);
app.printf("WoTFrame::loadChars(6)");
				sprintf(str,"%s [%s]",ch.name,ch.lvl);
#ifdef WOT_GTK
				gtk_combo_box_append_text(GTK_COMBO_BOX(gtk.charscombo),str);
#endif /* WOT_GTK */
			}
app.printf("WoTFrame::loadChars(7)");
			nchars = list.size();
#ifdef WOT_GTK
			gtk_combo_box_set_active(GTK_COMBO_BOX(gtk.charscombo),0);
#endif /* WOT_GTK */
			if(sel && *sel) strcpy(str,sel);
			else strcpy(str,chars[0].name);
			loadChar(str);
app.printf("WoTFrame::loadChars(done)");
		}
	}
//app.printf("loadChars: %s",file);
}

void WoTFrame::startUpdatingMap() {
	maparea_id = g_timeout_add(2000,maparea_timeout_callback,this); 
}

void WoTFrame::stopUpdatingMap() {
	if(maparea_id) g_source_remove(maparea_id);
}

void WoTFrame::repaintMap() {
#ifdef WOT_GTK
app.printf("WoTFrame::repaintMap(1)");
	maparea_paint |= 1;
	if(gtk_notebook_get_current_page(GTK_NOTEBOOK(gtk.notebook))!=TAB_SERVER) return;
	GdkRectangle r = { 0,0,gtk.maparea->allocation.width,gtk.maparea->allocation.height };
	gtk_widget_draw(gtk.maparea,&r);
app.printf("WoTFrame::repaintMap(2)");
#endif /* WOT_GTK */
}

void WoTFrame::updateMap() {
#ifdef WOT_GTK
app.printf("WoTFrame::updateMap(1)");
	static bool painting = false;
	int p = maparea_paint;
	if(painting || !maparea_paint) return;
	painting = true;
	maparea_paint = 0;
	ServerWorld &world = *app.world;
	int x,y,w = gtk.maparea->allocation.width,h = gtk.maparea->allocation.height;
	int n,xres = w/world.mw,nres = xres/2,yres = h/world.mh;
	GdkGC *gc = gdk_gc_new(gtk.maparea_buffer);
	GdkColor col;

	if(p&2) {
		Map::mapcolor c;
		col.pixel = 0;
app.printf("WoTFrame::updateMap(2)");
		mapview &= 0xff;
		for(y=0; y<world.mh; y++) {
			for(x=0,n=((y&1)? nres : 0); x<world.mw; x++) {
				c = world.getColor(x,y,mapview);
				col.pixel = 0xff000000|(c.r<<16)|(c.g<<8)|(c.b);
				gdk_gc_set_foreground(gc,&col);
				gdk_draw_rectangle(gtk.maparea_map,gc,TRUE,x*xres+n,y*yres,xres,yres);
				if(n && x==world.mw-1) gdk_draw_rectangle(gtk.maparea_map,gc,TRUE,-n,y*yres,xres,yres);
			}
		}
	}
app.printf("WoTFrame::updateMap(3)");
	gdk_draw_drawable(gtk.maparea_buffer,gc,gtk.maparea_map,0,0,0,0,w,h);
app.printf("WoTFrame::updateMap(4)");

	col.pixel = 0;
	gdk_gc_set_foreground(gc,&col);
	Character *c;
	app.lock();
	for(aHashtable::iterator iter=Map::itemids.iterate(); (c=(Character *)iter.next());)
		if(c->instanceOf(NPC::getClass())) {
			x = c->mx*xres+((c->my&1)? nres : 0),y = c->my*yres;
			if(x>=w) x -= w;
			gdk_draw_rectangle(gtk.maparea_buffer,gc,TRUE,x+1,y+1,1,1);
		}
app.printf("WoTFrame::updateMap(5)");
	col.pixel = 0xff00ffff;
	gdk_gc_set_foreground(gc,&col);
	for(aHashtable::iterator iter=Map::itemids.iterate(); (c=(Character *)iter.next());)
		if(c->instanceOf(Player::getClass())) {
			x = c->mx*xres+((c->my&1)? nres : 0),y = c->my*yres;
			if(x>=w) x -= w;
			gdk_draw_rectangle(gtk.maparea_buffer,gc,TRUE,x,y,3,3);
		}
/*	Cloud *cld;
	for(n=Map::cloudids.size()-1; n>=0; n--) {
		cld = (Cloud *)Map::cloudids[n];
		x = cld->mx*xres+((cld->my&1)? nres : 0),y = cld->my*yres;
		if(x>=w) x -= w;
		gdk_draw_rectangle(maparea_buffer,gc,TRUE,x,y,2,2);
	}*/
app.printf("WoTFrame::updateMap(6)");
	app.unlock();
	if(gtk.font) {
		time_t rawtime;
		struct tm *timeinfo;
		char str[33];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(str,32,"%H:%M:%S",timeinfo);
		gdk_draw_text(gtk.maparea_buffer,gtk.font,gc,5,15,str,strlen(str));
	}
	g_object_unref(gc);
	painting = false;
app.printf("WoTFrame::updateMap(7)");
#endif /* WOT_GTK */
}

void WoTFrame::setMapView(int v) {
app.printf("WoTFrame::setMapView(v=%d)",v);
	if(mapview==v) return;
	mapview = v;
	maparea_paint |= 2;
	repaintMap();
}

void WoTFrame::insertClient(aConnection client) {
#ifdef WOT_GTK
#elif defined WOT_WXWIDGETS
	wxString str;
	str.Printf(wxT("[%d] %s%c"),client->getID(),client->getNick(),client->isActive()? '\0' : '^');
	clientsList->Append(str);
#elif defined WOT_FLTK13
#elif defined WOT_FLTK2
#endif /* WOT_GTK && WOT_WXWIDGETS && WOT_FLTK13 && WOT_FLTK2 */
}

void WoTFrame::removeClient(aConnection client) {
#ifdef WOT_GTK
#elif defined WOT_WXWIDGETS
	wxString str;
	str.Printf(wxT("[%d] %s%c"),client->getID(),client->getNick(),client->isActive()? '\0' : '^');
	int index = clientsList->FindString(str);
	if(index!=wxNOT_FOUND) clientsList->Delete(index);
#elif defined WOT_FLTK13
#elif defined WOT_FLTK2
#endif /* WOT_GTK && WOT_WXWIDGETS && WOT_FLTK13 && WOT_FLTK2 */
}

void WoTFrame::console(int col,const char *msg) {
#ifdef WOT_GTK
	if(gtk.chattextbuffer)
		gtk_text_buffer_insert_with_tags_by_name(gtk.chattextbuffer,&gtk.chattextiter,msg,-1,coltags[col],NULL);
#endif /* WOT_GTK */
}



