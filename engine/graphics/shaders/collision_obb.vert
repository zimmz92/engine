#version 460

// AABB (x,y,z)
// 0 = minimum value, 1 = maximum value
const uint OBB_VERTICES[24][3] = uint[24][3](
  uint[3](0,4,2),
  uint[3](0,4,5),
  uint[3](0,4,2),
  uint[3](3,4,2),
  uint[3](0,4,2),
  uint[3](0,1,2),
  uint[3](3,4,5),
  uint[3](0,4,5),
  uint[3](3,4,5),
  uint[3](3,4,2),
  uint[3](3,4,5),
  uint[3](3,1,5),
  uint[3](0,1,5),
  uint[3](3,1,5),
  uint[3](0,1,5),
  uint[3](0,1,2),
  uint[3](0,1,5),
  uint[3](0,4,5),
  uint[3](3,1,2),
  uint[3](3,1,5),
  uint[3](3,1,2),
  uint[3](0,1,2),
  uint[3](3,1,2),
  uint[3](3,4,2)
);

layout(location = 0) in vec3 position;

struct PointLight{
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    PointLight pointLights[10]; // This, 10, could be passed in as a specialization constant
    int numLights;
    float deltaTime;
} ubo;

struct VkAabbPositionsKHR{
    float    minX;
    float    minY;
    float    minZ;
    float    maxX;
    float    maxY;
    float    maxZ;
};

//all object matrices
struct EntityData{
	mat4 modelMatrix;
	mat4 normalMatrix;
	uint textureIndex[20][10];
    VkAabbPositionsKHR aabb;
    uint modelObbIndex;
    float spacer1[1];
};

layout(std430, set = 1, binding = 0) readonly buffer ObbBuffer{
	VkAabbPositionsKHR modelOBB[];
} obbBuffer;

layout(std430, set = 1, binding = 1) readonly buffer EntityBuffer{
	EntityData entities[];
} entityBuffer;

void main()
{
    uint entityIndex = entityBuffer.entities[gl_InstanceIndex].modelObbIndex;

    float obb[6] = {obbBuffer.modelOBB[entityIndex].minX,
                    obbBuffer.modelOBB[entityIndex].minY,
                    obbBuffer.modelOBB[entityIndex].minZ,
                    obbBuffer.modelOBB[entityIndex].maxX,
                    obbBuffer.modelOBB[entityIndex].maxY,
                    obbBuffer.modelOBB[entityIndex].maxZ};

    uint[3] obb_vertex_indices = OBB_VERTICES[gl_VertexIndex];
    vec3 obb_vertex_position = vec3(obb[obb_vertex_indices[0]],obb[obb_vertex_indices[1]],obb[obb_vertex_indices[2]]);
    vec4 positionWorld = vec4(obb_vertex_position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
}