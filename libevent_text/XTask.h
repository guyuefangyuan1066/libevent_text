#pragma once
class XTask
{
private:
	//XTask();
public:
	struct event_base* base = nullptr;

	int sock = 0;

	virtual bool Init() = 0;
	
};

