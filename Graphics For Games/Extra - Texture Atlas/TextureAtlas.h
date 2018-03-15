#pragma once
#include <map>
#include "./NCLGL/OGLRenderer.h"

/*
We can actually get away with simple integer math for the majority of
the texture atlas, so we cheat and have a Vector2 facsimile with integers.
*/
struct iVector2 {
	int x;
	int y;

	iVector2() {
		x = y = 0;
	}

	iVector2(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

/******************************************************************************
Class:AtlasTreeNode
Implements:
Author:Rich Davison
Description:A simple k-d tree data structure.

We can use this to partition a 2D space into regions - perfect for splitting
up a big texture atlas texture into lots of little textures.

Every node has two children (like a binary tree), but they are not necessarily
of equal size. Also we can split the node either horizontally or vertically...

So each node may be made up of a pair of nodes either side by side, or on top 
of each other.

This class doesn't care about texture data, all it works on is a 2D region,
made up of a size - the region's position will be determined by the node it
ends up a part of.



-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   Who is Atlas?
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

class AtlasTreeNode 	{
	friend class TextureAtlas;
public:
	AtlasTreeNode(iVector2 position,iVector2 size);

	/*
	Add a region of size <size> to the kd tree. Returns the node the region is
	part of if successful, otherwise returns NULL
	*/
	AtlasTreeNode* Add(const iVector2 &size);

	/*
	Returns whether this node has children (is a leaf) or not
	*/
	bool	IsLeaf() const;

	/*
	Transforms 'virtual' coordinates to 'physical' coordinates
	*/
	Vector2	VertexCoordsToAtlasCoords(Vector2 coords) const;

	/*
	Packs the position and size of this node into a single Vector4,
	suitable for sending to a vertex shader.
	*/
	Vector4 PackValuesForShader() const;

	/*
	Returns the atlas this node is a region of
	*/
	TextureAtlas* GetAtlas() {return atlas;}

protected:
	~AtlasTreeNode(void);

	AtlasTreeNode* childA;		//First child  (may be left or top)
	AtlasTreeNode* childB;		//Second child (may be right or bottom)

	bool			used;		//Is this node in use?
	iVector2		position;	//Position of left corner of node region
	iVector2		size;		//How big this node region is
	TextureAtlas*	atlas;		//Pointer to the 'parent'
};


/******************************************************************************
Class:TextureAtlas
Implements:
Author:Rich Davison
Description:A simple Texture Atlas. Allows multiple textures to be combined 
into one larger texture at runtime, to save on texture switching. Internally,
we use a kd-tree like structure to keep track of which positions in the 
atlas texture we have written to, so we can quickly determine a new position
for each new texture added to the atlas. 

We can use the class in two ways.

1) Stick a load of textures in an atlas. Use the VertexCoordsToAtlasCoords 
function of the kd tree node to manually determine the correct texture 
coordinates for a mesh using a texture held within the atlas, and update the
tex coords VBO once at run-time.

1) Stick a load of textures in an atlas. Give each mesh the texture obtained 
from GetAtlasTexture. Use the AtlasVertexShader to automatically determine
texture coordinates for each texture.

Method 2 is slightly slower (more work in vertex shader) but better supports
run-time dynamic changing of the texture atlas, and is a more 'black box' 
solution.

The texture atlas supports a 'border around' the textures. This is to act as
padding around the individual sub textures that make up the atlas, so that
they are separated by empty pixels in mipmaps - to reduce the chance of any
texture 'bleeding' where texels from sub-texture B get blended into
sub-texture A when drawing something using sub-texture A.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   Would you kindly?
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

class TextureAtlas	{
	friend class AtlasTreeNode;
public:
	TextureAtlas(GLuint width, GLuint height, GLuint border = 2);
	~TextureAtlas(void);

	//Returns the OpenGL texture for this atlas
	GLuint GetAtlasTexture() const {return textureName;}

	//returns TRUE if texture with <name> is already in the atlas
	bool	TextureInAtlas(const string &name) const;

	/*
	Returns the node representing the region for texture <name>, or NULL
	if the texture <name> does not exist in this atlas
	*/
	AtlasTreeNode*	GetTexture(const string &name) const;

	/*
	Adds a texture with the filepath/name of <name> to the atlas. Returns
	the node representing this texture if everything is successful, or NULL
	if the process fails.
	*/
	AtlasTreeNode*	AddTexture(const string &name);

	/*
	If we're using mipmaps, we need to rebuild them to reflect new data added
	to the primary texture mipmap level. We could do this automatically, but
	it'll be more efficient to just call this once for every 'batch' of
	textures we add to the texture atlas. 
	*/
	void	BuildMipMaps() const;

	/*
	Packs the size and border of this texture atlas into a single Vector3,
	suitable for sending to a shader.
	*/
	Vector3 PackValuesForShader() const;

protected:
	GLuint		textureName;	//OpenGL texture name for the texture of this atlas

	iVector2	size;			//dimensions of the texture for this atlas
	GLuint		border;			//How many pixels of gutter we have for each texture

	AtlasTreeNode* rootNode;	//Our root node - deleting this will delete the entire tree

	map<string,AtlasTreeNode*> addedTextures; //Map of textures for quick look-up
};