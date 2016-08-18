# DIRECTX 10 LIGHTING

![FPS](.//img//lighting.png)

basiceffects.fx

```c
//--------------------------------------------------------------------------------------
// basicEffect.fx
//
//--------------------------------------------------------------------------------------



RasterizerState DisableCulling
{
	CullMode = NONE;
};



Texture2D tex2D;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
	//BorderColor = float4(0, 0, 0, 0);
};

matrix World;
matrix View;
matrix Projection;
matrix WorldInverseTranspose;

// lights
float4 vLightPos;
float4 vLightColor;

// output color
float4 vOutputColor;

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;

	// light Normal
	float3 Norm : NORMAL;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Tex : TEXCOORD0;

	float3 Norm : TEXCOORD1;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );    
    output.Pos = mul( output.Pos, Projection );

	output.Color = input.Color;



	output.Norm = mul(input.Norm, World);

	output.Tex = input.Tex;

    return output;  
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 texturedPS(VS_OUTPUT input) : SV_Target
{
	//float4 finalColor = 0;
	//float3 toLight = 0;

	//float3 Position = mul(input.Pos, WorldInverseTranspose).xyz;
	//toLight = normalize(vLightPos - Position);
	//finalColor += saturate(dot(toLight, input.Norm) * vLightColor);

	//finalColor.a = 1;
	input.Norm = normalize(input.Norm);

	float4 finalColor = 0;
	float3 toLight = 0;

	float3 Position = mul(input.Pos, WorldInverseTranspose).xyz;

	toLight = normalize(vLightPos - Position);

	finalColor += saturate(dot(toLight, input.Norm) * vLightColor);

	finalColor.a = 1;

	float4 texColor = { 1, 1, 1, 1 };

	float4 OUT = (float4)0;
	texColor = tex2D.Sample(linearSampler, input.Tex);
	//OUT = tex2D.Sample(linearSampler, input.Tex);

	OUT = finalColor * texColor;

	//OUT.rgba = finalColor;

    //return tex2D.Sample( linearSampler, input.Tex );
	//OUT = tex2D.Sample(linearSampler, input.Tex);
	//return finalColor;
	//return tex2D.Sample(linearSampler, input.Tex);

	return OUT;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 finalColor = 0;
	float3 toLight = 0;

	float3 Position = mul(input.Pos, WorldInverseTranspose).xyz;
	toLight = normalize(vLightPos - Position);
	finalColor += saturate(dot(toLight, input.Norm) * vLightColor);

	finalColor.a = 1;

	   //return tex2D.Sample( linearSampler, input.Tex );

	return finalColor;
}

float4 noTexture(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}

//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(VS_OUTPUT input) : SV_Target
{
	return vOutputColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 full
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, texturedPS()));
		SetRasterizerState(DisableCulling);

    }
}

technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS() ));
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

//--------------------------------------------------------------------------------------
technique10 RenderLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}
```
