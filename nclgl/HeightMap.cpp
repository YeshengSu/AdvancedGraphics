#include "HeightMap.h"

HeightMap::HeightMap(std::string name, Vector3 HeightSize)
{
	unsigned char* data = SOIL_load_image(name.c_str(), &width, &height, NULL, 1);

	//std::ifstream file(name.c_str(), ios::binary);
	//	if (!file) {
	//		return;	//}	terrainHeight = 0.0f;
	if (!data)
		return;

	numVertices = width*height;
	numIndices = (width - 1)*(height - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < height; z++)
		{
			int offset = (x * width) + z;
			vertices[offset] = Vector3(x * HeightSize.x, data[offset] * HeightSize.y, z * HeightSize.z);
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X,z * HEIGHTMAP_TEX_Z);
			if (terrainHeight < data[offset] * HeightSize.y)
				terrainHeight = data[offset] * HeightSize.y;
		}
	}

	SOIL_free_image_data(data);

	numIndices = 0;
	for (int  x = 0; x < width -1; x++)
	{
		for (int z = 0; z < height -1; z++)
		{
			int a = (x * (width)) + z;
			int b = ((x + 1) * (width)) + z;
			int c = ((x + 1) * (width)) + (z + 1);
			int d = (x  * (width)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;
			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();
}


HeightMap::~HeightMap()
{
}
