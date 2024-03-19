#version 460

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    // Specify the size of the width/height of each point primitive to be created.
    gl_PointSize = 8.0;

    // Get the position of the point primitive from the compute shader.
    gl_Position = vec4(inPosition.xy, 0.0, 1.0);

    // Get the color from the compute shader.
    fragColor = inColor.rgb;

}
