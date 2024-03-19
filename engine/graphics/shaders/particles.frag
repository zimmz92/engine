#version 460

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {

    // Move the origin of the group of fragments that make up the "gl_point" from the upper left corner of the group of
    // fragments to the center of the group of fragments.
    vec2 coord = gl_PointCoord - vec2(0.5);

    // Display the specified color of the group of fragments that make up "gl_point" but make anything beyond a radius
    // of 0.5 increasingly opaque.
    outColor = vec4(fragColor, 0.5 - length(coord));

}
