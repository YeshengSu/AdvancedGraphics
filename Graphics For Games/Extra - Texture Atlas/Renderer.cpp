/*
In this code tutorial, we're going to take a look at how to implement a 
texture atlas - a method of adding a bunch of textures into one big texture,
one we can always have bound to a texture unit! Really, we need to fiddle 
around with the OGLRenderer and Mesh class code so that we only change the 
texture bound to a texture unit if it's different to the currently bound 
texture (to avoid any texture overhead caused by dumb drivers). But this'll 
still be handy to see how to mess with textures at run-time, as well as do a 
bit of vertex shader fun!

A texture atlas can be seen as one big texture, with lots of little textures
inside it. If you've ever seen the 'sprite sheet' for an old game, you can
think of those as a primitive type of texture atlas. What makes modern texture
atlases more interesting is the ability to make them 'transparent' to the rest
of the code, by hiding the transform
*/

#include "Renderer.h"

/*
To show off the texture atlas, we're going to add 4 textures to one texture
atlas. These 4!
*/
string textures[4] = {
	TEXTUREDIR"brick.tga",
	TEXTUREDIR"Barren reds.jpg",
	TEXTUREDIR"hellknight.tga",
	TEXTUREDIR"hellknight_local.tga"
};

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	/*
	To show the texture atlas and its subtextures, we're going to draw our
	old friend, the humble quad. 
	*/
	quad		= Mesh::GenerateQuad();


	/*
	We're going to use 2 different shaders to draw it, though! the first is just the
	basic vertex and fragment shader you were introduced to back in tutorial 3.

	The second is similar to the first, but has an additional function in it to perform
	texture atlas coordinate lookups in it. This means we can send texture coordinates
	from meshes, without the outside world 'knowing' we're using a texture atlas - 
	our quads textureCoordinates will automatically be translated to the appropriate place
	to render its texture correctly
	*/
	basicShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	atlasShader = new Shader("AtlasVertex.glsl", SHADERDIR"TexturedFragment.glsl");

	if(!basicShader->LinkProgram() ||
	   !atlasShader->LinkProgram()) {
		return;
	}

	//We need a camera!
	camera = new Camera(0,0,Vector3(0,0,100));

	/*
	Here's where we create our texture atlas. We're going to make it have a texture
	size of 2048 by 2048, and have a border of 8 pixels between each 'virtual texture'
	within the atlas texture.
	*/
	atlas = new TextureAtlas(2048,2048,8);

	/*
	Now to add some textures to it!
	*/
	atlas->AddTexture(textures[0]);
	atlas->AddTexture(textures[1]);
	atlas->AddTexture(textures[2]);

	/*
	Once we have our textures in our atlas, we can generate the mipmaps
	for it. We don't do this automatically so we don't waste time
	repeatedly building the mipmap chain when loading in groups of textures
	at a time.
	*/
	atlas->BuildMipMaps();

	/*
	Our atlas is clever enough to NOT add this texture again!
	*/
	atlas->AddTexture(textures[2]);

	/*
	We can add new textures after a call to BuildMipMaps...but they won't show up
	in the mipmaps, only the largest image. So we'll have to rebuild the mipmaps!
	*/
	atlas->AddTexture(textures[3]);
	atlas->BuildMipMaps();

	init = true;
}

/*
I think we know how destructors work by now...
*/
Renderer::~Renderer(void)	{
	delete quad;
	delete camera;
	delete atlas;
	delete basicShader;
	delete atlasShader;
	currentShader = NULL;
}

void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec);
}

void Renderer::RenderScene()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	

	viewMatrix  = camera->BuildViewMatrix();
	projMatrix	= Matrix4::Perspective(1.0f,10000.0f,(float)width/(float)height,45.0f);
	modelMatrix = Matrix4::Scale(Vector3(50,50,50));

	/*
	First we'll draw the entire texture atlas to a quad, so we can see what's in it! You 
	should see a mostly black quad, which has 4 little textures inside it.
	*/

	SetCurrentShader(basicShader);
	quad->SetTexture(atlas->GetAtlasTexture());
	UpdateShaderMatrices();
	quad->Draw();

	/*
	Now we'll draw each texture inside the texture atlas, using the same quad, with the same
	texture coordinates! The atlas shader, combined with the UpdateAtlasShader function
	will translate incoming vertex coordinates into the correct output coordinates
	*/
	SetCurrentShader(atlasShader);
	for(int i = 0; i < 4; ++i) {
		//First we find the 'node' inside the texture atlas for each texture
		AtlasTreeNode* node = atlas->GetTexture(textures[i]);

		//In our example, this should never happen!
		if(!node) {
			continue;
		}


		//And send its data to the vertex shader!
		UpdateAtlasShader(node);

		//Just to show how to do this outside of our special atlas shader, this is how to 
		//transform our 'virtual texture pos' to an actual texture position in the 
		//texture atlas. We're asking for the 'bottom left' point of each texture,
		//and getting in return the actual position of the bottom left of the sub texture
		//in the big 'texture atlas' texture. 
		Vector2 position = node->VertexCoordsToAtlasCoords(Vector2(0,0));

		//We've found the subtexture in the atlas, so we can bind the atlases 'big' texture
		//as our quads texture, safe in the knowledge that the atlas vertex shader will 
		//automatically translate texture coordinates between the texture we want the quad to 
		//have, and the texture atlas texture it has been assigned. 
		quad->SetTexture(node->GetAtlas()->GetAtlasTexture());

		//Just translates the quad we are drawing our textures to along the x axis, and makes it bigger
		modelMatrix = Matrix4::Translation(Vector3((float)120 + (i * 120), 0, 0)) * Matrix4::Scale(Vector3(50,50,50));
		UpdateShaderMatrices(); //Update the shader's matrices!
		quad->Draw(); //Draw the quad. 
	}

	glUseProgram(0); //We're done with shaders for now!

	SwapBuffers();	//Swap the front and back buffers around
}

/*
Like vertex attributes and lights etc, it's best to have a single function to update the atlas shader.
As long as we always use the same uniform names when we use atlasing in a vertex shader, this'll 
'just work'. We can keep all of the data we need to perform vertex atlasing in a Vector4 for the node,
and a Vector3 for the atlas. Aren't shaders great!

This should probably go in the Shader class, but I wanted each of these code-tutorials to be fairly
self-contained
*/
void	Renderer::UpdateAtlasShader(AtlasTreeNode*node) {
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "atlasInfo")  ,1,(float*)&node->GetAtlas()->PackValuesForShader());
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "textureInfo"),1,(float*)&node->PackValuesForShader());
}