#pragma once
#include <cmath>
#include <stdexcept>

struct float3 {
	float x;
	float y;
	float z;
	float3() {}
	float3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	float3 Normalize()
	{
		float length = std::sqrt(x * x + y * y + z * z);
		return float3(x / length, y / length, z / length);
	}

	float3 operator+(float3 const& other)
	{
		return float3(x + other.x, y + other.y, z + other.z);
	}

	float3 operator-(float3 const& other)
	{
		return float3(x - other.x, y - other.y, z - other.z);
	}

	float3 operator*(float3 const& other) // Cross product
	{
		return float3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	float3 operator*(float const& other)
	{
		return float3(x * other, y * other, z * other);
	}

	float3 operator/(float const& other)
	{
		return float3(x / other, y / other, z / other);
	}

	float operator[](int index)
	{
		switch (index)
		{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw std::out_of_range("Wrong index");
				return 0;
		}
	}

	static float3 Min(float3 f1, float3 f2)
	{
		return float3(std::fminf(f1.x, f2.x), std::fminf(f1.y, f2.y), std::fminf(f1.z, f2.z));
	}

	static float3 Max(float3 f1, float3 f2)
	{
		return float3(std::fmaxf(f1.x, f2.x), std::fmaxf(f1.y, f2.y), std::fmaxf(f1.z, f2.z));
	}
};

struct float4 {
	float x;
	float y;
	float z;
	float w;
	float4() {}
	float4(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		w = 0;
	}
	float4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	float operator[](int index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw std::out_of_range("Wrong index");
			return 0;
		}
	}
};

struct float3x3 {
	float _array[3][3];
	float3x3() {}
	float3x3(float3 _r1, float3 _r2, float3 _r3)
	{
		_array[0][0] = _r1.x;
		_array[0][1] = _r1.y;
		_array[0][2] = _r1.z;

		_array[1][0] = _r2.x;
		_array[1][1] = _r2.y;
		_array[1][2] = _r2.z;

		_array[2][0] = _r3.x;
		_array[2][1] = _r3.y;
		_array[2][2] = _r3.z;
	}
	float Determinant()
	{
		float pos = _array[0][0] * _array[1][1] * _array[2][2] + _array[1][0] * _array[2][1] * _array[0][2] + _array[2][0] * _array[0][1] * _array[1][2];
		float neg = _array[0][2] * _array[1][1] * _array[2][0] + _array[0][1] * _array[1][0] * _array[2][2] + _array[0][0] * _array[1][2] * _array[2][1];
		return pos - neg;
	}
};

struct float4x4 {
	float _array[4][4];
	float4x4() {}
	float4x4(float _newarray[4][4])
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				_array[i][j] = _newarray[i][j];
	}
	float4x4(float4 _r1, float4 _r2, float4 _r3, float4 _r4)
	{
		_array[0][0] = _r1.x;
		_array[0][1] = _r1.y;
		_array[0][2] = _r1.z;
		_array[0][3] = _r1.w;

		_array[1][0] = _r2.x;
		_array[1][1] = _r2.y;
		_array[1][2] = _r2.z;
		_array[1][3] = _r2.w;

		_array[2][0] = _r3.x;
		_array[2][1] = _r3.y;
		_array[2][2] = _r3.z;
		_array[2][3] = _r3.w;

		_array[3][0] = _r4.x;
		_array[3][1] = _r4.y;
		_array[3][2] = _r4.z;
		_array[3][3] = _r4.w;
	}
	
	float4x4 Transpose()
	{
		float resultArray[4][4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				resultArray[i][j] = _array[j][i];

		return resultArray;
	}

	float4x4 Adjacent()
	{
		float resultArray[4][4];

		///Row-1
		resultArray[0][0] = float3x3(float3(_array[1][1], _array[1][2], _array[1][3]), 
									 float3(_array[2][1], _array[2][2], _array[2][3]), 
									 float3(_array[3][1], _array[3][2], _array[3][3])).Determinant();

		resultArray[0][1] = -float3x3(float3(_array[1][0], _array[1][2], _array[1][3]),
									  float3(_array[2][0], _array[2][2], _array[2][3]),
									  float3(_array[3][0], _array[3][2], _array[3][3])).Determinant();

		resultArray[0][2] = float3x3(float3(_array[1][0], _array[1][1], _array[1][3]),
									 float3(_array[2][0], _array[2][1], _array[2][3]),
									 float3(_array[3][0], _array[3][1], _array[3][3])).Determinant();

		resultArray[0][3] = -float3x3(float3(_array[1][0], _array[1][1], _array[1][2]),
									  float3(_array[2][0], _array[2][1], _array[2][2]),
									  float3(_array[3][0], _array[3][1], _array[3][2])).Determinant();
		///Row-2
		resultArray[1][0] = -float3x3(float3(_array[0][1], _array[0][2], _array[0][3]), 
									 float3(_array[2][1], _array[2][2], _array[2][3]), 
									 float3(_array[3][1], _array[3][2], _array[3][3])).Determinant();

		resultArray[1][1] = float3x3(float3(_array[0][0], _array[0][2], _array[0][3]),
									  float3(_array[2][0], _array[2][2], _array[2][3]),
									  float3(_array[3][0], _array[3][2], _array[3][3])).Determinant();

		resultArray[1][2] = -float3x3(float3(_array[0][0], _array[0][1], _array[0][3]),
									 float3(_array[2][0], _array[2][1], _array[2][3]),
									 float3(_array[3][0], _array[3][1], _array[3][3])).Determinant();

		resultArray[1][3] = float3x3(float3(_array[0][0], _array[0][1], _array[0][2]),
									  float3(_array[2][0], _array[2][1], _array[2][2]),
									  float3(_array[3][0], _array[3][1], _array[3][2])).Determinant();
		///Row-3
		resultArray[2][0] = float3x3(float3(_array[0][1], _array[0][2], _array[0][3]), 
									 float3(_array[1][1], _array[1][2], _array[1][3]), 
									 float3(_array[3][1], _array[3][2], _array[3][3])).Determinant();

		resultArray[2][1] = -float3x3(float3(_array[0][0], _array[0][2], _array[0][3]),
									  float3(_array[1][0], _array[1][2], _array[1][3]),
									  float3(_array[3][0], _array[3][2], _array[3][3])).Determinant();

		resultArray[2][2] = float3x3(float3(_array[0][0], _array[0][1], _array[0][3]),
									 float3(_array[1][0], _array[1][1], _array[1][3]),
									 float3(_array[3][0], _array[3][1], _array[3][3])).Determinant();

		resultArray[2][3] = -float3x3(float3(_array[0][0], _array[0][1], _array[0][2]),
									  float3(_array[1][0], _array[1][1], _array[1][2]),
									  float3(_array[3][0], _array[3][1], _array[3][2])).Determinant();
		///Row-4
		resultArray[3][0] = -float3x3(float3(_array[0][1], _array[0][2], _array[0][3]), 
									 float3(_array[1][1], _array[1][2], _array[1][3]), 
									 float3(_array[2][1], _array[2][2], _array[2][3])).Determinant();

		resultArray[3][1] = float3x3(float3(_array[0][0], _array[0][2], _array[0][3]),
									  float3(_array[1][0], _array[1][2], _array[1][3]),
									  float3(_array[2][0], _array[2][2], _array[2][3])).Determinant();

		resultArray[3][2] = -float3x3(float3(_array[0][0], _array[0][1], _array[0][3]),
									 float3(_array[1][0], _array[1][1], _array[1][3]),
									 float3(_array[2][0], _array[2][1], _array[2][3])).Determinant();

		resultArray[3][3] = float3x3(float3(_array[0][0], _array[0][1], _array[0][2]),
									  float3(_array[1][0], _array[1][1], _array[1][2]),
									  float3(_array[2][0], _array[2][1], _array[2][2])).Determinant();

		return resultArray;
	}

	float Determinant()
	{
		float a1 = _array[0][0] * float3x3(float3(_array[1][1], _array[1][2], _array[1][3]),
			float3(_array[2][1], _array[2][2], _array[2][3]),
			float3(_array[3][1], _array[3][2], _array[3][3])).Determinant();

		float a2 = -_array[0][1] * float3x3(float3(_array[1][0], _array[1][2], _array[1][3]),
			float3(_array[2][0], _array[2][2], _array[2][3]),
			float3(_array[3][0], _array[3][2], _array[3][3])).Determinant();

		float a3 = _array[0][2] * float3x3(float3(_array[1][0], _array[1][1], _array[1][3]),
			float3(_array[2][0], _array[2][1], _array[2][3]),
			float3(_array[3][0], _array[3][1], _array[3][3])).Determinant();

		float a4 = -_array[0][3] * float3x3(float3(_array[1][0], _array[1][1], _array[1][2]),
			float3(_array[2][0], _array[2][1], _array[2][2]),
			float3(_array[3][0], _array[3][1], _array[3][2])).Determinant();

		return a1 + a2 + a3 + a4;
	}

	float4x4 Invert()
	{
		return Adjacent().Transpose() * (1.0 / Determinant());
	}

	float4 operator[](int index1)
	{
		return float4(_array[index1][0], _array[index1][1], _array[index1][2], _array[index1][3]);
	}

	float4x4 operator*(float value)
	{
		float rslt[4][4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				rslt[i][j] = _array[i][j] * value;
			}

		return rslt;
	}

	float4x4 operator*(float4x4 newMatrix)
	{
		float rslt[4][4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				rslt[i][j] = 0;

				for (int k = 0; k < 4; k++) {
					rslt[i][j] += _array[i][k] * newMatrix[k][j];
				}
			}

		return rslt;
	}
};

struct RenderData {
	float frame;
	float NumberOfRaysPerPixel;
	int NumberOfSpheres;
	int NumMeshes;

	float4 SunLightDirection;
	float4 SkyColorHorizon;
	float4 SkyColorZenith;
	float4 GroundColor;
	float4 SunColor;

	float SunFocus;
	float SunIntensity;
	float EnvironmentIntensity;
	float screenWidth;

	float screenHeight;
	float _float1;
	float _float2;
	float _float3;

	int isTestVisualizer;
	int _bool1;
	int _bool2;
	int _bool3;

	float4x4 test_modelWorldToLocalMaxtix;
};

struct AccumulatorData {
	int frame;
	float screenWidth;
	float screenHeight;
	int _3;
};

struct Vertex {
	float x, y;
	float r, g, b;
};

struct RayTracingMaterial
{
	float4 color;
	float4 emissionColor;
	float emissionStrength;
	float smoothness;
	float specularProbability;
	float _;
	float4 specularColor;
};

struct Sphere
{
	float3 position;
	float radius;
	RayTracingMaterial material;
	Sphere()
	{

	}
};

struct RenderData1 {
	Sphere spheres[4];
};