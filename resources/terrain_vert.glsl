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

vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;

    // Permutations
    i = mod(i, 289.0 );
    vec4 p = permute( permute( permute(
    i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
    + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
    + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
    dot(p2,x2), dot(p3,x3) ) );
}

float scale(float i, float iLow, float iHigh, float oLow, float oHigh)
{
    return ((i - iLow) / (iHigh - iLow)) * (oHigh - oLow) + oLow;
}

float noise(vec3 position)
{
    float octaves = 8;
    float persistence = 0.8;
    float frequency = 0.003;
    float low = 0.f;//-2000.f;
    float high = 5.f;//3000.0f;

    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 2.0;
    for (int i = 0; i < octaves; i++)
    {
        total += snoise(position * frequency) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    float n = total / maxAmplitude;
    return scale(n, 0, 1, low, high);
}

float mountain_noise(vec3 position)
{
    float octaves = 11;
    float persistence = 0.5;
    float frequency = 0.03;

    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 2.0;
    float low = 0.f;
    float high = 10.f;
    for (int i = 0; i < octaves; i++)
    {
        total += ((1.0 - abs(snoise(position * frequency))) * 2.0 - 1.0) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    float n = total / maxAmplitude;
    return scale(n, 0, 1, low, high);
}

float mountain_range_noise(vec3 position)
{
    float octaves = 7;
    float persistence = 0.7;
    float frequency = 0.002;

    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 2.0;
    float low = 0.f;
    float high = 6.f;
    for (int i = 0; i < octaves; i++)
    {
        total += ((1.0 - abs(snoise(position * frequency))) * 2.0 - 1.0) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    float n = total / maxAmplitude;
    n = pow(n, 3);
    n = scale(n, 0, 1, low, high);
    n = clamp(n, 0, 1);
    return n*mountain_noise(position);
}

void main()
{
    vec3 vertNor = vec3(0, 1, 0);
    float n;

    n = mountain_range_noise(vertPos.xyz);
    gl_Position = P * V * M * vec4(vertPos.xyz + vertNor*(0.5 + 0.5 * n), 1.0);
    pos =  M * vec4(vertPos.xyz + vertNor*(0.5 + 0.5 * n), 1.0);
    /*
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
    */
    water = 0.0;

    fragNor = normalize(vertNor);
    iLightVec = normalize(lightPos - (V * M * vertPos).xyz);
    ViewVec = normalize(cameraPos - (V * M * vec4(normalize(fragNor), 1.0)).xyz);
    HVec = normalize(iLightVec + ViewVec);
}
