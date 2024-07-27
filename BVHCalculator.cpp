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
		BVHTriangle tri = BVHTriangle
		(
			rawObject.vertices[rawObject.triangles[i]],
			rawObject.vertices[rawObject.triangles[i + 1]],
			rawObject.vertices[rawObject.triangles[i + 2]],

			rawObject.normals[i],
			rawObject.normals[i + 1],
			rawObject.normals[i + 2]
		);

		trigs.push_back(tri);
	}

	triangles = trigs;

	BVHNode root = {};
	root.Bounds = bounds;

	root.triangleIndex = 0;
	root.triangleCount = trigs.size();

	nodes.push_back(&root);
	Split(&root, 0);

	std::vector<BVHTriangleData> triangleData = {};
	
	for (int i = 0; i < triangles.size(); i++)
	{
		triangleData.push_back(triangles[i].GetData());
	}
	
	newObject.Triangles = triangleData;
	
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

	int minCostSplitAxis = 0;
	float minCostSplitPos = 0;
	float minCost = 9999999;
	
	for (float splitNormPos = 0.2f; splitNormPos < 1; splitNormPos += 0.2f)
	{
		for (int splitAxis = 0; splitAxis < 3; splitAxis++)
		{
			float splitPos = parentNode->Bounds.Min[splitAxis] + (parentNode->Bounds.Max[splitAxis] - parentNode->Bounds.Min[splitAxis]) * splitNormPos;

			BVHNode* tmp_child1 = new BVHNode();
			BVHNode* tmp_child2 = new BVHNode();

			BVHBoundingBox tmp_bound1 = {};
			BVHBoundingBox tmp_bound2 = {};

			tmp_child1->Bounds = tmp_bound1;
			tmp_child2->Bounds = tmp_bound2;
			
			for (int i = parentNode->triangleIndex; i < parentNode->triangleIndex + parentNode->triangleCount; i++)
			{
				auto tri = triangles[i];
				bool inA = tri.Center()[splitAxis] < splitPos;

				BVHNode* child = inA ? tmp_child1 : tmp_child2;
				child->Bounds.GrowToInclude(tri);
				child->triangleCount++;
			}

			if (tmp_child1->triangleCount == 0 || tmp_child2->triangleCount == 0)
				continue;

			float curCost = tmp_child1->Bounds.Cost() + tmp_child2->Bounds.Cost();
			if (curCost < minCost)
			{
				minCostSplitAxis = splitAxis;
				minCostSplitPos = splitPos;
				minCost = curCost;
			}
		}
	}

	/*
	float3 size = parentNode->Bounds.Max - parentNode->Bounds.Min;
	minCostSplitAxis = size.x > std::fmax(size.y, size.z) ? 0 : size.y > size.z ? 1 : 2;
	minCostSplitPos = parentNode->Bounds.Center()[minCostSplitAxis];*/

	if (minCost == 9999999)
		return;

	BVHNode* child1 = new BVHNode();
	BVHNode* child2 = new BVHNode();

	BVHBoundingBox bound1 = {};
	BVHBoundingBox bound2 = {};

	child1->Bounds = bound1;
	child2->Bounds = bound2;

	child1->triangleIndex = parentNode->triangleIndex;
	child2->triangleIndex = parentNode->triangleIndex;

	parentNode->childIndex = nodes.size();
	nodes.push_back(child1);
	nodes.push_back(child2);

	for (int i = parentNode->triangleIndex; i < parentNode->triangleIndex + parentNode->triangleCount; i++)
	{
		auto tri = triangles[i];
		bool inA = tri.Center()[minCostSplitAxis] < minCostSplitPos;

		BVHNode* child = inA ? child1 : child2;
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

	if (child1->triangleCount > 0 && child2->triangleCount > 0)
	{
		Split(child1, depth + 1);
		Split(child2, depth + 1);
	}
}
