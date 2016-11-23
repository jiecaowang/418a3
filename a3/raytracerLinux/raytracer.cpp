/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/

#define _USE_MATH_DEFINES

#include "Common.h"
#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <time.h>

#define sqr(a) pow(a, 2)

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
}

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans; 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, 
		Vector3D up ) {
	Matrix4x4 mat; 
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat; 
}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) {
	SceneDagNode *childPtr;

	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel; 
	if (node->obj) {
		// Perform intersection.
		if (node->obj->intersect(ray, _worldToModel, _modelToWorld)) {
			Point3D texturePoint(_worldToModel * ray.intersection.point);
			ray.intersection.enteringMaterial = node->mat->getMaterial(texturePoint[0], texturePoint[1]);
		}
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray);
		childPtr = childPtr->next;
	}

	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

void Raytracer::computeShading( Ray3D& ray ) {
	LightListNode* curLight = _lightSource;
	while (curLight != NULL) {
		// Each lightSource provides its own shading function.
		curLight->light->shade(ray);

		if(!ray.intersection.none){
			Colour totalColour;
			Colour currentColour = ray.col;
			for(int i = 0; i < 4; i++) {
				Vector3D newDir = ray.intersection.point - (curLight->light->get_position() + Vector3D(0, i*0.0000025, 0));
				newDir.normalize();
				Ray3D newRay(curLight->light->get_position(), newDir, 0.0);

        		//compute new shading
        		traverseScene(_root, newRay);

        		if(!newRay.intersection.none && !newRay.intersection.point.isClose(ray.intersection.point)){
        		    //in shadow
        	    	currentColour = 0.6 * currentColour;
        		}
        		totalColour = totalColour + currentColour;
        	}
        	totalColour = 0.25 * totalColour;
        	ray.col = totalColour;
        	ray.col.clamp();
		
		curLight = curLight->next;
		}
	}
}

void Raytracer::initPixelBuffer() {
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			_rbuffer[i*_scrWidth+j] = 0;
			_gbuffer[i*_scrWidth+j] = 0;
			_bbuffer[i*_scrWidth+j] = 0;
		}
	}
}

void Raytracer::flushPixelBuffer( char *file_name ) {
	bmp_write( file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}

Colour Raytracer::shadeRay( Ray3D& ray, int recursiveRecurance) {
	Colour col(0.0, 0.0, 0.0); 
	traverseScene(_root, ray); 
	
	/* Compute ray shading
 	 * if we need to recurse, then return blend, otherwise, return col
 	 */
	if(!ray.intersection.none) {
		if(ray.intersection.enteringMaterial->isRefractive){
            if (recursiveRecurance > 0){
				double incomingIndex;
				double outgoingIndex;

                incomingIndex = ray.refractiveIndex;
				outgoingIndex = ray.intersection.enteringMaterial->refractiveIndex;

				if (isNotCriticalAngle(ray, incomingIndex, outgoingIndex)){
                    Vector3D refractedDir = refract(ray, incomingIndex, outgoingIndex);
					Ray3D newRay(ray.intersection.point + EPSILON * refractedDir, refractedDir, outgoingIndex);
                    Colour newCol = shadeRay(newRay, recursiveRecurance-1);
					col = newCol;
					// std::cout << " New col: " << newCol << "\n";
					col.clamp();
				} else {
					Vector3D reflectedDir = reflect(ray);
					// change direction of ray
					Ray3D newRay(ray.intersection.point + EPSILON * reflectedDir, reflectedDir, incomingIndex);

					//compute new shading
                    Colour newCol = shadeRay(newRay, recursiveRecurance-1);
					
					col = newCol;

					col.clamp();	
				}
			}
		} else {
			computeShading(ray);
			col = ray.col;

            if (recursiveRecurance > 0 && isSpecular(ray.intersection.enteringMaterial)){

				Vector3D reflectedDir = reflect(ray);
				// change direction of ray
				Ray3D newRay(ray.intersection.point + EPSILON * reflectedDir, reflectedDir, ray.refractiveIndex);

				//compute new shading
                Colour newCol = shadeRay(newRay, recursiveRecurance - 1);
				
				col = 1 * ray.col + .1 * newCol;

				col.clamp();
			}
		}
	}

	return col; 
}

Vector3D Raytracer::reflect(Ray3D& ray){
	Vector3D view = -ray.dir;
    Vector3D reflectedRayDir = (2 * (view.dot(ray.intersection.normal)) * ray.intersection.normal) - view;
    assert(ray.dir.dot(reflectedRayDir) != 0);

    return reflectedRayDir;
}

bool Raytracer::isNotCriticalAngle( Ray3D& ray, double incomingIndex, double outgoingIndex ) {
	double criticalAngle = asin(outgoingIndex/incomingIndex);
	double incomingTheta = acos(-1 * ray.dir.dot(ray.intersection.normal));
	return (incomingTheta < criticalAngle);
}

Vector3D Raytracer::refract(Ray3D& ray, double incomingIndex, double outgoingIndex){
	Vector3D incoming = Vector3D(ray.dir);
    ASSERT(incoming.isNormalized());
	incoming.normalize();
	Vector3D normal = Vector3D(ray.intersection.normal);
	normal.normalize();

	double r = incomingIndex/outgoingIndex;
	double c = (normal).dot(incoming);

	Vector3D refractive = r * incoming + (r * c - sqrt(1 - pow(r, 2) * (1 - pow(c, 2)))) * -normal;
	refractive.normalize();

	return refractive; 		 
}		 

int Raytracer::isSpecular(Material* mat){
	return (mat->specular[0] * mat->specular[0] +  mat->specular[1] * mat->specular[1] + mat->specular[2] * mat->specular[2]) > 0.75;
}

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, 
		Vector3D up, double fov, char* fileName ) {
	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	double factor = (double(height)/2)/tan(fov*M_PI/360.0);

	initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);

	// Construct a ray for each pixel.
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			// Sets up ray origin and direction in view space, 
			// image plane is at z = -1.
			Point3D origin(0, 0, 0);
			Point3D imagePlane;
			imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor;
			imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor;
			imagePlane[2] = -1;

			// TODO: Convert ray to world space and call 
			// shadeRay(ray) to generate pixel colour.
			
			Ray3D ray(origin, Vector3D(imagePlane[0], imagePlane[1], imagePlane[2]), 1.0);
			ray.dir =  viewToWorld * ray.dir;
			ray.dir.normalize();
			ray.origin = viewToWorld * ray.origin;
			
			// one center ray per pixel
			// Colour col = shadeRay(ray, 0, 0); 
			// anti aliasing by shooting multiple ray per pixel
			Colour col = shootRaysPerPixel(ray, 1);

			_rbuffer[i*width+j] = int(col[0]*255);
			_gbuffer[i*width+j] = int(col[1]*255);
			_bbuffer[i*width+j] = int(col[2]*255);
		}
	}

	flushPixelBuffer(fileName);
}

Vector3D Raytracer::getStochasticOffset(double factor){
	double xOffset = (((double) rand() / (RAND_MAX)) + 0.5)/factor;
	double yOffset = (((double) rand() / (RAND_MAX)) + 0.5)/factor;
	double zOffset = (((double) rand() / (RAND_MAX)) + 0.5)/factor;
	return Vector3D(xOffset, yOffset, zOffset);
}

Colour Raytracer::shootRaysPerPixel(Ray3D& centerRay, int rayNum){
	// shoot multiple stochastic rays around this ray
	
	Colour sumCol(0.0, 0.0, 0.0); 
	int	i = rayNum;
	while(i > 0){
		Ray3D stochasticRay(centerRay.origin, centerRay.dir, centerRay.refractiveIndex);
		stochasticRay.dir.normalize();
		Colour stochasticCol = shadeRay(stochasticRay, RECURSIVE_RECURRANCE);
		sumCol = sumCol + stochasticCol;
		i--;
	}
	sumCol = (1.0/rayNum) * sumCol;
	return sumCol;
}

int main(int argc, char* argv[])
{	
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;
	int width = 600; 
	int height = 400; 

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 0, 5), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), new glass());
    SceneDagNode* plane = raytracer.addObject(new UnitSquare(), new checkerBoard());
    //SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), new bronze());


	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };

	double factor2[3] = { 10.0, 10.0, 10.0 };
	double factor3[3] = { 1.0, 1.0, 1.0 };

	//raytracer.translate(cylinder, Vector3D(-2, 2, -4));
	//raytracer.scale(cylinder, Point3D(0, 0, 0), factor3);

	raytracer.translate(sphere, Vector3D(0, 0, -5));
	raytracer.rotate(sphere, 'x', -45); 
	raytracer.rotate(sphere, 'z', 45); 

	raytracer.translate(plane, Vector3D(0, 0, -7));	
	raytracer.rotate(plane, 'z', 45); 
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
    time_t start_timer;
    time(&start_timer);

	raytracer.render(width, height, eye, view, up, fov, "view1.bmp");
	
    time_t finish_time;
    time(&finish_time);
    std::cout << "done view 1 in " << difftime(finish_time, start_timer) <<  " seconds" <<std::endl;

	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);

    time(&start_timer);
	raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");
    time(&finish_time);
    std::cout << "done view 1 in " << difftime(finish_time, start_timer) << " seconds" << std::endl;
	
	return 0;
}

