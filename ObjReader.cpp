#include "ObjReader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "DataTypes.h"
#include "BVHDataTypes.h"
#include <iostream>
#include "BVHCalculator.h"
#include "VectorUtils.h"

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

float3 getFLoat3OfString(string line, int offset)
{
	float floats[3];
	int normalIndex = 0;
	int index = offset;
	int lineSize = line.size();
	for (int i = offset; i < lineSize; i++)
	{
		if (line[i] == ' ')
		{
			floats[normalIndex] = std::stof(line.substr(index, i - index));
			index = i + 1;
			normalIndex++;
		}
	}
	floats[normalIndex] = std::stof(line.substr(index, lineSize));
	return float3(floats[0], floats[1], floats[2]);
}

int* getFaceIndices(string line, int offset)
{
	int* ints = new int[9];
	int intIndex = 0;
	int index = offset;
	int lineSize = line.size();
	for (int i = offset; i < lineSize; i++)
	{
		if (line[i] == ' ' || line[i] == '/')
		{
			ints[intIndex] = i == index ? 0 : std::stoi(line.substr(index, i - index));
			index = i + 1;
			intIndex++;
		}
		if (intIndex >= 9)
			return nullptr;
	}
	ints[intIndex] = std::stoi(line.substr(index, lineSize));
	return ints;
}

void ObjReader::ReadFile(const char filePath[], bool flattenFaces, float3 pos, float3 rot, float3 scale)
{
	std::ifstream infile(filePath);
	std::string line;

	std::vector<float3> vertices = {};
	std::vector<int> trianglesv = {}; // Vertex index
	std::vector<int> trianglesvt = {}; // Vertex tangen index
	std::vector<int> trianglesvn = {}; // Vertex normal index
	std::vector<float3> normals = {};

	while (std::getline(infile, line))
	{
		if (line[0] == 'v' && line[1] == 'n')
		{
			//std::vector<std::string> v = split(line, ' ');
			//float3 newNormal = float3(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
			normals.push_back(getFLoat3OfString(line, 3));
		}
		if (line[0] == 'v' && line[1] == ' ')
		{
			//std::vector<std::string> v = split(line, ' ');
			//float3 newVector = float3(std::stof(v[1]), std::stof(v[2]), std::stof(v[3]));
			vertices.push_back(getFLoat3OfString(line, 2));
		}
		if (line[0] == 'f' && line[1] == ' ')
		{
			int* intArray = getFaceIndices(line, 2);
			if (intArray == nullptr)
				continue;

			trianglesv.push_back(intArray[0]);
			trianglesv.push_back(intArray[3]);
			trianglesv.push_back(intArray[6]);

			trianglesvt.push_back(intArray[1]);
			trianglesvt.push_back(intArray[4]);
			trianglesvt.push_back(intArray[7]);

			trianglesvn.push_back(intArray[2]);
			trianglesvn.push_back(intArray[5]);
			trianglesvn.push_back(intArray[8]);
			continue;
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

	std::vector<float3> finalnormals = {};
	for (int j = 0; j < trianglesv.size(); j += 3)
	{
		trianglesv[j] = trianglesv[j] - 1;
		trianglesv[j+1] = trianglesv[j+1] - 1;
		trianglesv[j+2] = trianglesv[j+2] - 1;

		BVHTriangle newTrig = BVHTriangle
		(
			vertices[trianglesv[j]],
			vertices[trianglesv[j + 1]],
			vertices[trianglesv[j + 2]]
		);

		float3 normal1;
		float3 normal2;
		float3 normal3;

		if (flattenFaces)
		{
			normal1 = newTrig.Normal();
			normal2 = newTrig.Normal();
			normal3 = newTrig.Normal();
		}
		else
		{
			normal1 = normals[trianglesvn[j] - 1];
			normal2 = normals[trianglesvn[j + 1] - 1];
			normal3 = normals[trianglesvn[j + 2] - 1];
		}

		finalnormals.push_back(normal1);
		finalnormals.push_back(normal2);
		finalnormals.push_back(normal3);
	}
	
	rawObject = RawObject();
	rawObject.vertices = vertices;
	rawObject.normals = finalnormals;
	rawObject.triangles = trianglesv;

	BVHCalculator calculator;
	bvhObject = calculator.CalculateBVH(rawObject);

	bvhObject.MeshInfo.modelLocalToWorldMaxtix = VectorUtils::CreateWorldToLocalMatrix(pos, rot, scale);
	bvhObject.MeshInfo.modelWorldToLocalMaxtix = VectorUtils::CreateWorldToLocalMatrix(pos, rot, scale).Invert();

	CalculateDebugData();
}

void ObjReader::CalculateDebugData()
{
	debugData.minLeafTrig = 9999;
	debugData.maxLeafTrig = 0;

	debugData.minLeafDepth = 9999;
	debugData.maxLeafDepth = 0;

	GetDebugLeaf(bvhObject.Nodes[0], 0);
}

void ObjReader::GetDebugLeaf(BVHNode node, int depth)
{
	if (node.childIndex == 0)
	{
		debugData.minLeafTrig = min(debugData.minLeafTrig, node.triangleCount);
		debugData.maxLeafTrig = max(debugData.maxLeafTrig, node.triangleCount);

		debugData.minLeafDepth = min(debugData.minLeafDepth, depth);
		debugData.maxLeafDepth = max(debugData.maxLeafDepth, depth);
		return;
	}

	GetDebugLeaf(bvhObject.Nodes[node.childIndex], depth + 1);
	GetDebugLeaf(bvhObject.Nodes[node.childIndex + 1], depth + 1);
}