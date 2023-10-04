#version 460

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out int baseInstance;

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
} ubo;

//all object matrices
struct ObjectData{
	mat2 transform;
	vec2 offset;
	uint textureIndex;
};

layout(set = 2, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} objectBuffer;

void main() {
  gl_Position = vec4(objectBuffer.objects[gl_BaseInstance].transform * position + objectBuffer.objects[gl_BaseInstance].offset, 0.0, 1.0);
  fragColor = color;
  fragTexCoord = uv;
  baseInstance = gl_BaseInstance;
}