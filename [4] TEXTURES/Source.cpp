#include "dxManager.h"
#include "Window.h"

//#pragma comment (lib, "d3d10.lib")

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
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	static int num;

	if (FAILED(wnd.InitWindow(hInstance, nCmdShow, hWnd)))
		dx.fatalError(L"Win Api Creation Failed!");
	 
	if (FAILED(dx.InitDevice(&hWnd)))
	{
		dx.cleanUpDevice();
		dx.fatalError(L"DirectX Initialization Failed!");
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
				//wchar_t  str[256];
				//wsprintf(str, L"It works! - number: %d \n", num );
				//num++;
				//OutputDebugString(str);
			dx.Render();
		}
	}

	dx.cleanUpDevice();

	return (int)msg.wParam;


}