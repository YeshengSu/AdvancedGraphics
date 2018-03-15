/*
In this code tutorial we're going to do something really simple - transforming
a world space position into a screen space position. Why would we do this?
Think about any game you've played where highlighting something puts a box
around it (think RTS games), or incoming missiles have a reticle surrounding
them, or even if something has had text above it.

Fortunately, it's very easy to turn a world space position to a screen space
position. In fact, you've already done it, right from tutorial 2b! We get
a world space position (in our shaders, this is the vertex position transformed
by the model matrix), and then multiply it by the view and projection matrices.

We have to do a tiny little bit more to get it into screen coordinates, but as
you'll see, it's not very much!

When you run this demo program, you should see a triangle on screen. It'll have
a red 'targeting reticle' around its origin - try moving around using the
camera, and you'll see how the reticle is always centered on the triangle. We
draw this reticle in orthographic mode, which is useful for GUIs, and for 
'flat' things.

*/

#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	camera		= new Camera(0,0,Vector3(0,0,100));	//Standard camera
	triangle	= Mesh::GenerateTriangle();			//we know about triangles..
	quad		= Mesh::GenerateQuad();				//and quads...

	//We set a couple of textures, here. Only one of them is new!
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"reticle.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//We don't do anything fancy with shaders this time around, so we just
	//need to use the tutorial 3 shader :)
	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if(!currentShader->LinkProgram()) {
		return;
	}

	//The targetting reticle is alpha blended, so we need to enable blending...
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}


/*
Clean up your mess!
*/
Renderer::~Renderer(void)	{
	delete triangle;
	delete quad;
	delete camera;
}

/*
Update the camera, perhaps?
*/
void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec);
}

void Renderer::RenderScene()	{
	//We start off simply, by just drawing the triangle using a perspective
	//projection. You should recognise what all of this code does!!!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	glUseProgram(currentShader->GetProgram());			//Switch on our shader

	viewMatrix  = camera->BuildViewMatrix();
	projMatrix	= Matrix4::Perspective(1.0f,10000.0f,(float)width/(float)height,45.0f);
	modelMatrix = Matrix4::Scale(Vector3(50,50,50));

	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);	//New! and to move...

	triangle->Draw();

	//So far, so normal! What we're going to do now though, is work out
	//The triangle's origin position in screen space, and draw a targetting
	//Reticle around it using an orthographic matrix.


	Vector3 screenPos = WorldPosToScreenPos(modelMatrix.GetPositionVector());

	//We now have the screen position, now to draw a quad. We simply set
	//up the orthographic matrix to go from 0-width and 0-height, and
	//draw a quad at screePos (scaled to some appropriate size).

	viewMatrix.ToIdentity();
	projMatrix	= Matrix4::Orthographic(-1,1,width,0,height,0);
	modelMatrix = Matrix4::Translation(screenPos) *
				  Matrix4::Scale(Vector3(25,25,25));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

	SwapBuffers();	
}

/*
Here's how WorldPosToScreenPos works. It's essentially the same
as the process that goes on in a shader. We take in a world position
(we don't need to transform by the model matrix, as we're using the 
world space position of the object, not its local space vertices), 
and multiply it by the view and projection matrices. This takes the
position into clip space, which runs from -1 to 1 on each axis. So,
we have to transform it into a more useful space, ideally screen
space. We can do that by transforming the clip space coordinate by 
multiplying by 0.5 (going to -0.5 to 0.5) and adding 0.5 (0.0 to 1.0)
and then multiplying that value by the screen dimensions!

If screenPos.z is less than 1, the world space position is behind
our camera...(the translation we do to the model matrix will
fix that though, so we don't see the reticle unless targets
are on screen!)
*/
Vector3	Renderer::WorldPosToScreenPos(Vector3 worldPos) {	
	Vector3 screenPos = projMatrix * viewMatrix * Vector3(worldPos);

	Vector3 outpos = Vector3(
		(screenPos.x * 0.5f + 0.5f) * width,
		(screenPos.y * 0.5f + 0.5f) * height,
		(1.0f + screenPos.z) * 0.5f
	);

	return outpos;
}