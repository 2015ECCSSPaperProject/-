#include	"iextreme.h"
#include	"Framework.h"
#include	"../IEX/OKB.h"
#include	"../Scene/SceneMain.h"
#include	"../Scene/SceneTitle.h"
#include	"../Scene/SceneMakePoster.h"
#include	"../sound/SoundManager.h"
#include	"../score/Score.h"
#include	"../../resource.h"
#include<crtdbg.h>

//*****************************************************************************************************************************
//
//
//
//*****************************************************************************************************************************

char*	AppTitle = "ペーパーボーズ クライアント";

BOOL	bFullScreen = FALSE;
DWORD	ScreenMode = SCREEN720p;

Framework*	MainFrame = NULL;



//*****************************************************************************************************************************
//		アプリケーション初期化
//*****************************************************************************************************************************

BOOL	InitApp( HWND hWnd )
{
	srand((unsigned int)time(NULL));

	//	IEXシステム初期化
	IEX_Initialize( hWnd, bFullScreen, ScreenMode );
	IEX_InitAudio();
	IEX_InitInput();

	// テキスト初期化
	Text::Init();

	//静的Debug関連
	Debug::Init();

	// キーボード入力初期化
	OKB_Init();

	// ナンバー初期化
	Number::Init();

	// サウンド初期化
	SoundManager::Initialize();

	// フォント初期化
	Font::Initialize();

	//	システムの初期化
	SYSTEM_Initialize();
	//	メインフレームワーク生成
	MainFrame = new Framework();
	//	初期シーン登録
	MainFrame->ChangeScene(new SceneTitle());

	return TRUE;
}

short mouse_wheel_delta = 0;

//*****************************************************************************************************************************
//		ウィンドウプロシージャ
//*****************************************************************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
	case WM_DESTROY: PostQuitMessage(0); return 0;
	case WM_KEYDOWN:
			switch (wParam){
			case VK_ESCAPE: PostMessage(hWnd, WM_CLOSE, 0, 0); return 0;
			case VK_F1:		iexSystem::OpenDebugWindow(); return 0;
			}
			break;
	case WM_MOUSEWHEEL:
		mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(wParam);
		if (mouse_wheel_delta >= 120){
			mouse_wheel_delta = 0;
			Up_wheel();		// OKBの関数
		}
		else if (mouse_wheel_delta <= 120){
			mouse_wheel_delta = 0;
			Down_wheel();	// OKBの関数
		}
		break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//*****************************************************************************************************************************
//		ウィンドウ作成
//*****************************************************************************************************************************
HWND InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	HWND			hWnd;

	//	スクリーンサイズ取得
	RECT	WindowSize;
	iexSystem::GetScreenRect( ScreenMode, WindowSize );

	//	ウィンドウクラス設定
	WNDCLASS	wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon = LoadIcon(hInstance, LPSTR(IDI_ICON1));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "IEX2010";
	RegisterClass(&wc);

	//	ウィンドウ作成
	if (!bFullScreen){
		AdjustWindowRect(&WindowSize, WS_OVERLAPPEDWINDOW, FALSE);
		hWnd = CreateWindow("IEX2010", AppTitle, WS_SYSMENU,
								0, 0, WindowSize.right-WindowSize.left, WindowSize.bottom-WindowSize.top,
								NULL, NULL, hInstance, NULL);
	} else {
		hWnd = CreateWindow( "IEX2010", AppTitle, WS_POPUP, 0, 0, WindowSize.right,WindowSize.bottom, NULL, NULL, hInstance, NULL);
		ShowCursor(FALSE);
	}
	if( !hWnd ) return NULL;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    return hWnd;
}

//*****************************************************************************************************************************
//		WinMain
//*****************************************************************************************************************************
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// メモリリーク検知器
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// メモリ確保した瞬間検知器
	//_CrtSetBreakAlloc(20604);


	MSG		msg;
	HWND	hWnd;

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		ScreenMode = SCREEN1080p;
		bFullScreen = TRUE;
	}

	hWnd = InitWindow(hInstance, nCmdShow);
	InitApp(hWnd);

	//	メインループ
	for(;;){
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
			if( !GetMessage(&msg, NULL, 0, 0) ) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			if( MainFrame->Update() ) 
				MainFrame->Render();
		}
	}

	//	全解放
	Number::CleanUp();// ナンバー


	//解放の先頭で静的Debug関連解放
	Debug::CleanUp();
	delete	MainFrame;
	SYSTEM_Release();
	iexSystem::CloseDebugWindow();
	IEX_ReleaseInput();
	IEX_Release();
	IEX_ReleaseAudio();
	OKB_Release();
	Text::CleanUpModule();
	SoundManager::Release();
	Font::Release();

	return 0;
}
