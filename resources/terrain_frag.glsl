#version 330 core

uniform sampler2D shadowDepth;

in vec4 fPosLS;
//in vec2 vTexCoord;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform float numberLights;
uniform sampler2D lighting;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec4 color;

in float water;
in vec4 pos;

/*
vec3 calcLight(vec3 light, vec3 viewVec, vec3 fragNor)
{


    vec3 k_a = MatAmb;
    vec3 k_s = MatSpec;
    vec3 k_d = MatDif;

    vec3 diffuse = MatDif*(clamp(dot(fragNor, lightVec), 0, 1));//*texColor.xyz;


    vec3 phongColor = diffuse + specular;

    return phongColor;
}
*/
vec3 calcLight(vec3 lightPosition, vec3 fragNor, vec3 viewVec)
{
    float distance = distance(lightPosition, pos.xyz);
    distance = pow(distance, 1.4);    
    vec3 lightColor = vec3(0);
    
    vec3 lightDirection = normalize(lightPosition - pos.xyz);

    // get basic diffuse by dotting normal with light direction
    float diffuse = clamp(dot(vec3(fragNor), lightDirection), 0, 1);
    // add distance attenuation
    diffuse *= clamp(6/distance, 0, 0.7);
    // increase falloff
    diffuse = pow(diffuse, 0.9);

    vec3 halfVec = normalize(distance + viewVec);
    vec3 specular = MatSpec*pow((10/distance)*pow(clamp(dot(halfVec, fragNor), 0, 1), shine),2);

    lightColor += diffuse * vec3(1,1,1) + specular;
    return lightColor;
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

void main()
{

    vec3 fdx = vec3(dFdx(pos.x), dFdx(pos.y), dFdx(pos.z));
    vec3 fdy = vec3(dFdy(pos.x), dFdy(pos.y), dFdy(pos.z));
    vec3 norm = normalize(cross(fdx, fdy));
    vec3 viewVec = normalize(cameraPos - (V * M * vec4(normalize(norm), 1.0)).xyz);
    vec3 finalColor = vec3(0);//= MatAmb;

//    highp int nl = int(numberLights);
//    int i, j;
//    for (i = 0; i < nl; i++) {
//        for (j = 0; j < nl; j++) {
//            ivec2 ij = ivec2(i, j);
//            vec4 light = texelFetch(lighting, ij, 0);
            //finalColor += calcLight(lightPos, viewVec, norm);
            finalColor += calcLight(lightPos, norm, viewVec);
//        }
//    }

    float shade = TestShadow(fPosLS);
    //Outcolor = amb*(texColor0) + (1.0-Shade)*texColor0*BaseColor;
    //color = vec4(finalColor, 1.0);
    color = vec4((1.0 - shade)*finalColor, 1.0);
    //color = vec4(MatAmb + finalColor, 1.0);
}
