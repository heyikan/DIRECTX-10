#include "Window.h"


//static Window *wnd;




LRESULT CALLBACK  Window::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	Window *c = (Window *)GetWindowLong(hWnd, GWLP_USERDATA);

	if (c == NULL)
		return DefWindowProc(hWnd, msg, wParam, lParam);

	return c->WndProc(hWnd, msg, wParam, lParam);
}



/*******************************************************************
* Main Window Procedure - handles application events
*******************************************************************/
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:	switch (wParam)
	{
	case 't':	{

					if (texturesEnabled) disableTextures();
					else enableTextures();

					texturesEnabled = !texturesEnabled;
	}
						break;
	case 's':	{
					swapTexture();
					//dx.createRectangle();
	}
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
HRESULT Window::InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &Window::MessageRouter;
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
	RECT rect = { 0, 0, windowWidth, windowHeight};
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