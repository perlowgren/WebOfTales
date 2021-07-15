#ifndef _SERVER_CREATEACCDLG_H
#define _SERVER_CREATEACCDLG_H


#ifdef WOT_GTK
#include <gtk/gtk.h>
#endif /* WOT_GTK */


#ifdef WOT_GTK
class CreateAccountDlg {
private:
	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void ok_clicked_callback(GtkWidget *widget,gpointer data);
	static void cancel_clicked_callback(GtkWidget *widget,gpointer data);

public:
	struct {
		GtkWidget *window;
		GtkWidget *user;
		GtkWidget *password;
		GtkWidget *repeat;
		GtkWidget *email;
		GtkWidget *name;
		GtkWidget *lang;
		GtkWidget *country;
		GtkWidget *ok;
		GtkWidget *cancel;
	} gtk;

	CreateAccountDlg(GtkWindow *parent,const char *caption);
	~CreateAccountDlg();
};
#endif /* WOT_GTK */


#endif /* _SERVER_CREATEACCDLG_H */


