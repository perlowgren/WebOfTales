
#include "../config.h"
#include <stdio.h>
#include <string.h>
#include <libamanita/aWord.h>
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "CreateWorldDlg.h"
#include "ServerWorld.h"



#ifdef WOT_GTK
gboolean CreateWorldDlg::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	CreateWorldDlg *dlg = (CreateWorldDlg *)data;
	delete dlg;
	return FALSE;
}

void CreateWorldDlg::random_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateWorldDlg *dlg = (CreateWorldDlg *)data;
	gtk_entry_set_text(GTK_ENTRY(dlg->gtk.name),dlg->word.generate(4,12));
}

void CreateWorldDlg::ok_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateWorldDlg *dlg = (CreateWorldDlg *)data;

	strncpy(dlg->name,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.name)),32);
	if(!*dlg->name) {
		ShowMessage(dlg->gtk.window,app.get("cwd_err_nm_empty"),app.get("cwd_caption"));
	} else {
		aHttp http(app);
		http.setFormValue("u","%s",app.local_user);
		http.setFormValue("p","%s",app.local_password);
		http.setFormValue("w","%s",dlg->name);
		const char *file = http.post(WEB_HOST,WEB_PATH "createworld.php");
		if(!file || !*file) {
			ShowMessage(dlg->gtk.window,app.get("login_err_csrv"),app.get("reg_err_capt"));
		} else if(!strncmp(file,"[ERROR]",7)) {
			ShowMessage(dlg->gtk.window,app.get(&file[7]),app.get("reg_err_capt"));
		} else {
			gtk_widget_set_sensitive(dlg->gtk.ok,false);

			dlg->continents = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.continents));
			dlg->landmass = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.landmass));
			dlg->mountains = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.mountains));
			dlg->islands = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.islands));
			dlg->forest = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.forest));
			dlg->cities = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.cities));
			dlg->technology = (int)gtk_range_get_value(GTK_RANGE(dlg->gtk.technology));

			g_timeout_add(200,progress_timeout_callback,dlg);
			dlg->thread.start(_create,dlg);
		}
	}
}

void CreateWorldDlg::cancel_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateWorldDlg *dlg = (CreateWorldDlg *)data;
	delete dlg;
}

gboolean CreateWorldDlg::progress_timeout_callback(gpointer data) {
	CreateWorldDlg *dlg = (CreateWorldDlg *)data;
app.printf("InstallDlg::progress_timeout_callback(progress=%p,d1=%f)",dlg->gtk.progress,dlg->progress);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(dlg->gtk.progress),(gdouble)dlg->progress);
	if(dlg->progress==1.0) {
		aHttp http(app);
		http.setFormValue("c","create");
		http.setFormValue("u","%s",app.local_user);
		http.setFormValue("p","%s",app.local_password);
		http.setFormValue("w","%s",dlg->name);
		const char *file = http.post(WEB_HOST,WEB_PATH "createworld.php");
		if(!file || !*file) {
			ShowMessage(dlg->gtk.window,app.get("login_err_csrv"),app.get("reg_err_capt"));
		} else if(!strncmp(file,"[ERROR]",7)) {
			ShowMessage(dlg->gtk.window,app.get(&file[7]),app.get("reg_err_capt"));
		} else {
			app.world->setID((uint32_t)atol(&file[4]));
			app.world->writeMap();
			app.world->saveWorld();
			app.consolef(TXT_GREEN,"\n%s\n*%s %d%%\n*%s %d%%\n*%s %d%%\n",
				app.get("cwd_result"),
				app.get("cwd_landmass"),(int)(app.world->getResult().landmass*100.0f),
				app.get("cwd_mountains"),(int)(app.world->getResult().mountains*100.0f),
				app.get("cwd_forest"),(int)(app.world->getResult().forest*100.0f)
			);
			char wid[32];
			sprintf(wid,"%d",app.world->getID());
			app.setProperty("wid",wid);
			app.setProperty("world",app.world->getName());
			app.startServer();

			ShowMessage(dlg->gtk.window,app.get("cwd_world_complete"),app.get("cwd_world_complete_capt"));
			gtk_widget_set_sensitive(dlg->gtk.ok,false);
			delete dlg;
		}
		return FALSE;
	}
	return TRUE;
}

CreateWorldDlg::CreateWorldDlg(GtkWindow *parent,const char *caption) {
	char s[256];
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *label;

	continents = 5;
	landmass = 40;
	mountains = 30;
	islands = 50;
	forest = 50;
	cities = 50;
	technology = 50;
	progress = 0.0;

	gtk.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gtk.window),caption);
	gtk_window_set_transient_for(GTK_WINDOW(gtk.window),parent);
	gtk_window_set_modal(GTK_WINDOW(gtk.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(gtk.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gtk.window),GTK_WIN_POS_CENTER);
	sprintf(s,"%simages/icons/icon16.png",app.getHomeDir());
	gtk_window_set_icon(GTK_WINDOW(gtk.window),app.createPixbuf(s));
	gtk_container_set_border_width(GTK_CONTAINER(gtk.window),2);

	table = gtk_table_new(1,4,FALSE);

	label = gtk_label_new(app.get("cwd_world_name"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.name = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.name,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	gtk.random = gtk_button_new_with_label(app.get("cwd_rand_name"));
	g_signal_connect(G_OBJECT(gtk.random),"clicked",G_CALLBACK(random_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),gtk.random,0,1,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_continents"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,3,4,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.continents = gtk_hscale_new_with_range(1.0,10.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.continents),continents);
	gtk_table_attach(GTK_TABLE(table),gtk.continents,0,1,4,5,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_landmass"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,5,6,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.landmass = gtk_hscale_new_with_range(10.0,90.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.landmass),landmass);
	gtk_table_attach(GTK_TABLE(table),gtk.landmass,0,1,6,7,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_mountains"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,7,8,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.mountains = gtk_hscale_new_with_range(0.0,60.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.mountains),mountains);
	gtk_table_attach(GTK_TABLE(table),gtk.mountains,0,1,8,9,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_islands"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,9,10,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.islands = gtk_hscale_new_with_range(20.0,100.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.islands),islands);
	gtk_table_attach(GTK_TABLE(table),gtk.islands,0,1,10,11,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_forest"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,11,12,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.forest = gtk_hscale_new_with_range(0.0,100.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.forest),forest);
	gtk_table_attach(GTK_TABLE(table),gtk.forest,0,1,12,13,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_cities"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,13,14,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.cities = gtk_hscale_new_with_range(20.0,100.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.cities),cities);
	gtk_table_attach(GTK_TABLE(table),gtk.cities,0,1,14,15,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cwd_technology"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,15,16,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.technology = gtk_hscale_new_with_range(10.0,100.0,1.0);
	gtk_range_set_value(GTK_RANGE(gtk.technology),technology);
	gtk_table_attach(GTK_TABLE(table),gtk.technology,0,1,16,17,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	gtk.progress = gtk_progress_bar_new();
	gtk_table_attach(GTK_TABLE(table),gtk.progress,0,1,17,18,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	hbox = gtk_hbox_new(FALSE,5);
	gtk.ok = gtk_button_new_with_label(app.get("ok"));
	g_signal_connect(G_OBJECT(gtk.ok),"clicked",G_CALLBACK(ok_clicked_callback),this);
	gtk_widget_set_size_request(gtk.ok,100,-1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk.ok,FALSE,FALSE,0);
	gtk.cancel = gtk_button_new_with_label(app.get("cancel"));
	g_signal_connect(G_OBJECT(gtk.cancel),"clicked",G_CALLBACK(cancel_clicked_callback),this);
	gtk_widget_set_size_request(gtk.cancel,100,-1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk.cancel,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,18,19,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(gtk.window),table);
	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
}

CreateWorldDlg::~CreateWorldDlg() {
	thread.kill();
	gtk_widget_destroy(gtk.window);
}
#endif /* WOT_GTK */


void CreateWorldDlg::create() {
	app.stopServer();
	app.consolef(TXT_BLUE,app.get("cwd_creating_world"));
app.printf("CreateWorldDlg::create(1,worldname=%s,continents=%d,landmass=%d,mountains=%d,islands=%d,forest=%d,cities=%d,technology=%d)",
			name,continents,landmass,mountains,islands,forest,cities,technology);
	app.world->createWorld(name,continents,landmass,mountains,islands,forest,cities,technology);
app.printf("CreateWorldDlg::create(2)");
	int t,e;
	for(t=0,e=-1; !app.world->isCompleted(); t++) {
		app.world->create(t);
		if(e!=app.world->getEra()) {
app.printf("CreateWorldDlg::create(3,t=%d)",t);
			// app.mainFrame->updateMap(ROTATION);
			e = app.world->getEra();
			progress = (double)e/6.0;
app.printf("CreateWorldDlg::create(e=%d,era=%s)",e,app.world->getEraDescription(e));
			app.consolef(TXT_BLUE,app.world->getEraDescription(e));
		}// else if((app.world->getTime()%10)==0) app.mainFrame->updateMap(ROTATION);
		thread.pause(10);
	}
	progress = 1.0;
app.printf("CreateWorldDlg::create(4,t=%d)",t);
}



