#version 330 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

//uniform sampler2D shadowTexture;
uniform sampler2D shadowDepth;

in vec4 fPosLS;
in vec2 vTexCoord;

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

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {

    float bias = 0.005;
    //1: shift the coordinates from -1, 1 to 0 ,1
    // clip space range is -1 to 1 and depth buffer is 0 to 1
    vec3 shifted = (LSfPos.xyz + vec3(1))*0.5;

    //2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
    float Ldepth = texture(shadowDepth, shifted.xy).r; // reading rgb from texture

    //3: compare to the current depth (.z) of the projected depth
    if (Ldepth < shifted.z - bias)
    {
        return 1.0;
    }

    //4: return 1 if the point is shadowed

    return 0.0;
}

/* Very simple Diffuse shader with a directional light*/
void main()
{
    float shade;
    //vec4 texColor = texture(shadowTexture, vTexCoord);
    shade = TestShadow(fPosLS);

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
    //color = vec4(MatAmb + (1.0-shade)*finalColor, 1.0);

    //Outcolor = amb*(texColor0) + (1.0-Shade)*texColor0*BaseColor; // add ambient color regardless

}
