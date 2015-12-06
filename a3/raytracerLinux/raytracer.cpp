/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cassert>

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
			if (!ray.intersection.setMat){
				ray.intersection.mat = node->mat;		
			}
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
			Vector3D newDir = ray.intersection.point - curLight->light->get_position();
			newDir.normalize();
			Ray3D newRay(curLight->light->get_position(), newDir);

        	//compute new shading
        	traverseScene(_root, newRay);

        	if(!newRay.intersection.none && !newRay.intersection.point.isClose(ray.intersection.point)){
        		//std::cout << "old intersection!     " << ray.intersection.point << std::endl;
        		//std::cout << "new intersection!     " << newRay.intersection.point << std::endl;
        	    //in shadow
        	    ray.col = 0.6 * ray.col;
        	}
        }
	
		curLight = curLight->next;
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

Colour Raytracer::shadeRay( Ray3D& ray, int reflectionRecurance  ) {
	Colour col(0.0, 0.0, 0.0); 
	traverseScene(_root, ray); 
	
	/* Compute ray shading
 	 * if we need to recurse, then return blend, otherwise, return col
 	 */
 	 if(reflectionRecurance == 0 && !ray.intersection.none)
 	 {
 	 	//std::cout << "origin: " << ray.origin << "   intersection: " << ray.intersection.point << "    t_value: " << ray.intersection.t_value << std::endl;
 	 }

	if(!ray.intersection.none) {
		computeShading(ray);
		col = ray.col;

		if(reflectionRecurance > 0 && isSpecular(ray.intersection.mat)){

			Vector3D reflectedDir = reflect(ray);
			// change direction of ray
			Ray3D newRay(ray.intersection.point + EPSILON * reflectedDir, reflectedDir);

			//compute new shading
			Colour newCol = shadeRay(newRay, reflectionRecurance-1);
			
			col = 1 * ray.col + .1 * newCol;

			col.clamp();
		}
	}
	return col;

/*
	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {
		computeShading(ray); 
        // std::cout << "I am reflecting" << std::endl;
		
        reflectionRecurance--;
        Colour reflectedRayColour = shadeRay(reflectedRay, reflectionRecurance);
        if (isnan(reflectedRay.col[0]) ){
	       	std::cout << reflectedRayDir << std::endl;
	       	std::cout << reflectedRayColour << std::endl;
        }
        ray.col = 0 * ray.col + 1 * reflectedRayColour;
		col = ray.col;  
	}
*/
	// You'll want to call shadeRay recursively (with a different ray, 
	// of course) here to implement reflection/refraction effects.  

	return col; 
}

Vector3D Raytracer::reflect(Ray3D& ray){
	Vector3D view = -ray.dir;
    Vector3D reflectedRayDir = (2 * (view.dot(-ray.intersection.normal)) * -ray.intersection.normal) - view;
    //reflectedRayDir.normalize();

    std::cout << ray.intersection.normal.dot(ray.intersection.normal) << std::endl;
    std::cout << reflectedRayDir.dot(ray.dir) << std::endl;
    assert(ray.dir.dot(reflectedRayDir) != 0);

    return reflectedRayDir;
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
			
			Ray3D ray(origin, Vector3D(imagePlane[0], imagePlane[1], imagePlane[2]));
			ray.dir =  viewToWorld * ray.dir;
			ray.dir.normalize();
			ray.origin = viewToWorld * ray.origin;
			
			// one center ray per pixel
			// Colour col = shadeRay(ray, 0); 
			// anti aliasing by shooting multiple ray per pixel
			Colour col = shootMultiRayPerPixel(ray, 3, factor, 0);

			_rbuffer[i*width+j] = int(col[0]*255);
			_gbuffer[i*width+j] = int(col[1]*255);
			_bbuffer[i*width+j] = int(col[2]*255);
		}
	}

	flushPixelBuffer(fileName);
}

// double getStochasticOffset(double factor){
// 	return (((double) rand() / (RAND_MAX)) + 0.5)/factor
// }

Colour Raytracer::shootMultiRayPerPixel(Ray3D& centerRay, int rayNum, double factor, int reflectionRecurance){
	// shoot multiple rays around this ray, at most 4 rays
	if (rayNum == 4){
		double r1x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r1y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r3x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r3y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r4x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r4y= ((double) rand() / (RAND_MAX)) + 0.5;
		Ray3D ray1(centerRay.origin, Vector3D(centerRay.dir[0] + r1x/factor, centerRay.dir[1] + r1y/factor, centerRay.dir[2]));
		Ray3D ray2(centerRay.origin, Vector3D(centerRay.dir[0] + r2x/factor, centerRay.dir[1] + r2y/factor, centerRay.dir[2]));
		Ray3D ray3(centerRay.origin, Vector3D(centerRay.dir[0] + r3x/factor, centerRay.dir[1] + r3y/factor, centerRay.dir[2]));
		Ray3D ray4(centerRay.origin, Vector3D(centerRay.dir[0] + r4x/factor, centerRay.dir[1] + r4y/factor, centerRay.dir[2]));
		ray1.dir.normalize();
		ray2.dir.normalize();
		ray3.dir.normalize();
		ray4.dir.normalize();
		Colour col1 = shadeRay(ray1, reflectionRecurance);
		Colour col2 = shadeRay(ray2, reflectionRecurance);
		Colour col3 = shadeRay(ray3, reflectionRecurance);
		Colour col4 = shadeRay(ray4, reflectionRecurance);
		Colour finalCol = 0.25 * (col1 + col2 + col3 + col4);
		return finalCol;
	} else if (rayNum == 3) {
		double r1x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r1y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r1z = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2z = ((double) rand() / (RAND_MAX)) + 0.5;
		double r3x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r3y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r3z = ((double) rand() / (RAND_MAX)) + 0.5;
		Ray3D ray1(centerRay.origin, Vector3D(centerRay.dir[0] + r1x/factor, centerRay.dir[1] + r1y/factor, centerRay.dir[2] + r1z/factor));
		Ray3D ray2(centerRay.origin, Vector3D(centerRay.dir[0] + r2x/factor, centerRay.dir[1] + r2y/factor, centerRay.dir[2] + r2z/factor));
		Ray3D ray3(centerRay.origin, Vector3D(centerRay.dir[0] + r3x/factor, centerRay.dir[1] + r3y/factor, centerRay.dir[2] + r3z/factor));
		ray1.dir.normalize();
		ray2.dir.normalize();
		ray3.dir.normalize();
		Colour col1 = shadeRay(ray1, reflectionRecurance);
		Colour col2 = shadeRay(ray2, reflectionRecurance);
		Colour col3 = shadeRay(ray3, reflectionRecurance);
		Colour finalCol = (0.3333333) * (col1 + col2 + col3);
		return finalCol;
	} else if (rayNum == 2) {
		double r1x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r1y = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2x = ((double) rand() / (RAND_MAX)) + 0.5;
		double r2y = ((double) rand() / (RAND_MAX)) + 0.5;
		Ray3D ray1(centerRay.origin, Vector3D(centerRay.dir[0] + r1x/factor, centerRay.dir[1] + r1y/factor, centerRay.dir[2]));
		Ray3D ray2(centerRay.origin, Vector3D(centerRay.dir[0] + r2x/factor, centerRay.dir[1] + r2y/factor, centerRay.dir[2]));
		ray1.dir.normalize();
		ray2.dir.normalize();
		Colour col1 = shadeRay(ray1, reflectionRecurance);
		Colour col2 = shadeRay(ray2, reflectionRecurance);
		Colour finalCol = 0.5 * (col1 + col2);
		return finalCol;
	} else if (rayNum == 1) {
		centerRay.dir.normalize();
		Colour finalCol = shadeRay(centerRay, reflectionRecurance);
		return finalCol;
	} else {
		std::cout << "Error, reflectionRecurance is more then 4\n";
	}
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

	// // Defines a material for shading.
	Material gold( Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648), 
			Colour(0.628281, 0.555802, 0.366065), 
			51.2 );
	Material jade( Colour(0, 0, 0), Colour(0.54, 0.89, 0.63), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8 );
	Material checkboardWhite( Colour(0, 0, 0), Colour(0.9, 0.9, 0.9), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8 );
	Material checkboardBlack( Colour(0, 0, 0), Colour(0.1, 0.1, 0.1), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8 );
	// Material gold( Colour(0.3, 0.3, 0.3), Colour(0.23, 0.23, 0.23), 
	// 		Colour(0.516228, 0.516228, 0.516228), 
	// 		12.8 );
	// Material jade( Colour(0, 0, 0), Colour(0.23, 0.23, 0.23), 
	// 		Colour(0.516228, 0.516228, 0.516228), 
	// 		12.8 );

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 0, 5), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &gold );
	SceneDagNode* plane = raytracer.addObject( new UnitCheckboard(checkboardWhite, checkboardBlack), &jade );
	
	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -5));	
	raytracer.rotate(sphere, 'x', -45); 
	raytracer.rotate(sphere, 'z', 45); 
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

	raytracer.translate(plane, Vector3D(0, 0, -7));	
	raytracer.rotate(plane, 'z', 45); 
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	raytracer.render(width, height, eye, view, up, fov, "view1.bmp");
	
	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");
	
	return 0;
}

