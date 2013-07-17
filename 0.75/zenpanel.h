/*
 *  zenpanel.h
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include <glib.h> 
#include <libintl.h>
#define _(String) gettext (String)

#ifdef DEBUG
#define dbg(fmt,args...) fprintf(stderr, fmt , ## args)
#else
#define dbg(fmt,args...) do { } while (0)
#endif

#define VERSION "0.75"

#ifndef _DEFAULT_DESKTOPPATH
#define _DEFAULT_DESKTOPPATH "/usr/share/applications"
#endif

#ifndef _DEFAULT_BANNER
#define _DEFAULT_BANNER "/usr/share/systemtools/banner.png"
#endif

#ifndef _DEFAULT_CATEGORY
#define _DEFAULT_CATEGORY "X-Zenpanel"
#endif

#ifndef _DEFAULT_TITLE
#define _DEFAULT_TITLE "Control Panel"
#endif

#ifndef _DEFAULT_ICON
#define _DEFAULT_ICON "zenpanel"
#endif

FILE * z_open_file (const char * filename);
char *z_read_parameter (const char *pname, FILE *fp);
void z_close_file (FILE *fp);
void  z_parse_cmd(char *line, char **argv);

typedef struct  {
	int toto;
	gchar filename[512];
	gchar icon[256];
	gchar name[256];
	gchar cmd[512];
	gchar categories[256];
}Z_desktop_file;

