#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "dxManager.h"
#include "Window.h"
#pragma comment (lib, "d3d10.lib")

/*******************************************************************
* Global Variables
*******************************************************************/
HWND hWnd;					//window handle


// class declarations
dxManager dx;
Window wnd;


/*******************************************************************
* WinMain
*******************************************************************/
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	
	// Set up the application window
	if (FAILED(wnd.InitWindow(hInstance, nCmdShow , hWnd))) dx.fatalError(L"Win Api Creation Failed!");



	// Ýnitialize DirectX 10
	if (FAILED(dx.InitDevice(&hWnd))) {
		dx.cleanUpDevice();
		dx.fatalError(L"DirectX Initialization Failed!");
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		dx.Render();
	}

	dx.cleanUpDevice();

	return (int)msg.wParam;


}