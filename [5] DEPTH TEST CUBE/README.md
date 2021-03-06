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

![depth](.//img//depth1.jpg)

* So as you can see to draw our cube we need to define each vertex and add it to the vertex buffer, then call the draw method 6 times. Each draw call draws a single triangle list with 4 vertices, drawing each face. So in this method we send 24 vertices down the pipeline and use 4 draw calls. This is a little crazy just to draw a single cube with only 8 vertices. There must be a simple more efficient method of doing this and there is: indexing.

## Index Buffers vs Vertex Buffes

>Vertex buffer stores vertex coordinates.
Index buffer stores indices.
If you connect vertices stored in vertex buffer in order defined by index array you'll get proper primitives rendered.

>Bu uygulamanın üçgen çiziminden en önemli farklı vertex buffera ek olarak index buffer kullanılmasıdır. Bilindiği gibi vertex buffer köşe noktalarının koordinatlarını tutuyordu. Index buffer bu köşe noktalarını indisleyerek üçgenleri oluşturur:


![index](.//img//vertexindex.png)

> 6 yüzeye sahip küpün her bir yüzeyi 2 üçgenden oluştuğundan indices[] dizisinde 6x2=12 üçgeni temsil etmek üzere toplam 12x3=36 tane indis vardır. Küpün sadece vertex buffer ile de çizilmesi mümkündür. Yalnız bu durumda 36 tane köşe noktası içeren bir vertex buffer tanımlanmalıdır. Bu ise bellek israfı demektir.


## Index Buffers

What indexing does is let you pass a vertex buffer containing all the key vertices down the pipeline, and also a list of the order the vertices must be drawn in. So for our cube you’ll send the 8 vertices down the pipeline followed by a list of the order to draw them in. It’ll be a bit clearer once you see the code and of course read the index buffer sections in the SDK docs.

```c
//lock vertex buffer for CPU use
pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**) &v );

//vertices for a cube
v[0] = vertex( D3DXVECTOR3(-1,1,-1), D3DXVECTOR4(1,0,0,1) ); //front top left
v[1] = vertex( D3DXVECTOR3(1,1,-1), D3DXVECTOR4(0,1,0,1) ); //front top right
v[2] = vertex( D3DXVECTOR3(-1,-1,-1), D3DXVECTOR4(0,0,1,1) ); //front bottom left
v[3] = vertex( D3DXVECTOR3(1,-1,-1), D3DXVECTOR4(1,1,0,1) ); //front bottom right
v[4] = vertex( D3DXVECTOR3(-1,1,1), D3DXVECTOR4(1,0,0,1) ); //back top left
v[5] = vertex( D3DXVECTOR3(1,1,1), D3DXVECTOR4(0,1,0,1) ); //back top right
v[6] = vertex( D3DXVECTOR3(-1,-1,1), D3DXVECTOR4(0,0,1,1) ); //back bottom left
v[7] = vertex( D3DXVECTOR3(1,-1,1), D3DXVECTOR4(1,1,0,1) ); //back bottom right

pVertexBuffer->Unmap();
```

##### output:

![Just Vertex](.//img//justvertex.PNG)

* so it needs to crete indexes of that cube:

```c

//create indexes for a cube
unsigned int* i = NULL;
pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**) &i );

//front face
i[0] = 2;
i[1] = 0;
i[2] = 3;
i[3] = 1;
i[4] = 0xffffffff; //start new strip

//right face
i[5] = 3;
i[6] = 1;
i[7] = 7;
i[8] = 5;
i[9] = 0xffffffff;

//left face
i[10] = 6;
i[11] = 4;
i[12] = 2;
i[13] = 0;
i[14] = 0xffffffff;

//back face
i[15] = 7;
i[16] = 5;
i[17] = 6;
i[18] = 4;
i[19] = 0xffffffff;

//top face
i[20] = 0;
i[21] = 4;
i[22] = 1;
i[23] = 5;
i[24] = 0xffffffff;

//bottom face
i[25] = 6;
i[26] = 2;
i[27] = 7;
i[28] = 3;

pIndexBuffer->Unmap();

//send vertices down pipeline
for( UINT p = 0; p < techDesc.Passes; p++ )
{
    //apply technique
    pBasicTechnique->GetPassByIndex( p )->Apply( 0 );

    //draw 5 quads - 29 indexes = 4 indexes x 6 faces + 5 breaks
    pD3DDevice->DrawIndexed( 29, 0, 0 );
}

```
* Well not really once you look at it, we add the 8 key vertices to the vertex buffer exactly as before. Then we map the index buffer in exactly the same way as the vertex buffer and start filling it with the indexes of vertices in the vertex buffer. So for the front face we’re telling it, draw the front bottom left vertex, then the front top left vertex, then the front bottom right and finally the front top right. Now whats the 0xffffffff mean? Well that indicates that a new line list or triangle list must be started at that point, It does the same job as calling a draw call for each face without any of the overhead.

* So lets do some basic maths on what we saved by using indexing rather than the standard vertex buffer method. We had a vertex buffer filled with 24 vertices (each vertex weighing in at 224bits) so the vertex buffer 672bytes large. In the second case the vertex buffer is 224bytes, and we have an index buffer with 29 32bit ints (116bytes) so a total of: 340bytes. That’s nearly 50% reduction in memory used, not to mention we are only using a single draw call compared to 4 draw calls when using just a vertex buffer.

* So how do we create the index buffer? Well in DX10 all buffers are the same so we create an index buffer the in the same manner we created a vertex buffer with a few minor changes:

```c


// create index buffer
	D3D10_BUFFER_DESC bd;
	//change buffer desc bytewidth to index type and set bind type to index buffer
	bd.ByteWidth = sizeof(unsigned int)* numOfVertices;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;

	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;


  hr = g_pD3DDevice->CreateBuffer(&bd, NULL, &g_pIndexBuffer);
if (FAILED(hr)) return fatalError("Create Index Buffer Error");

g_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD , 0 , (void**) &i);
.
.
.
.
g_pIndexBuffer->Unmap();

UINT offset = 0;
	g_pD3DDevice->IASetIndexBuffer(g_pIndexBuffer , DXGI_FORMAT_R32_UINT , offset);

```

* We still use the createBuffer method to create the index buffer, we just change the bytewidth since the buffer stores 32bit **unsigned ints.** Once the buffer is created we bind it to the Input Assembly by calling the **IASetIndexBuffer** method ( all we need to specify is a pointer to the buffer, the format of the indexes, and the offset, in case we wish to use only a set portion of the buffer ).

##### output:

![Vertex Index Cube](.//img//vertexindexcube.PNG)

## Depth Testing (Z-buffering)

* In DX10, depth testing is accomplished by making use of a depth stencil, there is a nicely detailed section in the SDK docs regarding the Output-Merger Stage, and here they cover how DX10 accomplishes the depth stencil test internally.

* So lets just briefly go over what depth testing is, we have a **depth buffer** that stores the distance for each pixel in the screen to the camera, so for every pixel we draw from the pixel shader, we compare it’s distance to the camera to the distance stored in the depth buffer, if the new pixel is closer than the distance in the depth buffer then it is drawn and the depth buffer is updated with that pixels distance. That way we only draw the closest visible objects to the viewer, obstruction further objects.

* So lets enable this in DX10:

```c
//dx manager members
ID3D10Texture2D* pDepthStencil;
ID3D10DepthStencilView* pDepthStencilView;

bool dxManager::createRenderTargetsAndDepthBuffer( UINT width, UINT height )
{
    //try to get the back buffer
    ID3D10Texture2D* pBackBuffer;

    if ( FAILED( pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &pBackBuffer) ) ) return fatalError("Could not get back buffer");

    //try to create render target view
    if ( FAILED( pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView) ) ) return fatalError("Could not create render target view");
    pBackBuffer->Release();

    //create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;

    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    if( FAILED( pD3DDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil ) ) ) return fatalError("Could not create depth stencil texture");

    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if( FAILED( pD3DDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &pDepthStencilView ) ) ) return fatalError("Could not create depth stencil view");

    //set render targets
    pD3DDevice->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );

    return true;
}
```
* First we add two new members in the dxmanager class, a ID3D10Texture2D depth stencil pointer and a depth stencil view pointer. Then we create a new texture and assign it to the depth stencil pointer. After this we create a view to the texture by making use of a depth stencil view desc, sort of like the way we created texture views.

* The final step is to modify the Output manager’s render targets to include the depth stencil, this automatically enables depth testing. Once we run the program, we get this result:


![Depth Stencil](.//img//depthstencil.PNG)
