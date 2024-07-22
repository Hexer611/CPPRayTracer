#pragma once
#include <d3d11.h>
#include "DataTypes.h"

class VectorUtils {
public:
	static float4x4 CreateWorldToLocalMatrix(float3 pos, float3 rot, float3 scale);

private:
};
