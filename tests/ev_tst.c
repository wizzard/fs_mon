#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/http_struct.h>

static void on_read (int fd, short event, void *arg)
{
    char buf[255];
    int len;

    printf ("On read !\n");

	len = read (fd, buf, sizeof(buf) - 1);

    printf ("Read: %d bytes\n", len);

}

int main (int argc, char *argv[])
{
    int fd;
    struct event_base *evbase;
    struct event *ev;

    evbase = event_base_new ();
    printf ("method: %s\n", event_base_get_method (evbase));

    fd = open (argv[1], O_RDONLY);
    if (fd < 0) {
        printf ("Failed to open !\n");
        return -1;
    }
    ev = event_new (evbase, fd, EV_READ|EV_PERSIST, on_read, NULL);
    if (!ev) {
        printf ("Failed to create event !\n");
        return -1;
    }

    event_base_dispatch (evbase);

    return 0;
}
