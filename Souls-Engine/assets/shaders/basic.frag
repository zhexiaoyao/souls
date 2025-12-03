#version 330 core
out vec4 FragColor;

in vec3 Color;

uniform bool useOverrideColor;
uniform vec3 overrideColor;

void main()
{
    if (useOverrideColor) {
        FragColor = vec4(overrideColor, 1.0);
    } else {
        FragColor = vec4(Color, 1.0);
    }
}

