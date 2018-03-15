#include "TextureAtlas.h"

/*
Basic Texture atlas initialisation. Automatically creates the OpenGL texture
required to keep the 'sub-textures' in 
*/
TextureAtlas::TextureAtlas(GLuint width, GLuint height, GLuint border)	{
	this->border = border;
	size.x		 = width;
	size.y		 = height;

	//The 'root' of the k-d tree has a position of 0, and a size equal to the
	//texture we are going to cut up to keep sub-textures in
	rootNode = new AtlasTreeNode(iVector2(),size);
	rootNode->atlas = this;

	glGenTextures(1, &textureName);			//Generate a single texture name

	/*
	Generate the actual texture. Really, we should guarantee the texture unit
	we are going to borrow is empty (or reset it when we're done), but for this
	example, we'll assume you aren't going to have 30+ textures bound at once when
	you make the texture atlas!
	*/
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, textureName);
	/*
	glTexImage2D will create a black texture of our width and height for us. We also want to
	clamp it, and set the filtering to bilinear.
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/*
	We've generated our texture, so we're done with it, for now.
	*/
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

/*
Deletes our texture, and our k-d tree structure. 
*/
TextureAtlas::~TextureAtlas(void)	{
	glDeleteTextures(1,&textureName);
	delete rootNode;
}

/*
Adds a texture to the atlas.
*/
AtlasTreeNode*	TextureAtlas::AddTexture(const string &name)	 {
	//If it's already in the atlas, just return the 
	//pre-exisiting node
	AtlasTreeNode*node = GetTexture(name);
	if(node) {
		return node;
	}

	iVector2 size;		//How big is this texture?
	int		 channels;	//Is this an RGB or RGBA texture?

	//Use SOIL to attempt to load in the texture
	unsigned char *texData = SOIL_load_image(name.c_str(),&size.x,&size.y,&channels,0);

	//Soil returns a pointer to memory if it loads in the texture, or NULL if it fails...
	if(!texData) {
		cout << "SOIL failed to load texture:" << name << endl;
		return NULL;
	}

	//Is there space in this atlas?
	node = rootNode->Add(size);

	if(!node) {	//Nope!
		cout << "No room in texture atlas for texture:" << name << endl;
		delete texData;
		return NULL;
	}
	/*
	If we've got this far, we have some texture data! We can now merge it with the existing
	texture for this atlas. To do this, we use glTexSubImage2D, which, as with everything
	else in OpenGL works on the currently bound shader. So, we bind the texture atlas
	shaderm, and then use glTexSubImage2D, which takes in a position to place the new 
	texture data, the dimensions of the new texture data, and how many channels the texture
	data has. It then sits and updates the biggest texture with the texture data - mipmaps
	don't get updated, and so will have 'black' where the primary level has the new texture.
	*/
	
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexSubImage2D(GL_TEXTURE_2D,0,node->position.x,node->position.y,size.x,size.y,channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,texData);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete texData;	//It's on the graphics card now, so we don't need it any more!

	addedTextures.insert(make_pair(name,node));	//Add it to the map, for quick look-up!

	return node;
}

/*
Seeks through the map and returns the AtlasTreeNode of the texture <name>

Looking through the map is faster than going through the k-d tree, due
to the way maps are arranged (although seeking through a map would be
even faster if we cut out the "../textures/" bit of the name...)
*/
AtlasTreeNode*	TextureAtlas::GetTexture(const string &name) const {
	map<string,AtlasTreeNode*>::const_iterator i = addedTextures.find(name);

	if(i != addedTextures.end()) {
		return i->second;
	}
	return NULL;
}

/*
Regenerates the mipmaps for the texture atlas, so that new textures 
make their way into the smaller mipmap levels
*/
void	TextureAtlas::BuildMipMaps() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureName);
	

	/*
	To prevent colour bleeding (where texture a's data ends up in texture b's samples)
	we can limit the max mipmap level and max amount of anistropic filter applied.

	If every mipmap level is half the size, a border of 4 in the level 0 will be 2
	in the second level, and 0 in the third - so we get two 'clean' levels before
	mipmaps make texture data get mushed together. So we can limit the mipmaps
	to (border / 2) so that texture sampling is never performed on mipmaps with
	such 'shared' samples. Try removing these two lines if you don't quite 'get'
	this - you should see textures bleeding into each other as the camera moves
	away from the quads.
	*/

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, border / 2); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, border / 2);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
We can do atlas texture coordinate lookups in the vertex shader! To do so the vertex
shader needs to know the texture atlas size and border - we can pack these together
into a single Vector3, and send it off for vertex processing. 
*/
Vector3 TextureAtlas::PackValuesForShader() const {
	return Vector3((float)size.x,(float)size.y,(float)border);
}

/*
Now For the AtlasTreeNode Functions!
*/

/*
Constructor. Creates a node at <position>, of <size>, and initialises
other variables.
*/
AtlasTreeNode::AtlasTreeNode(iVector2 position,iVector2 size) {
	this->position	= position;
	this->size		= size;

	childA			= NULL;
	childB			= NULL;
	used			= false;
}

/*
By deleting node children, we can ensure the entire kd-tree is deleted just
by calling delete on the root node.
*/
AtlasTreeNode::~AtlasTreeNode(void) {
	delete childA;
	delete childB;
}
/*
Adds a region of size <texSixe> to the kd-tree structure, returning the added
node if successful, or NULL if unsuccessful. 
*/
AtlasTreeNode* AtlasTreeNode::Add(const iVector2 &texSize) {
	/*
	If we have a border, the node we're going to add will be slightly larger
	than the texture size, so we'll use temporary variables x and y that 
	reflect this
	*/
	int x = texSize.x + atlas->border;
	int y = texSize.y + atlas->border;

	

	if(!IsLeaf()){	//This isn't a leaf node, add it to a child!
		if(childA)	{
			/*
			Instead we'll recursively go down the kd-tree and find a node we can
			place the texture in, and return it!
			*/
			AtlasTreeNode* n = childA->Add(texSize);
			if(n) {
				return n;
			}
			else {	//didn't find a suitable node down the first child route
				if(childB)	{//Try to find a child down the second child route
					return childB->Add(texSize);
				}
			}
		}
	}
	else{
		//were at the end of the tree, and everything's used
		if(used)	{
			return NULL;
		}
		//the texture is too big for this node!
		if(x > size.x || y > size.y)	{
			return NULL;
		}
		//size matches exactly!
		if(x == size.x && y == size.y)	{
			used = true;
			return this;
		}
		//Otherwise, we have to split this node in two
		int subwidth  = (int)size.x - x;
		int subheight = (int)size.y - y;

		/*
		If we've get this far, we have to split the node into two children.
		We fairly arbitrarily decide whether to split vertically or horizontally,
		in a bid to minimize wasted space. In either case, we make one child have a dimension
		exactly correct for the texture region, and one made up of the remainder.

		ASCII ART TIME

		|-----------|			|-----|-------|			|-----|-------|
		|			|			|	  |		  |			|TeX:)|		  |
		|			|  ---->	|	  |		  |	---->	|TeX:)|		  |
		|			|		    |	  |		  |			|-----|		  |
		|-----------|			|-----|-------|			|-----|-------|
		*/
		//split --
		if(subwidth <= subheight)	{
			childA = new AtlasTreeNode(iVector2(position.x,position.y)	, iVector2(size.x,y));			//
			childB = new AtlasTreeNode(iVector2(position.x,position.y+y), iVector2(size.x,size.y-y));	//
		}
		//split |
		else{
			childA = new AtlasTreeNode(iVector2(position.x  ,position.y), iVector2(x, size.y));	//
			childB = new AtlasTreeNode(iVector2(position.x+x,position.y), iVector2(size.x -x,size.y));	//
		}

		childA->atlas = atlas;
		childB->atlas = atlas;

		return childA->Add(texSize);
	}
	return NULL;
}

/*
Is this node a leaf? IE does it have child nodes or not...
*/
bool AtlasTreeNode::IsLeaf() const	{
	if(childA != NULL || childB != NULL)	{
		return false;
	}
	return true;
}

/*
This function takes in 'virtual texture' coordinates, and returns the
physical texture coordinates. IE if we want the texel at 0,0 of a texture
that begins at (512,512) of a (1024,1024) texture atlas, this function
will return (0.5,0.5)
*/
Vector2	AtlasTreeNode::VertexCoordsToAtlasCoords(Vector2 coords) const {
	Vector2 out;
	//we start by dividing the node's position by the atlases size - giving
	//us a 0-1 range for the start of the texture position. This'll be the 
	//translated (0,0) position of the texture.
	out.x = (float)position.x / (float)atlas->size.x;
	out.y = (float)position.y / (float)atlas->size.y;

	//Now we need to do the same for the node's size (minus the border!), which
	//we then multiply by our input coords, giving us the correct output coords.
	//Easy!

	out.x += ((float)(size.x - atlas->border) / (float)atlas->size.x) * coords.x;
	out.y += ((float)(size.y - atlas->border) / (float)atlas->size.y) * coords.y;

	return out;
}

/*
We can do texture atlas coordinate lookups in the vertex shader! To do so, we need to
know the position and the size of the node that represents the texture we want to
apply to the object. We can pack these inside a single Vector4, suitable for sending
to the vertex shader :)
*/
Vector4 AtlasTreeNode::PackValuesForShader() const {
	return Vector4((float)position.x,(float)position.y,(float)size.x,(float)size.y);
}