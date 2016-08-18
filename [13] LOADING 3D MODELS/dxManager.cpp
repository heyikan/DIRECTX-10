#include "dxManager.h"
#include <string>
using namespace std;




/*******************************************************************
* Constructor
*******************************************************************/
dxManager::dxManager() {

	// window handle
	w_hWnd = NULL;

	g_pD3DDevice = NULL;
	g_pSwapChain = NULL;
	g_pRenderTargetView = NULL;

	textureIndex = 0;

	g_pVertexBuffer = NULL;
	g_pVertexLayout = NULL;
	g_pBasicEffect = NULL;
	g_pRS = NULL;

	windowHeight = 600;
	windowWidth = 800;

	texturesEnabled = true;

	D3DXMatrixIdentity(&g_WorldMatrix);



}
/*******************************************************************
* Destructor
*******************************************************************/
dxManager::~dxManager()
{
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pRS) g_pRS->Release();
	if (g_pBasicEffect) g_pBasicEffect->Release();

	//free textures
	for (unsigned int i = 0; i < g_textureSRV.size(); i++)
	{
		g_textureSRV[i]->Release();
	}

	//for (int i = 0; i < meshCount; i++)
	//if (meshes[i])
	//	meshes[i]->Release();
}
/*******************************************************************
* Initializes Direct3D Device
*******************************************************************/
bool dxManager::initialize(HWND* hW, HINSTANCE* hIns)
{
	//window handle
	w_hWnd = hW;
	w_hIns = hIns;
	//get window dimensions
	RECT rc;
	GetClientRect(*w_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	viewPortHeight = height;
	viewPortWidth = width;

	// CREATE DIRECT INPUT
	if (FAILED((Cam.InitDirectInput(w_hIns, w_hWnd))))
		return false;



	// CREATE DEVICE
	//*****************************************************************************
	if (FAILED(createSwapChainAndDevice()))
		return false;


	// CREATE FONT
	if (FAILED(createFont()))
		return false;


	// INPUT ASSEMBLY STAGE
	//*****************************************************************************
	if (FAILED(loadShadersAndCreateInputLayouts()))
		return false;

	// RASTERIZER STAGE SETUP
	//*****************************************************************************
	createViewports();
	initRasterizerState();

	// OUTPUT-MERGER STAGE
	//*****************************************************************************
	if (FAILED(createRenderTargetsAndDepthBuffer()))
		return false;

	// LOAD TEXTURES
	//*****************************************************************************
	if (FAILED(loadTextures()))
		return false;


	// SET UP SCENE VARIABLES
	//*****************************************************************************
	createLights();

	//if (FAILED(createMeshCube()))
	//	return false;

	if (FAILED(load3DMaxMesh(L"sphere.dat")))
		return false;



	//if (FAILED(load3DMaxMesh(L"cube.dat")))
	//	return false;

	// Set up the view and projection matrices
	//*****************************************************************************
	if (FAILED(setMatrices()))
		return false;

	//everything completed successfully
	return true;
}


/*******************************************************************
* Create Swap Chain and D3D device
*******************************************************************/
HRESULT dxManager::createSwapChainAndDevice()
{
	//Set up DX swap chain
	//--------------------------------------------------------------
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set buffer dimensions and format
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = viewPortWidth;
	swapChainDesc.BufferDesc.Height = viewPortHeight;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;;

	//set refresh rate
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	//sampling settings
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;

	//output window handle
	swapChainDesc.OutputWindow = *w_hWnd;
	swapChainDesc.Windowed = true;

	// Open DEBUG MODE
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif


	// Open Hardware Mode:

	IDXGIFactory* pFactory = NULL;
	IDXGIAdapter* pAdapter = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

	pFactory->EnumAdapters(0, &pAdapter);


	//Create the D3D device
	//--------------------------------------------------------------
	hr = D3D10CreateDeviceAndSwapChain(pAdapter,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		D3D10_SDK_VERSION,
		&swapChainDesc,
		&g_pSwapChain,
		&g_pD3DDevice);

	if (FAILED(hr)) return fatalError("D3D device creation failed");


	return S_OK;
}
HRESULT dxManager::createFont()
{
	D3DX10_FONT_DESC fd;
	fd.Height = 17;
	fd.Width = 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = OUT_TT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy_s(fd.FaceName, "Cambria");
	hr = D3DX10CreateFontIndirect(g_pD3DDevice, &fd, &Font);

	if (FAILED(hr))
		fatalError("Font Error!!");

	//D3DX10CreateFont(g_pD3DDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &Font);
	hr = D3DX10CreateSprite(g_pD3DDevice, 256, &ppSprite);
	if (FAILED(hr))
		fatalError("Font Sprite Error!!");
	return S_OK;
}

/*******************************************************************
* Shader Loader
*******************************************************************/
HRESULT dxManager::loadShadersAndCreateInputLayouts()
{

	hr = D3DX10CreateEffectFromFile("basicEffect.fx",
		NULL, NULL,
		"fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		g_pD3DDevice,
		NULL,
		NULL,
		&g_pBasicEffect,
		NULL,
		NULL);

	if (FAILED(hr)) return fatalError("Could not load effect file!");

	g_pBasicTechnique = g_pBasicEffect->GetTechniqueByName("full");
	if (g_pBasicTechnique == NULL) return fatalError("Could not find specified technique!");

	g_pBasicTechniqueLight = g_pBasicEffect->GetTechniqueByName("full");
	if (g_pBasicTechnique == NULL) return fatalError("Could not find specified technique!");



	//create matrix effect pointers
	g_pViewMatrixEffectVariable = g_pBasicEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionMatrixEffectVariable = g_pBasicEffect->GetVariableByName("Projection")->AsMatrix();
	g_pWorldMatrixEffectVariable = g_pBasicEffect->GetVariableByName("World")->AsMatrix();

	g_pWorldInverseTransposeVariable = g_pBasicEffect->GetVariableByName("g_WorldInverseTranspose")->AsMatrix();
	g_pLightPosVariable = g_pBasicEffect->GetVariableByName("vLightPos")->AsVector();
	g_pLightColorVariable = g_pBasicEffect->GetVariableByName("vLightColor")->AsVector();
	g_pOutputColorVariable = g_pBasicEffect->GetVariableByName("vOutputColor")->AsVector();


	g_pTextureSR = g_pBasicEffect->GetVariableByName("tex2D")->AsShaderResource();

	numOfInputLayoutElements = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);

	//create input layout
	D3D10_PASS_DESC PassDesc;
	g_pBasicTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

	hr = g_pD3DDevice->CreateInputLayout(vertexInputLayout,
		numOfInputLayoutElements,
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pVertexLayout);

	if (FAILED(hr))  return fatalError("Could not create Input Layout!");

	// Set the input layout
	g_pD3DDevice->IASetInputLayout(g_pVertexLayout);


	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pBasicTechnique->GetDesc(&techDesc);
	return S_OK;
}
/*******************************************************************
* Set up Viewports
*******************************************************************/
void dxManager::createViewports()
{
	//create viewport structure	
	g_viewPort.Width = viewPortWidth;
	g_viewPort.Height = viewPortHeight;
	g_viewPort.MinDepth = 0.0f;
	g_viewPort.MaxDepth = 1.0f;
	g_viewPort.TopLeftX = 0;
	g_viewPort.TopLeftY = 0;

	g_pD3DDevice->RSSetViewports(1, &g_viewPort);
}
/*******************************************************************
* Initialize Rasterizer State
*******************************************************************/
void dxManager::initRasterizerState()
{
	//set rasterizer	
	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_NONE;
	rasterizerState.FillMode = D3D10_FILL_SOLID;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	ID3D10RasterizerState*		g_pRS;
	g_pD3DDevice->CreateRasterizerState(&rasterizerState, &g_pRS);
	g_pD3DDevice->RSSetState(g_pRS);
	g_pRS->Release();
}
/*******************************************************************
* Create Rendering Targets
*******************************************************************/
HRESULT dxManager::createRenderTargetsAndDepthBuffer()
{
	//try to get the back buffer
	ID3D10Texture2D* pBackBuffer;

	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr)) return fatalError("Could not get back buffer");

	//try to create render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	if (FAILED(hr))return fatalError("Could not create render target view");

	pBackBuffer->Release();



	// create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = windowWidth;
	descDepth.Height = windowHeight;

	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;

	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;

	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = g_pD3DDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr)) return fatalError("Depth Stencil View Error!");

	// create depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = g_pD3DDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr)) return fatalError("Creating Depth Stencil View Error");

	g_pD3DDevice->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	return S_OK;
}
/*******************************************************************
* Texture Loader
*******************************************************************/
HRESULT dxManager::loadTextures()
{
	vector<string> filenames;
	filenames.push_back("textures/t1.bmp");
	filenames.push_back("textures/t2.bmp");
	filenames.push_back("textures/t3.bmp");

	//load textures
	for (int i = 0; i < (int)filenames.size(); i++)
	{
		g_textureSRV.push_back(NULL);
		hr = D3DX10CreateShaderResourceViewFromFile(g_pD3DDevice, filenames[i].c_str(), NULL, NULL, &g_textureSRV[i], NULL);
		if (FAILED(hr))
		{
			char err[255];
			sprintf_s(err, "Could not load texture: %s!", filenames[i].c_str());
			return fatalError(err);
		}
	}

	return S_OK;
}




void dxManager::createLights()
{
	//D3DXVECTOR4 vLightPos = D3DXVECTOR4(0.0f, 0.0f, 21.0f, 1.0f);
	vLightPos = D3DXVECTOR4(3.0f, 12.0f, 21.0f, 1.0f);
	vLightColors = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	g_pLightPosVariable->SetFloatVectorArray((float*)vLightPos, 0, 2);
	g_pLightColorVariable->SetFloatVectorArray((float*)vLightColors, 0, 2);


}

HRESULT dxManager::load3DMaxMesh(std::wstring filename)
{
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

		//g_pMesh->GenerateAdjacencyAndPointReps(0.001f);
		g_pMesh->Optimize(D3DX10_MESHOPT_ATTR_SORT | D3DX10_MESHOPT_VERTEX_CACHE, 0, 0);
		g_pMesh->CommitToDevice();

		meshCount++;
		meshes.push_back(g_tempMesh);
	}
	else
	{
		return fatalError("Load Mesh File - Failed");
	}

	return hr;
}


HRESULT dxManager::createMeshCube()
{

	// init vertices
	vertex* v;
	v = new vertex[24];

	// Top
	v[0] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	v[1] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	v[2] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	v[3] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// Bottom
	v[4] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	v[5] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	v[6] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	v[7] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f));

	// Left
	v[8] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	v[9] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	v[10] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
	v[11] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f));

	// Right
	v[12] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	v[13] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	v[14] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	v[15] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f));

	// Front Face
	v[16] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	v[17] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	v[18] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	v[19] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f));

	// Back Face
	v[20] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	v[21] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	v[22] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	v[23] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));


	unsigned int i[36] = {
		3, 1, 0, 2, 1, 3,
		6, 4, 5, 7, 4, 6,
		11, 9, 8, 10, 9, 11,
		14, 12, 13, 15, 12, 14,
		19, 17, 16, 18, 17, 19,
		22, 20, 21, 23, 20, 22,
	};

	hr = D3DX10CreateMesh(g_pD3DDevice, vertexInputLayout, numOfInputLayoutElements, "POSITION", 24, 6, D3DX10_MESH_32_BIT, &g_pMesh);
	if (FAILED(hr))
		return fatalError("Could not create the Mesh!");

	g_pMesh->SetVertexData(0, v);
	g_pMesh->SetIndexData(i, 36);
	g_pMesh->CommitToDevice();

	return S_OK;
}


HRESULT	dxManager::setMatrices()
{

	// Set up matrix
	RECT rct;
	GetClientRect(*w_hWnd, &rct);
	//D3DXMATRIX matProj;

	float aspect = (rct.right - rct.left) / (float)(rct.bottom - rct.top);

	D3DXMatrixIdentity(&g_WorldMatrix);

	D3DXMatrixLookAtLH(&g_viewMatrix, &Cam.Position, &Cam.Target, &Cam.Up);

	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, D3DX_PI / 4, aspect, 1.0f, 100.0f);



	g_pWorldMatrixEffectVariable->SetMatrix((float*)&g_WorldMatrix);
	g_pViewMatrixEffectVariable->SetMatrix((float*)&g_viewMatrix);
	g_pProjectionMatrixEffectVariable->SetMatrix((float*)&g_projectionMatrix);



	D3DXMATRIX                  g_WorldInverseTranspose;

	D3DXMatrixInverse(&g_WorldInverseTranspose, 0, &g_WorldMatrix);
	D3DXMatrixTranspose(&g_WorldInverseTranspose, &g_WorldInverseTranspose);
	g_pWorldInverseTransposeVariable->SetMatrix((float*)&g_WorldInverseTranspose);

	return S_OK;
}


/*******************************************************************
* Enable/Disable Texturing
*******************************************************************/
void dxManager::enableTextures()
{
	g_pBasicTechnique = g_pBasicEffect->GetTechniqueByName("full");
}

void dxManager::disableTextures()
{
	g_pBasicTechnique = g_pBasicEffect->GetTechniqueByName("texturingDisabled");
}
/*******************************************************************
* Enable/Disable Texturing
*******************************************************************/
void dxManager::swapTexture()
{
	textureIndex++;

	if (textureIndex > (int)g_textureSRV.size() - 1) textureIndex = 0;
}
/*******************************************************************
* Scene Renderer
*******************************************************************/
void dxManager::renderScene(HRTimer* hTimer)
{
	this->hTimer = hTimer;


	if (Cam.buttonListener())
		setMatrices();

	//clear scene
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	g_pD3DDevice->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pD3DDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	//set texture
	g_pTextureSR->SetResource(g_textureSRV[textureIndex]);

	//rotate object - rotation should be timer based but i'm lazy
	D3DXMATRIX temp;
	static float r = 0;
	r += 0.001f;

	D3DXVECTOR3 rcOrigin(-14, 0, 0);

	//draw lots of cubes
	for (int cols = 0; cols < 10; cols++)
	{
		for (int rows = 0; rows < 15; rows++)
		{
			//position cube
			D3DXMatrixRotationY(&g_WorldMatrix, r);
			D3DXMatrixTranslation(&temp, rcOrigin.x + 4 * cols, 0, rcOrigin.z + 4 * rows);
			g_WorldMatrix *= temp;
			g_pWorldMatrixEffectVariable->SetMatrix(g_WorldMatrix);


			//draw cube
			for (UINT p = 0; p < techDesc.Passes; p++)
			{
				//apply technique
				g_pBasicTechnique->GetPassByIndex(p)->Apply(0);
				g_pMesh->DrawSubset(0);

				//for (UINT subsetID = 0; subsetID < meshSubset[0]; ++subsetID)
				//{
				//	//fxDiffuseMapVar->SetResource(TextureResourceViews[subsetID]);    //if we draw meshes[1], we will need to add meshSubsets[0] to subsetID here
				//	g_pBasicTechnique->GetPassByIndex(p)->Apply(0);
				//	g_pMesh->DrawSubset(subsetID);
				//}

			}
		}
	}



	D3DXMATRIX mLight;
	D3DXMATRIX mLightScale;
	D3DXMatrixTranslation(&mLight, vLightPos.x, vLightPos.y, vLightPos.z);
	D3DXMatrixScaling(&mLightScale, 0.3f, 0.3f, 0.3f);
	mLight = mLightScale * mLight;

	// Update the world variable to reflect the current light
	g_pWorldMatrixEffectVariable->SetMatrix((float*)&mLight);
	g_pOutputColorVariable->SetFloatVector((float*)&vLightColors);

	g_pBasicTechniqueLight->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pBasicTechniqueLight->GetPassByIndex(p)->Apply(0);
		g_pMesh->DrawSubset(0);
	}


	drawDatasToScreen();

	g_pSwapChain->Present(0, 0);

}


void dxManager::drawDatasToScreen()
{
	// save depth stencil state
	ID3D10DepthStencilState* dss; UINT foo = 1; g_pD3DDevice->OMGetDepthStencilState(&dss, &foo);

	D3DXCOLOR fontColor(1.0f, 1.0f, 0.0f, 1.0f);
	RECT rectangle;
	SetRectEmpty(&rectangle);

	char  Targets[80], Position[80], Up[80], Timers[80];
	sprintf_s(Targets, "X: %f Y: %f Z: %f \n", Cam.Target.x, Cam.Target.y, Cam.Target.z);
	sprintf_s(Position, "X: %f Y: %f Z: %f \n", Cam.Position.x, Cam.Position.y, Cam.Position.z);
	sprintf_s(Up, "X: %f Y: %f Z: %f \n", Cam.Up.x, Cam.Up.y, Cam.Up.z);
	sprintf_s(Timers, "FPS: %.0f \n", 1 / hTimer->getElapsedTime());
	//sprintf(Timers, "totalTime: %f elapsedTime: %f \n", hTimer->getTotalTime(), 1/hTimer->getElapsedTime());

	ppSprite->Begin(D3DX10_SPRITE_SAVE_STATE);
	rectangle = { 5, 5, 0, 0 };
	Font->DrawText(0, "---Targets---", -1, &rectangle, DT_NOCLIP, fontColor);	rectangle.top += 20;
	Font->DrawText(0, Targets, -1, &rectangle, DT_NOCLIP, fontColor);			rectangle.top += 20;
	Font->DrawText(0, "---Position---", -1, &rectangle, DT_NOCLIP, fontColor);	rectangle.top += 20;
	Font->DrawText(0, Position, -1, &rectangle, DT_NOCLIP, fontColor);			rectangle.top += 20;
	Font->DrawText(0, "---Up---", -1, &rectangle, DT_NOCLIP, fontColor);		rectangle.top += 20;
	Font->DrawText(0, Up, -1, &rectangle, DT_NOCLIP, fontColor);				rectangle.top += 20;

	rectangle = { 5, 5, 0, 0 };
	rectangle.left = windowWidth - 70;
	//Font->DrawText(0, "---Timer---", -1, &rectangle, DT_NOCLIP, fontColor);				rectangle.top += 20;
	Font->DrawText(0, Timers, -1, &rectangle, DT_NOCLIP, fontColor);				rectangle.top += 20;

	ppSprite->End();

	// load depth stencil state
	g_pD3DDevice->OMSetDepthStencilState(dss, 1);
}

/*******************************************************************
* Fatal Error Handler
*******************************************************************/
HRESULT dxManager::fatalError(const LPCSTR msg)
{
	MessageBox(*w_hWnd, msg, "Fatal Error!", MB_ICONERROR);
	return E_FAIL;
}

wchar_t* dxManager::convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

