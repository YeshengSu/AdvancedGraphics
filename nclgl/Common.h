/******************************************************************************
Author:Rich Davison
Description: Some random variables and functions, for lack of a better place 
to put them.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

//It's pi(ish)...
static const float		PI = 3.14159265358979323846f;	

//It's pi...divided by 360.0f!
static const float		PI_OVER_360 = PI / 360.0f;

//Radians to degrees
static inline double RadToDeg(const double deg)	{
	return deg * 180.0 / PI;
};

//Degrees to radians
static inline double DegToRad(const double rad)	{
	return rad * PI / 180.0;
};

//I blame Microsoft...
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define SHADERDIR	"../../Shaders/"
#define MESHDIR		"../../Meshes/"
#define TEXTUREDIR  "../../Textures/"
#define SOUNDSDIR	"../../Sounds/"

#define SAFE_DELETE(x)       if(x){delete x;x = nullptr;}

#define WEEK_2_CODE
#define USE_MD5MESH
#define MD5_USE_HARDWARE_SKINNING
#define MD5_USE_NORMALS
#define MD5_USE_TANGENTS_BUMPMAPS
#define OBJ_USE_NORMALS
#define OBJ_USE_TANGENTS_BUMPMAPS