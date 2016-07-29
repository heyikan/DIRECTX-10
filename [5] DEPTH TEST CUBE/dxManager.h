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




#include "BtnCtrl.h"

//create a basic vertex type
struct vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;
	D3DXVECTOR2 texCoord;

	vertex( D3DXVECTOR3 p, D3DXVECTOR4 c )
	{
		pos = p;
		color = c;
	}
	
	vertex( D3DXVECTOR3 p, D3DXVECTOR4 c, D3DXVECTOR2 tex )
	{
		pos = p;
		color = c;
		texCoord = tex;
	}
};

class dxManager:public BtnCtrl
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
	ID3D10InputLayout*			g_pVertexLayout;

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

	//texture storage
	std::vector<ID3D10ShaderResourceView*> g_textureSRV;
	ID3D10EffectShaderResourceVariable* g_pTextureSR;
	
	
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
	HRESULT createRenderTargets();

	//texture loader	
	HRESULT loadTextures();

	

	HRESULT initVertexBuffer();
	HRESULT initIndexBuffer();




	//fatal error handler
	HRESULT fatalError(const LPCSTR msg);





	
};

#endif