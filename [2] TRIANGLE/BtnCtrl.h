#ifndef _BTNCTRL
#define _BTNCTRL



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

private:

	// Step Constant for increase or decrease
	float stepCount;

	float X;
	float Y;
	float Z;

};



#endif