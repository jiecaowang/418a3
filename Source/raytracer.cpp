#define _USE_MATH_DEFINES
#include <cmath>

#include "Common.h"
#include "raytracer.h"
#include "bmp_io.h"
#include "Ray.h"
#include "Optics.h"
#include <iostream>
#include <time.h>

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


void SceneDagNode::translate(Vector3D trans) 
{
	Matrix4x4 translation;

	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	this->trans = this->trans * translation;
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	invtrans = translation*invtrans;
}

void SceneDagNode::scale(Point3D origin, double factor[3]) 
{
	Matrix4x4 scale;

	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	trans = trans*scale;

	scale[0][0] = 1 / factor[0];
	scale[0][3] = origin[0] - 1 / factor[0] * origin[0];
	scale[1][1] = 1 / factor[1];
	scale[1][3] = origin[1] - 1 / factor[1] * origin[1];
	scale[2][2] = 1 / factor[2];
	scale[2][3] = origin[2] - 1 / factor[2] * origin[2];
	invtrans = scale*invtrans;
}


void SceneDagNode::rotate(char axis, double angle)
{
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
		    trans = trans * rotation; 	
			angle = -angle;
		} 
		else {
			invtrans = rotation*invtrans; 
		}	
	}
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
			// need to figure out a way of exiting a material given this system
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
				Ray3D newRay(curLight->light->get_position(), newDir);

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

Colour Raytracer::shadeRay( Ray3D& ray, int recursiveRecurance) {
    ASSERT(ray.dir.isNormalized());
	if (recursiveRecurance == 0)
	{
		return ray.col;
	}

	Colour col(0.0, 0.0, 0.0); 
	traverseScene(_root, ray); 
	
	/* Compute ray shading
 	 * if we need to recurse, then return blend, otherwise, return col
 	 */
	if (ray.intersection.none)
	{
		return col;
	}
	if (ray.pTravelingThroughMaterial == ray.intersection.enteringMaterial)
	{
		ray.intersection.enteringMaterial = new air();
	}
	
	if(ray.intersection.enteringMaterial->GetRefractiveIndex() > 0){
		double incomingIndex = ray.pTravelingThroughMaterial->GetRefractiveIndex();
		double outgoingIndex = ray.intersection.enteringMaterial->GetRefractiveIndex();

		if (Optics::isCriticalAngle(ray.dir, ray.intersection.normal, incomingIndex, outgoingIndex)){
            // change direction of ray
            Vector3D reflectedDir = Optics::reflect(ray.dir, ray.intersection.normal);

            // send new ray with origin ray.origin + (offset * dir)
            Ray3D newRay(ray.intersection.point + g_epsilon * reflectedDir, reflectedDir);
            newRay.pTravelingThroughMaterial = ray.pTravelingThroughMaterial;

            //compute new shading
            col = shadeRay(newRay, recursiveRecurance - 1);

		} else {
            Vector3D refractedDir = Optics::refract(ray.dir, ray.intersection.normal, incomingIndex, outgoingIndex);
            Ray3D newRay(ray.intersection.point + g_epsilon * refractedDir, refractedDir);
			newRay.pTravelingThroughMaterial = ray.intersection.enteringMaterial;
			newRay.TraveledThroughMaterial.push(ray.pTravelingThroughMaterial);
            col = shadeRay(newRay, recursiveRecurance - 1);
		}
	} else {
		computeShading(ray);
		col = ray.col;

        if (ray.intersection.enteringMaterial->isSpecular()){

			Vector3D reflectedDir =  Optics::reflect(ray.dir, ray.intersection.normal);
			// change direction of ray
			Ray3D newRay(ray.intersection.point + g_epsilon * reflectedDir, reflectedDir);
			newRay.pTravelingThroughMaterial = ray.pTravelingThroughMaterial;
			//compute new shading
            Colour newCol = shadeRay(newRay, recursiveRecurance - 1);
				
			col = 1 * ray.col + .1 * newCol;
		}
	}

	return col; 
}

void Raytracer::render(Point3D eye, Vector3D view, Vector3D up, double fov) 
{
	Matrix4x4 viewToWorld;
	double factor = (double(_backBuffer->GetHeight()) / 2) / tan(fov*M_PI / 360.0);

	_backBuffer->InitRenderTarget();
	viewToWorld = initInvViewMatrix(eye, view, up);

	// Construct a ray for each pixel.
	for (int i = 0; i < _backBuffer->GetHeight(); i++) {
		for (int j = 0; j < _backBuffer->GetWidth(); j++) {
			// Sets up ray origin and direction in view space, 
			// image plane is at z = -1.
			Point3D origin(0, 0, 0);
			Point3D imagePlane;
			imagePlane[0] = (-double(_backBuffer->GetWidth())/2 + 0.5 + j)/factor;
			imagePlane[1] = (-double(_backBuffer->GetHeight())/2 + 0.5 + i)/factor;
			imagePlane[2] = -1;

			// Convert ray to world space and call 
			// shadeRay(ray) to generate pixel colour.
			
			Ray3D ray(origin, Vector3D(imagePlane[0], imagePlane[1], imagePlane[2]));
			ray.dir =  viewToWorld * ray.dir;
			ray.dir.normalize();
			ray.origin = viewToWorld * ray.origin;
			ray.pTravelingThroughMaterial = new air();
			
			// one center ray per pixel
			// anti aliasing by shooting multiple ray per pixel
			Colour col = shootRaysPerPixel(ray, 1);
			_backBuffer->SetPixelColour(i, j, col);
		}
	}
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
		Ray3D stochasticRay(centerRay.origin, centerRay.dir);
		stochasticRay.pTravelingThroughMaterial = centerRay.pTravelingThroughMaterial;
		stochasticRay.dir.normalize();
		Colour stochasticCol = shadeRay(stochasticRay, g_recursive_recurrance);
		sumCol = sumCol + stochasticCol;
		i--;
	}
	sumCol = (1.0/rayNum) * sumCol;
	return sumCol;
}

void Raytracer::SetRenderTarget(RenderTarget* backBuffer)
{
	_backBuffer = backBuffer;
}

void Raytracer::ShootDebugRay(Point3D origin, Vector3D dir)
{

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

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 0, 5), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject(new UnitSphere(), new glass());
    SceneDagNode* plane = raytracer.addObject(new UnitSquare(), new checkerBoard());
    //SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), new bronze());

	//cylinder->translate(Vector3D(-2, 2, -4));
	//cylinder->scale(Point3D(0, 0, 0), factor3);

	sphere->translate(Vector3D(0, 0, -5));
	sphere->rotate('x', -45);
	sphere->rotate('z', 45);

	plane->translate(Vector3D(0, 0, -17));	
	plane->rotate('z', 45); 
	double factor[3] = { 10.0, 10.0, 10.0 };
	plane->scale(Point3D(0, 0, 0), factor);

	RenderTarget backBuffer(height, width);
	raytracer.SetRenderTarget(&backBuffer);

	// Render the scene
	time_t start_timer, finish_time;

	{
		// Camera parameters.
		Point3D eye(0, 0, 1);
		Vector3D view(0, 0, -1);
		Vector3D up(0, 1, 0);
		double fov = 60;

		time(&start_timer);
		raytracer.render(eye, view, up, fov);
		time(&finish_time);

		std::cout << "done view 1 in " << difftime(finish_time, start_timer) << " seconds" << std::endl;
		bmp_write("view1.bmp", backBuffer.GetWidth(), backBuffer.GetHeight(),
			backBuffer.GetRedChannel(), backBuffer.GetGreenChannel(), backBuffer.GetBlueChannel());

		backBuffer.ClearRenderTarget();
	}
	
	{
		// Camera parameters.
		Point3D eye(4, 2, 1);
		Vector3D view(-4, -2, -6);
		Vector3D up(0, 1, 0);
		double fov = 60;

		// Render it from a different point of view.
		time(&start_timer);
		raytracer.render(eye, view, up, fov);
		time(&finish_time);

		std::cout << "done view 2 in " << difftime(finish_time, start_timer) << " seconds" << std::endl;
		bmp_write("view2.bmp", backBuffer.GetWidth(), backBuffer.GetHeight(),
			backBuffer.GetRedChannel(), backBuffer.GetGreenChannel(), backBuffer.GetBlueChannel());

		backBuffer.ClearRenderTarget();
	}

	return 0;
}
