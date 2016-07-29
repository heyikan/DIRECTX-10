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
bool dxManager::initialize( HWND* hW )
{
	//window handle
	w_hWnd = hW;
	
	//get window dimensions
	RECT rc;
    GetClientRect( *w_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

	viewPortHeight = height;
	viewPortWidth = width;

	// CREATE DEVICE
	//*****************************************************************************
	if (FAILED(createSwapChainAndDevice()))
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
	if (FAILED(createRenderTargets()))
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

	if (FAILED(createCube()))
		return false;


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
HRESULT dxManager::createRenderTargets()
{
	//try to get the back buffer
	ID3D10Texture2D* pBackBuffer;	
	
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);

	if ( FAILED( hr )) return fatalError("Could not get back buffer");

	//try to create render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	if ( FAILED( hr ) )return fatalError("Could not create render target view");
	
	pBackBuffer->Release();
	g_pD3DDevice->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

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
	

	//create matrix effect pointers
	g_pViewMatrixEffectVariable = g_pBasicEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionMatrixEffectVariable = g_pBasicEffect->GetVariableByName( "Projection" )->AsMatrix();
	g_pWorldMatrixEffectVariable = g_pBasicEffect->GetVariableByName( "World" )->AsMatrix();
	g_pTextureSR = g_pBasicEffect->GetVariableByName( "tex2D" )->AsShaderResource();

	//define input layout
	D3D10_INPUT_ELEMENT_DESC layout[] = 
	{	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	//create input layout
	D3D10_PASS_DESC PassDesc;
	g_pBasicTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	hr = g_pD3DDevice->CreateInputLayout(layout,
										numElements,
										PassDesc.pIAInputSignature,
										PassDesc.IAInputSignatureSize,
										&g_pVertexLayout);

	if ( FAILED( hr ))  return fatalError("Could not create Input Layout!");

	// Set the input layout
	g_pD3DDevice->IASetInputLayout( g_pVertexLayout );

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
	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

}

HRESULT dxManager::createCube()
{
	if(FAILED(initVertexBuffer()))
		return E_FAIL;
	
	if(initIndexBuffer())
		return E_FAIL;

	return S_OK;

}


HRESULT dxManager::initVertexBuffer()
{
	// Creating Vertex Buffer.
	
	// init num of vertices
	numOfVertices = 24;

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
	v[0] = vertex(D3DXVECTOR3(-1, 1, -1), D3DXVECTOR4(1, 0, 0, 1)); //front top left
	v[1] = vertex(D3DXVECTOR3(1, 1, -1), D3DXVECTOR4(0, 1, 0, 1)); //front top right
	v[2] = vertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR4(0, 0, 1, 1)); //front bottom left
	v[3] = vertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR4(1, 1, 0, 1)); //front bottom right
	v[4] = vertex(D3DXVECTOR3(-1, 1, 1), D3DXVECTOR4(1, 0, 0, 1)); //back top left
	v[5] = vertex(D3DXVECTOR3(1, 1, 1), D3DXVECTOR4(0, 1, 0, 1)); //back top right
	v[6] = vertex(D3DXVECTOR3(-1, -1, 1), D3DXVECTOR4(0, 0, 1, 1)); //back bottom left
	v[7] = vertex(D3DXVECTOR3(1, -1, 1), D3DXVECTOR4(1, 1, 0, 1)); //back bottom right

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


	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	return S_OK;

}

HRESULT dxManager::initIndexBuffer()
{


	// create index buffer
	D3D10_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(vertex)* numOfVertices;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	hr = g_pD3DDevice->CreateBuffer(&bd, NULL, &g_pIndexBuffer);
	if (FAILED(hr)) return fatalError("Create Index Buffer Error");
 
	// create indexes for a cube
	unsigned int* i = NULL;

	g_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD , 0 , (void**) &i);

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

	g_pIndexBuffer->Unmap();

	UINT offset = 0;
	g_pD3DDevice->IASetIndexBuffer(g_pIndexBuffer , DXGI_FORMAT_R32_UINT , offset);

	return S_OK;
}

HRESULT	dxManager::setMatrices()
{
	D3DXVECTOR3 camera[3] = { 
		//D3DXVECTOR3(0.0f, 0.0f, -5.0f),
		D3DXVECTOR3(getX(), getY(), getZ()),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f) 
	};
	D3DXMatrixLookAtLH(&g_viewMatrix, &camera[0], &camera[1], &camera[2]);

	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, (float)D3DX_PI * 0.5f, (float)viewPortWidth / (float)viewPortHeight, 0.1f, 100.0f);

	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);
	g_pWorldMatrixEffectVariable->SetMatrix(w);
	g_pViewMatrixEffectVariable->SetMatrix(g_viewMatrix);
	g_pProjectionMatrixEffectVariable->SetMatrix(g_projectionMatrix);

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
void dxManager::renderScene()
{

	if (buttonListener())
		setMatrices();



	//clear scene
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	g_pD3DDevice->ClearRenderTargetView(g_pRenderTargetView, ClearColor);


	//set texture
	g_pTextureSR->SetResource(g_textureSRV[textureIndex]);

	//get technique desc
	D3D10_TECHNIQUE_DESC techDesc;
	g_pBasicTechnique->GetDesc( &techDesc );
	
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		//apply technique
		g_pBasicTechnique->GetPassByIndex(p)->Apply(0);
				
		//draw rectangle
		//g_pD3DDevice->Draw(numOfVertices, 0);

		// draws just vertex buffers 
		//g_pD3DDevice->Draw(4, 0);
		//g_pD3DDevice->Draw(4, 4);
		//g_pD3DDevice->Draw(4, 8);
		//g_pD3DDevice->Draw(4, 12);
		//g_pD3DDevice->Draw(4, 16);
		//g_pD3DDevice->Draw(4, 20);

		g_pD3DDevice->DrawIndexed(29,0,0);
	}

	//flip buffers
	g_pSwapChain->Present(0, 0);
}

/*******************************************************************
* Fatal Error Handler
*******************************************************************/
HRESULT dxManager::fatalError(const LPCSTR msg)
{
	MessageBox(*w_hWnd, msg, "Fatal Error!", MB_ICONERROR);
	return E_FAIL;
}