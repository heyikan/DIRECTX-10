#ifndef _FCAMERA
#define	_FCAMERA

#include <d3d10.h>
#include <d3dx10.h>

class fcamera
{
public:
	fcamera();
	~fcamera();

	void UpdateCamera(D3DXMATRIX &View);


	float moveLeftRight;
	float moveBackForward;
	float speed;

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

	bool buttonListener();

private:


	//void BtnCtrl::printCoordinate()
	//{
	//	char  str[80];
	//	sprintf(str, "X: %f Y: %f Y: %f rot: %f \n", getX(), getY(), getZ(), getRotate());
	//	OutputDebugString(str);
	//}


};



#endif