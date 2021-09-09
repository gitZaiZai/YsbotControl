#include "StdAfx.h"
#include "ThreadTaskTypeBase.h"


ThreadTaskTypeBase::ThreadTaskTypeBase(std::string name) :exitFlag(false), thread_alive(false),task_cycle_time(200),
	task_name(name),bNewInfo(false)
{
	InitializeCriticalSection(&m_cs);

}
 
ThreadTaskTypeBase::~ThreadTaskTypeBase(void)
{
	stop_task_thread();

	DeleteCriticalSection(&m_cs);
 
	CString name = CString(task_name.c_str());
	name += _T(" thread destructor!\n");
	TRACE(name);

	//std::cout << task_name << " thread destructor!\n";
}

void ThreadTaskTypeBase::stop_task_thread()
{
	do 
	{
		exitFlag = true;
		Sleep(1);
	} while ( thread_alive ); // important for terminate thread.
	m_Thread = NULL;
}

bool ThreadTaskTypeBase::start_task_thread()
{
	if ( !(m_Thread = AfxBeginThread(task_loop, this)) )
		return false;
	TRACE("Thread started motion Loop \n");
	return true;
}

unsigned int ThreadTaskTypeBase::task_loop( LPVOID pParam )
{
	ThreadTaskTypeBase *ptask = ( ThreadTaskTypeBase* )pParam;
	std::string strtask = ptask->task_name;
	CString name = CString(strtask.c_str());
	CString msg = name + _T(" task start!-----\n");
	TRACE(msg);
	std::cout << ptask->task_name << " task start!------\n";

	ptask->thread_alive = true;

    ptask->thread_task(ptask);

	ptask->thread_alive = false;

 	msg = name + _T(" task end! ------\n");
	TRACE(msg);

	std::cout << strtask << " task end! ******\n";

	return 0;
}

void ThreadTaskTypeBase::InitLock()
{
	InitializeCriticalSection(&m_cs);
}

void ThreadTaskTypeBase::Lock()
{
	EnterCriticalSection(&m_cs);
}

void ThreadTaskTypeBase::UnLock()
{
	LeaveCriticalSection(&m_cs);	
}

void ThreadTaskTypeBase::ReleaseLock()
{
	DeleteCriticalSection(&m_cs);
}