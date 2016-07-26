
#ifndef DXMANAGER
#define DXMANAGER

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>

#pragma comment (lib, "d3d10.lib")

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
};

class dxManager
{

	public:

		//constructor and destructor
		dxManager();
		~dxManager();

	//--------------------------------------------------------------------------------------
	// Forward declarations
	//--------------------------------------------------------------------------------------
	HRESULT InitDevice(HWND* hW);
	HRESULT createSwapChainAndDevice();
	HRESULT createRenderTargetView();
	HRESULT createViewPort();

	void Render();


	//fatal error handler
	bool fatalError(LPCWSTR msg);

	// to initialize window width and height...
	int windowWidth;
	int windowHeight;


	private:


		/*******************************************************************
		* Members
		********************************************************************/

		// WinApi Classes
		HWND*                       g_hWnd;

		ID3D10Device*               g_pd3dDevice;
		IDXGISwapChain*				g_pSwapChain;
		ID3D10RenderTargetView*		g_pRenderTargetView;

		D3D10_DRIVER_TYPE			g_driverType;


};


#endif