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
}
/*******************************************************************
* Initializes Direct3D Device
*******************************************************************/
bool dxManager::initialize( HWND* hW , HINSTANCE* hIns)
{
	//window handle
	w_hWnd = hW;
	w_hIns = hIns;
	//get window dimensions
	RECT rc;
    GetClientRect( *w_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

	viewPortHeight = height;
	viewPortWidth = width;

	// CREATE DIRECT INPUT
	if (FAILED((Cam.InitDirectInput(w_hIns , w_hWnd))))
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

	//create vertex buffer
	//---------------------------------------------------------------------------------
	//if (FAILED(createRectangle()))
	//	return false;

	//if (FAILED(createCube()))
	//	return false;



	if (FAILED(createMeshCube()))
		return false;

	// Set up the view and projection matrices
	//*****************************************************************************
	if (FAILED(setMatrices()))
		return false;
	


	//everything completed successfully
	return true;
}


HRESULT dxManager::createFont()
{
	D3DX10_FONT_DESC fd;
	fd.Height = 17;
	fd.Width= 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = OUT_TT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	//wcscpy(fd.FaceName, L"Impact");
	strcpy(fd.FaceName, "Cambria");
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

	if ( FAILED( hr)) return fatalError("D3D device creation failed");


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
	g_pD3DDevice->CreateRasterizerState( &rasterizerState, &g_pRS);	
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

	if ( FAILED( hr )) return fatalError("Could not get back buffer");

	//try to create render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	if ( FAILED( hr ) )return fatalError("Could not create render target view");
	
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
	if ( g_pBasicTechnique == NULL ) return fatalError("Could not find specified technique!");	
	
	g_pBasicTechniqueLight = g_pBasicEffect->GetTechniqueByName("full");
	if (g_pBasicTechnique == NULL) return fatalError("Could not find specified technique!");



	//create matrix effect pointers
	g_pViewMatrixEffectVariable = g_pBasicEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionMatrixEffectVariable = g_pBasicEffect->GetVariableByName( "Projection" )->AsMatrix();
	g_pWorldMatrixEffectVariable = g_pBasicEffect->GetVariableByName( "World" )->AsMatrix();
	
	g_pWorldInverseTransposeVariable = g_pBasicEffect->GetVariableByName("g_WorldInverseTranspose")->AsMatrix();
	g_pLightPosVariable = g_pBasicEffect->GetVariableByName("vLightPos")->AsVector();
	g_pLightColorVariable = g_pBasicEffect->GetVariableByName("vLightColor")->AsVector();
	g_pOutputColorVariable = g_pBasicEffect->GetVariableByName("vOutputColor")->AsVector();


	g_pTextureSR = g_pBasicEffect->GetVariableByName( "tex2D" )->AsShaderResource();

	//float* pData = NULL;

	// g_pViewMatrixEffectVariable->

	//define input layout
	//D3D10_INPUT_ELEMENT_DESC layout[] = 
	//{	
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	//};



	numOfInputLayoutElements = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);

	//create input layout
	D3D10_PASS_DESC PassDesc;
	g_pBasicTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	hr = g_pD3DDevice->CreateInputLayout(vertexInputLayout,
										numOfInputLayoutElements,
										PassDesc.pIAInputSignature,
										PassDesc.IAInputSignatureSize,
										&g_pVertexLayout);

	if ( FAILED( hr ))  return fatalError("Could not create Input Layout!");

	// Set the input layout
	g_pD3DDevice->IASetInputLayout( g_pVertexLayout );

	g_pBasicTechnique->GetDesc(&techDesc);
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
	for ( int i=0; i < (int) filenames.size(); i++ )
	{
		g_textureSRV.push_back(NULL);
		hr = D3DX10CreateShaderResourceViewFromFile(g_pD3DDevice, filenames[i].c_str(), NULL, NULL, &g_textureSRV[i], NULL);
		if (FAILED(hr))
		{
			char err[255];
			sprintf_s(err, "Could not load texture: %s!", filenames[i].c_str());
			return fatalError( err );
		}
	}

	return S_OK;
}


HRESULT dxManager::createRectangle()
{

	numOfVertices = 4;

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex)* numOfVertices;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	hr = g_pD3DDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
	if (FAILED(hr)) return fatalError("Could not create vertex buffer!");;

	// Set vertex buffer
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	g_pD3DDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	vertex* v = NULL;

	//lock vertex buffer for CPU use
	g_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

	v[0] = vertex(D3DXVECTOR3(-1, -1, 0), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 2.0f));
	v[1] = vertex(D3DXVECTOR3(-1, 1, 0), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[2] = vertex(D3DXVECTOR3(1, -1, 0), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(2.0f, 2.0f));
	v[3] = vertex(D3DXVECTOR3(1, 1, 0), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(2.0f, 0.0f));

	g_pVertexBuffer->Unmap();

	// Set primitive topology 
	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

HRESULT dxManager::createCube()
{
	if(FAILED(initVertexBuffer()))
		return E_FAIL;
	
	if(initIndexBuffer())
		return E_FAIL;

	return S_OK;

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
	g_pMesh->SetIndexData(i , 36);
	g_pMesh->CommitToDevice();

	return S_OK;
}


HRESULT dxManager::initVertexBuffer()
{
	// Creating Vertex Buffer.
	
	// init num of vertices
	numOfVertices = 50;

	// [1] Creating buffer.
	//	  [1][1] Create description
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertex)*numOfVertices;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	//	  [1][2] Allocate Buffers --> g_pVertexBuffer
	hr = g_pD3DDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
	if (FAILED(hr)) return fatalError("Vertex Buffer Error!");

	// Set vertex Buffer
	UINT strides = sizeof(vertex);
	UINT offset = 0;
	
	// init vertex buffer to g_pd3ddevice 
	g_pD3DDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &strides, &offset);

	// init vertices
	vertex* v = NULL;

	//lock vertex buffer for CPU use
	g_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

	//vertices for a cube
	//v[0] = vertex( D3DXVECTOR3(-1,  1, -1), D3DXVECTOR4(1, 0, 0, 1) , D3DXVECTOR2(0.0f , 0.0f) ); //front top left
	//v[1] = vertex( D3DXVECTOR3( 1,  1, -1), D3DXVECTOR4(0, 1, 0, 1) , D3DXVECTOR2(0.0f , 2.0f) ); //front top right
	//v[2] = vertex( D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 0, 1, 1) , D3DXVECTOR2(2.0f , 0.0f) ); //front bottom left
	//v[3] = vertex( D3DXVECTOR3( 1, -1, -1), D3DXVECTOR4(1, 1, 0, 1) , D3DXVECTOR2(2.0f , 2.0f) ); //front bottom right
	//v[4] = vertex( D3DXVECTOR3(-1,  1,  1), D3DXVECTOR4(1, 0, 0, 1) , D3DXVECTOR2(0.0f , 0.0f)); //back top left
	//v[5] = vertex( D3DXVECTOR3( 1,  1,  1), D3DXVECTOR4(0, 1, 0, 1) , D3DXVECTOR2(0.0f , 2.0f)); //back top right
	//v[6] = vertex( D3DXVECTOR3(-1, -1,  1), D3DXVECTOR4(0, 0, 1, 1) , D3DXVECTOR2(2.0f , 0.0f)); //back bottom left
	//v[7] = vertex( D3DXVECTOR3( 1, -1,  1), D3DXVECTOR4(1, 1, 0, 1) , D3DXVECTOR2(2.0f , 2.0f)); //back bottom right

	// Top
	v[0] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[1] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[2] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[3] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));

	// Bottom
	v[4] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[5] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[6] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[7] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));

	// Left
	v[8] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[9] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[10] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[11] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));

	// Right
	v[12] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[13] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[14] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[15] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));

	// Front Face
	v[16] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[17] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[18] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[19] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));

	// Back Face
	v[20] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1), D3DXVECTOR2(0.0f, 0.0f));
	v[21] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 1, 0, 1), D3DXVECTOR2(1.0f, 0.0f));
	v[22] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(0, 0, 1, 1), D3DXVECTOR2(1.0f, 1.0f));
	v[23] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 1, 0, 1), D3DXVECTOR2(0.0f, 1.0f));


	g_pVertexBuffer->Unmap();

	//lock vertex buffer for CPU use

	////CUBE DRAW METHOD 1
	////VERTEX VALUES NOT 100% CORRECT - TOO LAZY TO CORRECT
	//g_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);
	////vertices for a cube
	//v[0] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(1, 0, 0, 1));
	//v[1] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(0, 1, 0, 1));
	//v[2] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(0, 0, 1, 1));
	//v[3] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(1, 1, 0, 1));
	//v[4] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(1, 0, 0, 1));
	//v[5] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 1, 0, 1));
	//v[6] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 0, 1, 1));
	//v[7] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(1, 1, 0, 1));
	//v[8] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1));
	//v[9] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(0, 1, 0, 1));
	//v[10] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 0, 1, 1));
	//v[11] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 1, 0, 1));
	//v[12] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1));
	//v[13] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(0, 1, 0, 1));
	//v[14] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 0, 1, 1));
	//v[15] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(1, 1, 0, 1));
	//v[16] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(1, 0, 0, 1));
	//v[17] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 1, 0, 1));
	//v[18] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(0, 0, 1, 1));
	//v[19] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(1, 1, 0, 1));
	//v[20] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 0, 0, 1));
	//v[21] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(0, 1, 0, 1));
	//v[22] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 0, 1, 1));
	//v[23] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(1, 1, 0, 1));


	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	return S_OK;

}

HRESULT dxManager::initIndexBuffer()
{


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

	// create indexes for a cube
	unsigned int* indices = NULL;

	g_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&indices);

	indices[0] = 3;
	indices[1] = 1;
	indices[2] = 0;

	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;
	// -------------
	indices[6] = 6;
	indices[7] = 4;
	indices[8] = 5;

	indices[9] = 7;
	indices[10] = 4;
	indices[11] = 6;
	// -------------
	indices[12] = 11;
	indices[13] = 9;
	indices[14] = 8;

	indices[15] = 10;
	indices[16] = 9;
	indices[17] = 11;
	// -------------
	indices[18] = 14;
	indices[19] = 12;
	indices[20] = 13;

	indices[21] = 15;
	indices[22] = 12;
	indices[23] = 14;
	// --------------

	indices[24] = 19;
	indices[25] = 17;
	indices[26] = 16;

	indices[27] = 18;
	indices[28] = 17;
	indices[29] = 19;
	// ----------------

	indices[30] = 22;
	indices[31] = 20;
	indices[32] = 21;

	indices[33] = 23;
	indices[34] = 20;
	indices[35] = 22;
	// ----------------

	g_pIndexBuffer->Unmap();




	UINT offset = 0;
	g_pD3DDevice->IASetIndexBuffer(g_pIndexBuffer , DXGI_FORMAT_R32_UINT , offset);

	return S_OK;
}

HRESULT	dxManager::setMatrices()
{
	
	// Set up matrix
	RECT rct;
	GetClientRect(*w_hWnd, &rct);
	//D3DXMATRIX matProj;

	float aspect = (rct.right - rct.left) / (float)(rct.bottom - rct.top);

	//D3DXVECTOR3 camera[3] = {
	//	D3DXVECTOR3(0.0f, 0.0f, -5.0f),
	//	//D3DXVECTOR3(getX(), getY(), getZ()),
	//	D3DXVECTOR3(0.0f, 0.0f, 1.0f),
	//	D3DXVECTOR3(0.0f, 1.0f, 0.0f)
	//};

	//D3DXMATRIX viewMatrix;
	//gCamera.CalculateViewMatrix(&viewMatrix);


	D3DXMatrixIdentity(&g_WorldMatrix);

	//D3DXMatrixLookAtLH(&g_viewMatrix, &camera[0], &camera[1], &camera[2]);
	//D3DXMatrixLookAtLH(&g_viewMatrix, &Cam.Position, &Cam.Target, &Cam.Up);

	D3DXMatrixLookAtLH(&g_viewMatrix, &Cam.Position, &Cam.Target, &Cam.Up);

	//D3DXMatrixLookAtLH(&viewMatrix, &gCamera.m_look  , &gCamera.m_right , &gCamera.m_up);

	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, D3DX_PI / 4, aspect, 1.0f, 100.0f);
	


	g_pWorldMatrixEffectVariable->SetMatrix((float*) &g_WorldMatrix);
	g_pViewMatrixEffectVariable->SetMatrix((float*)&g_viewMatrix);
	g_pProjectionMatrixEffectVariable->SetMatrix((float*)&g_projectionMatrix);

	//char  str[80];
	//sprintf(str, "11: %f 12: %f 13: %f 14: %f \n", g_WorldMatrix._11, g_WorldMatrix._12, g_WorldMatrix._13, g_WorldMatrix._14);
	//OutputDebugString(str);
	//sprintf(str, "21: %f 22: %f 23: %f 24: %f \n", g_WorldMatrix._21, g_WorldMatrix._22, g_WorldMatrix._23, g_WorldMatrix._24);
	//OutputDebugString(str);
	//sprintf(str, "31: %f 32: %f 33: %f 34: %f \n", g_WorldMatrix._31, g_WorldMatrix._32, g_WorldMatrix._33, g_WorldMatrix._34);
	//OutputDebugString(str);
	//sprintf(str, "41: %f 42: %f 43: %f 44: %f \n", g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43, g_WorldMatrix._44);
	//OutputDebugString(str);


	//if (system("CLS")) system("clear");
	//fprintf(stdout, " ------- WORLD MATRIX -------\n");
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_WorldMatrix._11, g_WorldMatrix._12, g_WorldMatrix._13, g_WorldMatrix._14);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_WorldMatrix._21, g_WorldMatrix._22, g_WorldMatrix._23, g_WorldMatrix._24);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_WorldMatrix._31, g_WorldMatrix._32, g_WorldMatrix._33, g_WorldMatrix._34);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43, g_WorldMatrix._44);

	//fprintf(stdout, " ------- View MATRIX -------\n");
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_viewMatrix._11, g_viewMatrix._12, g_viewMatrix._13, g_viewMatrix._14);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_viewMatrix._21, g_viewMatrix._22, g_viewMatrix._23, g_viewMatrix._24);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_viewMatrix._31, g_viewMatrix._32, g_viewMatrix._33, g_viewMatrix._34);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_viewMatrix._41, g_viewMatrix._42, g_viewMatrix._43, g_viewMatrix._44);

	//fprintf(stdout, " ------- Projection MATRIX -------\n");
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_projectionMatrix._11, g_projectionMatrix._12, g_projectionMatrix._13, g_projectionMatrix._14);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_projectionMatrix._21, g_projectionMatrix._22, g_projectionMatrix._23, g_projectionMatrix._24);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_projectionMatrix._31, g_projectionMatrix._32, g_projectionMatrix._33, g_projectionMatrix._34);
	//fprintf(stdout, "11: %f 12: %f 13: %f 14: %f \n", g_projectionMatrix._41, g_projectionMatrix._42, g_projectionMatrix._43, g_projectionMatrix._44);


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

	if ( textureIndex > (int) g_textureSRV.size() - 1 ) textureIndex = 0;
}
/*******************************************************************
* Scene Renderer
*******************************************************************/
void dxManager::renderScene(HRTimer* hTimer)
{
	this->hTimer = hTimer;


	//if (gCamera.buttonListener()){
	//	setMatrices();
	//	gCamera.printCoordinate();
	//}

	if (Cam.buttonListener())
		setMatrices();

	D3DXVECTOR4 vLightPos = D3DXVECTOR4(0.0f, 0.0f, 21.0f, 1.0f);
	D3DXVECTOR4 vLightColors = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);



	D3DXMATRIX                  g_WorldInverseTranspose;

	D3DXMatrixInverse(&g_WorldInverseTranspose, 0, &g_WorldMatrix);
	D3DXMatrixTranspose(&g_WorldInverseTranspose, &g_WorldInverseTranspose);
	g_pWorldInverseTransposeVariable->SetMatrix((float*)&g_WorldInverseTranspose);

	g_pLightPosVariable->SetFloatVectorArray((float*)vLightPos,0,2);
	g_pLightColorVariable->SetFloatVectorArray((float*)vLightColors,0,2);

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

			}
		}
	}



	D3DXMATRIX mLight;
	D3DXMATRIX mLightScale;
	D3DXMatrixTranslation(&mLight, vLightPos.x, vLightPos.y, vLightPos.z);
	D3DXMatrixScaling(&mLightScale, 0.2f, 0.2f, 0.2f);
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

	////get technique desc
	//D3D10_TECHNIQUE_DESC techDesc;
	//g_pBasicTechnique->GetDesc( &techDesc );
	//
	//for( UINT p = 0; p < techDesc.Passes; ++p )
	//{
	//	//apply technique
	//	g_pBasicTechnique->GetPassByIndex(p)->Apply(0);
	//			
	//	//draw rectangle
	//	//g_pD3DDevice->Draw(numOfVertices, 0);

	//	// draws just vertex buffers 
	//	//g_pD3DDevice->Draw(4, 0);
	//	//g_pD3DDevice->Draw(4, 4);
	//	//g_pD3DDevice->Draw(4, 8);
	//	//g_pD3DDevice->Draw(4, 12);
	//	//g_pD3DDevice->Draw(4, 16);
	//	//g_pD3DDevice->Draw(4, 20);
	//	//g_pD3DDevice->Draw(4, 24);
	//	//g_pD3DDevice->Draw(4, 0);
	//	//g_pD3DDevice->Draw(4, 3);
	//	//g_pD3DDevice->DrawIndexed(36, 0, 0);
	//	//g_pD3DDevice->DrawIndexed(29,0,0);
	//	g_pMesh->DrawSubset(0);
	//}

	//D3DXMATRIX temp;

	//D3DXMATRIX world2;
	//D3DXMatrixIdentity(&world2);

	//D3DXMatrixTranslation(&temp, -4.0f, 0, 0);

	//D3DXMatrixMultiply(&world2 , &world2 , &temp);

	////g_WorldMatrix *= temp;

	//g_pWorldMatrixEffectVariable->SetMatrix((float*) &world2);


	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	//apply technique
	//	g_pBasicTechnique->GetPassByIndex(p)->Apply(0);
	//	g_pMesh->DrawSubset(0);
	//}

	//setMatrices();
	//hr = ppSprite->Begin(0);
	//LPCWSTR left = L"Some text to render";
	//RECT rect;
	//rect.top = 0;
	//rect.bottom = windowHeight;
	//rect.left = 0;
	//rect.right = windowWidth;
	//hr = Font->DrawTextW(ppSprite, left, -1, &rect, DT_LEFT | DT_TOP | DT_CALCRECT, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	//hr = Font->DrawTextW(ppSprite, left, -1, &rect, DT_LEFT | DT_TOP, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	////hr = sprite->Flush();
	//hr = ppSprite->End();
	//g_pD3DDevice->OMSetBlendState(NULL, 0, 0xffffffff);




	
	//flip buffers
	/*g_pSwapChain->Present(0, 0);*/
}


void dxManager::drawDatasToScreen()
{
	// save depth stencil state
	ID3D10DepthStencilState* dss; UINT foo = 1; g_pD3DDevice->OMGetDepthStencilState(&dss, &foo);

	D3DXCOLOR fontColor(1.0f, 1.0f, 0.0f, 1.0f);
	RECT rectangle;
	SetRectEmpty(&rectangle);

	char  Targets[80], Position[80], Up[80] , Timers[80];
	sprintf(Targets, "X: %f Y: %f Z: %f \n", Cam.Target.x, Cam.Target.y, Cam.Target.z);
	sprintf(Position, "X: %f Y: %f Z: %f \n", Cam.Position.x, Cam.Position.y, Cam.Position.z);
	sprintf(Up, "X: %f Y: %f Z: %f \n", Cam.Up.x, Cam.Up.y, Cam.Up.z);
	sprintf(Timers, "FPS: %.0f \n", 1 / hTimer->getElapsedTime());
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
	Font->DrawText(0, Timers, -1, &rectangle, DT_NOCLIP  , fontColor);				rectangle.top += 20;
	
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
