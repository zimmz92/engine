#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out int baseInstance;

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

//all object matrices
struct ObjectData{
	mat4 modelMatrix;
	mat4 normalMatrix;
	uint textureIndex;
};

layout(set = 2, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} objectBuffer;

const float AMBIENT = 0.02;

void main() {
    vec4 positionWorld = objectBuffer.objects[gl_BaseInstance].modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
    fragNormalWorld = normalize(mat3(objectBuffer.objects[gl_BaseInstance].normalMatrix) * normal);
    fragPosWorld = positionWorld.xyz;
    fragColor = color;
    fragTexCoord = uv;
    baseInstance = gl_BaseInstance;
}