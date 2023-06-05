#pragma once

#include "Euclidean.h"
#include "Ray.h"

// Base class for a light source.  You could define different types
// of lights here, but point light is sufficient for most scenes you
// might want to render.  Different light sources shade the ray 
// differently.
class LightSource {
public:
	virtual ~LightSource() = default;
	virtual void shade( Ray3D& ) = 0;
	virtual Point3D get_position() const = 0; 
};

// A point light is defined by its position in world space and its
// colour.
class PointLight : public LightSource {
public:
    PointLight(Point3D pos, Colour col);
    PointLight(Point3D pos, Colour ambient, Colour diffuse, Colour specular);
	virtual ~PointLight() = default;
	void shade( Ray3D& ray );
	Point3D get_position() const { return _pos; }
	
private:
	Point3D _pos;
	Colour _col_ambient;
	Colour _col_diffuse; 
	Colour _col_specular; 
};
