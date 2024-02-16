#version 450

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

layout(push_constant) uniform Push {
    VkAabbPositionsKHR obb;
    float spacer2[2];

    vec3 translation;
    float spacer3;

    vec3 rotation;
    float spacer4;

    vec3 scale;
    float spacer5;

} push;

void main() {
    const float c3 = cos(push.rotation.z);
    const float s3 = sin(push.rotation.z);
    const float c2 = cos(push.rotation.x);
    const float s2 = sin(push.rotation.x);
    const float c1 = cos(push.rotation.y);
    const float s1 = sin(push.rotation.y);
    //const vec3 invScale = 1.0 / push.scale;

    float aabb[6] = {push.translation.x,
                  push.translation.y,
                  push.translation.z,
                  push.translation.x,
                  push.translation.y,
                  push.translation.z};

   float obb[6] = {push.obb.minX*push.scale.x,
                   push.obb.minY*push.scale.y,
                   push.obb.minZ*push.scale.z,
                   push.obb.maxX*push.scale.x,
                   push.obb.maxY*push.scale.y,
                   push.obb.maxZ*push.scale.z};

    mat3 rotationMatrix = {{
            (c1 * c3 + s1 * s2 * s3),
            (c2 * s3),
            (c1 * s2 * s3 - c3 * s1)
        },
        {
            (c3 * s1 * s2 - c1 * s3),
            (c2 * c3),
            (c1 * c3 * s2 + s1 * s3)
        },
        {
            (c2 * s1),
            (-s2),
            (c1 * c2)
        }
    };

    for(uint i = 0; i<3; i++){
        for(uint j = 0; j<3; j++){
            float e = rotationMatrix[j][i] * obb[j];
            float f = rotationMatrix[j][i] * obb[j+3];
            aabb[i] += e < f ? e : f;
            aabb[i + 3] += e < f ? f : e;
        }
    }

    uint[3] aabb_vertex_indices = AABB_VERTICES[gl_VertexIndex];
    vec3 aabb_vertex_position = vec3(aabb[aabb_vertex_indices[0]],aabb[aabb_vertex_indices[1]],aabb[aabb_vertex_indices[2]]);
    vec4 positionWorld = vec4(aabb_vertex_position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;


}