# DIRECTX-10 - TEXTURE
* DirectX 10 Texture Examples.
Created with OOP Design.

## Loading the Textures
So let’s load our textures, our first step is setting up storage for our textures and a way to pass it through to the shader program:
```c
// Texture variables
ID3D10ShaderResourceView*	          textureSRV;
ID3D10EffectShaderResourceVariable*	pTextureSR;
```
Textures are loaded as ID3D10 resources, to which you need to create a view; a view tells DX how to access a particular resource. Since a texture is a shader resource that’s the type of view we need. The second variable is how we pass the texture id to the HLSL shaders.

So how do we load a texture from an image file? Well if you thought it would be a complicated and involved process you’re wrong, it’s a single function call

__(D3DX10CreateShaderResourceViewFromFile),__ and the code to load multiple textures into our texture storage.

```c
HRESULT D3DX10CreateShaderResourceViewFromFile(
        ID3D10Device *pDevice,
        LPCTSTR pSrcFile,
        D3DX10_IMAGE_LOAD_INFO *pLoadInfo,
        ID3DX10ThreadPump *pPump,
        ID3DX10ShaderResourceView **ppShaderResourceView,
        HRESULT *pHResult
};
```

```c
// Load texture maps
hr = D3DX10CreateShaderResourceViewFromFile(g_pd3dDevice , L".\\Textures\\t1.bmp", NULL , NULL ,&g_textureSRV , NULL);

```
