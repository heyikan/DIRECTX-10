#include "BtnCtrl.h"



BtnCtrl::BtnCtrl()
{
	stepCount = 1.0f;

	X = 0.0f;
	Y = 1.0f;
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