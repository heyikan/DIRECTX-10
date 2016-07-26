#ifndef _WINDOW
#define _WINDOW

#include <windows.h>
#include "dxManager.h"



class Window:public dxManager
{



public:
	Window();
	~Window();

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);

	void setHINSTANCE(HINSTANCE tmp);
	HINSTANCE getHINSTANCE();

private:
	HINSTANCE w_Hinstance;
	HWND* w_HWnd;
};

#endif