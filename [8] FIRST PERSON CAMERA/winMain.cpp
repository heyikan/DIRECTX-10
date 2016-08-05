#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include <vector>
using namespace std;

#include "dxManager.h"
//#include "Window.h"

/*******************************************************************
* Global Variables
*******************************************************************/
HWND hWnd;					//window handle
//int windowWidth = 800;	
//int windowHeight = 600;

//directX manager
dxManager dx;
//Window wnd;


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
/*******************************************************************
* WinMain
*******************************************************************/
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	
	// Set up the application window
	if (!InitWindow( hInstance, nCmdShow, hWnd)) return 0;
	
	//set up directx manager
	if (!dx.initialize(&hWnd , &hInstance)) return 0;
	
	// Main message loop
    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);			
		}	
	
		dx.renderScene();
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

		case 't':	{

						if (dx.texturesEnabled) dx.disableTextures();
						else dx.enableTextures();

						dx.texturesEnabled = !dx.texturesEnabled;
					}
					break;

		case 'q':	{
						dx.swapTexture();
					}
					break;

		//case 'x':	{
		//				dx.increaseX();
		//				dx.setMatrices();
		//}
		//			break;

		//case 'y':	{
		//				dx.increaseY();
		//				dx.setMatrices();
		//}
		//			break;

		//case 'z':	{
		//				dx.increaseZ();
		//				dx.setMatrices();
		//}
					break;
		}
	break;

					// Allow the user to press the escape key to end the application
	case WM_KEYDOWN:	switch (wParam)
	{
	case VK_ESCAPE: PostQuitMessage(0);
		break;
	}

						break;

						// The user hit the close button, close the application
	case WM_DESTROY:	PostQuitMessage(0);
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
		"Bobby Anguelov's DirectX 10 Tutorial 3 - Basic Texturing",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
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