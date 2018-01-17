// 201221066_hyunjin.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "201321047_주수강_5.h"
#include "egl/egl.h"
#include "vg/openvg.h"
#include "vgLoadImage.h"

#define MAX_LOADSTRING 100
#define KEY_DOWN(code) (GetAsyncKeyState(code)&0x8000)

void TimerProc();

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

EGLDisplay display;
EGLSurface surface;
EGLContext context;

VGImage	PlayerImg;
VGImage EnemyImg;
VGImage masterImg;
VGImage	BulletImg;
VGImage	BackGroundImg;

struct Enemy {
	int x, y, shoot, die;
} enemy[100];

struct Master {
	int x, y, shoot, die;
} master[100];

struct Player {
	int x, y;
} player;

struct Bullet {
	int x, y, shoot;
} bullet[100];

int by = 0;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY201321047__5, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY201321047__5));

	// 기본 메시지 루프입니다.
	DWORD lastTime = GetTickCount();

	while (1)
	{
		if( PeekMessage(&msg, NULL, 0, 0, 1) )
		{
			if(!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{ TranslateMessage(&msg);
			DispatchMessage(&msg); }
			if( msg.message == WM_QUIT ) break;
		}
		DWORD curTime = GetTickCount();
		if( curTime-lastTime>32 ) // 30 frame per second
		{
			lastTime = lastTime+33;
			TimerProc();
		}
	}
	return (int) msg.wParam;
}


//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY201321047__5));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 650, 510, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if(wParam==VK_SPACE){
			for( int i=0; i<100; i++ ){
				if( !bullet[i].shoot ){
					bullet[i].y = player.y+30;
					bullet[i].x = player.x+35;
					bullet[i].shoot = 1;
					break;
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		{
			display = eglGetDisplay( GetDC(hWnd));
			eglInitialize( display, NULL, NULL );
			eglBindAPI( EGL_OPENVG_API );
			EGLint conf_list[] = { EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_NONE };
			EGLConfig config;
			EGLint    num_config;
			eglChooseConfig( display, conf_list, &config, 1, &num_config );
			surface = eglCreateWindowSurface( display, config, hWnd, NULL );
			context = eglCreateContext( display, 0, NULL, NULL );

			for( int i=0; i<100; i++ ){
			enemy[i].shoot=0;
			enemy[i].die=0;
			bullet[i].shoot=0;
			master[i].shoot = 0;
			master[i].die = 0;
			}
			player.x = 320-35;
			player.y = 20;

			PlayerImg = vgLoadImage(TEXT("player.png"));
			EnemyImg = vgLoadImage(TEXT("enemy.png"));
			masterImg = vgLoadImage(TEXT("master.png"));
			BulletImg = vgLoadImage(TEXT("bullet.png"));
			BackGroundImg = vgLoadImage(TEXT("back.jpg"));

		}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void draw()
{
	eglMakeCurrent( display, surface, surface, context );
	vgSeti( VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	VGfloat color[4]={0,0,0,1};
	vgSetfv(VG_CLEAR_COLOR, 4, color );
	vgClear(0,0,1450,850);

	int k = by/480;

	vgLoadIdentity();
	vgTranslate(0,k*480-by);
	vgDrawImage(BackGroundImg);

	vgLoadIdentity();
	vgTranslate(0,(k+1)*480-by);
	vgDrawImage(BackGroundImg);

	for( int i=0; i<100; i++ ){
		if(master[i].shoot){
			vgLoadIdentity();
			vgTranslate(master[i].x,master[i].y);
			vgDrawImage(masterImg);
		}
	}

	for( int i=0; i<100; i++ ){
		if(enemy[i].shoot){
			vgLoadIdentity();
			vgTranslate(enemy[i].x,enemy[i].y);
			vgTranslate(0,42);
			vgScale(1,-1);
			vgDrawImage(EnemyImg);
		}
	}
	for( int i=0; i<100; i++ ){
		if( bullet[i].shoot ){
			vgLoadIdentity();
			vgTranslate(bullet[i].x,bullet[i].y);
			vgDrawImage(BulletImg);
		}
	}
	
	vgLoadIdentity();
	vgTranslate(player.x,player.y);
	vgDrawImage(PlayerImg);

	eglSwapBuffers( display, surface );
}


void TimerProc()
{
	by +=5;
	
	if( KEY_DOWN(VK_RIGHT) && player.x<640) 
		player.x +=10;
	if( KEY_DOWN(VK_LEFT) && player.x>0)
		player.x -=10;

	if( rand()%30 == 9 ){
		for( int i=0; i<100; i++ ){
			if( !enemy[i].shoot ){
				enemy[i].x = rand()%551+50;
				enemy[i].y = 640;
				enemy[i].shoot = 1;
				break;
			}
		}
	}
	for( int i=0; i<100; i++ ){
		if( enemy[i].shoot ){
			enemy[i].y-=15;
			if( enemy[i].y<-50 ) enemy[i].shoot = 0;
		}
	}

	if( rand()%60 == 8 ){
		for( int i=0; i<100; i++ ){
			if( !master[i].shoot ){	
				master[i].x = rand()%551+50;
				master[i].y = 640;
				master[i].shoot = 1;
				break;
			}
		}
	}
	for( int i=0; i<100; i++ ){
		if( master[i].shoot ){
			master[i].y-=10;
			if( master[i].y<-60 ) master[i].shoot = 0;
		}
	}


	for( int i=0; i<100; i++ ){
		if( bullet[i].shoot ){
			bullet[i].y+=15;
			if(bullet[i].y>640 ) bullet[i].shoot = 0;
		}
	}

	for( int i=0; i<100; i++ ) if( enemy[i].shoot ){
		for( int j=0; j<100; j++) if(bullet[j].shoot ){
			if(bullet[j].x<enemy[i].x+48 && enemy[i].x<bullet[j].x+14
				&& bullet[j].y<enemy[i].y+42 && enemy[i].y<bullet[j].y+14)
			{
				enemy[i].shoot=0;
				bullet[j].shoot=0;
			}
		}
	}

	for( int i=0; i<100; i++ ) if( master[i].shoot ){
		for( int j=0; j<100; j++) if(bullet[j].shoot ){
			if(bullet[j].x<master[i].x+59 && master[i].x<bullet[j].x+14
				&& bullet[j].y<master[i].y+43 && master[i].y<bullet[j].y+14)
			{
				master[i].shoot=0;
				bullet[j].shoot=0;
			}
		}
	}
	
	for( int i=0; i<100; i++ ){ 
		if( master[i].shoot ){
			if(player.x<master[i].x+59 && master[i].x<player.x+69
				&& player.y<master[i].y+33 && master[i].y<player.y+35){
				PostQuitMessage(0);
			}
		}
	}

	draw();
}