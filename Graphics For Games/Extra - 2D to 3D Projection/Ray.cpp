#include "Ray.h"

Ray::Ray(Vector3 position, Vector3 direction)	{
	collided		= false;
	this->position	= position;
	this->direction = direction;
}


Ray::~Ray(void)	{
}

/*
Tests an entire SceneGraph for ray/sphere collisions. Any nodes that are
hit by the ray will be added to the Ray's collision vector!
*/
bool	Ray::IntersectsNodes(SceneNode &root) {
	Vector3 collision;

	//Do the root node first
	IntersectsNode(root,collision);

	//In a 'proper' game, the root node would have a bounding sphere big enough to
	//encapsulate all of its children, so if it didn't collide, we could early out...
	//In our easy scene graph though, each node has its own bounding sphere!

	for(vector<SceneNode*>::const_iterator i = root.GetChildIteratorStart(); i != root.GetChildIteratorEnd(); ++i) {
		IntersectsNodes(*(*i));
	}

	return collided;
}

/*
Ray / Sphere code borrowed from http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html ;)

If the Ray has hit the passed in node, it'll be added to the collisions vector, along with
where it was hit.

*/
bool	Ray::IntersectsNode(SceneNode &node, Vector3 &intersectPos) {
	//We're going to do the intersect test in space local to the scene graph sphere!
	Vector3 relativePos = position - node.GetWorldTransform().GetPositionVector();

	//How far apart are the ray origin and sphere centre?
	float relativeDist  = relativePos.Length();

	//Map the relative position to somewhere along our direction
	float dot = Vector3::Dot(relativePos,direction);

	//if the dot product is greater than 0, our direction vector is facing away from the point
	//BUT! the ray might be inside the sphere, in which case dot will be >0 but <lightRadius
	//if it's greater than lightRadius, it's facing away AND outside the sphere
	if(dot > node.GetBoundingRadius()) {
		return false;
	}
	//How far along the ray did we map?
	float d = (node.GetBoundingRadius()*node.GetBoundingRadius()) - ((relativeDist*relativeDist) - (dot*dot));

	bool thisCollision = false;

	if(d > 0) {
		//If d is greater than 0, we've definitely intersected the sphere!
		collided = true;
		thisCollision = true;

		//The intersection position will be somewhere along the ray!
		intersectPos = position + (direction * (relativeDist - sqrt(d)));
		//add the collision to this ray's collision vector!
		collisions.push_back(RayCollision(&node,intersectPos));
	}

	return thisCollision;
}