#pragma once
#include <string>
#include "BVHDataTypes.h"

class BVHCalculator
{
public:
	BVHObject CalculateBVH(RawObject rawObject);
	void Split(BVHNode*rawObject, int depth);
};