#include "Surface.h"

RenderTarget::RenderTarget(int Height, int Width)
{
	_Height = Height;
	_Width = Width;
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::InitRenderTarget()
{
	int BufferSize = _Height * _Width;
	_rBuffer.resize(BufferSize, 0);
	_gBuffer.resize(BufferSize, 0);
	_bBuffer.resize(BufferSize, 0);
}

void RenderTarget::SetPixelColour(int x, int y, Colour pixelColour)
{
	double maxChannel = 255;
	// guarantee pixelColour is between 0 and 1.
	_rBuffer[x*_Width + y] = unsigned char(pixelColour[0] * maxChannel);
	_gBuffer[x*_Width + y] = unsigned char(pixelColour[1] * maxChannel);
	_bBuffer[x*_Width + y] = unsigned char(pixelColour[2] * maxChannel);
}

unsigned char* RenderTarget::GetRedChannel()
{
	return _rBuffer.data();
}

unsigned char* RenderTarget::GetGreenChannel()
{
	return _gBuffer.data();
}

unsigned char* RenderTarget::GetBlueChannel()
{
	return _bBuffer.data();
}

void RenderTarget::ClearRenderTarget()
{
	_rBuffer.clear();
	_gBuffer.clear();
	_bBuffer.clear();
}

int RenderTarget::GetHeight()
{
	return _Height;
}

int RenderTarget::GetWidth()
{
	return _Width;
}
