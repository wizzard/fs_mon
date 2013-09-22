#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h> 

#include <glib.h>
#include <gio/gio.h>


static void
changed_cb (GFileMonitor      *monitor,
            GFile             *file,
            GFile             *other_file,
            GFileMonitorEvent  event,
            gpointer           data)
{
    if (other_file)
        g_printf ("========== XXXXXXXXXXXXX\n");

    switch (event) {
        case G_FILE_MONITOR_EVENT_CHANGED:
            g_printf ("Changed: %s \n", g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_DELETED:
            g_printf ("event deleted: %s \n", g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_CREATED:
            g_printf ("event created: %s \n", g_file_get_path (file));
            break;
        case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
            g_printf ("event changed: %s \n", g_file_get_path (file));
            break;
    }
}

int main (int argc, char *argv[])
{
  GFile *file;
  GError *error = NULL;
  GMainLoop *loop;
  GFileMonitor *dir_monitor;

  file = g_file_new_for_path (argv[1]);
  dir_monitor = g_file_monitor_directory (file, 0, NULL, &error);

  loop = g_main_loop_new (NULL, FALSE);

  g_signal_connect (dir_monitor, "changed", G_CALLBACK (changed_cb), NULL);

  g_main_loop_run (loop);

   return 0;
}
