#pragma once
#include<vector>
class XThread;
class XTask;
class XThreadPool
{
	

private:
	XThreadPool() {};
	int threadcount = 0;
	std::vector<XThread*>threads;
	int lastThread = -1;
public:
	void Init(int count);
	static XThreadPool* get() {
		static XThreadPool p;
		return &p;
	}
	void Dispatch(XTask* task);
	~XThreadPool() {

	};
};

