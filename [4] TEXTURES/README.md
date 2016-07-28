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
v[1] = vertex( D3DXVECTOR3(-1,1,0),D3DXVECTOR4(0,1,0,1),D3DXVECTOR2(0.0f, 0.0f) );
v[2] = vertex( D3DXVECTOR3(1,-1,0),D3DXVECTOR4(0,0,1,1),D3DXVECTOR2(1.0f, 1.0f) );
v[3] = vertex( D3DXVECTOR3(1,1,0),D3DXVECTOR4(1,1,0,1),D3DXVECTOR2(1.0f, 0.0f) );
```

* Since we changed the vertex struct we also need to modify the inputlayout accordingly, remember that the input layout tells DX what each vertex looks like. All we need to change is the input_element_desc for the input layout:

```c
D3D10_INPUT_ELEMENT_DESC layout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};
```


## Texturing using the Pixel Shader

So lets just tweak the shader program (Texture.fx), first thing we need to do is specify a texture variable for the shader as such:

```c
Texture2D tex2D;
```
* Then we create a samplerState, remember when I said earlier that we sample the texture to get a color at a specific set of u,v coordinates, a texture resource in HLSL has a sample method that samples that texture and returns the value at a specific point, now the way in which it sample that texture is done via the sampleState object. This object sets all the parameters for the default sampler. A very basic samplerState is show below:

```c
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
```
* It has three elements: a filter which specifies how the sampling is to be done over the texture, in our case we’re just doing a basic linear sampling (all the sampling states and their descriptions are available in the SDK docs, the addressU and addressV specify how to handle u and v values that lie outside the 0 to 1 range.

* Remember how I said that u and v are in the range 0 to 1, well that’s not exactly true, sometimes to want to texture an object using 4 smaller versions of a texture rather than stretching the texture to fit the the object. The below image shows what happens for a few common addressU , addressV values:

![texture](.//img//textures2.jpg)

* There are other state variables for the samplerState object, once again all the info necessary is in the sdk docs.

* Now we need to modify the vs_input and ps_input structs to handle the extra 2d texcoord variable:

```c
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD;
};
```

* And the vector and pixel shaders accordingly. The only difference in the pixel shader is that for texturing we need to return the sampled color and not the vertex color. We use the sample method on the texture object , the sampler state we defined earlier and the texture coordinates we specified earlier.

```c
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Color = input.Color;
    output.Tex = input.Tex;

    return output;
}

float4 textured( PS_INPUT input ) : SV_Target
{
    return tex2D.Sample( linearSampler, input.Tex );
}

float4 noTexture( PS_INPUT input ) : SV_Target
{
    return input.Color;
}
```

* We also create two new techniques “full” and “texturing disabled”:

```c
technique10 full
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, textured() ) );
    }
}

technique10 texturingDisabled
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, noTexture() ) );
    }
}
```

* Then we load these two techniques exactly as we did in the previous tutorial. If we compile and run the program we’re presented with a now textured quad.
