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
	
	// H is defined in: http://www.dgp.toronto.edu/~karan/courses/418/Lectures/lecture6.pdf
    if (!ray.intersection.none){

        Vector3D view = Vector3D(ray.origin[0] - ray.intersection.point[0], ray.origin[1] - ray.intersection.point[1], ray.origin[2] - ray.intersection.point[2]);
        Vector3D incident =  -1 *  ray.dir;
        Vector3D H = view + incident;
        H.normalize();

        Colour ambient = _col_ambient * ray.intersection.mat->ambient;
        Colour diffuse = (ray.intersection.normal.dot(incident)) * (_col_diffuse * ray.intersection.mat->diffuse);
        //diffuse.clamp();
        Colour specular = pow(ray.intersection.normal.dot(H), ray.intersection.mat->specular_exp) * (_col_specular * ray.intersection.mat->specular);

        // std::cout << "specular Before:\t\t" << s << "\n";
        //specular.clamp();
        // std::cout << "specular After:\t\t" << s << "\n";

        ray.col =  ambient  + diffuse +  specular;
        ray.col.clamp();

            //std::cout << "error" << std::endl;
        
    }
	

	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

}

