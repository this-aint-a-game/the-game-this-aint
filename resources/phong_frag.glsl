#version 330 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatSpec;
uniform vec3 MatDif;
uniform float shine;

uniform sampler2D lighting;

uniform float numberLights;

in vec3 fragNor;
in vec3 viewVec;
in vec4 pos;

vec3 calcLight(vec3 light)
{
	//vec3 lightVec = normalize(light.xyz);
	vec3 l = normalize(light - (pos).xyz);
	vec3 halfVec = normalize(l + viewVec);
	//vec3 k_a = MatAmb;
	vec3 k_s = MatSpec;
	vec3 k_d = MatDif;

	vec3 diffuse = MatDif*(clamp(dot(fragNor, l), 0, 1));//*texColor.xyz;
	vec3 specular = MatSpec*pow(clamp(dot(halfVec, fragNor), 0, 1), shine);

	vec3 phongColor = (k_d * diffuse) + (k_s * specular);
	//    vec3 phongColor = diffuse + specular;


	return phongColor;
}

/* Very simple Diffuse shader with a directional light*/
void main()
{
    float shade;

    vec3 finalColor = vec3(0);// = MatAmb;

	highp int nl = int(numberLights);
	int i, j;
	for (i = 0; i < nl; i++) {
		for (j = 0; j < nl; j++) {
			ivec2 ij = ivec2(i, j);
			vec4 light = texelFetch(lighting, ij, 0);
			finalColor += calcLight(light.xyz)/numberLights;
		}
	}

    color = vec4(MatAmb + finalColor, 1.0);

}
