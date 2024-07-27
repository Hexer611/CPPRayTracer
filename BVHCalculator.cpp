#include "BVHCalculator.h"
#include "BVHDataTypes.h"
#include "ObjReader.h"
#include <vector>
#include <math.h>

int MAXDEPTH = 4;
std::vector<BVHNode*> nodes = {};
std::vector<BVHTriangle> triangles = {};

BVHObject BVHCalculator::CalculateBVH(RawObject rawObject)
{
	BVHObject newObject = {};
	newObject.MeshInfo = {};
	newObject.MeshInfo.material = {};
	newObject.MeshInfo.material.color = float4(1,1,1,1);

	BVHBoundingBox bounds = {};
	for (auto vertice : rawObject.vertices)
	{
		bounds.GrowToInclude(vertice);
	}

	newObject.MeshInfo.boundsMin = bounds.Min;
	newObject.MeshInfo.boundsMax = bounds.Max;

	std::vector<BVHTriangle> trigs = {};

	for (int i = 0; i < rawObject.triangles.size(); i+=3)
	{
		BVHTriangle tri = {};
		tri.posA = rawObject.vertices[rawObject.triangles[i]];
		tri.posB = rawObject.vertices[rawObject.triangles[i + 1]];
		tri.posC = rawObject.vertices[rawObject.triangles[i + 2]];

		//tri.normalA = rawObject.normals[rawObject.triangles[i]];
		//tri.normalB = rawObject.normals[rawObject.triangles[i + 1]];
		//tri.normalC = rawObject.normals[rawObject.triangles[i + 2]];

		tri.normalA = rawObject.normals[i];
		tri.normalB = rawObject.normals[i + 1];
		tri.normalC = rawObject.normals[i + 2];

		trigs.push_back(tri);
	}

	triangles = trigs;

	BVHNode root = {};
	root.Bounds = bounds;

	root.triangleIndex = 0;
	root.triangleCount = trigs.size();

	nodes.push_back(&root);
	Split(&root, 0);
	newObject.Triangles = triangles;
	
	for (auto newNode : nodes)
	{
		newObject.Nodes.push_back(*newNode);
	}

	return newObject;
}

void BVHCalculator::Split(BVHNode *parentNode, int depth)
{
	if (depth >= 32)
		return;

	BVHNode *child1 = new BVHNode();
	BVHNode *child2 = new BVHNode();

	BVHBoundingBox bound1 = {};
	BVHBoundingBox bound2 = {};

	child1->Bounds = bound1;
	child2->Bounds = bound2;

	child1->triangleIndex = parentNode->triangleIndex;
	child2->triangleIndex = parentNode->triangleIndex;

	parentNode->childIndex = nodes.size();
	nodes.push_back(child1);
	nodes.push_back(child2);

	auto size = parentNode->Bounds.Max - parentNode->Bounds.Min;
	int splitAxis = size.x > std::fmaxf(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
	float splitPos = parentNode->Bounds.Center()[splitAxis];

	for (int i = parentNode->triangleIndex; i < parentNode->triangleIndex + parentNode->triangleCount; i++)
	{
		auto tri = triangles[i];
		if (tri.Center().x > 2.3)
			tri.normalA = tri.normalA;
		bool inA = tri.Center()[splitAxis] <= splitPos;
		
		BVHNode *child = inA ? child1 : child2;
		child->Bounds.GrowToInclude(tri);
		child->triangleCount++;

		if (inA)
		{
			int swapIndex = child->triangleIndex + child->triangleCount - 1;
			auto swapTri = triangles[i];
			triangles[i] = triangles[swapIndex];
			triangles[swapIndex] = swapTri;
			child2->triangleIndex++;
		}
	}
	float min1 = 9999;
	float max1 = 0;
	for (int i = child1->triangleIndex; i < child1->triangleIndex + child1->triangleCount; i++)
	{
		min1 = min(min1, triangles[i].Min().x);
		max1 = max(max1, triangles[i].Max().x);
	}

	if (child1->triangleCount > 0 && child2->triangleCount > 0)
	{
		Split(child1, depth + 1);
		Split(child2, depth + 1);
	}
}
