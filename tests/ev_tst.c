#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/http_struct.h>

int main ()
{
    struct event_base *evbase;

    evbase = event_base_new ();

    printf ("method: %s\n", event_base_get_method (evbase));
    return 0;
}
