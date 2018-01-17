// 201321047_�ּ���_5.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//
#include <stdlib.h>
#include <time.h>
#include "stdafx.h"
#include "201321047_�ּ���_5.h"
#include "egl\egl.h"
#include "vg\openvg.h"
#include "vgLoadImage.h"
#include "DXUTsound.h"

#define MAX_LOADSTRING 100
#define KEY_DOWN(code) (( GetAsyncKeyState(code)&0x80000)?1:0)
// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

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

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY201321047__5, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY201321047__5));

	// �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
//  ����:
//
//    Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
//    �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
//    �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
//    �ش� ���� ���α׷��� �����
//    '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
		// �޴� ������ ���� �м��մϴ�.
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
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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