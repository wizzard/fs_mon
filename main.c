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

#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>

#include "khash.h"

static long unsigned int total_handlers = 0;
KHASH_MAP_INIT_INT(h32, char*)
static khash_t(h32) *h;
static int global_wd = 0;

#define REPORT_EACH 1000

static int fd;

int scan_dir (const char *dir_path);

static void dir_changed_cb (GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event, gpointer data)
{
    switch (event) {
        case G_FILE_MONITOR_EVENT_CHANGED:
            g_printf ("Changed: %s \n", g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_DELETED:
            g_printf ("event deleted: %s \n", g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_CREATED:
            g_printf ("event created: %s \n", g_file_get_path (file));
            scan_dir (g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
            g_printf ("event changed: %s \n", g_file_get_path (file));
            break;

        case G_FILE_MONITOR_EVENT_MOVED:
            g_printf ("event moved: %s  old: %s\n", g_file_get_path (file), g_file_get_path (other_file));
            break;

    }
}

static int install_watch (const char *dir_path)
{
    GFile *file;
    GError *error = NULL;
    GFileMonitor *dir_monitor;

    file = g_file_new_for_path (dir_path);
    if (!file) {
        g_printf ("Failed to create file object from path: %s\n", dir_path);
        return -1;
    }

    dir_monitor = g_file_monitor_directory (file, 0, NULL, &error);
    if (!dir_monitor) {
        g_printf ("Failed to create directory monitor for: %s\n", dir_path);
        return -1;
    }

    g_signal_connect (dir_monitor, "changed", G_CALLBACK (dir_changed_cb), NULL);
    
    return 0;
}

// 0: ok
// 1: error
int scan_dir (const char *dir_path)
{
    DIR *dir;
    static struct dirent *ent;
    static struct stat st;
    char *cur_path;
    char *next_file;
    khiter_t k;
    int ret;

    if (lstat (dir_path, &st) == -1) {
        fprintf (stderr, "Failed to stat file: %s\n", dir_path);
        return -1;
    }
    
    if (!S_ISDIR (st.st_mode))
        return 0;

    dir = opendir (dir_path);
    if (!dir) {
        perror ("opendir: ");
        return -1;
    }

	if (dir_path[strlen (dir_path) - 1] != '/' ) {
		cur_path = g_strdup_printf ("%s/", dir_path);
	} else {
		cur_path = (char *)dir_path;
	}

    ret = install_watch (cur_path);
    if (ret < 0) {
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

    kh_value(h, k) = strdup (cur_path);
    **/
    if (total_handlers % REPORT_EACH == 0) {
        fprintf (stderr, "So far: %lu watches added\n", total_handlers);
    }

    ent = readdir (dir);
    while (ent) {
        if ((strcmp (ent->d_name, ".")) && strcmp (ent->d_name, "..")) {
            next_file = g_strdup_printf ("%s%s", cur_path, ent->d_name);

            if (lstat (next_file, &st) == -1) {
                fprintf (stderr, "Failed to stat file: %s\n", next_file);
                return -1;
            }

			if (S_ISDIR (st.st_mode) && !S_ISLNK (st.st_mode)) {
				g_free (next_file);
				next_file = g_strdup_printf ("%s%s/", cur_path, ent->d_name);
                if (scan_dir (next_file) < 0) {
                    fprintf (stderr, "Aborting !\n");
                    return -1;
                }
            }

            g_free (next_file);
        }

        ent = readdir (dir);
    }

    closedir (dir);

    if (cur_path != dir_path)
        g_free (cur_path);

    return 0;
}

int main (int argc, char *argv[])
{
    char target[FILENAME_MAX];
    GMainLoop *loop;

    if (argc < 2) {
        fprintf (stderr, "Watching the current directory\n");
        strcpy (target, ".");
    } else {
        fprintf (stderr, "Watching %s\n", argv[1]);
        strcpy (target, argv[1]);
    }

    h = kh_init(h32);

    if (scan_dir (argv[1]) < 0) {
        return 1;
    }

    fprintf (stderr, "Total: %lu watches \n", total_handlers);
    fflush (stderr);

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    kh_destroy(h32, h);

    return 0;
}
