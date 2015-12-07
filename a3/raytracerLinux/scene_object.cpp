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
	if(t-EPSILON < 0){
		return false;
	}
	
	double xInModel = rayInModel.origin[0] + t * rayInModel.dir[0];
	double yInModel = rayInModel.origin[1] + t * rayInModel.dir[1];
	// Making check board

	if(-0.5 <= xInModel && xInModel <= 0.5 && -0.5 <= yInModel && yInModel <= 0.5){
		//std::cout << "intersection! tvalue: " << t << std::endl;
		// An intersection has occured, now check if we should update
		if (ray.intersection.none || t < ray.intersection.t_value){
			// this unit square is the front most one to be intersected
			// so we are update
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * Point3D(xInModel, yInModel, 0);
			Vector3D newNormal = transNorm(worldToModel, Vector3D(0, 0, 1));
			newNormal.normalize();
			ray.intersection.normal = newNormal;
			ray.intersection.none = false;
			return true;
		}
	}
	return false;
}

bool UnitCheckboard::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;
	double t = -(rayInModel.origin[2])/rayInModel.dir[2];
	if(t-EPSILON < 0){
		return false;
	}
	
	double xInModel = rayInModel.origin[0] + t * rayInModel.dir[0];
	double yInModel = rayInModel.origin[1] + t * rayInModel.dir[1];
	

	if(-0.5 <= xInModel && xInModel <= 0.5 && -0.5 <= yInModel && yInModel <= 0.5){
		//std::cout << "intersection! tvalue: " << t << std::endl;
		// An intersection has occured, now check if we should update
		if (ray.intersection.none || t < ray.intersection.t_value){
			// this unit square is the front most one to be intersected
			// so we are update
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * Point3D(xInModel, yInModel, 0);
			Vector3D newNormal = transNorm(worldToModel, Vector3D(0, 0, 1));
			newNormal.normalize();
			ray.intersection.normal = newNormal;
			ray.intersection.none = false;
			// set material for check board, since we know this is an intersection on checkboard
			// making 10 by 10 grid
			int xTest = ((int) floor((xInModel + 0.5) * 20)) % 2;
			int yTest = ((int) floor((yInModel + 0.5) * 20)) % 2;
			if (xTest == yTest){
				ray.intersection.mat = &whiteCellMat;
			} else {
				ray.intersection.mat = &blackCellMat;
			}
			ray.intersection.setMat = true; 
			return true;
		}
	}
	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	//Src=http://www.ahinson.com/algorithms_general/Sections/Geometry/IntersectionOfParametricLineAndSphere.pdf

	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;

	double A = rayInModel.dir.dot(rayInModel.dir);
	double B = rayInModel.dir.dot(rayInModel.origin - Point3D());
	double C = (rayInModel.origin - Point3D()).dot(rayInModel.origin - Point3D()) - 1;


	double t_value;
	double soln = B*B - A*C;
	if(soln < 0){
		return false; // No intersections
	} else if (soln == 0){
		t_value = -B/A;
	} else {
		double t1 = (-B + sqrt(soln))/A;
		double t2 = (-B - sqrt(soln))/A;
		if(t1 <= 0 && t2 < 0) {
			//std::cout << "no intersection" << std::endl;
			return false;
		} else if(t1 > 0 && t2 < 0) {
			// std::cout << "using t1: " << t1 << std::endl;
			t_value = t1;
		} else if(t1 > t2 && t2 > 0) {
			// std::cout << "using t2: " << t2 << std::endl;
			t_value = t2;
		} else {
			std::cout << "ERROR t1: " << t1 << "  t2: " << t2  << std::endl;
		}

		//std::cout << "t1 " << t1 << "   t2 " << t2 << std::endl;
		//std::cout << t_value << std::endl;
	}

	//Point3D pointDiff = ray.intersection.point - (rayInModel.origin + t_value * rayInModel.dir);

	if (!ray.intersection.none && ray.intersection.t_value < t_value ){
		std::cout << "false alarm. current value > t_value " << std::endl;
	    return false;
	}

	ray.intersection.none = false;
	ray.intersection.t_value = t_value;
    ray.intersection.point = rayInModel.origin + t_value * rayInModel.dir;
	ray.intersection.normal = ray.intersection.point - Point3D();
	ray.intersection.normal = worldToModel.transpose() * ray.intersection.normal;
	ray.intersection.normal.normalize();
	ray.intersection.point = modelToWorld * ray.intersection.point;

	return true;
}

