#include "XThreadPool.h"
#include"XThread.h"
#include<thread>
#include<iostream>
#include"XTask.h"
using namespace std;
void XThreadPool::Init(int count) {
	this->threadcount = count;
	for (int i = 0; i < threadcount; ++i) {
		XThread* p = new XThread();
		p->id = i + 1;
		p->Start();
		threads.push_back(p);
	    this_thread::sleep_for(10ms);
	}
	std::cout << "create threads success \n";
}
 void XThreadPool::Dispatch(XTask* task) {
	 if (!task) {
		 std::cout << "task error \n";
	 }
	 int tid = (lastThread + 1) % threadcount;
	 lastThread = tid;
	 auto a=threads[tid];
	 
	 a->addTask(task);
	 
	 a->Activate();

}