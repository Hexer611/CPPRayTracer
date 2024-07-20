#pragma once
#include "DataTypes.h"
#include <vector>
#include <limits>

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
	
	float3 Center()
	{
		return (posA + posB + posC) / 3.0;
	}
};

struct BVHBoundingBox
{
	float3 Min;
	float3 Max;

	BVHBoundingBox()
	{
		Min = float3(1, 1, 1) * 999999;
		Max = float3(1, 1, 1) * -999999;
	}

	float3 Center()
	{
		return (Max + Min) / 2.0;
	}

	void GrowToInclude(float3 point)
	{
		Min = float3::Min(Min, point);
		Max = float3::Max(Max, point);
	}

	void GrowToInclude(BVHTriangle triangle)
	{
		GrowToInclude(triangle.posA);
		GrowToInclude(triangle.posB);
		GrowToInclude(triangle.posC);
	}
};

struct BVHNode
{
	int childIndex;
	int triangleIndex;
	int triangleCount;
	BVHBoundingBox Bounds;
	BVHNode()
	{
		childIndex = 0;
		triangleIndex = 0;
		triangleCount = 0;
		Bounds = {};
	}
};

struct BVHObject
{
	MeshInfo MeshInfo;
	std::vector<BVHNode> Nodes;
	std::vector<BVHTriangle> Triangles;
};

struct RawObject
{
	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<int> triangles;
};
