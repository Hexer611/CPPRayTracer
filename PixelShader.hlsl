float3 ViewParams;
float4x4 CamLocalToWorldMatrix;
int NumMeshes;
int NumTriangles;
int MaxBounceCount = 1;

float _boxThreshold;
float _triThreshold;
//int _testType;

float3 SunLightDirection;
float3 SkyColorHorizon;
float3 SkyColorZenith;
float3 GroundColor;
float3 SunColor;
float SunFocus;
float SunIntensity;
float EnvironmentIntensity;

cbuffer ErdemBuff : register(b0)
{
    int Frame;
    int NumSpheres;
    float NumberOfRaysPerPixel;
    float _1f;
};

struct appdata
{
	float4 vertex : POSITION; // vertex position
	float2 uv : TEXCOORD0; // texture coordinate
};

struct v2f
{
	float2 uv : TEXCOORD0;
	float4 vertex : SV_POSITION;
};

struct Ray
{
	float3 origin;
	float3 dir;
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

struct HitInfo
{
	bool didHit;
	float dst;
	float3 hitPoint;
	float3 normal;
	RayTracingMaterial material;
};

struct BVHBoundingBox
{
	float3 Min;
	float3 Max;
};

struct BVHNode
{
	BVHBoundingBox Bounds;
	int childIndex;
	int triangleIndex;
	int triangleCount;
};

struct MeshInfo
{
	int numTriangles;
	int firstTriangleIndex;
	int nodesStartIndex;
	float3 boundsMin;
	float3 boundsMax;
	RayTracingMaterial material;
};
			
struct Triangle
{
	float3 posA, posB, posC;
	float3 normalA, normalB, normalC;
};

struct Sphere
{
	float3 position;
	float radius;
	RayTracingMaterial material;
};

cbuffer Buffer1 : register(b1)
{
    Sphere Spheres[4];
};

Triangle Triangles[1];
MeshInfo Meshes[1];
BVHNode Nodes[1];

float NoBoundsHit(Ray ray, float3 boxMin, float3 boxMax)
{
	float3 invDir = 1 / ray.dir;
	float3 tMin = (boxMin - ray.origin) * invDir;
	float3 tMax = (boxMax - ray.origin) * invDir;
	float3 t1 = min(tMin, tMax);
	float3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	bool didhit = tFar >= tNear;
	return didhit ? tNear : 1.#INF;
}

HitInfo RaySphere(Ray ray, float3 center, float radius)
{
	HitInfo hitInfo = (HitInfo)0;
	float3 offsetRayOrigin = ray.origin - center;

	float a = dot(ray.dir, ray.dir);
	float b = 2 * dot(offsetRayOrigin, ray.dir);
	float c = dot(offsetRayOrigin, offsetRayOrigin) - radius * radius;

	float disc = b*b-4*a*c;

	if (disc >= 0)
	{
		float dst = (-b - sqrt(disc) ) / (2*a);
		if (dst >= 0)
		{
			hitInfo.didHit = true;
			hitInfo.dst = dst;
			hitInfo.hitPoint = ray.origin + ray.dir * dst;
			hitInfo.normal = normalize(hitInfo.hitPoint - center);
		}
	}

	return hitInfo;
}

HitInfo RayTriangle(Ray ray, Triangle tri)
{
	float3 edgeAB = tri.posB - tri.posA;
	float3 edgeAC = tri.posC - tri.posA;
	float3 normalVector = cross(edgeAB, edgeAC);
	float3 ao = ray.origin - tri.posA;
	float3 dao = cross(ao, ray.dir);

	float determinant = -dot(ray.dir, normalVector);
	float invDet = 1 / determinant;
				
	// Calculate dst to triangle & barycentric coordinates of intersection point
	float dst = dot(ao, normalVector) * invDet;
	float u = dot(edgeAC, dao) * invDet;
	float v = -dot(edgeAB, dao) * invDet;
	float w = 1 - u - v;
				
	// Initialize hit info
	HitInfo hitInfo;
	hitInfo.didHit = determinant >= 1E-6 && dst >= 0 && u >= 0 && v >= 0 && w >= 0;
	hitInfo.hitPoint = ray.origin + ray.dir * dst;
	hitInfo.normal = normalize(tri.normalA * w + tri.normalB * u + tri.normalC * v);
	hitInfo.dst = dst;
	return hitInfo;
}

HitInfo BVHRayCollision(int firstNodeIndex, Ray ray, inout int2 stats)
{
	int nodeIndexStack[32];
	int stackIndex = 0;
	nodeIndexStack[stackIndex++] = firstNodeIndex;

	HitInfo hitInfo;
	hitInfo.dst = 1.#INF;

	while (stackIndex > 0)
	{
		break;
		BVHNode curNode = Nodes[nodeIndexStack[--stackIndex]];

		if (curNode.childIndex == firstNodeIndex)
		{
			stats[1] += curNode.triangleCount;
			for (int i = curNode.triangleIndex; i < curNode.triangleIndex + curNode.triangleCount; i++)
			{
				Triangle tri = Triangles[i];
				HitInfo triHitInfo = RayTriangle(ray, tri);
								
				if (triHitInfo.didHit && triHitInfo.dst < hitInfo.dst)
				{
					hitInfo = triHitInfo;
				}
			}
		}
		else
		{
			stats[0]+=2;
			BVHNode Child1 = Nodes[curNode.childIndex + 0];
			BVHNode Child2 = Nodes[curNode.childIndex + 1];
							
			float dst1 = NoBoundsHit(ray, Child1.Bounds.Min, Child1.Bounds.Max);
			float dst2 = NoBoundsHit(ray, Child2.Bounds.Min, Child2.Bounds.Max);

			if (dst1 > dst2)
			{
				if (dst1 < hitInfo.dst) nodeIndexStack[stackIndex++] = curNode.childIndex + 0;
				if (dst2 < hitInfo.dst) nodeIndexStack[stackIndex++] = curNode.childIndex + 1;
			}
			else
			{
				if (dst2 < hitInfo.dst) nodeIndexStack[stackIndex++] = curNode.childIndex + 1;
				if (dst1 < hitInfo.dst) nodeIndexStack[stackIndex++] = curNode.childIndex + 0;
			}
		}
	}
				
	return hitInfo;
}

HitInfo CalculateRayCollision (Ray ray, inout int2 stats)
{
	HitInfo closestHit = (HitInfo)0;
	closestHit.dst = 1.#INF;
	
	for (int i = 0; i < NumSpheres; i++)
	{
		Sphere sphere = Spheres[i];
		HitInfo hitInfo = RaySphere(ray, sphere.position, sphere.radius);

		if (hitInfo.didHit && hitInfo.dst < closestHit.dst)
		{
			stats[0]++;
			closestHit = hitInfo;
			closestHit.material = sphere.material;
		}
	}

	for (int i = 0; i < NumMeshes; i++)
	{
		MeshInfo meshInfo = Meshes[i];
		BVHNode firstNode = Nodes[meshInfo.nodesStartIndex];

		HitInfo hitInfo = BVHRayCollision(meshInfo.nodesStartIndex, ray, stats);
		if (hitInfo.didHit && hitInfo.dst < closestHit.dst)
		{
			closestHit = hitInfo;
			closestHit.material = meshInfo.material;
		}
	}

	return closestHit;
}

// PCG (permuted congruential generator). Thanks to:
// www.pcg-random.org and www.shadertoy.com/view/XlGcRh
int NextRandom(inout int state)
{
    state = state * 747796405 + 2891336453;
    int result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return result;
}

float RandomValue(inout int state)
{
	return NextRandom(state) / 4294967295.0;
}

float RandomValueNormalDistribution(inout int state) // inout sets the variable state for outside too
{
	float theta = 2 * 3.1415926 * RandomValue(state);
	float rho = sqrt(-2 * log(RandomValue(state)));
	return rho * cos(theta);
}

float3 RandomDirection(inout int state)
{
	float x = RandomValueNormalDistribution(state);
	float y = RandomValueNormalDistribution(state);
	float z = RandomValueNormalDistribution(state);
				
	return normalize(float3(x,y,z));
}

float3 RandomHemisphereDirection(float3 normal, inout int rngState)
{
	float3 dir = RandomDirection(rngState);
	return dir * sign(dot(normal, dir));
}

float3 GetEnvironmentLight(Ray ray)
{
	float skyGradientT = pow(smoothstep(0, 0.4, ray.dir.y), 0.35);
	float3 skyGradient = lerp(SkyColorHorizon, SkyColorZenith, skyGradientT);
	float sun = pow(max(0, dot(ray.dir, -SunLightDirection)), 2) * SunIntensity;

	float groundToSkyT = smoothstep(-0.01, 0, ray.dir.y);
	return lerp(GroundColor, skyGradient, groundToSkyT) * EnvironmentIntensity + sun * SunColor;
}

float3 GetEnvironmentBackGround(Ray ray)
{
	float skyGradientT = pow(smoothstep(0, 0.4, ray.dir.y), 0.35);
	float3 skyGradient = lerp(SkyColorHorizon, SkyColorZenith, skyGradientT);
	float sun = pow(max(0, dot(ray.dir, -SunLightDirection)), SunFocus);

	float groundToSkyT = smoothstep(-0.01, 0, ray.dir.y);
	float sunMask = groundToSkyT >= 1;
	return lerp(GroundColor, skyGradient, groundToSkyT) + sun * sunMask;
}

float3 Trace(Ray ray, inout int rngState)
{
	float3 incomingLight = 0;
	float3 rayColor = 1;
	//bool hasHit = false;
	int2 stats = 0;
	int MaxBounceCount = 10;
	
	for (int i = 0; i <= MaxBounceCount; i++)
	{
		HitInfo hitInfo = CalculateRayCollision(ray, stats);
		
		if (hitInfo.didHit)
		{
			ray.origin = hitInfo.hitPoint;
			RayTracingMaterial material = hitInfo.material;
			bool isSpecularBounce = material.specularProbability >= RandomValue(rngState);
			float3 diffuseDir = normalize(hitInfo.normal + RandomDirection(rngState));
			float3 specularDir = reflect(ray.dir, hitInfo.normal);
            ray.dir = normalize(lerp(diffuseDir, specularDir, material.smoothness * isSpecularBounce));

			float3 emittedLight =  material.emissionColor * material.emissionStrength;
			incomingLight += emittedLight * rayColor;

			rayColor *= lerp(material.color, material.specularColor, isSpecularBounce);

			float p = max(rayColor.r, max(rayColor.g, rayColor.b));
			if (RandomValue(rngState) >= p) {
				break;
			}
			rayColor *= 1.0f / p;
			//hasHit = true;
        }
		else
        {
			incomingLight += GetEnvironmentLight(ray) * rayColor;
			/*if (hasHit)
				incomingLight += GetEnvironmentLight(ray) * rayColor;
			else
				incomingLight += GetEnvironmentBackGround(ray);
			*/
		}
    }
    
	float boxVis = stats[0] / _boxThreshold;
	float triVis = stats[1] / _triThreshold;

	int _testType = 3;
	switch (_testType)
	{
		case 0:
			return boxVis < 1 ? boxVis : float3(1,0,0);
		break;
		case 1:
			return triVis < 1 ? triVis : float3(1,0,0);
		break;
		case 2:
			if (max(boxVis, triVis) > 1) return 1;
			return float3(boxVis, 0, triVis);
		break;
		case 3:
			return incomingLight;
		break;
		default:
			return 0;
		break;
	}
	return incomingLight;
}

float4x4 CreateLocalToWorldMatrix(float3 scale, float3 rotation, float3 translation)
{
    // Scaling matrix
    float4x4 scaleMatrix = float4x4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    );

    // Rotation matrices
    float cosX = cos(rotation.x);
    float sinX = sin(rotation.x);
    float4x4 rotationXMatrix = float4x4(
        1, 0, 0, 0,
        0, cosX, -sinX, 0,
        0, sinX, cosX, 0,
        0, 0, 0, 1
    );

    float cosY = cos(rotation.y);
    float sinY = sin(rotation.y);
    float4x4 rotationYMatrix = float4x4(
        cosY, 0, sinY, 0,
        0, 1, 0, 0,
        -sinY, 0, cosY, 0,
        0, 0, 0, 1
    );

    float cosZ = cos(rotation.z);
    float sinZ = sin(rotation.z);
    float4x4 rotationZMatrix = float4x4(
        cosZ, -sinZ, 0, 0,
        sinZ, cosZ, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    // Combined rotation matrix
    float4x4 rotationMatrix = mul(rotationZMatrix, mul(rotationYMatrix, rotationXMatrix));

    // Translation matrix
    float4x4 translationMatrix = float4x4(
        1, 0, 0, translation.x,
        0, 1, 0, translation.y,
        0, 0, 1, translation.z,
        0, 0, 0, 1
    );
    // Local to World matrix: scaling -> rotation -> translation
    float4x4 localToWorldMatrix = mul(translationMatrix, mul(rotationMatrix, scaleMatrix));

    return localToWorldMatrix;
}

struct Input {
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

float4 main(Input input) : SV_TARGET
{
	float screenWidth = 584.0;
	float screenHeight = 361.0;
	float2 numPixels = float2(screenWidth, screenHeight);
	int2 _pixelCoord = int2(input.position.x, input.position.y);
	float2 pixelCoord = float2(_pixelCoord.x / numPixels.x, _pixelCoord.y / numPixels.y);
	pixelCoord.y = 1 - pixelCoord.y;

    int pixelIndex = _pixelCoord.y * numPixels.x + _pixelCoord.x;
    int rngState = pixelIndex + Frame * screenWidth * screenHeight;
	
	float cameraFOV = 60.0;
	float farPlane = 1000.0;
	float nearPlane = 1.0;
	float aspectRatio = screenWidth / screenHeight;

	float tanHalfFov = tan(cameraFOV / 2.0);
    float zRange = farPlane - nearPlane;
	
	float translateX = 1;
	float3 _WorldSpaceCameraPos = float3(translateX,0,0);
	float4x4 CamLocalToWorldMatrix = CreateLocalToWorldMatrix(float3(1,1,1), float3(0,0,0), _WorldSpaceCameraPos);

	float planeHeight = nearPlane * tan(radians(cameraFOV * 0.5f)) * 2;
    float planeWidth = planeHeight * aspectRatio;

	float3 ViewParams = float3(planeWidth, planeHeight, nearPlane);
	// Calculate ray
	float3 viewPointLocal = float3(pixelCoord - 0.5, 1) * ViewParams;
	float3 viewPoint = mul(CamLocalToWorldMatrix, float4(viewPointLocal, 1));

	Ray ray;
	ray.origin = _WorldSpaceCameraPos;
	ray.dir = normalize(viewPoint - ray.origin);

	float3 totalLight = 0;

    for (int i = 0; i < NumberOfRaysPerPixel; i++)
	{
		totalLight += Trace(ray, rngState);
	}

    float3 pixelColor = totalLight / NumberOfRaysPerPixel;
	return float4( pixelColor.xyz, 1.0);
}
