# DIRECTX 10 LOADING 3D MODELS

![FPS](.//img//3dModels.png)

See also: [braynzarsoft](http://www.braynzarsoft.net/viewtutorial/q16390-17-loading-3d-models)

* As you can see at the beginning of our code, we have included 3 new files into our project. The first two will be used to load the 3d model from a file, and the third one is so we can use vectors, which are basically dynamic arrays, meaning we do not have to declare the size when we first create them, and we can add things and take things out, whereas in an array, we must define the exact size when first making it, and that size can never be changed after we make it. You should already know this, but i thought i'd just mention it since it took me a long time to learn about vectors for some reason.

```c
#include <fstream>
#include <istream>
#include <vector>
```
* First we are creating a vector of ID3DX10Mesh objects to hold our meshes. Vectors will be usefull here if you want to load more than one model this way, and don't know how many. Then we make a variable to take count of our meshes that we loaded. The next line is declaring our new function, called LoadMesh(). It takes one argument, and that is the filename we will be loading the 3d model from. After that we have another vector to hold the texture (subset) count for each mesh, and then another vector to hold our texture resource views

```c
std::vector<ID3DX10Mesh*>		meshes;
int meshCount;
std::vector<UINT> meshSubset;
int meshTextures = 0;

```

* Lets now go down to our new function, the __load3DMaxMesh__() function, where we will be loading the 3d model from our file to an object created from the ID3DX10Mesh interface.

#### HRESULT dxManager::load3DMaxMesh(std::wstring filename)
```c
ID3DX10Mesh*	g_tempMesh;

	wifstream fileIn(filename.c_str());
	wstring skipString;

	UINT meshVertices = 0;
	UINT meshTriangles = 0;
	// hold the texture(subset) count for each mesh
	UINT tempMeshSubsets = 0;

	if (fileIn)
	{
		fileIn >> skipString; // #Subsets
		fileIn >> tempMeshSubsets;
		fileIn >> skipString; // #Vertices
		fileIn >> meshVertices;
		fileIn >> skipString; // #Faces (Triangle)
		fileIn >> meshTriangles;

		meshSubset.push_back(tempMeshSubsets);

		//hr = D3DX10CreateMesh(g_pD3DDevice, vertexInputLayout, 3, vertexInputLayout[0].SemanticName, meshVertices, meshTriangles, D3DX10_MESH_32_BIT, &g_pMesh);

		//if (FAILED(hr))
		//	return fatalError("Mesh Creation From File - Failed");


		hr = D3DX10CreateMesh(g_pD3DDevice, vertexInputLayout, numOfInputLayoutElements, "POSITION", meshVertices, meshTriangles, D3DX10_MESH_32_BIT, &g_pMesh);
		if (FAILED(hr))
			return fatalError("Could not create the Mesh!");

		vertex* v = new vertex[meshVertices];
		vertex tmpVertex;
		float x , y, z, w;


		D3DXVECTOR3 POSITION;
		D3DXVECTOR4 COLOR;
		D3DXVECTOR2 TEXTCOOD;
		D3DXVECTOR3 NORM;

		fileIn >> skipString; //#Vertex_Info
		for (UINT i = 0; i < meshVertices; ++i)
		{
			fileIn >> skipString; //Pos:
			fileIn >> x;
			fileIn >> y;
			fileIn >> z;

			POSITION = D3DXVECTOR3(x, y, z);

			//printf("[%d] - (%f , %f , %f) " ,i,x,y,z);

			// Color -- Default
			COLOR = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

			//printf("(%f , %f , %f , %f) ", 1.0f, 0.0f, 0.0f , 1.0f);

			fileIn >> skipString; //Norm:
			fileIn >> x;
			fileIn >> y;
			fileIn >> z;

			NORM = D3DXVECTOR3(x,y,z);

			//printf("(%f , %f , %f) ", x, y, z);

			fileIn >> skipString; // TextCoord;
			fileIn >> x;
			fileIn >> y;



			TEXTCOOD = D3DXVECTOR2(x ,y);

			//printf("(%f , %f ) \n", x, y);

			v[i] = vertex(POSITION, COLOR, TEXTCOOD, NORM);

		}
		g_pMesh->SetVertexData(0, v);

		delete[]v;

		DWORD* indices = new DWORD[meshTriangles * 3];
		UINT* attributeIndex = new UINT[meshTriangles];

		fileIn >> skipString;    //#Face_Index
		for (UINT i = 0; i < meshTriangles; ++i)
		{
			fileIn >> indices[i * 3 + 0];
			fileIn >> indices[i * 3 + 1];
			fileIn >> indices[i * 3 + 2];

			fileIn >> attributeIndex[i]; //Current Subset
		}
		g_pMesh->SetIndexData(indices, meshTriangles * 3);
		g_pMesh->SetAttributeData(attributeIndex);

		delete[] indices;
		delete[] attributeIndex;

		//g_tempMesh->GenerateAdjacencyAndPointReps(0.001f);
		//g_tempMesh->Optimize(D3DX10_MESHOPT_ATTR_SORT | D3DX10_MESHOPT_VERTEX_CACHE, 0, 0);
		g_pMesh->CommitToDevice();

		meshCount++;
		meshes.push_back(g_tempMesh);
	}
	else
	{
		return fatalError("Load Mesh File - Failed");
	}

	return hr;
```

* After we have the necessary information, we are able to create our mesh, and store it into our tempMesh. We can use the HRESULT hr here to make sure the mesh was created, if not, give error and return false out of the function. To create a mesh, we use the D3DX10CreateMesh() function, which has 8 parameters, here is the function:


```c
HRESULT D3DX10CreateMesh(
  __in   ID3D10Device *pDevice,
  __in   const D3D10_INPUT_ELEMENT_DESC *pDeclaration,
  __in   UINT DeclCount,
  __in   LPCSTR pPositionSemantic,
  __in   UINT VertexCount,
  __in   UINT FaceCount,
  __in   UINT Options,
  __out  ID3DX10Mesh **ppMesh
);
```
__pDevice__ - Pointer to the D3D device which we will create the texture with (d3dDevice).

__pDeclaration__ - This is a pointer to the first element in our vertex layout (D3D10_INPUT_ELEMENT_DESC array) we created.

__DeclCount__ - Number of elements in our vertex layout array.

__pPositionSemantic__ - This is the "POSITION" or element describing our vertex position in our vertex layout.

__VertexCount__ - numVertices, or number of vertices in the 3d model.

__FaceCount__ - numTriangles, or number of triangles (a.k.a. faces) in the 3d model.

__Options__ - This can be zero, one or both of the D3DX10_MESH_32_BIT or D3DX10_MESH_GS_ADJACENCY flags. The 32 bit flag tells the mesh to use 32 bits in the index, otherwise it uses 16 bits. The other flag says the mesh has adjacency information, which we will then have to use D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ to render our triangles, otherwise we use D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST to render our triangles.

__ppMesh__ - Returns our created mesh.
