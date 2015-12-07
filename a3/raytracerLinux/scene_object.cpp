/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

#define sqr(x) pow(x, 2)


bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// Theory: http://www.cs.toronto.edu/~liviu/CSC418_Tutorials_F2013/T7-2.jpg
	// Code: http://www.cs.toronto.edu/~liviu/CSC418_Tutorials_F2013/T7-3.jpg
	
	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;
	double t = -(rayInModel.origin[2])/rayInModel.dir[2];
	if(t < 0){
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
			return false;
		} else if(t1 > 0 && t2 < 0) {
			t_value = t1;
		} else if(t1 > t2 && t2 > 0) {
			t_value = t2;
		}
	}


	if (!ray.intersection.none && ray.intersection.t_value < t_value ){
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

bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	//src = https://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html

	Ray3D rayInModel;
	rayInModel.origin = worldToModel * ray.origin;
	rayInModel.dir = worldToModel * ray.dir;

	double A = sqr(rayInModel.dir[0])  + sqr(rayInModel.dir[1]);
	double B = (rayInModel.dir[0] * rayInModel.origin[0] + rayInModel.dir[1] * rayInModel.origin[1]);
	double C = sqr(rayInModel.origin[0]) + sqr(rayInModel.origin[1]) - 1;


	double t_value;
	double soln = B*B - A*C;
	/* intersection with infinite cylinder */
	if(soln < 0){
		return false; // No intersections
	} else if (soln == 0){
		t_value = -B/A;
	} else {
		double t1 = (-B + sqrt(soln))/A;
		double t2 = (-B - sqrt(soln))/A;
		if(t1 <= 0 && t2 < 0) {
			return false;
		} else if(t1 > 0 && t2 < 0) {
			t_value = t1;
		} else if(t1 > t2 && t2 > 0)
			t_value = t2;
	}

	/* now we bound by a plane at the top of the cylinder and a plane at the bottom */
	Point3D candidatePoint = rayInModel.origin + t_value * rayInModel.dir;
	Vector3D candidateNormal = candidatePoint - Point3D(0,0, candidatePoint[2]);
	if(candidatePoint[2] > 0.5){

		/* check plane at 0,0,0.5 */
		if(rayInModel.dir.dot(Vector3D(0,0,1)) == 0)
			return false; // If the normal and vector are perpendicular

		t_value = (Point3D(0, 0, 0.5) - rayInModel.origin).dot(Vector3D(0,0,1)) / (rayInModel.dir.dot(Vector3D(0,0,1)));
		candidatePoint = rayInModel.origin + t_value * rayInModel.dir;
		candidateNormal = Vector3D(0,0,1);

		if((sqr(candidatePoint[0]) + sqr(candidatePoint[1])) > 1)
			return false; // Make sure we're within the circle
		
			
	} else if(candidatePoint[2] < -0.5) {
		/* check plane at 0, 0, -1 */
		if(rayInModel.dir.dot(Vector3D(0,0,-1)) == 0)
			return false;

		t_value = (Point3D(0, 0, -0.5) - rayInModel.origin).dot(Vector3D(0,0,-1)) / (rayInModel.dir.dot(Vector3D(0,0,-1)));
		candidatePoint = rayInModel.origin + t_value * rayInModel.dir;
		candidateNormal = Vector3D(0,0,-1);

		if((sqr(candidatePoint[0]) + sqr(candidatePoint[1])) > 1)
			return false;
		
	}

	if (!ray.intersection.none && ray.intersection.t_value < t_value )
	    return false;
	

	ray.intersection.none = false;
	ray.intersection.t_value = t_value;
    ray.intersection.point = candidatePoint;
	ray.intersection.normal = candidateNormal;
	ray.intersection.normal = worldToModel.transpose() * ray.intersection.normal;
	ray.intersection.normal.normalize();
	ray.intersection.point = modelToWorld * ray.intersection.point;

	return true;
}