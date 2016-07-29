#ifndef _WINDOW
#define _WINDOW

#include "dxManager.h"



class Window:public dxManager
{

private:

	

public:


	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
	
	static LRESULT CALLBACK  MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK  WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

#endif