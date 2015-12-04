/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include <algorithm>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	// std::cout << "trying to shade" << std::endl;
	// std::cout << "type\t\t\t\tray\t\t\t\tlight" << std::endl;
	// std::cout << "ambient:\t\t" << ray.intersection.mat->ambient << "\t\t\t\t" << _col_ambient << std::endl;
	// std::cout << "diffuse:\t\t" << ray.intersection.mat->diffuse << "\t\t" << _col_diffuse << std::endl;
	// std::cout << "specular:\t\t" << ray.intersection.mat->specular << "\t\t" << _col_specular << std::endl;
	// std::cout << std::endl;
	
	// Everything taken from http://www.dgp.toronto.edu/~karan/courses/418/Notes/BasicRayTracing.pdf
	
    if (!ray.intersection.none){

        Colour I_a = _col_ambient;
        Colour I_d = _col_diffuse;
        Colour I_s = _col_specular;

        Colour r_a = ray.intersection.mat->ambient;
        Colour r_d = ray.intersection.mat->diffuse;
        Colour r_s = ray.intersection.mat->specular;
        double alpha = ray.intersection.mat->specular_exp;

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
	

	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

}

