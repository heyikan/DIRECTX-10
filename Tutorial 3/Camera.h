#ifndef _CAMERA
#define _CAMERA

#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.01745329251994329576923690768489

#include "MHRTimer.h"
#include "dxManager.h"

class camera
{

	/*******************************************************************
	* Members
	********************************************************************/
private:
	// view parameters
	float heading, pitch;	//in radians

	// matrices
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX rotationMatrix;

	// view vectors;
	const D3DXVECTOR3 dV, dU;		// default view and up vector
	D3DXVECTOR3 eye, view, up;		

	// movement vectors and movement toggles
	D3DXVECTOR3 forward, strafeRight;
	int movementToggles[4];			 //fwrd, bck, strfLft, strfRght

	// camera timer for movement
	//HRTIMER  camTimer(1000);
	
	
public:
	// constructor and destructor
	camera();
	virtual ~camera();

	// set projection methods
	void setPerspectiveProjection(float FOV , float aspectRatio , float zNear , float zFar);

	// camera positioning methods
	void setPositionAndView(float x , float y , float z , float hDeg , float pDeg);
	void adjustHeadingPitch(float hRad , float pRad);
	void setMovementToggle(int i , int v);

	// update camera view/position
	void updateView();
	
	void update();

	// get methods
	D3DXMATRIX* getViewMatrix(){ return &viewMatrix; }
	D3DXMATRIX* getProjectionMatrix(){ return &projectionMatrix; }

private:
	// create view, forward, strafe vectors from heading/pitch
	void updateView();
};



#endif