#ifndef _SERVER_LOGINDLG_H
#define _SERVER_LOGINDLG_H


#include <stdio.h>

#ifdef WOT_GTK
#include <gtk/gtk.h>
#endif /* WOT_GTK */



#ifdef WOT_GTK
class LoginDlg {
private:
	struct {
		GtkWidget *window;
		GtkWidget *user;
		GtkWidget *password;
		GtkWidget *createaccount;
		GtkWidget *login;
	} gtk;

	static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data);
	static void createaccount_clicked_callback(GtkWidget *widget,gpointer data);
	static void login_clicked_callback(GtkWidget *widget,gpointer data);

public:
	LoginDlg(GtkWindow *parent,const char *caption);
	~LoginDlg();
};
#endif /* WOT_GTK */


#endif /* _SERVER_LOGINDLG_H */


