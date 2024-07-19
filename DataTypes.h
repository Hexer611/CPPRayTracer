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
};

struct RenderData {
	int frame;
	int NumberOfSpheres;
	int NumMeshes;
	float NumberOfRaysPerPixel;

	float4 SunLightDirection;
	float4 SkyColorHorizon;
	float4 SkyColorZenith;
	float4 GroundColor;
	float4 SunColor;
	float SunFocus;
	float SunIntensity;
	float EnvironmentIntensity;
	float _float1;

	int isTestVisualizer;
	int _bool1;
	int _bool2;
	int _bool3;
};

struct AccumulatorData {
	int frame;
	int _1;
	int _2;
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