#include "Window.h"


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK  Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
				// Allow the user to press the escape key to end the application
			case WM_KEYDOWN:	
				switch (wParam)
			{
				// Check if the user hit the escape key
				case VK_ESCAPE: PostQuitMessage(0);
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


HRESULT Window::InitWindow(HINSTANCE hInstance, int nCmdShow , HWND& hWnd)
{
	// Register class
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
	wcex.lpszClassName = L"BlueScreen";
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex))
		//return dx.fatalError(L"RegisterClassEx Error When Initialize Window");
		return fatalError(L"RegisterClassEx Error When Initialize Window");

	// Create window

	RECT rc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(L"BlueScreen", L"DIRECTX 10 - TRIANGLE", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!hWnd)
		return fatalError(L"CreateWindow Error When Initialize Window");

	ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	
	return S_OK;
}

