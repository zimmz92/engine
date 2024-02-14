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

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
    float obb[6];
} push;

void main() {
    uint[3] aabb_vertex_indices = AABB_VERTICES[gl_VertexIndex];
    vec3 aabb_vertex_position = vec3(push.obb[aabb_vertex_indices[0]],push.obb[aabb_vertex_indices[1]],push.obb[aabb_vertex_indices[2]]);
    //vec4 positionWorld = push.modelMatrix * vec4(aabb_vertex_position, 1.0);
    vec4 positionWorld = vec4(aabb_vertex_position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
}