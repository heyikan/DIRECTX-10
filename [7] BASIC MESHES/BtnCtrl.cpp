#include "BtnCtrl.h"



BtnCtrl::BtnCtrl()
{
	stepCount = 0.03f;

	X = 0.0f;
	Y = 0.0f;
	Z = -8.0f;

	rotate = 0.0f;
}


void BtnCtrl::increaseX()
{
	X += stepCount;
}
void BtnCtrl::increaseY()
{
	Y += stepCount;
}
void BtnCtrl::increaseZ()
{
	Z += stepCount;
}

void BtnCtrl::decreaseX()
{
	X -= stepCount;
}

void BtnCtrl::decreaseY()
{
	Y -= stepCount;
}

void BtnCtrl::decreaseZ()
{
	Z -=  stepCount ;
}

void BtnCtrl::increaseRotate()
{
	rotate += stepCount;
}

void BtnCtrl::decreaseRotate()
{
	rotate -= stepCount;
}

void BtnCtrl::printCoordinate()
{
	char  str[80];
	sprintf(str, "X: %f Y: %f Y: %f rot: %f \n", getX(), getY(), getZ(), getRotate());
	OutputDebugString(str);
}


bool BtnCtrl::buttonListener()
{



	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('X') & 0x8000) {
		decreaseX();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Y') & 0x8000) {
		decreaseY();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Z') & 0x8000) {
		decreaseZ();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState('X') & 0x8000) {
		increaseX();


		return true;
		//wchar_t  str[256];
		//wsprintf(str, L"It works! - number: %d \n", getZ() );
		//OutputDebugString(str);

		//setMatrices();

	}

	if (GetAsyncKeyState('Y') & 0x8000) {
		increaseY();
		//setMatrices();

		return true;
	}
	if (GetAsyncKeyState('Z') & 0x8000) {
		increaseZ();
		//setMatrices();
		return true;
	}




	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		increaseZ();
		//setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		decreaseZ();
		//setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		increaseRotate();
		//setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		decreaseRotate();
		//setMatrices();
		return true;
	}

	return false;
}