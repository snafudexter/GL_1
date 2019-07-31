#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 frag_color;

uniform sampler2D texSampler1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	//Ambient
	float ambientFactor = 0.1f;
	vec3 ambient = lightColor * ambientFactor;

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float nDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = lightColor * nDotL;

	//Specular
	float shininess = 100.0f;
	float specularFactor = 0.8f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = lightColor * specularFactor * pow(NDotH, shininess);

	vec4 texel = texture(texSampler1, TexCoord);
	frag_color = vec4(ambient + diffuse + specular, 1.0f) * texel;
}