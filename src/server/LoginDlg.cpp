
#include "../config.h"
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "LoginDlg.h"
#include "CreateAccountDlg.h"



#ifdef WOT_GTK
gboolean LoginDlg::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	LoginDlg *dlg = (LoginDlg *)data;
	delete dlg;
	app.exit();
	gtk_main_quit();
	return FALSE;
}

void LoginDlg::createaccount_clicked_callback(GtkWidget *widget,gpointer data) {
	LoginDlg *dlg = (LoginDlg *)data;
	new CreateAccountDlg(GTK_WINDOW(dlg->gtk.window),app.get("cad_caption"));
}

void LoginDlg::login_clicked_callback(GtkWidget *widget,gpointer data) {
app.printf("LoginDlg::loginbutton_clicked(1)");
	LoginDlg *dlg = (LoginDlg *)data;
	strncpy(app.local_user,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.user)),32);
	strncpy(app.local_password,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.password)),32);
app.printf("login.. 1");
	if(!*app.local_user || !*app.local_password) {
		ShowMessage(dlg->gtk.window,app.get("login_err_uspw"),app.get("login_err_capt"));
		return;
	}
app.printf("login.. 2");
	aHttp http(app);
	http.setFormValue("u","%s",app.local_user);
	http.setFormValue("p","%s",app.local_password);
	const char *file = http.post(WEB_HOST,WEB_PATH "login.php");
	if(!file || !*file) {
		ShowMessage(dlg->gtk.window,app.get("login_err_csrv"),app.get("login_err_capt"));
	} else if(!strncmp(file,"[ERROR]",7)) {
		ShowMessage(dlg->gtk.window,app.get(&file[7]),app.get("login_err_capt"));
	} else {
		/*FILE *fp = fopen(DAT_PATH "properties.txt","w");
		fprintf(fp,"%s",file);
		fclose(fp);*/
		app.setProperties(file);
		/*app.properties.removeAll();
		app.properties.load(DAT_PATH "properties.txt");*/
		app.setLocalID(atoi(app.getProperty("id")));
		app.mainFrame->loadHosts();
		app.mainFrame->loadChars();
		delete dlg;
	}
app.printf("login: %s",file);
}


LoginDlg::LoginDlg(GtkWindow *parent,const char *caption) {
	char s[256];
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *label;

	gtk.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gtk.window),caption);
	gtk_window_set_transient_for(GTK_WINDOW(gtk.window),parent);
	gtk_window_set_modal(GTK_WINDOW(gtk.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(gtk.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(gtk.window),GTK_WIN_POS_CENTER);
	sprintf(s,"%simages/icons/icon16.png",app.getHomeDir());
	gtk_window_set_icon(GTK_WINDOW(gtk.window),app.createPixbuf(s));
	gtk_container_set_border_width(GTK_CONTAINER(gtk.window),2);

	table = gtk_table_new(2,5,FALSE);

	label = gtk_label_new(app.get("login_user"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,2,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.user = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(gtk.user),app.local_user);
	g_signal_connect(G_OBJECT(gtk.user),"activate",G_CALLBACK(login_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),gtk.user,0,2,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("login_password"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,2,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.password = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(gtk.password),app.local_password);
	gtk_table_attach(GTK_TABLE(table),gtk.password,0,2,3,4,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	hbox = gtk_hbox_new(FALSE,5);
	gtk.createaccount = gtk_button_new_with_label(app.get("login_create_account"));
	g_signal_connect(G_OBJECT(gtk.createaccount),"clicked",G_CALLBACK(createaccount_clicked_callback),this);
	gtk_widget_set_size_request(gtk.createaccount,100,-1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk.createaccount,FALSE,FALSE,0);
	gtk.login = gtk_button_new_with_label(app.get("login"));
	g_signal_connect(G_OBJECT(gtk.login),"clicked",G_CALLBACK(login_clicked_callback),this);
	gtk_widget_set_size_request(gtk.login,100,-1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk.login,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,1,2,4,5,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(gtk.window),table);
	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
}

LoginDlg::~LoginDlg() {
	gtk_widget_destroy(gtk.window);
}
#endif /* WOT_GTK */



