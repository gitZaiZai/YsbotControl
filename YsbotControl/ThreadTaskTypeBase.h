// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#pragma once

#include <string>
 
class ThreadTaskTypeBase
{
public:
	ThreadTaskTypeBase(std::string name);
	virtual ~ThreadTaskTypeBase(void);
	virtual unsigned int thread_task(ThreadTaskTypeBase* ) = 0;
	volatile bool    exitFlag;
	volatile bool    bNewInfo;
	bool start_task_thread ();
	void stop_task_thread ();

	void Lock();
	void UnLock();

	unsigned int task_cycle_time;

private:
	void InitLock();
	void ReleaseLock();

	CRITICAL_SECTION m_cs;
	CWinThread*		 m_Thread;
	bool             thread_alive;
	std::string      task_name;

	static unsigned int	task_loop(LPVOID pParam);

};

