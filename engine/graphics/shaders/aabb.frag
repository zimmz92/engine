#version 450

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
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}