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


	void Run();						/*�X���b�h�X�^�[�g*/
	void End(){ m_isEnd = true; }	/*�X���b�h�I��*/
	void ResumeThread();			/*�X���b�h�̍ĊJ*/
	void SuspendThread();		/*�X���b�h�̈ꎞ��~*/

	bool IsRun();
};

