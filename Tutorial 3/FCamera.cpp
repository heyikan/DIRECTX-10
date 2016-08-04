#include "FCamera.h"

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

	speed = 0.005f;
}

void fcamera::UpdateCamera(D3DXMATRIX &View)
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

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	Target = Position + Target;

	D3DXMatrixLookAtLH(&View, &Position, &Target, &Up);
}

bool fcamera::buttonListener()
{
	bool key = false;
	//if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('X') & 0x8000) {
	//	decreaseX();
	//	////setMatrices();
	//	return true;
	//}

	//if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Y') & 0x8000) {
	//	decreaseY();
	//	////setMatrices();
	//	return true;
	//}

	//if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Z') & 0x8000) {
	//	decreaseZ();
	//	////setMatrices();
	//	return true;
	//}

	//if (GetAsyncKeyState('X') & 0x8000) {
	//	increaseX();

	//	return true;
	//	//wchar_t  str[256];
	//	//wsprintf(str, L"It works! - number: %d \n", getZ() );
	//	//OutputDebugString(str);

	//	//setMatrices();

	//}

	//if (GetAsyncKeyState('Y') & 0x8000) {
	//	increaseY();
	//	//setMatrices();

	//	return true;
	//}
	//if (GetAsyncKeyState('Z') & 0x8000) {
	//	increaseZ();
	//	//setMatrices();
	//	return true;
	//}




	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		moveBackForward += speed;
		//increaseZ();
		//setMatrices();

		//return true;
		key = true;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		moveBackForward -= speed;
		//decreaseZ();
		//setMatrices();
		//return true;
		key = true;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		moveLeftRight += speed;
		//increaseRotate();
		//setMatrices();
		//return true;
		key = true;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		moveLeftRight -= speed;
		//decreaseRotate();
		//setMatrices();
		//return true;
		key = true;
	}



	return key;
}