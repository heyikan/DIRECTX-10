# DIRECTX-10 BASIC MESHES

* A mesh is a data structure that contains all the **vertex** and **index** buffers needed to draw an object. It’s a neater method of drawing objects as we’ll see.


## Mesh Basics

There are four steps to using meshes:
1.  Create the mesh.
2.  Fill the Mesh with the index and vertex data necessary
3.  Commit the mesh to the device
4.  Draw the mesh

*   So let’s create a new mesh, first things first, we’ll define an __ID3DX10Mesh*__ pointer called pMesh.

```c
f ( FAILED( D3DX10CreateMesh( pD3DDevice, vertexInputLayout, 2, "POSITION", 8, 12, D3DX10_MESH_32_BIT, &pMesh) ) ) return fatalError("Could not create mesh!");

//vertices for a cube
vertex v[8];

v[0] = vertex( D3DXVECTOR3(-1,1,-1), D3DXVECTOR4(1,0,0,1) ); //front top left
v[1] = vertex( D3DXVECTOR3(1,1,-1), D3DXVECTOR4(0,1,0,1) ); //front top right
v[2] = vertex( D3DXVECTOR3(-1,-1,-1), D3DXVECTOR4(0,0,1,1) ); //front bottom left
v[3] = vertex( D3DXVECTOR3(1,-1,-1), D3DXVECTOR4(1,1,0,1) ); //front bottom right
v[4] = vertex( D3DXVECTOR3(-1,1,1), D3DXVECTOR4(1,0,0,1) ); //back top left
v[5] = vertex( D3DXVECTOR3(1,1,1), D3DXVECTOR4(0,1,0,1) ); //back top right
v[6] = vertex( D3DXVECTOR3(-1,-1,1), D3DXVECTOR4(0,0,1,1) ); //back bottom left
v[7] = vertex( D3DXVECTOR3(1,-1,1), D3DXVECTOR4(1,1,0,1) ); //back bottom right

//create indexes for a cube
unsigned int i[36] = {  2,0,3,3,1,0,
                        3,1,7,7,5,1,
                        6,4,2,2,0,4,
                        7,5,6,6,4,5,
                        0,4,1,1,5,4,
                        6,2,7,7,3,2 };

//insert data into mesh and commit changes
pMesh->SetVertexData(0, v);
pMesh->SetIndexData(i, 36);
pMesh->CommitToDevice();
```

* Now we use __D3DX10CreateMesh__ to create the mesh, the parameters are the d3d device, the vertex input layout, number of elements in the vertex layout, the name of the element that stores the vertex position, number of vertices, number of faces, mesh flag and finally the output mesh pointer.

* There is a little trick here, remember how before we had triangle strips? Here we don’t, we have to specify every single triangle (referred to as a face in the mesh) by hand. So our index list looks a bit different from before.

* To add the vertex data and index data, we simply use the mesh SET methods, a mesh can have multiple vertex buffers and so when you set a vertex buffer, you need to specify in which slot you wish to store it. The index data SET method simply takes the index array and the number of indexes in it.

* The final step is to commit the mesh, every time you make a change to a mesh you need to commit it to the device before the changes will be taken into effect.

## Drawing the Mesh

So how do we draw a mesh?

```c
for( UINT p = 0; p < techDesc.Passes; p++ )
{
    //apply technique
    pBasicTechnique->GetPassByIndex( p )->Apply( 0 );
    pMesh->DrawSubset(0);
}
```

* How simple is that? We use the draw subset method of the mesh interface to draw the mesh, the value given to the method specifies with attribute group to draw, attribute groups for meshes will be covered in a later tutorial, for now just specify 0. And that’s how to create and draw a basic mesh.

### output

![depth](.//img//output.PNG)

![depth](.//img//output2.PNG)
