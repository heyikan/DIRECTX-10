#ifndef _DXMANAGER
#define _DXMANAGER

#include <windows.h>
//#include <vector>
#include <d3d10.h>
#include <d3dx10.h>

#include "BtnCtrl.h"


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct Vertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
	
	// Texture Vector (u,v)
	D3DXVECTOR2 texCoord;
	

};

class dxManager:public BtnCtrl
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
	HRESULT loadShadersAndCreateInputLayouts(LPWSTR fileName);
	HRESULT createAndSetInputLayout();
	HRESULT setRectangleVertices();
	HRESULT setMatrices();
	HRESULT setCubeVertices();
	HRESULT initRasterizerState();


	HRESULT setIndexBuffrers();
	HRESULT setVertexBuffers();

	HRESULT loadTextures();


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
		// Index Buffer
		ID3D10Buffer*				g_pIndexBuffer;

		// Look Matrices
		D3DXMATRIX                  g_pWorld;
		D3DXMATRIX                  g_pView;
		D3DXMATRIX                  g_pProjection;

		// Stencil Buffer
		ID3D10Texture2D*			g_pDepthStencil;
		ID3D10DepthStencilView*		g_pDepthStencilView;


		D3D10_DRIVER_TYPE			g_driverType;

		// Texture variables
		ID3D10ShaderResourceView*	g_textureSRV;
		ID3D10EffectShaderResourceVariable*	g_pTextureSR;


};


#endif