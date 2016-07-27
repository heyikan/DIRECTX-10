
#ifndef DXMANAGER
#define DXMANAGER

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>

#pragma comment (lib, "d3d10.lib")
//#pragma comment (lib, "d3d10d.lib")

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct Vertex
{
	D3DXVECTOR3 Pos;
	
	Vertex(D3DXVECTOR3 p)
	{
		Pos = p;
	}
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
	HRESULT createEffectAndGetTechniques();
	HRESULT createAndSetInputLayout();
	HRESULT setTriangleVertices();
	HRESULT setMatrices();

	void Render();

	void cleanUpDevice();

	//fatal error handler
	bool fatalError(LPCWSTR msg);

	// to initialize window width and height...
	int windowWidth;
	int windowHeight;


	private:

		// Common HRESULT
		HRESULT hr;

		/*******************************************************************
		* Members
		********************************************************************/

		// WinApi Classes
		HWND*                       g_hWnd;

		ID3D10Device*               g_pd3dDevice;
		IDXGISwapChain*				g_pSwapChain;
		ID3D10RenderTargetView*		g_pRenderTargetView;

		// Effect
		ID3D10Effect*				g_pEffect;
		// Technique For Effect
		ID3D10EffectTechnique*		g_pTechnique;
		// EffectVariable
		ID3D10EffectMatrixVariable* g_pWorldVariable;
		ID3D10EffectMatrixVariable* g_pViewVariable;
		ID3D10EffectMatrixVariable* g_pProjectionVariable;

		// Input Layout
		ID3D10InputLayout*			g_pVertexLayout;

		// Vertex Buffer
		ID3D10Buffer*				g_pVertexBuffer;

		// Look Matrices
		D3DXMATRIX                  g_pWorld;
		D3DXMATRIX                  g_pView;
		D3DXMATRIX                  g_pProjection;

		D3D10_DRIVER_TYPE			g_driverType;


};


#endif