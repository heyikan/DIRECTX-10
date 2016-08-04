/************************************************************************************
 *	DirectX 10 Manager Class
 *	----------------------------------------
 *	code by : bobby anguelov - banguelov@cs.up.ac.za
 *	downloaded from : takinginitiative.wordpress.org
 *
 *	code is free for use in whatever way you want, however if you work for a game
 *	development firm you are obliged to offer me a job :P (haha i wish)
 ************************************************************************************/

#ifndef DXMANAGER
#define DXMANAGER


#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <vector>


#include "BtnCtrl.h"
#include "vertexTypes.h"
//#include "FPCamera.h"
//#include "Camera.h"
#include "FCamera.h"

class dxManager
{
	/*******************************************************************
	* Members
	********************************************************************/	
private:

	//window handle shortcut
	HWND*						w_hWnd;
	
	//device vars
	ID3D10Device*				g_pD3DDevice;
	IDXGISwapChain*				g_pSwapChain;
	ID3D10RenderTargetView*		g_pRenderTargetView;
	D3D10_VIEWPORT				g_viewPort;
	ID3D10RasterizerState*		g_pRS;

	//input layout and vertex buffer
	ID3D10Buffer*				g_pVertexBuffer;
	ID3D10Buffer*				g_pIndexBuffer;

	ID3D10InputLayout*			g_pVertexLayout;
	ID3DX10Mesh*				g_pMesh;

	//effects and techniques
	ID3D10Effect*				g_pBasicEffect;
	ID3D10EffectTechnique*		g_pBasicTechnique;

	//effect variable pointers
	ID3D10EffectMatrixVariable* g_pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable* g_pProjectionMatrixEffectVariable;
	ID3D10EffectMatrixVariable* g_pWorldMatrixEffectVariable;
	
	//projection and view matrices
	D3DXMATRIX                  g_viewMatrix;
	D3DXMATRIX                  g_projectionMatrix;
	// World matrix
	D3DXMATRIX					g_WorldMatrix;

	//texture storage
	std::vector<ID3D10ShaderResourceView*> g_textureSRV;
	ID3D10EffectShaderResourceVariable* g_pTextureSR;

	// dx manager members
	ID3D10Texture2D*			g_pDepthStencil;
	ID3D10DepthStencilView*		g_pDepthStencilView;



	// technique
	D3D10_TECHNIQUE_DESC		techDesc;



	UINT numOfInputLayoutElements;
	
	/*******************************************************************
	* Methods
	********************************************************************/	
public:

	//constructor and destructor
	dxManager();
	~dxManager();

	//initialize directx device
	bool initialize(HWND*);	

	//settings
	void enableTextures();
	void disableTextures();
	void swapTexture();

	//scene function
	void renderScene();	

	// viewPort Widht and Height
	UINT viewPortHeight;
	UINT viewPortWidth;

	int windowHeight;
	int windowWidth;

	UINT numOfVertices;

	HRESULT createRectangle();
	HRESULT setMatrices();

	HRESULT createCube();
	HRESULT createMeshCube();

	bool texturesEnabled;
	int textureIndex;


private:

	// Common HRESULT variable for return value
	HRESULT hr;

	//initialization methods
	HRESULT createSwapChainAndDevice();
	HRESULT loadShadersAndCreateInputLayouts();
	void createViewports();
	void initRasterizerState();
	HRESULT createRenderTargetsAndDepthBuffer();

	//texture loader	
	HRESULT loadTextures();

	

	HRESULT initVertexBuffer();
	HRESULT initIndexBuffer();




	//fatal error handler
	HRESULT fatalError(const LPCSTR msg);



	wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

	
};

#endif