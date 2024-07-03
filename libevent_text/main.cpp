#include<event2/event.h>
#include<event2/listener.h>
#include<event2/bufferevent.h>
#include<iostream>
#include"XThreadPool.h"
#include <winsock.h>
#include"Ftpserver.h"
#pragma comment(lib,"ws2_32.lib")


void Readcb(struct bufferevent*bev,void*ctx) {
    std::cout << "Readcb \n";
    char buf[128] = { 0 };
    int len = bufferevent_read(bev, buf, sizeof(buf) - 1);
    std::cout << buf << std::endl;
    bufferevent_write(bev, "ok", 3);
}
void Writecb(struct bufferevent* bev, void* ctx) {
    std::cout << "Writecb \n";
}
void Eventcb(struct bufferevent* bev,short what, void* ctx) {
    if (what & BEV_EVENT_TIMEOUT && what & BEV_EVENT_READING) {

        std::cout << "time out reading \n";
        //��ȡ������
        bufferevent_free(bev);
        return;
    }
    else if (what & BEV_EVENT_TIMEOUT && what & BEV_EVENT_WRITING) {

        std::cout << "time out writeing\n";
        //����ع�
        bufferevent_free(bev);
        return;
    }
    else if (what & BEV_EVENT_ERROR) {
        std::cout << "error \n";
        //��ȡ������
        //����ع�
        bufferevent_free(bev);
        return;
    }
    else if (what & BEV_EVENT_EOF) {
        std::cout << "eof \n";
        bufferevent_free(bev);
        return;
    }
}
void listenCB(evconnlistener*evc, intptr_t client_socket, sockaddr*client_addr, int socklen, void*arg) {
    std::cout << "listenCB begin" << std::endl;
    //if (!(what & EV_READ)) {
    //    std::cout << "no read \n";
    //}
    //sockaddr_in sin;
    //memset(&sin, 0, sizeof(sin));
    //socklen_t size = sizeof(sin);
    //evutil_socket_t client_socket = accept(sock, (sockaddr*)&sin, &size);
    //if (client_socket <= 0) {
    //    std::cout << "client_socket error \n";
    //    return;
    //}
    char ip[16] = { 0 };
    evutil_inet_ntop(AF_INET, &((sockaddr_in*)client_addr)->sin_addr, ip, sizeof(ip));
    std::cout << ip << std::endl;
    XTask* tsk = new Ftpserver();
    tsk->sock = client_socket;
    XThreadPool::get()->Dispatch(tsk);
    //event_base* base = (event_base*)arg;
    //bufferevent* bev = bufferevent_socket_new(base,client_socket,BEV_OPT_CLOSE_ON_FREE);
    //if (!bev) {
    //    std::cout << "bufferevent error \n";
    //    return;
    //}
    //bufferevent_enable(bev, EV_READ | EV_WRITE);
    //timeval tl = { 10,0 };
    //bufferevent_set_timeouts(bev,&tl,0);
    //bufferevent_setcb(bev, Readcb, Writecb,Eventcb,base);
   // while (1) {}
    std::cout << "listenCB end" << std::endl;
}
int main() {
    std::cout << "server begin" << std::endl;
#ifdef _WIN32
    //��ʼ��Windows socket��
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }
#else
    //�Ͽ�socket���Ӻ󣬻��������̷����ź�ʹ���������˳������������������źŷ�ֹ�������˳���
    if (signal(SIGPIPE, SIG_IGN) == SIGERR)
        return 1;
#endif

    XThreadPool::get()->Init(10);




    //���������ģ�Ĭ������
    event_base* base = event_base_new();
    if (!base) {
        std::cout << "event_base error" << std::endl;
        return -1;
    }
    ////�����ļ�������������������Ϣ
    //evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    //if (sock <= 0) {
    //    std::cout << "event_base error" << std::endl;
    //    return -1;
    //}
    //
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);

    auto evc = evconnlistener_new_bind(base,listenCB,base,LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,10,(sockaddr*) & sin, sizeof(sin));
    event_base_dispatch(base);
    //int re = ::bind(sock,(sockaddr*)&sin,sizeof(sin));
    //if (re != 0) {
    //    std::cout << "bind error" << std::endl;
    //    return -1;
    //}
    //listen(sock, 10);

    ////ע��socket�¼��Ļص��������ص����� EV_PERSIST ���ܹ���ν����¼�
    ////Ĭ��ˮƽ������û�����ݴ������ʱһֱ��ȡ���ݡ�
    ////���һ�������ᴫ�ݵ�ǰ�������¼�
    //event* ev = event_new(base, sock,EV_READ | EV_PERSIST,listenCB,event_self_cbarg() );
    ////����¼����ڶ����������ó�ʱʱ�䣨ֵΪ0ʱ��û�г�ʱʱ�䣩��
    //event_add(ev, 0);

    ////�����¼�ѭ��
    //event_base_dispatch(base);

    ////�ͷ���Դ
    //evutil_closesocket(sock);
    //event_del(ev);
    //event_free(ev);
    //evconnlistener_free(evc);
    //event_base_free(base);


    return 0;
}
