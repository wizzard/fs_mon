/*
 * Copyright (C) 2013 Paul Ionkin <paul.ionkin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <fam.h>

#include "khash.h"

static long unsigned int total_handlers = 0;
KHASH_MAP_INIT_INT(h32, char*)
static khash_t(h32) *h;
static FAMConnection fc;

#define REPORT_EACH 1000

static int fd;

// 0: ok
// 1: error
int scan_dir (const char *dir_path)
{
    DIR *dir;
    static struct dirent *ent;
    static struct stat st;
    char *cur_path;
    char *next_file;
    int wd;
    khiter_t k;
    int ret;
    FAMRequest fr;

    dir = opendir (dir_path);
    if (!dir) {
        fprintf (stderr, "Can't open directory: %s\n", dir_path);
        return -1;
    }

	if (dir_path[strlen (dir_path) - 1] != '/' ) {
		asprintf (&cur_path, "%s/", dir_path);
	} else {
		cur_path = (char *)dir_path;
	}

    wd = FAMMonitorDirectory (&fc, cur_path, &fr, NULL);
    if (wd < 0) {
        fprintf (stderr, "add_watch failed. So far: %lu watches added \n", total_handlers);
        return -1;
    }
    total_handlers++;
/*
    k = kh_put(h32, h, wd, &ret);
    if (!ret) {
        fprintf (stderr, "Hash error !\n");
        return -1;
    }

    kh_value(h, k) = &fr;
*/
    if (total_handlers % REPORT_EACH == 0) {
        fprintf (stderr, "So far: %lu watches added\n", total_handlers);
    }

    ent = readdir (dir);
    while (ent) {
        if ((strcmp (ent->d_name, ".")) && strcmp (ent->d_name, "..")) {
            asprintf (&next_file,"%s%s", cur_path, ent->d_name);

            if (lstat (next_file, &st) == -1) {
                fprintf (stderr, "Failed to stat file: %s\n", next_file);
                return -1;
            }

			if (S_ISDIR (st.st_mode) && !S_ISLNK (st.st_mode)) {
				free (next_file);
				asprintf (&next_file,"%s%s/", cur_path, ent->d_name);
                if (scan_dir (next_file) < 0) {
                    fprintf (stderr, "Aborting !\n");
                    return -1;
                }
            }

            free (next_file);
        }

        ent = readdir (dir);
    }

    closedir (dir);

    return 0;
}

void process_events (FAMEvent* fe)
{
    printf("req %d: ", fe->fr.reqnum);
    switch (fe->code) {
	case FAMExists:
	    printf("%s Exists\n", fe->filename);
	    break;
	case FAMEndExist:
	    printf("%s EndExist\n", fe->filename);
	    break;
	case FAMChanged:
	    printf("%s Changed\n", fe->filename);
	    break;
	case FAMDeleted:
	    printf("%s was deleted\n", fe->filename);
	    break;
	case FAMStartExecuting:
	    printf("%s started executing\n", fe->filename);
	    break;
	case FAMStopExecuting:
	    printf("%s stopped executing\n", fe->filename);
	    break;
	case FAMCreated:
	    printf("%s was created\n", fe->filename);
	    break;
	case FAMMoved:
	    printf("%s was moved\n", fe->filename);
	    break;
	default:
	    printf("(unknown event %d on %s)\n", fe->code, fe->filename);
    }

    /*
    ssize_t len, i = 0;
    char buff[BUFF_SIZE] = {0};

    len = read (fd, buff, BUFF_SIZE);

    while (i < len) {
        struct inotify_event *pevent = (struct inotify_event *)&buff[i];

        if (pevent->mask & IN_ACCESS) {
         //   strcat(action, " was read");
         //   printf ("%s [%s]\n", action, pevent->name);
        }
        if (pevent->mask & IN_ATTRIB) {
          //  strcat(action, " Metadata changed");
         //   printf ("%s [%s]\n", action, pevent->name);
        }
        if (pevent->mask & IN_CLOSE_WRITE) {
            fprintf (stderr, "opened for writing was closed: %s\n", pevent->name);
        }
        if (pevent->mask & IN_CLOSE_NOWRITE) {
        //    strcat(action, " not opened for writing was closed");
        //    printf ("%s [%s]\n", action, pevent->name);
        }
        if (pevent->mask & IN_CREATE) {
            char *parent = NULL;
            khiter_t k;

            k = kh_get(h32, h, pevent->wd);

            if (k != kh_end(h)) {
                char *fname;
                static struct stat st;

                parent = kh_value(h, k);
                if (!parent)
                    fprintf (stderr, "OPS !!\n");
    
                fprintf (stderr, "created in watched directory: wd: %d [%s] parent: %s\n", pevent->wd, pevent->name, parent);

                asprintf (&fname,"%s%s", parent, pevent->name);

                if (lstat (fname, &st) != -1) {

                    if (S_ISDIR (st.st_mode) && !S_ISLNK (st.st_mode)) {
                        scan_dir (fname);
                    }

                } else {
                    fprintf (stderr, "Failed to stat file: %s\n", fname);
                }
            } else {
                fprintf (stderr, "OPS, parent not found ! wd: %d created in watched directory: [%s]\n", pevent->wd, pevent->name);

            }
        }
        if (pevent->mask & IN_DELETE) {
            fprintf (stderr, "deleted from watched directory: %s\n", pevent->name);
        }
        if (pevent->mask & IN_DELETE_SELF) {
            printf ("watched file/directory was itself deleted: %s\n", pevent->name);
            // XXX:
        }
        if (pevent->mask & IN_MODIFY) {
        //    strcat(action, " was modified");
        //    printf ("%s [%s]\n", action, pevent->name);
        }
        if (pevent->mask & IN_MOVE_SELF) {
            printf ("watched file/directory was itself moved: %s\n", pevent->name);
        }
        if (pevent->mask & IN_MOVED_FROM) {
            printf ("moved out of watched directory: %s\n", pevent->name);
        }
        if (pevent->mask & IN_MOVED_TO) {
            printf ("moved into watched directory: %s\n", pevent->name);
        }
        if (pevent->mask & IN_OPEN) {
        //    strcat(action, " was opened");
        //    printf ("%s [%s]\n", action, pevent->name);
        }

        i += sizeof(struct inotify_event) + pevent->len;
    }
}
*/

int main (int argc, char *argv[])
{
    char target[FILENAME_MAX];
    int fd;
    FAMEvent fe;

    if (argc < 2) {
        fprintf (stderr, "Watching the current directory\n");
        strcpy (target, ".");
    } else {
        fprintf (stderr, "Watching %s\n", argv[1]);
        strcpy (target, argv[1]);
    }

    h = kh_init(h32);

    fd = FAMOpen2 (&fc, argv[0]);
    if (fd < 0) {
        fprintf (stderr, "FAM init failed\n");
        return 1;
    }

    if (scan_dir (argv[1]) < 0) {
        return 1;
    }

    fprintf (stderr, "Total: %lu watches \n", total_handlers);
    fflush (stderr);

    for (; FAMPending (&fc); ) {
        if (FAMNextEvent (&fc, &fe) < 0) {
            printf (stderr, "FAMNextEvent() returned < 0!\n");
            exit (1);
        }
	    processDirEvents (&fe);
        fflush (stdout);
	}


    kh_destroy(h32, h);

    return 0;
}
