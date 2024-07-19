#pragma once
#include "BVHDataTypes.h"
#include <vector>

class ObjReader {
public:
	ObjReader();
	~ObjReader();

	void ReadFile(const char filePath[], bool flattenFaces = false);
	std::vector<BVHTriangle> TriangleData = {};
	RawObject rawObject;
	BVHObject bvhObject;
private:

};
