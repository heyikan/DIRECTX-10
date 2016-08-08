# Direcx 10 - FONT Example

```c
ID3DX10Font*				Font;
LPD3DX10SPRITE			ppSprite;
```

![font buffer problem](.//img//fontbufferproblem.png)

* __You will notice we have re-set our depth/stencil state, input layout, and topology. We need to do this because a call to the ID3DX10Font::DrawText() function changes all that. Not only does it change that, it changes the geometry shader in the effect file, so we need to remember to reset the geometry shader too, which we will do soon.__

* to solve problem save depthstencilstate before drawing text.
```c
ID3D10DepthStencilState* dss; UINT foo = 1; g_pD3DDevice->OMGetDepthStencilState(&dss, &foo);
//drawing text
.
.
.
//end drawing text
g_pD3DDevice->OMSetDepthStencilState(dss, 1);
```

* To drawing text:

#### Initialize state:

```c
HRESULT dxManager::createFont()
{
	D3DX10_FONT_DESC fd;
	fd.Height = 20;
	fd.Width= 0;
	fd.Weight = 0;
	fd.MipLevels = 1;
	fd.Italic = false;
	fd.CharSet = OUT_TT_PRECIS;
	fd.Quality = DEFAULT_QUALITY;
	fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	//wcscpy(fd.FaceName, L"Impact");
	strcpy(fd.FaceName, "Cambria");
	hr = D3DX10CreateFontIndirect(g_pD3DDevice, &fd, &Font);

	if (FAILED(hr))
		fatalError("Font Error!!");

	//D3DX10CreateFont(g_pD3DDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &Font);
	hr = D3DX10CreateSprite(g_pD3DDevice, 256, &ppSprite);
	if (FAILED(hr))
		fatalError("Font Sprite Error!!");
	return S_OK;
}

```

```c
ppSprite->Begin(D3DX10_SPRITE_SAVE_STATE);
Font->DrawText(0, "Hello World!", -1, &rectangle, DT_NOCLIP, fontColor);
ppSprite->End();
```

![buffer problem solved](.//img//fontwithbuffer.png)

![Font Final](.//img//fontfinal.png)
