/******************************************************************************
Class:Ray
Implements:
Author:Rich Davison
Description:A simple Ray class. 

A ray is a line in space. It has an origin, and a direction vector. The line
extends infinitely along the direction vector. 

We're going to cast Rays out of the mouse pointer and into our 3D world - any
objects that the Ray passes through (calculated using a Ray / Bounding Sphere
intersection test) is said to be 'hit' or 'intersected' by the ray. In this
demo, such objects will be coloured blue. This class will have to be extended
to perform ray/AABB or ray/OOBB intersection tests, if SceneNode is extended
out beyond simple bounding spheres.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   Who is Ray, anyway?
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../../NCLGL/Vector3.h"
#include "../../NCLGL/SceneNode.h"

#include <vector>
using namespace std;

/*
SceneNodes that are hit by the ray are stored in this simple struct.
*/
struct RayCollision	{
	SceneNode* node;	//Node that was hit
	Vector3 collidedAt; //WORLD SPACE position of the collision!

	RayCollision::RayCollision(SceneNode*node, Vector3 collidedAt) {
		this->node = node;
		this->collidedAt = collidedAt;
	}
};

class Ray	{
public:
	Ray(Vector3 position, Vector3 direction);
	~Ray(void);

	/*
	Tests a SceneNode for a ray collision, using the node's bounding sphere,
	as introduced in the scene management tutorial. 
	This function will test the passed in node's children, and so on,
	so we can test an entire scene graph from its root node!
	*/
	bool	IntersectsNodes(SceneNode &root);

	//Should be encapsulated, really...
	//This holds which nodes have been hit by a Ray, and where.
	vector<RayCollision> collisions;

protected:
	/*
	Function for the actual ray / sphere intersection test. Returns true
	if there's been an intersection
	*/
	bool	IntersectsNode(SceneNode &node, Vector3 &intersectPos);

	bool collided;		//Has this node collided with anything?

	Vector3 position;	//World space position
	Vector3 direction;	//Normalised world space direction
};

