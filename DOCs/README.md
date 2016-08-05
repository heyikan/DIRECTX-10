# Direcx 10 - Camera Example

![yaw](.//img//yawpitchroll.jpg))

* __yaw:__ rotation around the y axis
* __roll:__ rotation around the z axis
* __pitch:__ rotation aroun the x axis

* The view matrix is used to position and orientate the camera in world space
* The view matrix is made up of Up, Look and Right vectors and a Position element
* These vectors form an orthogonal camera axis, by rotating these vectors we can change the axis orientation in the world
* We rotate the axis by creating rotation matrix and transforming axis vectors using it
* We call rotation of the up vector (y) Yaw
* We call rotation of the look vector (z) Roll
* We call rotation of the right vector (x) Pitch.

## Direct3D View Matrix Calculation

* For this method we keep up, look and right vectors as members of our camera class. When we come to create our view matrix though we reset these vectors to their start values. This means we need to store accumulated values for yaw, pitch and roll (as opposed to changes). Therefore our camera class may have the following member data:

```c
D3DXVECTOR3 position;      // camera position
float yaw;                 // rotation around the y axis
float pitch;               // rotation around the x axis
float roll;                // rotation around the z axis
D3DXVECTOR3 up,look,right; // camera axis
```

* Each time we calculate our view matrix we set our axis back to the initial values of up pointing up y, look pointing down z and right pointing down x:

## Yaw rotation

* To be able to apply our Yaw rotation (rotation around the up axis)  we need to create a matrix capable of carrying out this rotation. We do this by calculating the matrix as a rotation around the up axis

```c
D3DXMATRIX yawMatrix;
D3DXMatrixRotationAxis(&yawMatrix, &up, yaw);
```

The __D3DXMatrixRotationAxis__ function is a really useful D3DX function that takes a vector and an angle and produces a matrix that will provide that requested rotation.

##### Note: All Direct3D calls that take an angle require the angle to be in radians not degrees. To convert:

```c
#define DEGTORAD(degree) ((D3DX_PI / 180.0f) * (degree)) // converts from degrees to radians
#define RADTODEG(radian) ((180.0f / D3DX_PI) * (radian)) // converts from radians to degrees
```
```c
D3DXMatrixRotationY(&matWorld,DEGTORAD(180))
```

To apply yaw we rotate the look & right vectors about the up vector using our matrix:

```c
D3DXVec3TransformCoord(&look, &look, &yawMatrix);
D3DXVec3TransformCoord(&right, &right, &yawMatrix);
```
```c
D3DXVECTOR3* D3DXVec3TransformCoord(
  _Inout_       D3DXVECTOR3 *pOut,
  _In_    const D3DXVECTOR3 *pV,
  _In_    const D3DXMATRIX  *pM
);

```

![yaw](.//img//uprightlook.jpg))


## Pitch rotation

* For pitch we create a matrix capable of rotating around the right axis:

```c
D3DXMATRIX pitchMatrix;
D3DXMatrixRotationAxis(&pitchMatrix, &right, pitch);
```
* We apply our pitch matrix by rotating the look and up vector around the right vector using our matrix:

```c
D3DXVec3TransformCoord(&look, &look, &pitchMatrix);
D3DXVec3TransformCoord(&up, &up, &pitchMatrix);
```

## Roll rotation

For roll we create a matrix capable of rotating around the look axis

```c
D3DXMATRIX rollMatrix;
D3DXMatrixRotationAxis(&rollMatrix, look, roll);
```

* We apply our roll matrix by rotating the up and right vectors around the look vector using our matrix:

```c
D3DXVec3TransformCoord(&m_right, &m_right, &rollMatrix);
D3DXVec3TransformCoord(&m_up, &m_up, &rollMatrix);
```

## Filling the view matrix

* Once we have created our rotation matrices and applied them to our axis vectors we are ready to fill in the view matrix. As well as rotation we need to fill in the position of the camera.

```c
D3DXMATRIX viewMatrix;
D3DXMatrixIdentity(&viewMatrix)
```

```c
viewMatrix._11 = right.x; viewMatrix._12 = up.x; viewMatrix._13 = look.x;
viewMatrix._21 = right.y; viewMatrix._22 = up.y; viewMatrix._23 = look.y;
viewMatrix._31 = right.z; viewMatrix._32 = up.z; viewMatrix._33 = look.z;

viewMatrix._41 = - D3DXVec3Dot( &position, &right );
viewMatrix._42 = - D3DXVec3Dot( &position, &up );
viewMatrix._43 = - D3DXVec3Dot( &position, &look );
```

* This can be difficult to visualise so try drawing these things or even better try creating some axis out of paper and manually seeing how these rotations work.

* Finally we have to actually set the view matrix:

```c
gD3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );
```

## Camera rotation

* To rotate our camera we can now simply provide functions in our camera to add to our rotation variables e.g.

```c
void Yaw(float amount)
{
   yaw+=amount;
}
```

* Since with the described method I am storing accumulated angles you may also want to check for the angle going over 360 degrees (2*PI radians) or below 0.

## Camera movement

* With the above system we always have up, right and look vectors and position stored as member variables of the class. This makes it very easy to move the camera in the correct direction e.g.

```c
position+=look*movementAmount;    // Move forward
position+=look*-movementAmount;    // Move backward
position+=right*movementAmount;   // Move right (strafe)
position+=right*-movementAmount;   // Move left
position+=up*movementAmount;      // Move up
position+=up*-movementAmount;      // Move down
```

Where __movementAmount__ is a floating point scalar value determining how far we should move.

__Note:__  if you implemented your camera without holding the camera axis you could use the view matrix instead e.g. to move forward you would add to the position the result of multiplying the view matrix and a vector 0,0,z,0, where z is the speed to move.

#### see also: [toymaker.info](http://www.toymaker.info/Games/html/camera.html)
