#pragma once

#include "Colour.h"
#include <string>

class Material {
public:
	Material() :
		ambient(Colour()),
		diffuse(Colour()),
		specular(Colour()),
		specular_exp(0.0),
		isRefractive(false),
		refractiveIndex(0.0)
	{}

	Material(Colour ambient, Colour diffuse, Colour specular, double exp, bool isRefractive, double refractiveIndex) :
		ambient(ambient), 
		diffuse(diffuse), 
		specular(specular),
		specular_exp(exp), 
		isRefractive(isRefractive), 
		refractiveIndex(refractiveIndex)
	{}

	Material(Colour ambient, Colour diffuse, Colour specular, double exp) :
		ambient(ambient), 
		diffuse(diffuse), 
		specular(specular),
		specular_exp(exp), 
		isRefractive(false), 
		refractiveIndex(0.0)
	{}

	// Ambient components for Phong shading.
	Colour ambient;
	// Diffuse components for Phong shading.
	Colour diffuse;
	// Specular components for Phong shading.
	Colour specular;
	// Specular exponent.
	double specular_exp;

	bool isRefractive;

	double refractiveIndex;

	virtual Material* getMaterial(double s, double t) {
		return this;
	}

	std::string name;
};


class air : public Material {
public:
	air();
};

// // Defines a material for shading.
class gold : public Material {
public:
	gold();
};

class jade : public Material {
public:
	jade();
};

class bronze : public Material {
public:
	bronze();
};

class glass : public Material {
public:
	glass();
};

class checkerBoard : public Material {
public:
	Material* getMaterial(double s, double t);
};
