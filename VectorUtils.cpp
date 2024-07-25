#include "VectorUtils.h"
#include "DataTypes.h"
#include <cmath>
#include <vector>

float4x4 VectorUtils::CreateWorldToLocalMatrix(float3 pos, float3 rot, float3 scale)
{
	float4x4 matrix = {};
	float t[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			t[i][j] = (i == j) ? 1 : 0;

	t[0][3] = pos.x;
	t[1][3] = pos.y;
	t[2][3] = pos.z;

	float r[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			r[i][j] = (i == j) ? 1 : 0;

	float rotxRad = rot.x * 3.14159 / 180;
	float rotyRad = rot.y * 3.14159 / 180;
	float rotzRad = rot.z * 3.14159 / 180;
	
	r[0][0] = cos(rotyRad) * cos(rotzRad);
	r[0][1] = sin(rotxRad) * sin(rotyRad) * cos(rotzRad) - cos(rotxRad) * sin(rotzRad);
	r[0][2] = cos(rotxRad) * sin(rotyRad) * cos(rotzRad) + sin(rotxRad) * sin(rotzRad);

	r[1][0] = cos(rotyRad) * sin(rotzRad);
	r[1][1] = sin(rotxRad) * sin(rotyRad) * sin(rotzRad) + cos(rotxRad) * cos(rotzRad);
	r[1][2] = cos(rotxRad) * sin(rotyRad) * sin(rotzRad) - sin(rotxRad) * cos(rotzRad);

	r[2][0] = -sin(rotyRad);
	r[2][1] = sin(rotxRad) * cos(rotyRad);
	r[2][2] = cos(rotxRad) * cos(rotyRad);
	
	float s[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			s[i][j] = (i == j) ? 1 : 0;

	s[0][0] = scale.x;
	s[1][1] = scale.y;
	s[2][2] = scale.z;

	float4x4 tMatrix = float4x4(t);
	tMatrix = tMatrix.Transpose();
	float4x4 rMatrix = float4x4(r);
	float4x4 sMatrix = float4x4(s);

	float4x4 resultMatrix = sMatrix * rMatrix * tMatrix;
	return resultMatrix;
}
