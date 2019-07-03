#version 330 core
in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D diffuse;
uniform sampler2D normal;

void main()
{
 frag_color = mix(texture(diffuse, TexCoord), texture(normal, TexCoord), 0.5);
}