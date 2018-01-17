// 201321047_주수강_5.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include <stdlib.h>
#include <time.h>
#include "stdafx.h"
#include "201321047_주수강_5.h"
#include "egl\egl.h"
#include "vg\openvg.h"
#include "vgLoadImage.h"
#include "DXUTsound.h"

#define MAX_LOADSTRING 100
#define KEY_DOWN(code) (( GetAsyncKeyState(code)&0x80000)?1:0)
// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

DWORD lastTime = GetTickCount();
EGLDisplay display;
EGLSurface surface;
EGLContext context;

CSoundManager* sm = NULL;

CSound* backSound =NULL;
CSound* blSound =NULL;
struct Missile{
	int shot,x,y;
}missiles[100];

struct ammo{
	int shot,x,y;
}ammos[100];

struct Enemy{
	int shot,x,y;
}enemy[10];

int fighterY;
int cx=0;
int count;
int heartcount=3;

VGImage backgrond;
VGImage missileImg;
VGImage fighterImg[10];
VGImage ammoImg;
VGImage enemyImg;
VGImage heart;
void TimerProc();
void Draw();

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
		if( curTime-lastTime>32 ) // 30 frame per second!
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
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY201321047__5);
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
		CW_USEDEFAULT, 0, 800, 650, NULL, NULL, hInstance, NULL);

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
	case WM_CREATE:
		{
			display = eglGetDisplay( GetDC(hWnd));
			eglInitialize( display, NULL, NULL );
			eglBindAPI( EGL_OPENVG_API );
			EGLint conf_list[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE };
			EGLConfig config;
			EGLint num_config;
			eglChooseConfig( display, conf_list, &config, 1, &num_config );
			surface = eglCreateWindowSurface( display, config, hWnd, NULL );
			context = eglCreateContext( display, 0, NULL, NULL );
			
			ammoImg=vgLoadImage(TEXT("ammo.png"));
			missileImg=vgLoadImage(TEXT("MISSILE.PNG"));
			fighterImg[0]=vgLoadImage(TEXT("player0.png"));
			fighterImg[1]=vgLoadImage(TEXT("player1.png"));
			fighterImg[2]=vgLoadImage(TEXT("player2.png"));
			fighterImg[3]=vgLoadImage(TEXT("player3.png"));
			fighterImg[4]=vgLoadImage(TEXT("player4.png"));
			fighterImg[5]=vgLoadImage(TEXT("player5.png"));
			fighterImg[6]=vgLoadImage(TEXT("player6.png"));
			fighterImg[7]=vgLoadImage(TEXT("player7.png"));
			fighterImg[8]=vgLoadImage(TEXT("player8.png"));
			fighterImg[9]=vgLoadImage(TEXT("player9.png"));
			enemyImg=vgLoadImage(TEXT("ENEMY.PNG"));
			backgrond=vgLoadImage(TEXT("BACK.PNG"));
			heart=vgLoadImage(TEXT("heart.png"));
			
			sm = new CSoundManager();
		
			sm -> Initialize(hWnd,DSSCL_NORMAL);

		
			sm ->Create(&blSound,TEXT("bl.wav"),0,GUID_NULL);
			sm ->Create(&backSound,TEXT("back.wav"),0,GUID_NULL);

			backSound->Play(0,1);


			fighterY = 320;
			count=0;
			for(int i=0;i<100;i++){
				missiles[i].shot = false;
				ammos[i].shot = false;
			}
			for(int i=0; i<10; i++)
				enemy[i].shot = false;
		}
		break;

	case WM_KEYDOWN:
		if( wParam==VK_SPACE){
			for( int i=0; i<100;i++){
				if(missiles[i].shot==0){
					missiles[i].shot=1;
					missiles[i].x=85;
					missiles[i].y=fighterY+10;
					break;
				}
			}
		}
	
	break;
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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
void Draw()
{
	eglMakeCurrent(display,surface,surface,context);
	float clearColor[4] = {0.5,0.4,0.4,1};//rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1
	vgSetfv(VG_CLEAR_COLOR,4,clearColor);
	vgClear(0,0,1000,1000);
	vgSeti(VG_MATRIX_MODE,VG_MATRIX_IMAGE_USER_TO_SURFACE);

	int k=cx/800;
	vgLoadIdentity();
	vgTranslate(-cx,0);
	vgTranslate(k*800,0);
	vgDrawImage( backgrond);
	vgLoadIdentity();
	vgTranslate(-cx,0);
	vgTranslate((k+1)*800,0);
	vgDrawImage( backgrond);
	for(int i=0;i<100;i++){
		if(missiles[i].shot==true){
			vgLoadIdentity();
			vgTranslate( missiles[i].x, missiles[i].y);
			vgDrawImage(missileImg);
		}
		if(ammos[i].shot==true){
			vgLoadIdentity();
			vgTranslate( ammos[i].x, ammos[i].y);
			vgDrawImage(ammoImg);
		}
	}
	for(int i=0; i<10; i++){
		if(enemy[i].shot ==true) {
			vgLoadIdentity();
			vgTranslate(enemy[i].x, enemy[i].y);
			vgDrawImage(enemyImg);
		}
	}

	vgLoadIdentity(); 
	vgTranslate(30,fighterY);
	vgDrawImage( fighterImg[count]);
	switch(heartcount)
	{
	case 3: vgLoadIdentity();vgTranslate(10,540);vgDrawImage( heart);
			vgLoadIdentity();vgTranslate(50,540);vgDrawImage( heart);
			vgLoadIdentity();vgTranslate(90,540);vgDrawImage( heart);
			break;
	case 2:	vgLoadIdentity();vgTranslate(10,540);vgDrawImage( heart);
			vgLoadIdentity();vgTranslate(50,540);vgDrawImage( heart);
			break;
	case 1: vgLoadIdentity();vgTranslate(10,540);vgDrawImage( heart);
			break;
	case 0: PostQuitMessage(0);break;
	}

	eglSwapBuffers(display,surface);
}
void TimerProc()
{
	unsigned int randomNum = (rand() % 300)+100;
	if(KEY_DOWN(VK_UP) && fighterY<500) fighterY +=10;
	if(KEY_DOWN(VK_DOWN) && fighterY>80) fighterY -=10;
	if(KEY_DOWN(VK_RIGHT)) {
		blSound->Reset();
		blSound->Play(0);
		for( int i=0; i<100;i++){
			if(ammos[i].shot==0){
				ammos[i].shot=1;
				ammos[i].x=85;
				ammos[i].y=fighterY+10;
				break;
			}
		}
	}
	for(int i=0; i<100; i++){
		if(ammos[i].shot){
			ammos[i].x+=10;
			if(ammos[i].x>800  ) ammos[i].shot=0;
		}
		if(missiles[i].shot){
			missiles[i].x+=10;
			if(missiles[i].x>800) missiles[i].shot=0;
		}
	}          

	count++;
	if(count>9) count=0;
		
	if(rand()%30 == 0){
		for(int i=0;i<10;i++){
			if(enemy[i].shot==0){
				enemy[i].shot=true;
				enemy[i].x=1000;
				enemy[i].y=randomNum;
				break;
			}
		}	
	}

	for(int i=0; i<10; i++){
		if(enemy[i].shot==true){
			enemy[i].x-=7;
			if(enemy[i].x<-100) enemy[i].shot=0;
		}
	}
	for(int i=0; i<100;i++){
		for (int j=0;j<10;j++){
			if (ammos[i].shot&& enemy[j].shot){
				int x11=ammos[i].x, x12=ammos[i].x+8;
				int y11=ammos[i].y, y12=ammos[i].y+8;
				int x21=enemy[j].x, x22=enemy[j].x+76;
				int y21=enemy[j].y, y22=enemy[j].y+40;
			
				if(x11<x22&&x21<x12&&y11<y22&&y21<y12)
				{
					ammos[i].shot=0;
					enemy[j].shot=0;
				}
			}
		}
	}
		for(int i=0; i<100;i++){
			for (int j=0;j<10;j++){
			if (missiles[i].shot&& enemy[j].shot){
				int x11=missiles[i].x, x12=missiles[i].x+8;
				int y11=missiles[i].y, y12=missiles[i].y+8;
				int x21=enemy[j].x, x22=enemy[j].x+76;
				int y21=enemy[j].y, y22=enemy[j].y+40;
			
				if(x11<x22&&x21<x12&&y11<y22&&y21<y12)
				{
					missiles[i].shot=0;
					enemy[j].shot=0;
				}
			}
		}
	}
	for (int i=0;i<10;i++){
		if (enemy[i].shot){
			int x11=30, x12=30+76;
			int y11=fighterY, y12= fighterY+40;
			int x21=enemy[i].x, x22=enemy[i].x+76;
			int y21=enemy[i].y, y22=enemy[i].y+40;
			
			if(x11<x22&&x21<x12&&y11<y22&&y21<y12)
			{
				heartcount--;
				enemy[i].shot=0;
			}
		}
	}
	cx+=5;
	Draw();
}