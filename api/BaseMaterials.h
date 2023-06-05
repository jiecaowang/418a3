#pragma once
#include "Colour.h"

class Material {
public:
	Material();
	Material(Colour ambient, Colour diffuse, Colour specular, double exp);
	Material(Colour ambient, Colour diffuse, Colour specular, double exp, double refractiveIndex);

	// Overload for mixed textures
	virtual Material* getMaterial(double s, double t);

	// set name of the material
	void SetName(std::string name);

	void SetAmbient(Colour ambient);
	void SetDiffuse(Colour diffuse);
	void SetSpecular(Colour specular, double specular_exponent);
	void SetRefraction(double refractiveIndex);

	Colour GetAmbient();
	Colour GetDiffuse();
	Colour GetSpecular();
	double GetSpecularExponent();
	double GetRefractiveIndex();

	bool isSpecular();

	friend std::ostream& operator <<(std::ostream& os, const Material& M);

private:
	// Ambient components for Phong shading.
	Colour _ambient;
	// Diffuse components for Phong shading.
	Colour _diffuse;
	// Specular components for Phong shading.
	Colour _specular;
	// Specular exponent.
	double _specular_exp;

	double _refractiveIndex;

	// saved isSpecularValue to save on costly square computations
	bool _isSpecular;

	std::string _name;
};

