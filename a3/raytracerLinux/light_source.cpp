/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	// std::cout << "trying to shade" << std::endl;
	// std::cout << "type\t\t\t\tray\t\t\t\tlight" << std::endl;
	// std::cout << "ambient:\t\t" << ray.intersection.mat->ambient << "\t\t\t\t" << _col_ambient << std::endl;
	// std::cout << "diffuse:\t\t" << ray.intersection.mat->diffuse << "\t\t" << _col_diffuse << std::endl;
	// std::cout << "specular:\t\t" << ray.intersection.mat->specular << "\t\t" << _col_specular << std::endl;
	// std::cout << std::endl;

	ray.col = _col_ambient * ray.intersection.mat->ambient + Colour(0.3, 0.3, 0.3);
	ray.col.clamp();

	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

}

