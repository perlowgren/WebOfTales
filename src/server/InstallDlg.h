#ifndef _SERVER_INSTALLDLG_H
#define _SERVER_INSTALLDLG_H


#include <stdio.h>
#include <libamanita/aThread.h>

#ifdef WOT_GTK
#include <gtk/gtk.h>
#endif /* WOT_GTK */

class aApplication;
class aVector;


#ifdef WOT_GTK
class InstallDlg {
private:
	struct {
		GtkWidget *window;
		GtkWidget *progress;
	} gtk;

	aApplication *app;
	aVector *files;
	aThread thread;
	double progress;
	int status;

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static gboolean show_callback(GtkWidget *widget,GdkEvent *event,gpointer data);
	static gboolean progress_timeout_callback(gpointer data);
	static void install(void *data);

public:
	InstallDlg(GtkWindow *parent,const char *caption,aApplication &app,aVector &files);
	~InstallDlg();

	static void update(void *obj,const char *file,int n,int max,int st);
};
#endif /* WOT_GTK */


#endif /* _SERVER_INSTALLDLG_H */


