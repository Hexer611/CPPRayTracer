#pragma once
#include "DataTypes.h"

struct BVHBoundingBox
{
	float3 Min;
	float3 Max;
};

struct BVHNode
{
	int childIndex;
	int triangleIndex;
	int triangleCount;
	BVHBoundingBox Bounds;
};

struct MeshInfo
{
	int numTriangles;
	int firstTriangleIndex;
	int nodesStartIndex;
	float3 boundsMin;
	float3 boundsMax;
	RayTracingMaterial material;
};

struct BVHTriangle
{
	float3 posA, posB, posC;
	float3 normalA, normalB, normalC;
};
