#ifndef _FPCAMERA
#define _FPCAMERA

#include"dxManager.h"

class FPCamera
{
public:



	D3DXVECTOR3 m_position;	// camera position
	float m_yaw;	// rotation around the y axis
	float m_pitch;	// rotation around the x axis
	float m_roll;	// rotation around the z axis
	D3DXVECTOR3 m_up, m_look, m_right;	// camera axis

	float RestrictAngleTo360Range(float angle) const;
	float kCameraMovementSpeed;

	FPCamera(D3DXVECTOR3 startPos);
	~FPCamera(void);

	void CalculateViewMatrix(D3DXMATRIX *viewMatrix);


	// Gets
	float GetYaw() const { return m_yaw; }
	float GetPitch() const { return m_pitch; }
	float GetRoll() const { return m_roll; }
	D3DXVECTOR3 GetPosition() const { return m_position; }

	// Move operations
	void MoveForward(){ m_position += m_look*kCameraMovementSpeed; }
	void MoveRight() { m_position += m_right*kCameraMovementSpeed; }
	void MoveUp() { m_position += m_up*kCameraMovementSpeed; }

	// Rotations
	void Yaw(); // rotate around x axis
	void Pitch(); // rotate around x axis
	void Roll(); // rotate around z axis	

	bool buttonListener();

	void printCoordinate();

	// Helper funtion to convert from rads to degrees
	int ToDegrees(float rads)
	{
		return (int)(180.0f / D3DX_PI*rads);
	}
};


#endif