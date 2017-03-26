#pragma once

#include <vector>
#include "Colour.h"

class RenderTarget
{
public:
	RenderTarget(int Height, int Width);
	~RenderTarget();

	void InitRenderTarget();
	void SetPixelColour(int x, int y, Colour pixelColour);
	void ClearRenderTarget();

	unsigned char* GetRedChannel();
	unsigned char* GetGreenChannel();
	unsigned char* GetBlueChannel();

	int GetHeight();
	int GetWidth();

private:
	int _Height;
	int _Width;

	std::vector<unsigned char> _rBuffer;
	std::vector<unsigned char> _gBuffer;
	std::vector<unsigned char> _bBuffer;
};