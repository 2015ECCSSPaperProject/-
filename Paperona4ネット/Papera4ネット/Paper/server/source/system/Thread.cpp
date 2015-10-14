
#include <process.h>
#include <Windows.h>

#include"iextreme.h"
#include"Thread.h"


//********************************************************************************************
//
//   
//
//********************************************************************************************
Thread::Thread(pf pFunction, void* pArgument) :
m_threadhandle(nullptr),
m_pArgument(pArgument),
m_IsSuspend(false),
m_pFunction(pFunction),
m_isEnd(false)
{
}
Thread::~Thread()
{
	m_isEnd = true;
	WaitForSingleObject(m_threadhandle, INFINITE);
	BOOL out = CloseHandle(m_threadhandle);
}


//---------------------------------------------------------------------
//   
//---------------------------------------------------------------------
unsigned __stdcall Thread::Function(void* t)
{
	Thread* thread = (Thread*)t;
	thread->m_pFunction(thread->m_pArgument, &thread->m_isEnd);
	_endthreadex(0);
	return 0;
}




//---------------------------------------------------------------------
//   
//---------------------------------------------------------------------
void Thread::Run()
{
	m_threadhandle = (HANDLE)_beginthreadex(nullptr, 0, Function, this, 0, nullptr);
}
void Thread::ResumeThread()
{
	if (IsRun() && m_IsSuspend)
	{
		::ResumeThread(m_threadhandle);
		m_IsSuspend = false;
	}
}
void Thread::SuspendThread()
{
	if (IsRun() && !m_IsSuspend)
	{
		::SuspendThread(m_threadhandle);
		m_IsSuspend = true;
	}
}
bool Thread::IsRun()
{
	if (m_threadhandle == null)
		return false;

	DWORD exitcode;
	GetExitCodeThread(m_threadhandle, &exitcode);
	return STILL_ACTIVE == exitcode;
}