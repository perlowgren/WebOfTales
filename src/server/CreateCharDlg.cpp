
#include "../config.h"

#include <libamanita/aVector.h>
#include <libamanita/net/aHttp.h>
#include "../Main.h"
#include "WebOfTales.h"
#include "WoTFrame.h"
#include "CreateCharDlg.h"
#include "../game/Race.h"


#ifdef WOT_GTK
gboolean CreateCharDlg::delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	CreateCharDlg *dlg = (CreateCharDlg *)data;
	delete dlg;
	return FALSE;
}

void CreateCharDlg::random_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateCharDlg *dlg = (CreateCharDlg *)data;
	gtk_entry_set_text(GTK_ENTRY(dlg->gtk.name),dlg->word.generate(4,12));
}

void CreateCharDlg::ok_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateCharDlg *dlg = (CreateCharDlg *)data;
	char str[256];
	strncpy(str,gtk_entry_get_text(GTK_ENTRY(dlg->gtk.name)),32);
	if(!*str) {
		ShowMessage(dlg->gtk.window,app.get("char_err_nm_empty"),app.get("char_caption"));
	} else {
		aHttp http(app);
		http.setFormValue("c","create");
		http.setFormValue("u","%s",app.local_user);
		http.setFormValue("p","%s",app.local_password);
		http.setFormValue("nm","%s",str);
		http.setFormValue("data","");
		const char *file = http.post(WEB_HOST,WEB_PATH "char.php");
		if(!file || !*file) {
			ShowMessage(dlg->gtk.window,app.get("login_err_csrv"),app.get("reg_err_capt"));
		} else if(!strncmp(file,"[ERROR]",7)) {
			ShowMessage(dlg->gtk.window,app.get(&file[7]),app.get("reg_err_capt"));
		} else {
			ShowMessage(dlg->gtk.window,app.get("char_complete"),app.get("reg_complete_capt"));
			app.mainFrame->loadChars(str);
			delete dlg;
		}
	}
}

void CreateCharDlg::cancel_clicked_callback(GtkWidget *widget,gpointer data) {
	CreateCharDlg *dlg = (CreateCharDlg *)data;
	delete dlg;
}

CreateCharDlg::CreateCharDlg(GtkWindow *parent,const char *caption) {
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

	table = gtk_table_new(1,4,FALSE);

	label = gtk_label_new(app.get("cad_name"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	gtk.name = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),gtk.name,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	gtk.random = gtk_button_new_with_label(app.get("char_rand_name"));
	g_signal_connect(G_OBJECT(gtk.random),"clicked",G_CALLBACK(random_clicked_callback),this);
	gtk_table_attach(GTK_TABLE(table),gtk.random,0,1,2,3,
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
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,3,4,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(gtk.window),table);
	gtk_widget_show_all(gtk.window);
	g_signal_connect(G_OBJECT(gtk.window),"delete_event",G_CALLBACK(delete_event_callback),this);
}

CreateCharDlg::~CreateCharDlg() {
	gtk_widget_destroy(gtk.window);
}
#endif /* WOT_GTK */


