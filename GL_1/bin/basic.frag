#version 330 core

uniform vec3 lightColor;

out vec4 frag_color;

uniform sampler2D texSampler1;

void main()
{
 frag_color = vec4(lightColor, 1.0f);
}