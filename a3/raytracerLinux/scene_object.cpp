/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	// 
	// Theory: http://www.cs.toronto.edu/~liviu/CSC418_Tutorials_F2013/T7-2.jpg
	// Code: http://www.cs.toronto.edu/~liviu/CSC418_Tutorials_F2013/T7-3.jpg
	
	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;
	double t = -(rayInModel.origin[2])/rayInModel.dir[2];
	// if(t <= 0){
	// 	return false;
	// }
	std::cout << "intersecting unit square";
	double xInModel = rayInModel.origin[0] + t * rayInModel.dir[0];
	double yInModel = rayInModel.origin[1] + t * rayInModel.dir[1];

	if(-0.5 <= xInModel && xInModel <= 0.5 && -0.5 <= yInModel && yInModel <= 0.5){
		// An intersection has occured, now check if we should update
		if (rayInModel.intersection.none || t < ray.intersection.t_value){
			// this unit square is the front most one to be intersected
			// so we are update
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * Point3D(xInModel, yInModel, 0);
			Vector3D newNormal = worldToModel.transpose() * Vector3D(0, 0, 1);
			newNormal.normalize();
			ray.intersection.normal = newNormal;
			ray.intersection.none = false;
			std::cout << ray.intersection.point;
			return true;
		}
	}
	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	//Src=http://www.ahinson.com/algorithms_general/Sections/Geometry/IntersectionOfParametricLineAndSphere.pdf

	if(!ray.intersection.none)
		std::cout << ray.intersection.none << std::endl;

	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;

	double A = rayInModel.dir.dot(rayInModel.dir);
	double B = 2 * rayInModel.dir.dot(rayInModel.origin - Point3D());
	double C = (rayInModel.origin - Point3D()).dot(rayInModel.origin - Point3D()) - 1;

	double soln = B*B - 4*A*C;
	if(soln > 0){
		double t1 = (-B + sqrt(soln))/(2*A);
		double t2 = (-B - sqrt(soln))/(2*A);
		ray.intersection.none = false;
		ray.intersection.point = ray.origin + t1 * ray.dir;
		ray.intersection.normal = Vector3D(ray.intersection.point[0], ray.intersection.point[1], ray.intersection.point[2]); 
		ray.intersection.t_value = t1;

		ray.intersection.point = modelToWorld * ray.intersection.point;
		ray.intersection.normal = modelToWorld * ray.intersection.normal;
		ray.intersection.normal.normalize();

		std::cout << "intersection: " << ray.intersection.point << std::endl;
		std::cout << "normal: " << ray.intersection.normal << std::endl;
		std::cout << "t_val: " << ray.intersection.t_value << std::endl;
		std::cout << std::endl;
		return true;
	}


	
	return false;
}

