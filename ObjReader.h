#pragma once
#include "BVHDataTypes.h"
#include <vector>

class ObjReader {
public:
	ObjReader();
	~ObjReader();

	void ReadFile(const char filePath[], bool flattenFaces = false, float3 pos = float3(0,0,0), float3 rot = float3(0,0,0), float3 scale = float3(1,1,1));
	std::vector<BVHTriangle> TriangleData = {};
	RawObject rawObject;
	BVHObject bvhObject;
private:

};
