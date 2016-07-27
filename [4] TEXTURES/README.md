# DIRECTX-10 - TEXTURE
* DirectX 10 Texture Examples.
Created with OOP Design.

## INTRODUCTION

![texture](.//img//textures.jpg)

If we look at the above picture, for a 128×128 pixel texture which we want to use for the quad on the right. The first we need to do is set up some sort of coordinate system for the texture. We use the letters u and v for the two axes, please notice that v increases in a downwards direction, u and v are known as texture coordinates. This is because in an image, the first pixel is the top left image and the last pixel is the bottom right. Another restriction on this coordinate system is that u and v are real numbers ranging from 0 to 1 where 1 is 100% of the texture dimension. So the texture coordinates u=0.5, v=0.5 will return the center pixel in a texture.

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
That function merges two steps, the creation of the texture and the resulting view to it, in one.

* Okay so now we’ve loaded the texture now what?

* Well we need to modify our vertex struct to support texture coordinates, texture coordinates have two dimensions, u and v, horizontal and vertical respectively. To attach a texture to a polygon you need to specify what part of the texture you want to put over the polygon.

```c
struct vertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR4 color;
    // Note that textures have two dimensional coordinates (u,v)
    D3DXVECTOR2 texCoord;
}
```

We use a 2d vector to specify the u,v coordinates for each vertex. So this also means the code for the qaud has changed to:

```c
v[0] = vertex( D3DXVECTOR3(-1,-1,0),D3DXVECTOR4(1,0,0,1),D3DXVECTOR2(0.0f, 1.0f) );
.
.

```
