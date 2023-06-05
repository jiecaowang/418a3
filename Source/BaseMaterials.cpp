#include "BaseMaterials.h"
#include "Common.h"

Material::Material() :
Material(Colour(), Colour(), Colour(), 0.0, 0.0)
{
}

Material::Material(Colour ambient, Colour diffuse, Colour specular, double exp) :
Material(ambient, diffuse, specular, exp, 0.0)
{
}

Material::Material(Colour ambient, Colour diffuse, Colour specular, double exp, double refractiveIndex) :
_ambient(ambient),
_diffuse(diffuse),
_specular(specular),
_specular_exp(exp),
_refractiveIndex(refractiveIndex)
{
	_isSpecular = (sqr(_specular[0]) + sqr(_specular[1]) + sqr(_specular[2])) > 0.75;
}

Material* Material::getMaterial(double s, double t)
{
	return this;
}

void Material::SetName(std::string name)
{
	_name = name;
}


void Material::SetAmbient(Colour ambient)
{
	_ambient = ambient;
}

void Material::SetDiffuse(Colour diffuse)
{
	_diffuse = diffuse;
}

void Material::SetSpecular(Colour specular, double specular_exponent)
{
	_specular = specular;
	_specular_exp = specular_exponent;
	_isSpecular = (sqr(_specular[0]) + sqr(_specular[1]) + sqr(_specular[2])) > 0.75;
}

void Material::SetRefraction(double refractiveIndex)
{
	_refractiveIndex = refractiveIndex;
}

Colour Material::GetAmbient()
{
	return _ambient;
}

Colour Material::GetDiffuse()
{
	return _diffuse;
}

Colour Material::GetSpecular()
{
	return _specular;
}

double Material::GetRefractiveIndex()
{
	return _refractiveIndex;
}

double Material::GetSpecularExponent()
{
	return _specular_exp;
}

bool Material::isSpecular()
{
	return _isSpecular;
}

std::ostream& operator <<(std::ostream& os, const Material& M) 
{
	return os << "M(" <<
		"A( " << M._ambient << ") " <<
		"D( " << M._diffuse << ") " <<
		"S( " << M._specular << ") " <<
		"shiny( " << M._specular_exp << ") " <<
		") ";
}
