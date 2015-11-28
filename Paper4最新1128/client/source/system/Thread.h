#pragma once
//*****************************************************************************************************************************
//
//		thread
//
//*****************************************************************************************************************************

class Thread final
{
private:
	typedef void(*pf)(void*, bool*);

	pf m_pFunction;

	//void(*m_pFunction)(void*,bool*);
	void* m_pArgument;

	HANDLE	m_threadhandle;
	bool	m_IsSuspend;
	bool    m_isEnd;

	static unsigned __stdcall Function(void* t);


public:
	Thread(pf pFunction, void* pArgument);
	~Thread();


	void Run();						/*スレッドスタート*/
	void End(){ m_isEnd = true; }	/*スレッド終了*/
	void ResumeThread();			/*スレッドの再開*/
	void SuspendThread();		/*スレッドの一時停止*/

	bool IsRun();
};

