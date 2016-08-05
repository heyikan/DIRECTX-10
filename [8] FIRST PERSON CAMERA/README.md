# DIRECTX-10 Firs Person Camera
DirectX 10 Examples Firs Person Camera

### Output
![FPS](.//img//fpsCam.png)


See Also: [braynzarsoft.net](http://www.braynzarsoft.net/viewtutorial/q16390-18-first-person-camera)

* Here we will make a simple functional first person camera. We will create a function that updates the camera, and we will call that function after we test for input from the user. Of course, it would be much better coding practice to use a camera class, but I try to keep things as explicit as possible, and use functions as often as possible. This lesson builds off the Direct Input lesson, but its "blended" with the render states lesson. Since we have already covered a wireframe grid, I will not explain that stuff in this lesson, only the new code.

* In this lesson, we will be learning a way to impliment a simple first person camera, which will enable us to look up, down, left, and right with the mouse, and move forward and back, and strafe left and right. It's not too difficult, so i'll just get right to the code and explain whats going on as we go.

* Here we initialize a bunch of vectors, floats, a matrix, and a function. The first two floats will be used to change our cameras position in the world. Next, we already know what Position, Target, and Up vectors are. After that we have DefaultForward, DefaultRight, Forward, and Right. The first two vectors will not change. They are the vector pointing to the positive z-axis (forward) and the vector pointing to the positive x-axis (right) of the world space. The other two, Forward and Right, will change. They will change whenever the mouse is moved. We will use these vectors with the Target vector to find the angle at which we should move if the user wants to move forward, backward, left or right. After the vectors, we initialize a matrix which will be used to rotate our Target vector. Then we have yaw and pitch. The yaw variable is a value used to rotate the camera left and right (on the y axis), and the pitch is to rotate the camera up and down (on the x/z-axis). The dirAngleX and dirAngleZ are the angles between the Target and DefaultForward, and Target and DefaultRight. These angles are what we will use to decide how much we should move on the x and z axis when the user wants to move the camera's position. Finally, we declare our UpdateCamera() function.

```c
float moveX = 0;
float moveZ = 0;

D3DXVECTOR3 Position = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
D3DXVECTOR3 Target = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
D3DXVECTOR3 Up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
D3DXVECTOR3 DefaultForward = D3DXVECTOR3(0.0f,0.0f,1.0f);
D3DXVECTOR3 DefaultRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
D3DXVECTOR3 Forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
D3DXVECTOR3 Right = D3DXVECTOR3(1.0f,0.0f,0.0f);

D3DXMATRIX rotationMatrix;

float heading = 0.0f;
float pitch = 0.0f;

void UpdateCamera();
```

* This is our UpdateCamera() function, which is the main thing in this lesson. The first thing you will see in this function, is a new directx function called D3DXMatrixRotationYawPitchRoll(). It has four parameters, The first parameter is the returned matrix, second is the yaw, third is the pitch, and fourth is the roll. This function is very usefull for cameras especially. you are able to rotate around all axis' at the same time, Yaw, Pitch, Roll. Yaw is the rotation left/right (y-axis), pitch is the rotation up/down (x-axis) and Roll is the rotation like your doing a cartwheel (z-axis). Our camera will be looking up/down and left/right, So we will be setting the yaw and pitch parameters of this function. Since we are not using the roll parameter, we set that to zero.

* The next line sets our cameras Target vector by rotating the DefaultForward vector with the rotationMatrix we just created, and setting it to our Target matrix. After that, we normalize our vector, because the last operation may have made one or more of the Target vectors values greater than 1.0f, or less than -1.0f, in which case it would be not of unit length.

```c
void UpdateCamera()
{
    D3DXMatrixRotationYawPitchRoll( &rotationMatrix, yaw, pitch, 0 );
    D3DXVec3TransformCoord( &Target, &DefaultForward, &rotationMatrix );
    D3DXVec3Normalize( &Target, &Target );
```

* Now we need to find the new Right and Forward directions of our camera. We will do this using a rotation matrix which will be rotated on the Y-axis. Since our camera is a first person camera, and we only need to move in the X and Z directions, We need to keep our camera's forward and right vectors pointing only in the x and z axis. We will use those vectors to move our camera back, forward, left and right. First we create a new matrix, then we rotate that matrix using the yaw variable we have set when the mouse moves.

```c
D3DXMATRIX RotateYTempMatrix;
D3DXMatrixRotationY(&RotateYTempMatrix, yaw);
```

* Next we transform the camera's right, up, and forward vectors using the RotateYTempMatrix matrix we just defined, and rotate the default right, up, and default forward vectors then set the result in the right, up, and forward vectors.

```c
D3DXVec3TransformNormal(&Right, &DefaultRight, &RotateYTempMatrix);
D3DXVec3TransformNormal(&Up, &Up, &RotateYTempMatrix);
D3DXVec3TransformNormal(&Forward, &DefaultForward, &RotateYTempMatrix);
```

* Next we update the position of our camera using the two values, moveLeftRight, and moveBackForward, and the two vectors, Right, and Forward. To move the camera left and right, we multiply the moveLeftRight variable with the Right vector, and add that to the Position vector. Then to move back and forward, we multiply moveBackForward with the Forward vector and add that to the Position vector too. The moveLeftRight and moveBackForward values will be calculated when direct input detects a certain key was pressed (in the case of this lesson A,S,D, or W).

```c
Position += moveLeftRight*Right;
Position += moveBackForward*Forward;

moveLeftRight = 0.0f;
moveBackForward = 0.0f;
```
* Now we add the Position of our camera to the target vector, then update our View matrix by using the D3DXMatrixLookAtLH() D3D function. This function updates our View matrix we use to calculate our WVP matrix by using the cameras position, Target, and Up vectors.

```c
Target = Position + Target;    

D3DXMatrixLookAtLH( &View, &Position, &Target, &Up );
```

* Finally we go to our DirectInput() function where we will check if a key was pressed or the mouse was moved and calculate the values to change our cameras position and target. First we need to make a variable called speed, which we can change if we want the camera to move faster or slower, like running or walking. Then we check if one of the A,S,D, or W buttons were pressed, and update the moveLeftRight and moveBackForward variables with the speed variable. We then check to see if our mouse has moved, and if it has, we update the yaw and pitch values, then set the lastmouse state to the current mouse state, so we can tell if the mouse has moved next time. After we check for input and update the variables accordingly, we call our UpdateCamera() function to update our camera! Thats all the new code for this lesson. All the other code in this lesson not covered here was covered in an earlier chapter.

```c

   float speed = 0.005f;

   if(keyboardState[DIK_A] & 0x80)
   {
       moveLeftRight -= speed;
   }
   if(keyboardState[DIK_D] & 0x80)
   {
       moveLeftRight += speed;
   }
   if(keyboardState[DIK_W] & 0x80)
   {
       moveBackForward += speed;
   }
   if(keyboardState[DIK_S] & 0x80)
   {
       moveBackForward -= speed;
   }

   if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
   {
       yaw += mouseLastState.lX * 0.001f;

       pitch += mouseCurrState.lY * 0.001f;

       mouseLastState = mouseCurrState;
   }

   UpdateCamera();
```

* So thats a way to make a simple camera, its not perfect by any means, but it gets the job done! hope you find it usefull!
