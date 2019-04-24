#version  330 core

//References:
//http://www.davidcornette.com/glsl/noise.html
//http://mrl.nyu.edu/~perlin/doc/oscar.html#noise
//https://www.redblobgames.com/maps/terrain-from-noise/
//https://blogs.igalia.com/itoral/2016/10/13/opengl-terrain-renderer-rendering-the-terrain-mesh/

layout(location = 0) in vec4 vertPos;

uniform sampler2D permTexture;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform int octave;
uniform float power;
uniform float freq;

out vec3 fragNor;
out vec3 iLightVec;
out vec3 ViewVec;
out vec3 HVec;
out vec4 pos;
out float water;

#define ONE 0.00390625 		// 1/256
#define ONEHALF 0.001953125 // 0.5/256

float snoise (vec2 P){

    // Skew and unskew factors
    // This is (sqrt(3.0)-1.0)/2.0
    #define F2 0.366025403784
    // This is (3.0-sqrt(3.0))/6.0
    #define G2 0.211324865405

    //skew x,y space to detemine which cell of 2 sinmplices
    float s = (P.x + P.y) * F2; //skew
    vec2 Pi = floor(P + s);
    float t = (Pi.x + Pi.y) * G2; //unskew
    vec2 P0 = Pi - t;
    Pi = Pi * ONE + ONEHALF; //scale

    vec2 Pf0 = P - P0; //distance from cell origin

    vec2 o1;
    if (Pf0.x > Pf0.y){
        o1 = vec2(1.0, 0.0); //+x +y traversal
    } else {
        o1 = vec2(0.0, 1.0); //+y +x traversal
    }

    //noise from origin
    vec2 grad0 = texture(permTexture, Pi).rg * 4.0 - 1.0;
    float t0 = 0.5 - dot(Pf0, Pf0);
    float n0;
    if (t0 < 0.0){
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad0, Pf0);
    }

    //noise from middle corner
    vec2 Pf1 = Pf0 - o1 + G2;
    vec2 grad1 = texture(permTexture, Pi + o1*ONE).rg * 4.0 - 1.0;
    float t1 = 0.5 - dot(Pf1, Pf1);
    float n1;
    if (t1 < 0.0){
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad1, Pf1);
    }

    //noise from last corner
    vec2 Pf2 = Pf0 - vec2(1.0-2.0*G2);
    vec2 grad2 = texture(permTexture, Pi+vec2(ONE, ONE)).rg * 4.0 - 1.0;
    float t2 = 0.5 - dot(Pf2, Pf2);
    float n2;
    if (t2 < 0.0){
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad2, Pf2);
    }

    //sum and scale
    return 70.0*(n0+n1+n2);
}

void main()
{
    vec3 vertNor = vec3(0, 1, 0);
    float n;
    vec2 ptf = vertPos.xz*freq;
    if (octave == 0)
    {
        n = snoise(ptf);
    }
    else
    {
        n = 1.0 * snoise(ptf) + 0.5 * snoise(ptf*2.0) + 0.25 * snoise(vec2(vertPos.x*4*freq, vertPos.z*2*freq));
    }
    n = pow(n, power);

    gl_Position = P * V * M * vec4(vertPos.xyz + vertNor*(0.5 + 0.5 * n), 1.0);
    pos =  M * vec4(vertPos.xyz + vertNor*(0.5 + 0.5 * n), 1.0);

    vec4 rawPos = vec4(vertPos.xyz + vertNor*(0.5 + 0.5 * n), 1.0);
    if (rawPos.y < 0.55f)
    {
        water = 1.0;
    }
    else
    {
        water = 0.0;
    }

    fragNor = normalize(vertNor);
    iLightVec = normalize(lightPos - (V * M * vertPos).xyz);
    ViewVec = normalize(cameraPos - (V * M * vec4(normalize(fragNor), 1.0)).xyz);
    HVec = normalize(iLightVec + ViewVec);
}
