#include "Window.h"


Window::Window()
{
	w_Hinstance = NULL;
	w_HWnd = NULL;

}

Window::~Window()
{

}



//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
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

// Setters & Getters
void Window::setHINSTANCE(HINSTANCE tmp)
{
	w_Hinstance = tmp;
}

HINSTANCE Window::getHINSTANCE()
{
	return w_Hinstance;
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


	//HINSTANCE tmpIns = NULL;
	//HWND      tmpHwnd = NULL;
	// Create window
	w_Hinstance= hInstance;
	RECT rc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(L"BlueScreen", L"DIRECTX 10 - BLUESCREEN", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!hWnd)
		return fatalError(L"CreateWindow Error When Initialize Window");

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return S_OK;
}

