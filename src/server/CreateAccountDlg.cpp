
#include "../config.h"
#include <string.h>

#include <libamanita/aString.h>
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "WebOfTales.h"
#include "CreateAccountDlg.h"




#ifdef WOT_GTK
gboolean CreateAccountDlg::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	CreateAccountDlg *dlg = (CreateAccountDlg *)data;
	delete dlg;
	return FALSE;
}

void CreateAccountDlg::ok_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateAccountDlg *dlg = (CreateAccountDlg *)data;
	char str[256];
	strncpy(app.local_user,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.user)),32);
	strncpy(app.local_password,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.password)),32);
	strncpy(str,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.repeat)),32);
	if(strcmp(app.local_password,str)!=0)
		ShowMessage(dlg->gtk.window,app.get("reg_pwd_mismatch"),app.get("reg_err_capt"));
	else {
		strncpy(str,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.email)),128);
		aHttp http(app);
		http.setFormValue("c","create");
		http.setFormValue("u","%s",app.local_user);
		http.setFormValue("p","%s",app.local_password);
		http.setFormValue("e","%s",str);
		const char *file = http.post(WEB_HOST,WEB_PATH "createaccount.php");
		if(!file || !*file) {
			ShowMessage(dlg->gtk.window,app.get("login_err_csrv"),app.get("reg_err_capt"));
		} else if(!strncmp(file,"[ERROR]",7)) {
			ShowMessage(dlg->gtk.window,app.get(&file[7]),app.get("reg_err_capt"));
		} else {
			ShowMessage(dlg->gtk.window,app.get("reg_complete"),app.get("reg_complete_capt"));
			delete dlg;
		}
	}
}

void CreateAccountDlg::cancel_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateAccountDlg *dlg = (CreateAccountDlg *)data;
	delete dlg;
}


CreateAccountDlg::CreateAccountDlg(GtkWindow *parent,const char *caption) {
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

	table = gtk_table_new(2,4,FALSE);

	label = gtk_label_new(app.get("login_user"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.user = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.user,1,2,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("login_password"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.password = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.password,1,2,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cad_repeat"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.repeat = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.repeat,1,2,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new(app.get("cad_email"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,3,4,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.email = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.email,1,2,3,4,
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
	gtk_table_attach(GTK_TABLE(table),hbox,0,2,4,5,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(gtk.window),table);
	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
}

CreateAccountDlg::~CreateAccountDlg() {
	gtk_widget_destroy(gtk.window);
}
#endif /* WOT_GTK */


