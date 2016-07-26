// World View Projection matisleri vertex shaderda d�nya koordinalar�ndan ekran koordinatlar�na
// d�n���m� ger�ekle�tirmek i�in kullan�l�r.
// World matrisi ile 3d nesneler i�in �l�ekleme, d�nme ve �teleme d�n���mleri yap�l�r.
// World matrisi directx program�nda D3DXMatrixIdentity ile identity matris olarak tan�mlanm��t�r.


// View matrisinde bak�� noktas�n�n konumu ve bak�� do�rultusu ile ilgili bilgiler tutulur.

// Projection matrisi perspektif d�n���m� yapmak �zere setlenir.



//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
matrix World;
matrix WorldInverseTranspose;
matrix View;
matrix Projection;
float4 vLightPos[2];
float4 vLightColor[2];
float4 vOutputColor;


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = mul(input.Norm, World);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalColor = 0;
	float3 toLight = 0;

	float3 Position = mul(input.Pos, WorldInverseTranspose).xyz;

	//do NdotL lighting for 2 lights
	for (int i = 0; i<2; i++)
	{
		toLight = normalize(vLightPos[i] - Position);
		finalColor += saturate(dot(toLight, input.Norm) * vLightColor[i]);
	}
	finalColor.a = 1;
	return finalColor;
}


//--------------------------------------------------------------------------------------
// PSSolid - render a solid color
//--------------------------------------------------------------------------------------
float4 PSSolid(PS_INPUT input) : SV_Target
{
	return vOutputColor;
}


//--------------------------------------------------------------------------------------
technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}


//--------------------------------------------------------------------------------------
technique10 RenderLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PSSolid()));
	}
}

