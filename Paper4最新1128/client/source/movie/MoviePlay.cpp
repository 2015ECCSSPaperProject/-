
#include "Movie.h"

IMediaControl *MoviePlay::pMediaControl;
long MoviePlay::moviestate;
double MoviePlay::playrate;
IMediaPosition* MoviePlay::pMediaPosition;
IMediaSeeking* MoviePlay::pMediaSeeking;
MoviePlay::MoviePlay()
{
	CoInitialize( NULL );

	CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&pGraphBuilder );

	pGraphBuilder->QueryInterface( IID_IMediaControl, (LPVOID *)&pMediaControl );

	pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *)&pMediaEvent);
	
	pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVideoWindow);

	pGraphBuilder->QueryInterface(IID_IMediaPosition, (LPVOID *)&pMediaPosition);

	pGraphBuilder->QueryInterface(IID_IMediaSeeking, (LPVOID *)&pMediaSeeking);

}

MoviePlay::~MoviePlay()
{
	pMediaControl -> Release();
	pMediaEvent -> Release();
	pVideoWindow -> Release();
	pMediaPosition->Release();
	pMediaSeeking->Release();
	pGraphBuilder -> Release();

	CoUninitialize();
}

void MoviePlay::SetSource( char* filepass )
{
	LPSTR lstr = filepass;

	BSTR bstr;

	int bstrlen = (int)MultiByteToWideChar( CP_ACP, 0, lstr, strlen(lstr), NULL, 0 );
	bstr = SysAllocStringLen( NULL, bstrlen );

	MultiByteToWideChar( CP_ACP, 0, lstr, strlen(lstr), bstr, bstrlen );

	pMediaControl -> RenderFile( bstr );
	pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVideoWindow);
}

void MoviePlay::SetWindow( OAHWND window, long left, long top, long width, long height )
{
	pVideoWindow->put_Owner( window );
	pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
	pVideoWindow->SetWindowPosition( left, top, width, height );


}

void MoviePlay::Play( float rate )
{
	// Full Screen 開始
	//pVideoWindow->put_FullScreenMode(OATRUE);

	(HANDLE)_beginthreadex( NULL, 0, PlayThread, NULL, 0, NULL );
}

void MoviePlay::Stop()
{
	// Full Screen 終了
	//pVideoWindow->put_FullScreenMode(OAFALSE);
	pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVideoWindow);

	pMediaControl -> Stop();
	pMediaControl->GetState( 0, &moviestate );


}

void MoviePlay::Pause()
{
	
}

void MoviePlay::Reset()
{
	//pMediaControl->
	LONGLONG hoge = 0;
	pMediaSeeking->SetPositions(&hoge, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);


}


bool MoviePlay::GetStateRun()
{
	if( moviestate == State_Running ) return true;
	
	return false;
}

void MoviePlay::Rewind( int time )
{

}

void MoviePlay::FastForward( int time )
{
	if( GetAsyncKeyState( 'O' ) && 0x001 )pMediaSeeking->SetRate( 2.0 );
}

unsigned __stdcall MoviePlay::PlayThread( void *p )
{
	// 再生開始

	pMediaControl -> Run();
	while( true ){
		pMediaControl->GetState( 0, &moviestate );
		if( moviestate == State_Stopped ) break;
	}
	_endthreadex( 0 );

	return 0;
}