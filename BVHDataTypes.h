#pragma once
#include "DataTypes.h"
#include <vector>
#include <limits>
using namespace std;

struct MeshInfo
{
	int numTriangles;
	int firstTriangleIndex;
	int nodesStartIndex;
	int _;
	float3 boundsMin;
	float3 boundsMax;
	float _1;
	float _2;
	float4x4 modelWorldToLocalMaxtix;
	float4x4 modelLocalToWorldMaxtix;
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
	float3 Min()
	{
		float minX = min(min(posA.x, posB.x), posC.x);
		float minY = min(min(posA.y, posB.y), posC.y);
		float minZ = min(min(posA.z, posB.z), posC.z);
		return float3(minX, minY, minZ);
	}
	float3 Max()
	{
		float maxX = max(max(posA.x, posB.x), posC.x);
		float maxY = max(max(posA.y, posB.y), posC.y);
		float maxZ = max(max(posA.z, posB.z), posC.z);
		return float3(maxX, maxY, maxZ);
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
		/*
		float3 triangleMin = triangle.Min();
		float3 triangleMax = triangle.Max();

		Min.x = min(triangleMin.x, Min.x);
		Min.y = min(triangleMin.y, Min.y);
		Min.z = min(triangleMin.z, Min.z);

		Max.x = max(triangleMax.x, Max.x);
		Max.y = max(triangleMax.y, Max.y);
		Max.z = max(triangleMax.z, Max.z);
		*/
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

struct BVHObjectDebugData
{
	int minLeafTrig;
	int maxLeafTrig;

	int minLeafDepth;
	int maxLeafDepth;
};

struct RawObject
{
	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<int> triangles;
};
