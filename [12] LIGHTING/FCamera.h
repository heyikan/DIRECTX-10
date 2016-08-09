#ifndef _FCAMERA
#define	_FCAMERA

#include <d3d10.h>
#include <d3dx10.h>
#include <dinput.h>
#include <windows.h>

class fcamera
{
public:
	fcamera();
	//~fcamera();

	void UpdateCamera();

	HRESULT InitDirectInput(HINSTANCE* , HWND*);

	IDirectInputDevice8*	DIKeyboard;
	IDirectInputDevice8*	DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;


	float moveLeftRight;
	float moveBackForward;
	float moveUpDown;


	float buttonSensitivity;
	float mouseSensitivity;

	float moveX;
	float moveZ;

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Target;
	D3DXVECTOR3 Up;
	D3DXVECTOR3 DefaultForward;
	D3DXVECTOR3 DefaultRight;
	D3DXVECTOR3 Forward;
	D3DXVECTOR3 Right;

	D3DXMATRIX rotationMatrix;

	float yaw = 0.0f;
	float pitch = 0.0f;

	float heading = 0.0f;

	bool buttonListener();

private:


	HRESULT hr;

	//void BtnCtrl::printCoordinate()
	//{
	//	char  str[80];
	//	sprintf(str, "X: %f Y: %f Y: %f rot: %f \n", getX(), getY(), getZ(), getRotate());
	//	OutputDebugString(str);
	//}

	//fatal error handler
	HRESULT fatalError(const LPCSTR msg);
	HINSTANCE* h_Instance;
	HWND* w_hWND;
};



#endif