#pragma once
#include <dshow.h>
#include	<process.h>

class MoviePlay
{
private:
	static unsigned __stdcall PlayThread( void* p );
	//����Đ��p
	IGraphBuilder *pGraphBuilder;
	static IMediaControl *pMediaControl;

	//���悪�I���܂ōĐ�����p
	IMediaEvent *pMediaEvent;
	static long moviestate;

	//�E�B���h�E�̕ύX�p
	IVideoWindow *pVideoWindow;

	//�V�[�N���邽��
	static IMediaSeeking* pMediaSeeking;

	static IMediaPosition* pMediaPosition;

	static double playrate;

	
public:
	MoviePlay();
	~MoviePlay();

	void SetSource( char* filepass );
	void SetWindow( OAHWND window, long left, long top, long width, long height );
	void Rewind( int time );
	void FastForward( int time );
	bool GetStateRun();
	/*
	�Đ����[�g��WMV�EWMA�`���̃t�@�C���͑Ή����Ă��܂���
	*/
	void Play( float rate = 1.0f );
	void Stop();
	void Pause();
	void Reset();

};