#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int baseInstance;

layout(location = 0) out vec4 outColor;

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

layout(set = 1, binding = 0) uniform sampler2D texSampler[8];

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

  uint texIndex = objectBuffer.objects[baseInstance].textureIndex;

  if(texIndex == 9)
      {
          outColor = vec4(fragColor, 1.0);
      }
      else
      {
          outColor = texture(texSampler[texIndex], fragTexCoord);
      }
}