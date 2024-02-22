#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) flat in int baseInstance;

layout(location = 0) out vec4 outColor;

struct PointLight{
    vec4 position;
    vec4 color;
};

layout (constant_id = 0) const int MATERIAL_ID = 0;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    PointLight pointLights[10]; // This, 10, could be passed in as a specialization constant
    int numLights;
    float deltaTime;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler[8];

struct VkAabbPositionsKHR{
    float    minX;
    float    minY;
    float    minZ;
    float    maxX;
    float    maxY;
    float    maxZ;
};

//all object matrices
struct ObjectData{
	mat4 modelMatrix;
	mat4 normalMatrix;
	uint textureIndex[20][10];
    VkAabbPositionsKHR aabb;
    uint modelObbIndex;
    float spacer1[1];
};

layout(std430, set = 2, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} objectBuffer;

void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++){
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
        directionToLight = normalize(directionToLight); // do this after attenuation to not mess up the attenuation calc

        float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0);
        vec3 intensity =  light.color.xyz * light.color.w * attenuation;
        diffuseLight += intensity * cosAngIncidence;

        // specular Lighting
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0 ,1);
        // TODO Pass in power as it's on value per model for materials.
        blinnTerm = pow(blinnTerm, 512.0); // Higher power = sharper highlights
        specularLight += intensity * blinnTerm;
    }

    uint texIndex = objectBuffer.objects[baseInstance].textureIndex[MATERIAL_ID][0];


    if(texIndex == 9)
    {
        outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
    }
    else
    {
        outColor = vec4(diffuseLight * fragColor * texture(texSampler[texIndex], fragTexCoord).rgb + specularLight * fragColor * texture(texSampler[uint(texIndex)], fragTexCoord).rgb, 1.0);
    }
}