#ifndef _SERVER_WORLDDLG_H
#define _SERVER_WORLDDLG_H

#ifdef WOT_GTK
#include <gtk/gtk.h>
#endif /* WOT_GTK */

#include <libamanita/aThread.h>
#include <libamanita/aWord.h>


#ifdef WOT_GTK
class CreateWorldDlg {
private:
	aWord word;
	aThread thread;			// aThread for the Create World function

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void random_clicked_callback(GtkWidget *widget,gpointer data);
	static void ok_clicked_callback(GtkWidget *widget,gpointer data);
	static void cancel_clicked_callback(GtkWidget *widget,gpointer data);
	static gboolean progress_timeout_callback(gpointer data);

	static void _create(void *data) { ((CreateWorldDlg *)data)->create(); }
	void create();

public:
	struct {
		GtkWidget *window;
		GtkWidget *name;
		GtkWidget *random;
		GtkWidget *continents;
		GtkWidget *landmass;
		GtkWidget *mountains;
		GtkWidget *islands;
		GtkWidget *forest;
		GtkWidget *cities;
		GtkWidget *technology;
		GtkWidget *progress;
		GtkWidget *ok;
		GtkWidget *cancel;
	} gtk;

	char name[33];
	int continents;
	int landmass;
	int mountains;
	int islands;
	int forest;
	int cities;
	int technology;
	double progress;

	CreateWorldDlg(GtkWindow *parent,const char *caption);
	~CreateWorldDlg();
};
#endif /* WOT_GTK */

#endif /* _SERVER_WORLDDLG_H */


