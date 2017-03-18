#include "Materials.h"

air::air()
{
	SetName("Air");
	SetRefraction(1.0);
}

gold::gold()
{
	SetName("Gold");
	SetAmbient(Colour(0.3, 0.3, 0.3));
	SetDiffuse(Colour(0.75164, 0.60648, 0.22648));
	SetSpecular(Colour(0.628281, 0.555802, 0.366065), 51.2);
}

glass::glass()
{
	SetName("Glass");
	SetRefraction(1.30);
}

bronze::bronze()
{
	SetName("Bronze");
	SetAmbient(Colour(0.19125, 0.0735, 0.0225));
	SetDiffuse(Colour(0.7038, 0.27048, 0.0828));
	SetSpecular(Colour(0.8, 0.8, 0.8), 0.1 * 128);
}

jade::jade()
{
	SetName("Jade");
	SetAmbient(Colour(0.0, 0.0, 0.0));
	SetDiffuse(Colour(0.54, 0.89, 0.63));
	SetSpecular(Colour(0.316228, 0.316228, 0.316228), 12.8);
}

checkerBoard::checkerBoard(){
	White.SetAmbient(Colour(0.0, 0.0, 0.0));
	White.SetDiffuse(Colour(0.9, 0.9, 0.9));
	White.SetSpecular(Colour(0.316228, 0.316228, 0.316228), 12.8);

	Black.SetAmbient(Colour(0.0, 0.0, 0.0));
	Black.SetDiffuse(Colour(0.1, 0.1, 0.1));
	Black.SetSpecular(Colour(0.316228, 0.316228, 0.316228), 12.8);
}

Material* checkerBoard::getMaterial(double s, double t){
	int xTest = static_cast<int>(floor((s + 0.5) * 10)) % 2;
	int yTest = static_cast<int>(floor((t + 0.5) * 10)) % 2;
	return (xTest == yTest) ? &White : &Black;
}
