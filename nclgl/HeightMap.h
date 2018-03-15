#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Vector3.h"
#include "Mesh.h"

#define RAW_WIDTH	257
#define RAW_HEIGHT  257

#define HEIGHTMAP_X 32.0f
#define HEIGHTMAP_Z 32.0f
#define HEIGHTMAP_Y 10.0f
#define HEIGHTMAP_TEX_X 1.0f/32.0f
#define HEIGHTMAP_TEX_Z 1.0f/32.0f

class HeightMap : public Mesh
{
public:
	HeightMap(std::string name, Vector3 HeightSize = Vector3(HEIGHTMAP_X, HEIGHTMAP_Y, HEIGHTMAP_Z));
	~HeightMap();

	float GetTerrainHeight()const { return terrainHeight; }
	int GetWidth()const { return width; }
	int GetHeight()const { return height; }
private :
	float terrainHeight;
	int width;
	int height;
};

