#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;
//uniform vec3 MatAmb;
//uniform vec3 MatSpec;
//uniform vec3 MatDif;
//uniform float shine;
//uniform float numberLights;
uniform vec3 view;

//uniform sampler2D lighting;

//out vec3 finalColor;
out vec3 fragNor;
out vec3 viewVec;
out vec4 pos;
out vec2 vTexCoord;
out vec4 fPosLS;

// Vertex color = (material emission) + (global ambient)*(material ambient) +
// Sigma(1/( k1+ k2*d + k3*d^2)) * [(light ambient)*(material ambient) + (max{L.N, 0})*(light
// diffuse)*(material diffuse) + (max{H.N,0})^n *(light specular)*(material specular) ]

/*
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
*/

void main()
{
	//finalColor = MatAmb;
	fragNor = normalize(vertNor.xyz);
	viewVec = normalize(view - (V * M * vec4(normalize(fragNor), 1.0)).xyz);

	gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
	pos = M * vec4(vertPos.xyz, 1.0);

	vTexCoord = vertTex;
	fPosLS = LS*pos;

	/*
	highp int nl = int(numberLights);
	int i, j;
	for (i = 0; i < nl; i++) {
		for (j = 0; j < nl; j++) {
			ivec2 ij = ivec2(i, j);
			vec4 light = texelFetch(lighting, ij, 0);
			finalColor += calcLight(light.xyz, view, fragNor);
		}
	}
	*/
}