#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 MatAmb;
uniform vec3 MatSpec;
uniform vec3 MatDif;
uniform float shine;
uniform float numberLights;

uniform sampler2D lighting;

out vec3 finalColor;

// Vertex color = (material emission) + (global ambient)*(material ambient) +
// Sigma(1/( k1+ k2*d + k3*d^2)) * [(light ambient)*(material ambient) + (max{L.N, 0})*(light
// diffuse)*(material diffuse) + (max{H.N,0})^n *(light specular)*(material specular) ]

vec3 calcLight(vec3 light, vec3 viewVec, vec3 fragNor)
{
	vec3 lightVec = normalize(light.xyz);
	vec3 l = normalize(lightVec - (gl_Position).xyz);
	vec3 halfVec = normalize(l + viewVec);

	vec3 k_s = MatSpec;
	vec3 k_d = MatDif;

	vec3 diffuse = MatDif*(clamp(dot(fragNor, lightVec), 0, 1));//*texColor.xyz;
	vec3 specular = MatSpec*pow(clamp(dot(halfVec, fragNor), 0, 1), shine);

	vec3 phongColor = (k_d * diffuse) + (k_s * specular);
//	vec3 phongColor = diffuse + specular;


	return phongColor;
}

void main()
{
	finalColor = MatAmb;
	vec3 fragNor = normalize((M * vec4(normalize(vertNor), 0.0)).xyz);
	vec3 viewVec = normalize(vec3(0.0, 0.0, 0.0) - (V * M * vertPos).xyz);
	gl_Position = P * V * M * vertPos;

	highp int nl = int(numberLights);
	int i, j;
	for (i = 0; i < nl; i++) {
		for (j = 0; j < nl; j++) {
			ivec2 ij = ivec2(i, j);
			vec4 light = texelFetch(lighting, ij, 0);
			finalColor += calcLight(light.xyz, viewVec, fragNor);
		}
	}



}