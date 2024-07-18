#pragma once
#include "BHVDataTypes.h"
#include <vector>

class ObjReader {
public:
	ObjReader();
	~ObjReader();

	void ReadFile(const char filePath[], bool flattenFaces = false);
	std::vector<BVHTriangle> TriangleData = {};
private:

};
