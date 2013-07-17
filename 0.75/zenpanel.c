/*
 *  zenpanel.c
 *  Authors : 
 * - Vincent Kergonna <vincent.k@orange.fr> 
 * - Jean-Philippe Guillemin <jp.guillemin@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */



#include "zenpanel.h"

/* usage */
int usage(void) {
	printf(_("\nzenpanel v%s : Zenwalk System Tools Manager\n"), VERSION);
	printf(_("Usage : zenpanel [options] ...\n"));
	printf(_("[-b banner] : path to the banner image\n"));
	printf(_("[-t title] : Window title\n"));
	printf(_("[-i icon] : icon name in theme\n"));
	printf(_("[-c Category] : Freedesktop Category that must match\n"));
	printf(_("[-d directory] : where to scan for desktop files\n"));
	printf(_("[-W] : window Width\n"));
	printf(_("[-H] : window Height\n"));
	return(EXIT_SUCCESS);
}

const gchar *desktop_dir = {_DEFAULT_DESKTOPPATH};
const gchar *category = {_DEFAULT_CATEGORY};
const gchar *banner = {_DEFAULT_BANNER};
const gchar *windowtitle = {_DEFAULT_TITLE};
const gchar *windowicon = {_DEFAULT_ICON}; 

gint width = 550;
gint height = 250;


enum {
	ICON_COLUMN,
	TEXT_COLUMN,
	DATA_COLUMN
};


static void destroy (GtkWidget *widget, gpointer data){
	gtk_main_quit();
}

void child_exited (pid_t pid)
{
	pid = waitpid(-1, NULL, WNOHANG);
}

static void icon_activated (GtkIconView *view, GtkTreePath *tree, gpointer data){
	pid_t pid;
	GtkListStore *store;
	GtkTreeIter iter;
	gchar *cmd = NULL;
	gchar *argv[64];
	store = GTK_LIST_STORE (data);

	gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, tree);
	gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
                     DATA_COLUMN, &cmd, -1);
	
	z_parse_cmd(cmd, argv); 
                     
	dbg("cmd=%s\n",cmd);
	pid = fork();
	if (pid == 0) {
		if (execvp(*argv, argv)) {
			exit(EXIT_FAILURE);
		}
	} else if (pid < 0) {
		dbg("Unable to fork");   
	} else if (pid > 0) {
		return;
	}
	return;
}


static void add_item_to_store (GtkListStore *store,
								gchar *icon_name,
								gchar *text,
								gchar *cmd){
					
	GtkTreeIter iter;
	GtkIconTheme *icon_theme;
	GdkPixbuf *pixbuf = NULL;
	
	if (!icon_name || !text || !cmd) {
		return;
	}
	
	icon_theme = gtk_icon_theme_get_default();	
	gtk_list_store_append (store, &iter);
	
	pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                   icon_name, /* icon name */
                                   32, /* size */
                                   0,  /* flags */
                                   NULL);
	
	if (pixbuf) {
		gtk_list_store_set (store, &iter, ICON_COLUMN, pixbuf, 
			TEXT_COLUMN, text, DATA_COLUMN, cmd, -1);
		g_object_unref (pixbuf);
	} else {
		dbg("cannot create pixbuf from icon %s\n", icon_name);
	}
}

/*
 * This function reads .desktop files from a given directory and create the
 * icons in the panel.
 * 
 */
void
display_icons_in_panel (GtkListStore *store, const char * dir){

	Z_desktop_file df;
	
	const gchar *locale = setlocale(LC_CTYPE, "");
	gchar name_locale[128];
	g_snprintf(name_locale, sizeof(name_locale), "%s%s", "Name[", g_strndup (locale, 2));
	
	GDir *d = g_dir_open(dir, 0, 0);
    const gchar *file;
    FILE *fd;
	if (d) {
		while((file = g_dir_read_name(d))) {	
			g_snprintf(df.filename, sizeof(df.filename), "%s/%s", dir, file);
			
			if (g_strrstr_len (df.filename, strlen(df.filename), ".desktop")) {
				fd = z_open_file (df.filename);
				if ( !fd ) continue;
				g_snprintf(df.categories, sizeof(df.categories), "%s", z_read_parameter ("Categories", fd));
				g_strstrip (df.categories);
				if (strlen(df.categories) > 0 ) {
					if ( g_strrstr_len(df.categories, strlen(df.categories), category) ) {
						
						g_snprintf(df.icon, sizeof(df.icon), "%s", z_read_parameter ("Icon", fd));
						g_snprintf(df.name, sizeof(df.name), "%s", z_read_parameter (name_locale, fd));
						if (g_strrstr_len (df.name, strlen(df.name), "not found")) {g_snprintf(df.name, sizeof(df.name), "%s", z_read_parameter ("Name", fd));}

						g_snprintf(df.cmd, sizeof(df.cmd), "%s", z_read_parameter ("Exec", fd));
						g_strstrip (df.icon);
						g_strstrip (df.name);
						g_strstrip (df.cmd);
						add_item_to_store (store, df.icon, df.name, df.cmd);
					}
				}
				z_close_file (fd);
			}
		}
		g_dir_close(d);
	}
}


void create_main_window (void){	
	GtkWidget *window = NULL;
	GtkWidget *button = NULL;
	GtkWidget *icon_view = NULL;
	GtkListStore *store = NULL;
	GtkWidget *bannerwidget = NULL;
	GdkPixbuf *pxbuf;
	window = gtk_dialog_new();
	gtk_window_set_title (GTK_WINDOW (window), windowtitle);

	
	gtk_window_resize (GTK_WINDOW (window), width, height);
	gtk_window_set_icon_name (GTK_WINDOW (window), windowicon);
	
	
	/* The banner */
	pxbuf = gdk_pixbuf_new_from_file_at_scale(banner, width + 25, 35, FALSE, NULL);
	bannerwidget = gtk_image_new_from_pixbuf (pxbuf);
	
	g_signal_connect(G_OBJECT(window),
			 "delete_event", G_CALLBACK(destroy), NULL);

	g_signal_connect(G_OBJECT(window),
			 "destroy", G_CALLBACK(destroy), NULL);

	/* We create and define the icon view */
	icon_view = gtk_icon_view_new ();
	gtk_icon_view_set_item_width (GTK_ICON_VIEW (icon_view), 120);
	gtk_icon_view_set_row_spacing (GTK_ICON_VIEW (icon_view), 12);
	gtk_icon_view_set_column_spacing (GTK_ICON_VIEW (icon_view), 15);
	
	store = gtk_list_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
	
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), ICON_COLUMN);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view), TEXT_COLUMN);
	gtk_icon_view_set_model (GTK_ICON_VIEW (icon_view), GTK_TREE_MODEL (store));
	
	g_signal_connect (G_OBJECT (icon_view), "item-activated", 
					G_CALLBACK(icon_activated), store);

	display_icons_in_panel (store, desktop_dir);
	
	button = gtk_button_new_from_stock("gtk-close");
	gtk_signal_connect (GTK_OBJECT (button), "clicked", G_CALLBACK (destroy), NULL);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (window)->action_area), button);
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), bannerwidget, FALSE, FALSE, 1);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), icon_view, TRUE, TRUE, 5);
	gtk_widget_show_all (window);
}


int main (int argc, char **argv){
	
	setlocale(LC_ALL, "");

	if(textdomain( "zenpanel" ) == NULL) {
		return EXIT_FAILURE;
	}

	if( bindtextdomain( "zenpanel", _LOCALEPATH ) == NULL) {
		return EXIT_FAILURE;
	}     
	textdomain ("zenpanel");

	/* I know it's archaic, as I am :)  */
	int i;	
	while ((i = getopt(argc, argv, "d:W:H:c:b:t:i:h")) != EOF){
		switch (i){
			case 'd':
				desktop_dir = optarg;
				break;
			case 'c':
				category = optarg;
				break;
			case 'b':
				banner = optarg;
				break;
			case 't':
				windowtitle = optarg;
				break;
			case 'i':
				windowicon = optarg;
				break;				
			case 'W':
				width = atoi(optarg);
				break;
			case 'H':
				height = atoi(optarg);
				break;
			case 'h':
				usage(); exit(EXIT_FAILURE);
		}
	}

	gtk_set_locale ();

	signal (SIGCHLD, child_exited);
		
	gtk_init (&argc, &argv);

	create_main_window ();

	gtk_main();
	
	exit (EXIT_SUCCESS);
}


