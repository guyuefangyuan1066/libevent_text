#include "Ftpserver.h"
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<iostream>

static void EventCB(bufferevent* bev,short what, void* arg) {

	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)) {
		std::cout << "eof OR error OR timeout\n";

		bufferevent_free(bev);
		delete ((Ftpserver*)arg);
	}
}
static void ReadCB(bufferevent* bev,void *arg) {
	char buf[128] = { 0 };
	while (1) {
		int l = bufferevent_read(bev, buf, sizeof(buf) - 1);
		if (l <= 0) {
			return;
		}
		//buf[l] = '\0';
		std::cout << buf;
		break;
	}
}
bool Ftpserver::Init() {

	std::cout << "Ftpserver::Init() \n";
	bufferevent* bev =bufferevent_socket_new(base,sock,BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, ReadCB, 0, EventCB, this);
	bufferevent_enable(bev, EV_READ |EV_WRITE );
	timeval t = { 10 };
	bufferevent_set_timeouts(bev,&t,0);
	return true;
}
Ftpserver::Ftpserver() {

}

