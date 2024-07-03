#pragma once
#include<event2/event.h>
#include<list>
#include<mutex>
class XTask;
class XThread
{
private:
	int notify_send_it = 0;
	std::list<XTask*>task;
	struct event_base* base = nullptr;
	std::mutex task_lock;
public:

	int id;
	XThread();
	~XThread();
	void Start();
	void Main();
	bool Steup();
	void addTask(XTask*task );
	void Activate();
    void NotifyCB(evutil_socket_t fd, short what);
};

