#include "ObjReader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "DataTypes.h"
#include "BHVDataTypes.h"
#include <iostream>

ObjReader::ObjReader()
{

}

ObjReader::~ObjReader()
{

}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

void ObjReader::ReadFile(const char filePath[])
{
	std::ifstream infile(filePath);
	std::string line;

	std::vector<float3> vertices = {};
	std::vector<int> triangles = {};
	std::vector<float3> normals = {};

	float multiplier = 1;
	while (std::getline(infile, line))
	{
		if (line[0] == 'v' && line[1] == 'n')
		{
			std::vector<std::string> v = split(line, ' ');
			float3 newNormal = float3(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
			normals.push_back(newNormal);
		}
		if (line[0] == 'v' && line[1] == ' ')
		{
			std::vector<std::string> v = split(line, ' ');
			float3 newVector = float3(std::stof(v[1]) * multiplier, std::stof(v[2]) * multiplier, std::stof(v[3]) * multiplier);
			vertices.push_back(newVector);
		}
		if (line[0] == 'f' && line[1] == ' ')
		{
			std::vector<std::string> v = split(line, ' ');
			if (v.size() > 4) // We don't support triangles with more than 3 vertices (first index is 'f')
				continue;

			for (int i = 1; i < 4; i++)
			{
				int vertexIndex = 0;
				int vertexUVIndex = 0;
				int vertexNormalIndex = 0;

				std::string curString = "";

				std::vector<std::string> vv = split(v[i], '/');
				triangles.push_back(std::stoi(vv[0]));
			}
		}
	}
	
	for (int j = 0; j < triangles.size(); j+=3)
	{
		BVHTriangle newTrig = {};
		newTrig.posA = vertices[triangles[j]-1];
		newTrig.posB = vertices[triangles[j+1]-1];
		newTrig.posC = vertices[triangles[j+2]-1];
		newTrig.normalA = float3(0, 0, 0);
		newTrig.normalB = float3(0, 0, 0);
		newTrig.normalC = float3(0, 0, 0);
		TriangleData.push_back(newTrig);
	}
}
