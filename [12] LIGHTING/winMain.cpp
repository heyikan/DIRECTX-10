#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <vector>
using namespace std;

#include "dxManager.h"
#include "HRTimer.h"
//#include "Window.h"

/*******************************************************************
* Global Variables
*******************************************************************/
HWND hWnd;					//window handle
//int windowWidth = 800;	
//int windowHeight = 600;

//directX manager
dxManager dx;
HRTimer hTimer;
//Window wnd;

void RedirectIOToConsole();
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
/*******************************************************************
* WinMain
*******************************************************************/
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	

	RedirectIOToConsole();
	

	// Set up the application window
	if (!InitWindow( hInstance, nCmdShow, hWnd)) return 0;

	//set up direct x manager
	if (!dx.initialize(&hWnd , &hInstance)) return 0;

	// showing cursor:
	//ShowCursor(TRUE);

	//hTimer.StartTimer();
	// Main message loop
	hTimer.Reset();
    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);			
		}	
		else
		{
			//hTimer.
			hTimer.UpdateTime();
			dx.renderScene(&hTimer);
		}
    }

	return (int) msg.wParam;
}


/*******************************************************************
* Main Window Procedure - handles application events
*******************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:	switch (wParam)
		{

					case 't':	
					{

						if (dx.texturesEnabled) dx.disableTextures();
						else dx.enableTextures();
						dx.texturesEnabled = !dx.texturesEnabled;
					}
					break;

					case 'q':	
					{
						dx.swapTexture();
					}
					break;

		break;
		}
	break;



					// Allow the user to press the escape key to end the application
	case WM_KEYDOWN:	
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
		break;
		}

	break;

	// The user hit the close button, close the application
	case WM_DESTROY:	
		PostQuitMessage(0);
		break;


	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}



/*******************************************************************
* Initialize Main Window
********************************************************************/
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("DXTutorial");
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);




	//Resize the window
	RECT rect = { 0, 0, dx.windowWidth, dx.windowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);



	//create the window from the class defined above	
	hWnd = CreateWindow("DXTutorial",
		"Hakan Kaya's DirectX 10 Tutorial 11 - FPS",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		1000,
		300,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	//window handle not created
	if (!hWnd) return false;

	//if window creation was successful
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return true;
}


// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;


void RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stdout = *fp;

	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stderr = *fp;

	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	ios::sync_with_stdio();
}