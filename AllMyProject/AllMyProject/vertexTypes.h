//create a basic vertex type
struct vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR3 normal;

	vertex(D3DXVECTOR3 p, D3DXVECTOR4 c)
	{
		pos = p;
		color = c;
	}
	vertex()
	{
	}

	vertex(D3DXVECTOR3 p, D3DXVECTOR4 c, D3DXVECTOR2 tex)
	{
		pos = p;
		color = c;
		texCoord = tex;
	}


	vertex(D3DXVECTOR3 p, D3DXVECTOR4 c, D3DXVECTOR2 tex , D3DXVECTOR3 norm)
	{
		pos = p;
		color = c;
		texCoord = tex;
		normal = norm;
	}

};

const D3D10_INPUT_ELEMENT_DESC vertexInputLayout[] =
//define input layout
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};