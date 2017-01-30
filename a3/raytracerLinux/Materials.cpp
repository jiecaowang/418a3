#include "Materials.h"

air::air(){
	name = "Air";
	isRefractive = true;
	refractiveIndex = 1.0;
}

gold::gold(){
	// Ambient components for Phong shading.
	ambient = Colour(0.3, 0.3, 0.3);
	// Diffuse components for Phong shading.
	diffuse = Colour(0.75164, 0.60648, 0.22648);
	// Specular components for Phong shading.
	specular = Colour(0.628281, 0.555802, 0.366065);
	// Specular expoent.
	specular_exp = 51.2;
}

glass::glass(){
	name = "Glass";
	// Ambient components for Phong shading.
	ambient = Colour(0.0, 0.0, 0.0);
	// Diffuse components for Phong shading.
	diffuse = Colour(0.0, 0.0, 0.0);
	// Specular components for Phong shading.
	specular = Colour(0.0, 0.0, 0.0);
	// Specular expoent.
	specular_exp = 0;

	isRefractive = true;

	refractiveIndex = 1.00;
}

bronze::bronze(){
	ambient = Colour(0.19125, 0.0735, 0.0225);
	diffuse = Colour(0.7038, 0.27048, 0.0828);
	specular = Colour(0.8, 0.8, 0.8);
	specular_exp = 0.1 * 128;
}

jade::jade(){

	ambient = Colour(0, 0, 0);
	// Diffuse components for Phong shading.
	diffuse = Colour(0.54, 0.89, 0.63);
	// Specular components for Phong shading.
	specular = Colour(0.316228, 0.316228, 0.316228);
	// Specular expoent.
	specular_exp = 12.8;
}


Material* checkerBoard::getMaterial(double s, double t){
	int xTest = ((int)floor((s + 0.5) * 10)) % 2;
	int yTest = ((int)floor((t + 0.5) * 10)) % 2;
	if (xTest == yTest){
		return (new Material(Colour(0, 0, 0), Colour(0.9, 0.9, 0.9),
			Colour(0.316228, 0.316228, 0.316228),
			12.8)); // White
	}
	else {
		return (new Material(Colour(0, 0, 0), Colour(0.1, 0.1, 0.1),
			Colour(0.316228, 0.316228, 0.316228),
			12.8)); //Black
	}
}

std::ostream& operator <<(std::ostream& os, const Material& M) {
	return os << "M(" <<
		"A( " << M.ambient << ") " <<
		"D( " << M.diffuse << ") " <<
		"S( " << M.specular << ") " <<
		"shiny( " << M.specular_exp << ") " <<
		") ";
}