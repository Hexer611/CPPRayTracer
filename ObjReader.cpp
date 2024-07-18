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

void ObjReader::ReadFile(const char filePath[], bool flattenFaces)
{
	std::ifstream infile(filePath);
	std::string line;

	std::vector<float3> vertices = {};
	std::vector<int> trianglesv = {}; // Vertex index
	std::vector<int> trianglesvt = {}; // Vertex tangen index
	std::vector<int> trianglesvn = {}; // Vertex normal index
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
			float3 newVector = float3(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
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
				if (vv[0] != "")
					trianglesv.push_back(std::stoi(vv[0]));
				if (vv[1] != "")
					trianglesvt.push_back(std::stoi(vv[1]));
				if (vv[2] != "")
					trianglesvn.push_back(std::stoi(vv[2]));
			}
		}
	}

	for (int j = 0; j < trianglesv.size(); j+=3)
	{
		BVHTriangle newTrig = {};
		newTrig.posA = vertices[trianglesv[j] - 1];
		newTrig.posB = vertices[trianglesv[j + 1] - 1];
		newTrig.posC = vertices[trianglesv[j + 2] - 1];

		if (flattenFaces)
		{
			float3 A = newTrig.posB - newTrig.posA;
			float3 B = newTrig.posC - newTrig.posA;
			float3 Normal = A * B;
			Normal = Normal.Normalize();

			newTrig.normalA = Normal;
			newTrig.normalB = Normal;
			newTrig.normalC = Normal;
		}
		else
		{
			newTrig.normalA = normals[trianglesvn[j] - 1];
			newTrig.normalB = normals[trianglesvn[j + 1] - 1];
			newTrig.normalC = normals[trianglesvn[j + 2] - 1];
		}
		
		TriangleData.push_back(newTrig);
	}
}
