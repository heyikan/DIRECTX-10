#include "BtnCtrl.h"



BtnCtrl::BtnCtrl()
{
	stepCount = 0.003f;

	X = 0.0f;
	Y = 0.0f;
	Z = -8.0f;
}


void BtnCtrl::increaseX()
{
	X = stepCount + X;
}
void BtnCtrl::increaseY()
{
	Y = stepCount + Y;
}
void BtnCtrl::increaseZ()
{
	Z = stepCount + Z;
}

void BtnCtrl::decreaseX()
{
	X = stepCount - X;
}

void BtnCtrl::decreaseY()
{
	Y = stepCount - Y;
}

void BtnCtrl::decreaseZ()
{
	Z = stepCount - Z;
}

bool BtnCtrl::buttonListener()
{
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


	return false;
}