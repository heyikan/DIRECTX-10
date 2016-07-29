# DIRECTX-10 - DEPTH TEST
* DirectX 10 DEPTH TEST Examples.
Created with OOP Design.

see also: [takinginitiative](https://takinginitiative.wordpress.com/2008/11/29/directx-10-tutorial-3-textures/)


* Before I get to Depth Testing, let’s draw something a little more complicated that a quad, how about a cube. Using the same method as in tutorial 3 the code to draw a six sided cube is as follows:

```c

//CUBE DRAW METHOD 1
//------------------------------------------------------------
//VERTEX VALUES NOT 100% CORRECT - TOO LAZY TO CORRECT
//lock vertex buffer for CPU use
pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**) &v );

//vertices for a cube
v[0] = vertex( D3DXVECTOR3(-1,-1,-1), D3DXVECTOR4(1,0,0,1) );
v[1] = vertex( D3DXVECTOR3(-1,1,-1), D3DXVECTOR4(0,1,0,1) );
v[2] = vertex( D3DXVECTOR3(1,-1,-1), D3DXVECTOR4(0,0,1,1) );
v[3] = vertex( D3DXVECTOR3(1,1,-1), D3DXVECTOR4(1,1,0,1) );
v[4] = vertex( D3DXVECTOR3(1,-1,-1), D3DXVECTOR4(1,0,0,1) );
v[5] = vertex( D3DXVECTOR3(1,1,-1), D3DXVECTOR4(0,1,0,1) );
v[6] = vertex( D3DXVECTOR3(1,-1,1), D3DXVECTOR4(0,0,1,1) );
v[7] = vertex( D3DXVECTOR3(1,1,1), D3DXVECTOR4(1,1,0,1) );
v[8] = vertex( D3DXVECTOR3(-1,-1,1), D3DXVECTOR4(1,0,0,1) );
v[9] = vertex( D3DXVECTOR3(-1,1,1), D3DXVECTOR4(0,1,0,1) );
v[10] = vertex( D3DXVECTOR3(-1,-1,-1), D3DXVECTOR4(0,0,1,1) );
v[11] = vertex( D3DXVECTOR3(-1,1,-1), D3DXVECTOR4(1,1,0,1) );
v[12] = vertex( D3DXVECTOR3(-1,-1,1), D3DXVECTOR4(1,0,0,1) );
v[13] = vertex( D3DXVECTOR3(-1,1,1), D3DXVECTOR4(0,1,0,1) );
v[14] = vertex( D3DXVECTOR3(1,-1,1), D3DXVECTOR4(0,0,1,1) );
v[15] = vertex( D3DXVECTOR3(1,1,1), D3DXVECTOR4(1,1,0,1) );
v[16] = vertex( D3DXVECTOR3(-1,-1,1), D3DXVECTOR4(1,0,0,1) );
v[17] = vertex( D3DXVECTOR3(-1,-1,-1), D3DXVECTOR4(0,1,0,1) );
v[18] = vertex( D3DXVECTOR3(1,-1,1), D3DXVECTOR4(0,0,1,1) );
v[19] = vertex( D3DXVECTOR3(1,-1,-1), D3DXVECTOR4(1,1,0,1) );
v[20] = vertex( D3DXVECTOR3(-1,1,-1), D3DXVECTOR4(1,0,0,1) );
v[21] = vertex( D3DXVECTOR3(-1,1,1), D3DXVECTOR4(0,1,0,1) );
v[22] = vertex( D3DXVECTOR3(1,1,-1), D3DXVECTOR4(0,0,1,1) );
v[23] = vertex( D3DXVECTOR3(1,1,1), D3DXVECTOR4(1,1,0,1) );

pVertexBuffer->Unmap();

//send vertices down pipeline
for( UINT p = 0; p < techDesc.Passes; p++ )
{
    //apply technique
    pBasicTechnique->GetPassByIndex( p )->Apply( 0 );

    //draw 5 quads
    pD3DDevice->Draw( 4, 0 );
    pD3DDevice->Draw( 4, 4 );
    pD3DDevice->Draw( 4, 8 );
    pD3DDevice->Draw( 4, 12 );
    pD3DDevice->Draw( 4, 16 );
    pD3DDevice->Draw( 4, 20 );
}
```

* The code above produces the following cube (I’ve added rotation and moved the camera – take a look at the code for more details), noticed the depth problem, whatever face got drawn last is on top irrespective of whether it is obscure by another face.

![texture](.//img//depth1.jpg)

* So as you can see to draw our cube we need to define each vertex and add it to the vertex buffer, then call the draw method 6 times. Each draw call draws a single triangle list with 4 vertices, drawing each face. So in this method we send 24 vertices down the pipeline and use 4 draw calls. This is a little crazy just to draw a single cube with only 8 vertices. There must be a simple more efficient method of doing this and there is: indexing.
