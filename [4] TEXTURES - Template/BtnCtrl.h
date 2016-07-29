#ifndef _BTNCTRL
#define _BTNCTRL

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>


class BtnCtrl
{
public:

	BtnCtrl();


	void increaseX();
	void increaseY();
	void increaseZ();


	void decreaseX();
	void decreaseY();
	void decreaseZ();


	float getX(){ return X; }
	float getY(){ return Y; }
	float getZ(){ return Z; }

	bool buttonListener();

private:

	// Step Constant for increase or decrease
	float stepCount;

	float X;
	float Y;
	float Z;

};



#endif