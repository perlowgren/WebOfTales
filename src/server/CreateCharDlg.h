#ifndef _SERVER_CREATECHRDLG_H
#define _SERVER_CREATECHRDLG_H


#include <stdio.h>

#ifdef WOT_GTK
#include <gtk/gtk.h>
#endif /* WOT_GTK */

#include <libamanita/aWord.h>


#ifdef WOT_GTK
class CreateCharDlg {
private:
	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void random_clicked_callback(GtkWidget *widget,gpointer data);
	static void ok_clicked_callback(GtkWidget *widget,gpointer data);
	static void cancel_clicked_callback(GtkWidget *widget,gpointer data);

public:
	struct {
		GtkWidget *window;
		GtkWidget *name;
		GtkWidget *random;
		GtkWidget *ok;
		GtkWidget *cancel;
	} gtk;

	aWord word;

	CreateCharDlg(GtkWindow *parent,const char *caption);
	~CreateCharDlg();
};
#endif /* WOT_GTK */


#endif /* _SERVER_CREATECHRDLG_H */


