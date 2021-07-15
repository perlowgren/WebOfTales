
#include "../config.h"
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "InstallDlg.h"



#ifdef WOT_GTK
gboolean InstallDlg::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	gtk_main_quit();
	return TRUE;
}
gboolean InstallDlg::show_callback(GtkWidget *widget,GdkEvent *event,gpointer data) {
	InstallDlg *dlg = (InstallDlg *)data;
dlg->app->printf("InstallDlg::show_callback()");
	g_timeout_add(200,progress_timeout_callback,dlg);
	dlg->thread.start(install,dlg);
	return FALSE;
}


gboolean InstallDlg::progress_timeout_callback(gpointer data) {
	InstallDlg *dlg = (InstallDlg *)data;
dlg->app->printf("InstallDlg::progress_timeout_callback(progress=%p,d1=%f)",dlg->gtk.progress,dlg->progress);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(dlg->gtk.progress),(gdouble)dlg->progress);
	if(dlg->progress==1.0) {
#if defined __linux__
		if(dlg->status&1) dlg->app->updateFontCache();
#endif
		//dlg->app->init();
		delete dlg;
		g_spawn_command_line_async(::app.getExecutable(),NULL);
		gtk_main_quit();
		return FALSE;
	}
	return TRUE;
}

void InstallDlg::install(void *data) {
	InstallDlg *dlg = (InstallDlg *)data;
dlg->app->printf("InstallDlg::install_timeout_callback()");
	dlg->app->install(WEB_HOST,WEB_PATH "files/",*dlg->files,dlg->update,dlg);
}


InstallDlg::InstallDlg(GtkWindow *parent,const char *caption,aApplication &app,aVector &files) {
app.printf("InstallDlg::InstallDlg(\"%s\")",caption);
	gtk.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gtk.window),caption);
	gtk_window_set_modal(GTK_WINDOW(gtk.window),TRUE);
//	gtk_widget_set_size_request(gtk.window,250,80);
	gtk_window_set_resizable(GTK_WINDOW(gtk.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gtk.window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(gtk.window),5);

	gtk.progress = gtk_progress_bar_new();
	gtk_widget_set_size_request(gtk.progress,250,-1);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gtk.progress),(gdouble)1.0);
	gtk_container_add(GTK_CONTAINER(gtk.window),gtk.progress);
app.printf("InstallDlg::InstallDlg(progress=%p)",gtk.progress);

	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
	g_signal_connect(G_OBJECT(gtk.window),"map-event",G_CALLBACK(show_callback),this);

	this->app = &app;
	this->files = &files;
	progress = 0.0;
}

InstallDlg::~InstallDlg() {
	gtk_widget_destroy(gtk.window);
	delete files;
}

void InstallDlg::update(void *obj,const char *file,int n,int max,int st) {
	InstallDlg *dlg = (InstallDlg *)obj;
dlg->app->printf("InstallDlg::updateProgress(%d/%d)",n,max);
	dlg->progress = max>0? (double)n/(double)max : 1.0;
	dlg->status = st;
}


#endif /* WOT_GTK */



