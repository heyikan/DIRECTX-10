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

* So how do we create the index buffer? Well in DX10 all buffers are the same so we create an index buffer the in the same manner we created a vertex buffer with a few minor changes:

```c


	// create index buffer
	D3D10_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(vertex)* numOfVertices;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
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


##### output:

![Vertex Index Cube](.//img//vertexindexcube.PNG)
