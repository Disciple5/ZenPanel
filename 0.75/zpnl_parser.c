/*
 *  z_parser.c
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


#include "zenpanel.h"

FILE * z_open_file (const char * filename){
	FILE *fp = NULL;
	fp = fopen (filename, "r");
	if (fp) return fp;	
	dbg("can't open file : %s\n", filename);
	return NULL;
}


char * z_read_parameter (const char *pname, FILE *fp){
	char buffer[512];
	char *value = NULL;
	char *sub=NULL;
	char *p=NULL;
	
	/* look for a given parameter in the .desktop file. */
	if (!fp) {
		return NULL;
	}
	
	memset (buffer, 511, 0);
	rewind (fp);
	do {
		p = fgets (buffer, 511, fp);
		if (p) {
			sub = strstr (buffer, pname);
		} else {
			break;
		}
	} while (!sub);
	
	if (sub) {
		p = strchr (buffer, '=');
		if (p) {
			p+=1;
			value = strdup (p);
		}
		return value;
	}else{
		return "not found";
	}
free(buffer);
}

/*
 * Closes the currently opened file pointed by fp
 */
void z_close_file (FILE *fp){
	fclose (fp);
	fp = NULL;
}

/*
 * Parses the cmdline into arguments
 */
void  z_parse_cmd (char *line, char **argv){
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = '\0';                 /* mark the end of argument list  */
}
