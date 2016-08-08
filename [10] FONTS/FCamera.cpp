#include "FCamera.h"
#include <stdio.h>

fcamera::fcamera()
{
	Position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	Target = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	DefaultForward = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	DefaultRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	Forward = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	Right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	yaw = 0.0f;
	pitch = 0.0f;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;

	moveX = 0; 
	moveZ = 0;

	buttonSensitivity = 0.05f;
	mouseSensitivity = 0.005f;
}

void fcamera::UpdateCamera()
{
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, 0);
	D3DXVec3TransformCoord(&Target, &DefaultForward, &rotationMatrix);
	D3DXVec3Normalize(&Target, &Target);

	D3DXMATRIX RotateYTempMatrix;
	D3DXMatrixRotationY(&RotateYTempMatrix, yaw);

	D3DXVec3TransformNormal(&Right, &DefaultRight, &RotateYTempMatrix);
	D3DXVec3TransformNormal(&Up, &Up, &RotateYTempMatrix);
	D3DXVec3TransformNormal(&Forward, &DefaultForward, &RotateYTempMatrix);

	Position += moveLeftRight*Right;
	Position += moveBackForward*Forward;
	Position += moveUpDown*Up;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveUpDown = 0.0f;

	Target = Position + Target;
	
	

	//char  str[80];
	//sprintf(str, "X: %f Y: %f Y: %f \n", Target.x, Target.y, Target.z);
	//OutputDebugString(str);

	//D3DXMatrixLookAtLH(&View, &Position, &Target, &Up);
	
}

HRESULT fcamera::InitDirectInput(HINSTANCE* hInstance , HWND* hW)
{
	h_Instance = hInstance;
	w_hWND = hW;

	hr = DirectInput8Create(*hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);

	if (FAILED(hr))
		fatalError("Cannot Create Direct Input Device");

	// Binding keyboard and mouse
	DirectInput->CreateDevice(GUID_SysKeyboard , &DIKeyboard , NULL);

	DirectInput->CreateDevice(GUID_SysMouse , &DIMouse , NULL);

	DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	DIKeyboard->SetCooperativeLevel(*w_hWND, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	DIMouse->SetDataFormat(&c_dfDIMouse);
	//DIMouse->SetCooperativeLevel(*w_hWND, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	DIMouse->SetCooperativeLevel(*w_hWND, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	ShowCursor(TRUE);
	return S_OK;
}

/*******************************************************************
* Fatal Error Handler
*******************************************************************/
HRESULT fcamera::fatalError(const LPCSTR msg)
{
	MessageBox(*w_hWND, msg, "Fatal Error!", MB_ICONERROR);
	return E_FAIL;
}


bool fcamera::buttonListener()
{

	bool key = false;



	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(*w_hWND, WM_DESTROY, 0, 0);

	if (keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= buttonSensitivity;
		key = true;
	}
	if (keyboardState[DIK_D] & 0x80)
	{		
		moveLeftRight += buttonSensitivity;
		key = true;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += buttonSensitivity;
		key = true;
	}
	if (keyboardState[DIK_S] & 0x80)
	{	
		moveBackForward -= buttonSensitivity;
		key = true;
	}

	if (keyboardState[DIK_LCONTROL] & 0x80)
	{
		moveUpDown -= buttonSensitivity;
		key = true;
	}

	if (keyboardState[DIK_SPACE] & 0x80)
	{
		moveUpDown += buttonSensitivity;
		key = true;
	}
	//if (keyboardState[DIK_S] & 0x80)
	//{
	//	moveBackForward -= buttonSensitivity;
	//	key = true;
	//}
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		yaw += mouseLastState.lX * 0.001f;

		pitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
		key = true;
	}
	


	if (key){
		UpdateCamera();
		
	}

	return key;


}