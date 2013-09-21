#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

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
    int len;
    struct kevent ke;

    printf ("On read !\n");

	len = read (fd, &ke, sizeof (struct kevent));

    printf ("Read: %d bytes, id: %u\n", len, ke->ident);

}

int main (int argc, char *argv[])
{
    int fd;
    struct event_base *evbase;
    struct event *ev;

    evbase = event_base_new ();

    fd = open (argv[1], O_RDONLY);
    if (fd < 0) {
        printf ("Failed to open !\n");
        return -1;
    }
    
    printf ("method: %s, fd: %d \n", event_base_get_method (evbase), fd);
    
    ev = event_new (evbase, fd, EV_READ|EV_PERSIST, on_read, NULL);
    if (!ev) {
        printf ("Failed to create event !\n");
        return -1;
    }

    event_add (ev, NULL);
    event_base_dispatch (evbase);

    return 0;
}
