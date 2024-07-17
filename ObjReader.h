#pragma once
#include "BHVDataTypes.h"
#include <vector>

class ObjReader {
public:
	ObjReader();
	~ObjReader();

	void ReadFile(const char filePath[]);
	std::vector<BVHTriangle> TriangleData = {};
private:

};
