#version 330 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatSpec;
uniform vec3 MatDif;
uniform float shine;
uniform vec3 lightPos;
uniform sampler2D lighting;

uniform float numberLights;

in vec3 fragNor;
in vec3 viewVec;
in vec4 pos;

vec3 calcLight(vec3 lightPosition, vec3 fragNor)
{
	float distance = distance(lightPosition, pos.xyz);
	vec3 lightDirection = lightPosition - pos.xyz;
	float diffuse = pow(1/distance*clamp(dot(fragNor, lightDirection), 0, 1),2);

	vec3 lightVec = normalize(lightPosition.xyz);
	vec3 k_s = MatSpec;
	vec3 l = normalize(lightVec - (pos).xyz);
	vec3 halfVec = normalize(l + viewVec);
	vec3 specular = MatSpec*pow(clamp(dot(halfVec, fragNor), 0, 1), shine);

	vec3 lightColor = vec3(0);
	lightColor += (diffuse * vec3(1,1,1)) + (k_s * specular);
	return lightColor;

}

/* Very simple Diffuse shader with a directional light*/
void main()
{
    float shade;
    vec3 finalColor = vec3(0);// = MatAmb;

//	highp int nl = int(numberLights);
//	int i, j;
//	for (i = 0; i < nl; i++) {
//		for (j = 0; j < nl; j++) {
//			ivec2 ij = ivec2(i, j);
//			vec4 light = texelFetch(lighting, ij, 0);
//			finalColor += calcLight(light.xyz)/numberLights;
//		}
//	}
	finalColor += calcLight(lightPos, fragNor);

    color = vec4(MatAmb + finalColor, 1.0);

}
