#ifndef _WINDOW
#define _WINDOW

#include "dxManager.h"




class Window:public dxManager
{


public:

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
	static LRESULT CALLBACK  WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

#endif