#include "dxManager.h"
#include <string>




/*******************************************************************
* Constructor
*******************************************************************/
dxManager::dxManager()
{

	g_pd3dDevice = NULL;
	g_pSwapChain = NULL;
	g_pRenderTargetView = NULL;
	g_hWnd = NULL;
	g_driverType = D3D10_DRIVER_TYPE_NULL;
	
	g_pEffect = NULL;
	g_pTechnique = NULL;

	g_pWorldVariable = NULL;
	g_pViewVariable = NULL;
	g_pProjectionVariable = NULL;

	g_pVertexLayout = NULL;

	g_pVertexBuffer = NULL;

	

	// Seting up window width and height.
	windowWidth = 800;
	windowHeight = 600;

}

/*******************************************************************
* Destructor
*******************************************************************/
dxManager::~dxManager()
{
	cleanUpDevice();
}

void dxManager::cleanUpDevice()
{
	if (g_pd3dDevice) g_pd3dDevice->ClearState();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (g_pEffect) g_pEffect->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
}

void dxManager::Render()
{
	if (GetAsyncKeyState('X') & 0x8000) { 
		increaseX();
	
		//wchar_t  str[256];
		//wsprintf(str, L"It works! - number: %d \n", getZ() );
		//OutputDebugString(str);

		setMatrices();
	}

	if (GetAsyncKeyState('Y') & 0x8000) {
		increaseY();
		setMatrices();
	}
	if (GetAsyncKeyState('Z') & 0x8000) {
		increaseZ();
		setMatrices();
	}

	// Just clear the backbuffer
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	g_pd3dDevice->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Update Variables
	g_pWorldVariable->SetMatrix((float*) &g_pWorld);
	g_pViewVariable->SetMatrix((float*) &g_pView);
	g_pProjectionVariable->SetMatrix((float*)&g_pProjection);

	// Render a triangle
	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechnique->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pTechnique->GetPassByIndex(p)->Apply(0);
		g_pd3dDevice->Draw(3,0);
	}

	g_pSwapChain->Present(0, 0);
}

HRESULT dxManager::InitDevice(HWND* hW)
{
	HRESULT hr = S_OK;
	// window handle
	g_hWnd = hW;

	// Creating a rectangle structure to determine window's dimension.
	RECT rc;
	// GetClientRect : Retrieves the coordinates of a window's client area. 
	// The client coordinates specify the upper-left and lower-right corners of the client area. 
	GetClientRect(*g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	// Now windows dimensions determined...


	// Creating a Swap Chain. And Fills the id3d10Device
	if (FAILED(createSwapChainAndDevice())) 
		return false;

	// Create a render target view
	if (FAILED(createRenderTargetView()))
		return false;

	// Create viewport
	if (FAILED(createViewPort()))
		return false;


	// Creating Effect and Get Techniques by Name
	if (FAILED(createEffectAndGetTechniques()))
		return false;
	
	// creating the input layout
	if (FAILED(	createAndSetInputLayout()) )
		return false;

	// Setting the triangle vertices
	if (FAILED(setTriangleVertices()))
		return false;

	if (FAILED(setMatrices()))
		return false;

	return S_OK;



}


// Create Swap Chain and Initialize the i3d10device
HRESULT dxManager::createSwapChainAndDevice()
{

	HRESULT hr;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	D3D10_DRIVER_TYPE driverTypes[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	//Set up DX swap chain
	//--------------------------------------------------------------
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd , sizeof(sd));

	// Set the width and height of the buffers in the swap chain
	sd.BufferDesc.Width = windowWidth;
	sd.BufferDesc.Height = windowHeight;

	// Set the refresh rate. This is how often the buffers get swapped out
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	// Set the number of back buffers, 1 is the minimum and normally sufficient
	sd.BufferCount = 1;

	// Set the surface format of the buffers
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set how the buffers are used. Since you are drawing to the buffers, they are considered a render target
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	
	// A handle to the main application window
	sd.OutputWindow = *g_hWnd;

	//sampling settings
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	// Set whether you are running in a window or fullscreen mode
	sd.Windowed = TRUE;

	//Create the D3D device
	/* The
	Direct3D device, an object with the type ID3D10Device, is your main point of
	access to the video hardware.All Direct3D methods for drawing to the screen and
	accessing VRAM are handled through the ID3D10Device object. */
	//--------------------------------------------------------------
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags,
			D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice);
		if (FAILED(hr))
			fatalError(L"D3D10CreateDeviceAndSwapChain Error!");
	}
	if (FAILED(hr))
		fatalError(L"D3D device creation failed!");
	/*
	The D3D10CreateDeviceAndSwapChain function will fill the pSwapChain variable
	with a valid IDXGISwapChain object and the pD3DDevice variable with a valid
	ID3D10Device.
	*/

	return S_OK;
}


HRESULT dxManager::createRenderTargetView()
{
	HRESULT hr;
	ID3D10Texture2D*	pBackBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		fatalError(L"GetBuffer Error!");

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		fatalError(L"createRenderTargetView Error!");

	g_pd3dDevice->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	return S_OK;

}

HRESULT dxManager::createViewPort()
{
	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = windowWidth;
	vp.Height = windowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports(1 , &vp);

	return S_OK;
}

HRESULT dxManager::createEffectAndGetTechniques()
{
	// Create the effect
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	hr = D3DX10CreateEffectFromFile(L"Triangle.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, g_pd3dDevice, NULL,
		NULL, &g_pEffect, NULL, NULL);

	if (FAILED(hr))
		fatalError(L"The FX file cannot be located.Please run this executable from the directory that contains the FX file.");

	// Getting techniques

	// Obtain techniques
	g_pTechnique = g_pEffect->GetTechniqueByName("Render");

	// Obtain the variables for shader file
	g_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	g_pViewVariable = g_pEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionVariable = g_pEffect->GetVariableByName("Projection")->AsMatrix();

	return S_OK;

}

HRESULT dxManager::createAndSetInputLayout()
{
	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	// calculating number of elements
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC passDesc;
	g_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &g_pVertexLayout);
	if (FAILED(hr))
		fatalError(L"CreateInputLayout Error");
	

	// set the input Layout
	g_pd3dDevice->IASetInputLayout(g_pVertexLayout);


	return S_OK;

}

HRESULT dxManager::setTriangleVertices()
{
	// fill vertex buffer with vertices
	UINT numVertices = 3;
	//Vertex* v = NULL;

	Vertex vertices[] =
	{
		D3DXVECTOR3(0.0f, 1.0f, 1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f)
	};

	//v[0] = Vertex(D3DXVECTOR3(0.0f, 1.0f, 1.0f));
	//v[1] = Vertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f));
	//v[2] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f));

	// creating the vertex buffers for triangle.
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex)* numVertices;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;


	// Creating the buffer
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		fatalError(L"Create Buffer Error");

	// set buffer the vertex buffer for triangle
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pd3dDevice->IASetVertexBuffers(0,1,&g_pVertexBuffer , &stride , &offset);

	g_pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

HRESULT dxManager::setMatrices()
{
	// Initialize the world matrix
	
	D3DXMatrixIdentity(&g_pWorld);

	// Initialize the view matrix
	//D3DXVECTOR3 Eye(0.0f, 3.0f, -10.0f);
	D3DXVECTOR3 Eye(getX(), getY(), getZ());
	D3DXVECTOR3 At(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	
	D3DXMatrixLookAtLH(&g_pView , &Eye , &At, &Up);

	// Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH(&g_pProjection, (float)D3DX_PI * 0.25f, 1.33f, 0.1f, 100.0f);

	return S_OK;

}


/*******************************************************************
* Fatal Error Handler
*******************************************************************/
bool dxManager::fatalError(LPCWSTR msg)
{
	MessageBox(*g_hWnd, msg, L"Fatal Error!", MB_ICONERROR);
	return false;
}

