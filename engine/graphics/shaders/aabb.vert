#version 460

// AABB (x,y,z)
// 0 = minimum value, 1 = maximum value
const uint AABB_VERTICES[24][3] = uint[24][3](
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
    //uint arrayIndex = gl_BaseInstance + gl_InstanceIndex;
    uint arrayIndex = gl_InstanceIndex;

    float aabb[6] = {entityBuffer.entities[arrayIndex].aabb.minX,
                     entityBuffer.entities[arrayIndex].aabb.minY,
                     entityBuffer.entities[arrayIndex].aabb.minZ,
                     entityBuffer.entities[arrayIndex].aabb.maxX,
                     entityBuffer.entities[arrayIndex].aabb.maxY,
                     entityBuffer.entities[arrayIndex].aabb.maxZ};

    uint[3] aabb_vertex_indices = AABB_VERTICES[gl_VertexIndex];
    vec3 aabb_vertex_position = vec3(aabb[aabb_vertex_indices[0]],aabb[aabb_vertex_indices[1]],aabb[aabb_vertex_indices[2]]);
    vec4 positionWorld = vec4(aabb_vertex_position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
}