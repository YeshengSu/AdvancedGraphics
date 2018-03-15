/*
In this code tutorial, we're going to perform a 2D to 3D projection, turning 
a mouse pointer position into a 3D position in space. What good is that?
Well, it allows us to determine which object in 3D space is underneath the
mouse! In 'old' OpenGL, there was a special 'selection' mode, which allowed
you to determine which pixel belonged to which object, but it was really slow
and was eventually removed from OpenGL 3 onwards. So instead, we can do it
using a bit of vector math! By projecting a mouse pointer from 2D to 3D, we 
can form a direction vector known as a Ray, and using that Ray, test whether
we can draw a line through any object under the mouse. We can do this as,
in the Scene Management tutorial, we gave scene nodes a bound sphere - this
was just for frustum culling, but works just as well for our purposes here.
This process is known as 'picking'. In our simple test program, we'll 'pick'
every object under the mouse pointer, but the Ray class has been set up to
be easily extended so that objects hit by the mouse can be sorted into 
a distance order.


When you run this program, you'll see a load of boxes. When you press down the 
mouse, you'll see a triangular mouse pointer, if there's a box under it, it'll
turn blue. Sounds basic, but it'll show you how you can click on objects in
your games!

On a more general level, Ray casting can be used to see whether an object has
something in front of it (the object casts a ray out in front of it and sees
what it collides with!), or maybe to perform height checking...
*/

#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	camera		= new Camera(0,0,Vector3(0,0,100));
	cube		= new OBJMesh(MESHDIR"centeredCube.obj");	//A cube surrounding the origin!
	triangle    = Mesh::GenerateTriangle();	//And a triangle to use as the mouse pointer...

	cube->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if(!cube->GetTexture()) {
		return;
	}
	//We don't need to do anything fancy with shaders this time around, so we're
	//going to borrow the scene shader from tutorial 6
	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}

	//Our renderer's root node!
	root  = new SceneNode();
	root->SetBoundingRadius(0.0f);

	//To show off picking, we're going to Make a 10 by 10 grid of cubes.
	//As they're all going to be children of the root node, all 100
	//of them will be drawn using a single draw call on the root. 
	for(int x = 0; x < 10; ++x) {
		for(int z = 0; z < 10; ++z) {
			SceneNode* n = new SceneNode();
			n->SetMesh(cube);
			n->SetModelScale(Vector3(10,10,10));
			n->SetBoundingRadius(10.0f);
			n->SetTransform(Matrix4::Translation(Vector3(x*50.0f,0,-z*50.0f)));
			root->AddChild(n);
		}
	}

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer(void)	{
	delete cube;
	delete camera;
	delete triangle;
}

//If the left mouse is down, we're going to 'lock' the camera, and
//instead draw a mouse pointer!
void Renderer::UpdateScene(float msec)	{
	if(!Window::GetMouse()->ButtonHeld(MOUSE_LEFT)) {
		camera->UpdateCamera(msec);
	}
	root->Update(msec); //Always update the root!
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glUseProgram(currentShader->GetProgram());				//Switch on our shader


	//First off, some familiar code to simply draw our boxes scene graph
	viewMatrix  = camera->BuildViewMatrix();
	projMatrix	= Matrix4::Perspective(1.0f,15000.0f,(float)width/(float)height,45.0f);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);	//New! and to move...
	DrawNode(root);

	//We also set their colours to white - in the following code we set boxes under our mouse
	//to blue, so they'll be drawn blue in the 'next' frame, and then reset by this code.
	for(vector<SceneNode*>::const_iterator i = root->GetChildIteratorStart(); i != root->GetChildIteratorEnd(); ++i) {
		(*i)->SetColour(Vector4(1,1,1,1));
	}
	//If the left mouse button is held down, we're going to do some picking!
	if(Window::GetMouse()->ButtonHeld(MOUSE_LEFT))	{
		//We start by getting a normalised direction vector from the camera and mouse...
		Vector3 dir = GetMouseDirectionVector3((float)width/(float)height,45.0f, *camera);

		//And then forming a ray out of it, starting at the camera's position
		Ray r = Ray(camera->GetPosition(),dir);

		//Now we're going to test our ray against our node hierarchy
		r.IntersectsNodes(*root);

		//If it has collided against any nodes, set them to blue
		for(unsigned int i = 0; i < r.collisions.size(); ++i ) {
			r.collisions.at(i).node->SetColour(Vector4(0,0,1,1));
		}

		//In case you didn't quite know how, here's how to draw a textured 'mouse
		//pointer' in OpenGL. We go to an orthographic perspective, with a range 
		//of 0-width on the x-axis, and 0-height on the y axis. Then we draw
		//a shape at the mouse position. See how we still use the near plane of
		//-1, so that a mouse pointer drawn at a z of 0 will definitely show up!

		Vector2 mpos = Window::GetMouse()->GetAbsolutePosition();
		viewMatrix.ToIdentity();
		projMatrix	= Matrix4::Orthographic(-1.0f,1.0f,(float)width,0.0f,(float)height,0.0f);
		modelMatrix = Matrix4::Translation(Vector3(mpos.x,height - mpos.y,0)) * 
			Matrix4::Scale(Vector3(25,25,25));

		UpdateShaderMatrices();

		triangle->Draw();
	}
	//That's everything!
	glUseProgram(0);
	SwapBuffers();	
}

/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it. 

Just as we turn a world space position into a clip space position by multiplying 
it by the model, view, and projection matrices, we can turn a clip space 
position back to a 3D position by multiply it by the INVERSE of the 
view projection matrix (the model matrix has already been assumed to have 
'transformed' the 2D point). As has been mentioned a few times, inverting a 
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	Renderer::UnProject(Vector3 position, float aspect, float fov, Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect,fov);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)width) * 2.0f - 1.0f,
		(position.y / (float)height) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w,transformed.y / transformed.w,transformed.z / transformed.w);
}


/*
Here's how we generate an 'inverse' projection. It's very similar to the 
Matrix::Perpsective function (in fact, it'll only work with perspective 
matrices, but why would we use anything else!), but with some inverted
values (and a couple of hard coded 'inversions')

This function only holds true when znear < zfar and znear > 0.0f (both 
sensible restrictions!)

Technically we should have a znear and zfar parameter for this, but
as long as the above is true, this cheated matrix is 'good enough'.
*/
Matrix4 Renderer::GenerateInverseProjection(float aspect, float fov) {
	Matrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);

	m.values[0]		= aspect / h;
	m.values[5]		= tan(fov*PI_OVER_360);
	m.values[10]	= 0.0f;

	m.values[11]	= -0.5f;
	m.values[14]	= -1.0f;
	m.values[15]	= 0.5f;

	return m;
} 

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 Renderer::GenerateInverseView(Camera &c) {
	float pitch			=  c.GetPitch();
	float yaw			=  c.GetYaw();
	Vector3 position	=  c.GetPosition();

	Matrix4 iview = 
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0,-1,0)) * 
		Matrix4::Rotation(pitch, Vector3(-1,0,0));

	return iview;
}

/*
Here's how we generate the 3D normalised direction vector that we
use with our picking ray. We use our new unproject function /twice/
and then normalise the result. Here's how this works! We unproject
once right up against the near plane, and then once again, right
up against the far plane. This gives us 2 positions in space, both
'under' the mouse pointer from the camera's perspective. We can
then get the direction vector between them and normalise it,
giving us a direction vector that comes from our camera position,
and passes through the mouse pointer.

		Near				Far
		|					|
		|					|
		|					|
O->		|*				   *|
		|					|
		|					|
*/
Vector3 Renderer::GetMouseDirectionVector3(float aspect, float fov, Camera &cam) {
	Vector2 mpos = Window::GetMouse()->GetAbsolutePosition();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(mpos.x,
							height - mpos.y,
							0.0f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(mpos.x,
							height - mpos.y,
							0.99999999f
	);

	Vector3 a = UnProject(nearPos,aspect,fov,cam);
	Vector3 b = UnProject(farPos,aspect,fov,cam);
	Vector3 c = b-a;

	c.Normalise();

	return c;
}

/*
This is just the DrawNode function from the SceneGraphs tutorial!
*/
void	Renderer::DrawNode(SceneNode*n)	{
	if(n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),	1,false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),1,(float*)&n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"),(int)n->GetMesh()->GetTexture());

		n->GetMesh()->Draw();
	}

	for(vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode((*i));
	}
}