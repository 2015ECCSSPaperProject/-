#pragma once
#include <dshow.h>
#include	<process.h>

class MoviePlay
{
private:
	static unsigned __stdcall PlayThread( void* p );
	//動画再生用
	IGraphBuilder *pGraphBuilder;
	static IMediaControl *pMediaControl;

	//動画が終わるまで再生する用
	IMediaEvent *pMediaEvent;
	static long moviestate;

	//ウィンドウの変更用
	IVideoWindow *pVideoWindow;

	//シークするため
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
	再生レートはWMV・WMA形式のファイルは対応していません
	*/
	void Play( float rate = 1.0f );
	void Stop();
	void Pause();
	void Reset();

};