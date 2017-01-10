#include <cmath>
#include <algorithm>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {

    if (!ray.intersection.none){

        Colour I_a = _col_ambient;
        Colour I_d = _col_diffuse;
        Colour I_s = _col_specular;

        Colour r_a = ray.intersection.enteringMaterial->ambient;
        Colour r_d = ray.intersection.enteringMaterial->diffuse;
        Colour r_s = ray.intersection.enteringMaterial->specular;
        double alpha = ray.intersection.enteringMaterial->specular_exp;

        Colour r_g = Colour();
        Colour I_spec = Colour();

        Vector3D view = Vector3D(ray.origin - ray.intersection.point);

        Vector3D n = ray.intersection.normal;
        n.normalize();

        Vector3D s = Vector3D(get_position() - ray.intersection.point);
        s.normalize();

        Vector3D c = -ray.dir;
        c.normalize();

        Vector3D m = (2 * (s.dot(n)) * n) - s;
        m.normalize();

        Colour ambient = r_a * I_a;
        Colour diffuse = std::max(n.dot(s), 0.0) * r_d * I_d;
        Colour specular = pow(std::max(0.0, c.dot(m)), alpha) * r_s * I_s; 

        ray.col = ambient
        	+ diffuse
        	+ specular;

        ray.col.clamp();
    }
}

PointLight::PointLight(Point3D pos, Colour col) :
	_pos(pos), 
	_col_ambient(col),
	_col_diffuse(col), 
	_col_specular(col)
{

}

PointLight::PointLight(Point3D pos, Colour ambient, Colour diffuse, Colour specular) :
	_pos(pos), 
	_col_ambient(ambient), 
	_col_diffuse(diffuse),
	_col_specular(specular)
{

}