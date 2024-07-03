#include "XThread.h"
#include"XTask.h"
#include<iostream>
#include<thread>
#include<event2/event.h>
#ifdef _WIN32
#else
#include<unistd.h>
#endif // _WIN32
static void NotfyCB(evutil_socket_t fd,short what,void *arg) {
	((XThread*)arg)->NotifyCB(fd, what);
}

  void  XThread::NotifyCB(evutil_socket_t fd,short what) {
	std::cout << "NotifyCB begin \n";
	XThread* t = this;
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	int re = read(fd, buf, 1);
#endif
	if (re <= 0) {
		return;
	}
	std::cout << t->id << ":" << buf << std::endl;

	std::cout << "NotifyCB end \n";
	XTask* tsk = nullptr;
	//std::lock_guard<std::mutex> lock(task_lock);
	task_lock.lock();
	if (task.empty()) {
		return;
	}

	tsk = task.front();
	task.pop_front();
	task_lock.unlock();
	//lock.~lock_guard();
	tsk->Init();

}

XThread::XThread() {
	
}
void XThread::Start() {
	this->XThread::Steup();
	std::thread th(&XThread::Main,this);
	//分离主线程与子线程的连接
	th.detach();
}
void XThread::Main() {
	std::cout <<id<< "XThread::Main begin" << std::endl;
	event_base_dispatch(base);
	event_base_free(base);
	std::cout << id << "XThread::Main end" << std::endl;
}
bool XThread::Steup() {
	//win32中创建收发信息的套接字对，Linux中创建管道
#ifdef _WIN32
	evutil_socket_t fds[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0) {
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0) {
			std::cout << "vutil_socketpair error \n";
			return false;
		}
		evutil_make_socket_nonblocking(fds[0]);
		evutil_make_socket_nonblocking(fds[1]);
	}
#else
	int fds[2];
	if (pipe(fds)) {
		std::cout << "pipe error\n";
		return false;
	}
#endif // _WIN32
	notify_send_it = fds[1];//记录发送接口
	//设置上下文
	event_config* ev_cfg = event_config_new();
	event_config_set_flag(ev_cfg, EVENT_BASE_FLAG_NOLOCK);//设置无锁
	this->base = event_base_new_with_config(ev_cfg);
	if (!base) {
		this->base = event_base_new_with_config(ev_cfg);
		std::cout << "event_base_config error \n";
		return false;
	}
	event_config_free(ev_cfg);
	//添加管道监听事件，用于激活线程任务
	event* ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotfyCB, this);
	event_add(ev, 0);
	return true;
	}
void XThread::addTask(XTask* tsk) {
	if (!tsk) {
		std::cout << "tsk error \n";
	}
	tsk->base = this->base;
	//std::lock_guard<std::mutex> lock(task_lock);
	task_lock.lock();
	task.push_back(tsk);
	task_lock.unlock();
}
void XThread::Activate() {
#ifdef  _WIN32
	int re = send(this->notify_send_it, "W", 1, 0);
#else
	int re = write(this->notify_send_it, "L", 1);
#endif //  _WIN32
	if (re <= 0) {
		std::cout << "Activate error";
	}
	std::cout << id<<" Activate\n";
}