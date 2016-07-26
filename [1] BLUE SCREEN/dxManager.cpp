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


	// Seting up window width and height.
	windowWidth = 800;
	windowHeight = 600;

}

/*******************************************************************
* Destructor
*******************************************************************/
dxManager::~dxManager()
{
	if (g_pd3dDevice) g_pd3dDevice->ClearState();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

void dxManager::Render()
{
	// Just clear the backbuffer
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
	g_pd3dDevice->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
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
	//g_pSwapChain->GetDesc(&sd);

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

/*******************************************************************
* Fatal Error Handler
*******************************************************************/
bool dxManager::fatalError(LPCWSTR msg)
{
	MessageBox(*g_hWnd, msg, L"Fatal Error!", MB_ICONERROR);
	return false;
}

