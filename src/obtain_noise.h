
#ifndef OBTAIN_NOISE_H
#define OBTAIN_NOISE_H

glm::vec4 permute(glm::vec4 x){return mod(((x*34.0f)+1.0f)*x, 289.0f);}
glm::vec4 taylorInvSqrt(glm::vec4 r){return 1.79284291400159f - 0.85373472095314f * r;}

float snoise(glm::vec3 v){
    const glm::vec2  C = glm::vec2(1.0/6.0, 1.0/3.0) ;
    const glm::vec4  D = glm::vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    glm::vec3 Cx = glm::vec3(C.x, C.x, C.x);
    glm::vec3 i  = floor(v + dot(v, glm::vec3(C.y, C.y, C.y)));//C.yyy) );
    glm::vec3 x0 =   v - i + dot(i, Cx);//C.xxx) ;

    // Other corners
    glm::vec3 g = step(glm::vec3(x0.y, x0.z, x0.x), glm::vec3(x0.x, x0.y, x0.z));//(x0.yzx, x0.xyz);
    glm::vec3 l = 1.0f - g;
    glm::vec3 Lzxy = glm::vec3(l.z, l.x, l.y);
    glm::vec3 i1 = min(g, Lzxy);// g.xyz, l.zxy );
    glm::vec3 i2 = max(g, Lzxy);// g.xyz, l.zxy );

    //  x0 = x0 - 0. + 0.0 * C
    glm::vec3 x1 = x0 - i1 + 1.0f * Cx;
    glm::vec3 x2 = x0 - i2 + 2.0f * Cx;
    glm::vec3 x3 = x0 - 1.f + 3.0f * Cx;

    // Permutations
    i = mod(i, 289.0f);
    glm::vec4 p = permute( permute( permute(
            i.z + glm::vec4(0.0f, i1.z, i2.z, 1.0f))
                               + i.y + glm::vec4(0.0f, i1.y, i2.y, 1.0 ))
                      + i.x + glm::vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    glm::vec3  ns = n_ * (glm::vec3(D.w, D.y, D.z)) - (glm::vec3(D.x, D.z, D.x));//D.wyz - D.xzx;

    glm::vec4 j = p - 49.0f * floor(p * ns.z *ns.z);  //  mod(p,N*N)

    glm::vec4 x_ = floor(j * ns.z);
    glm::vec4 y_ = floor(j - 7.0f * x_ );    // mod(j,N)

    glm::vec4 NSyyyy = glm::vec4(ns.y, ns.y, ns.y, ns.y);
    glm::vec4 x = x_ *ns.x + NSyyyy;//ns.yyyy;
    glm::vec4 y = y_ *ns.x + NSyyyy;//ns.yyyy;
    glm::vec4 h = 1.0f - abs(x) - abs(y);

    glm::vec4 b0 = glm::vec4(glm::vec2(x.x, x.y), glm::vec2(y.x, y.y)); //x.xy, y.xy );
    glm::vec4 b1 = glm::vec4(glm::vec2(x.z, x.w), glm::vec2(y.z, y.w)); //x.zw, y.zw );

    glm::vec4 s0 = floor(b0)*2.0f + 1.0f;
    glm::vec4 s1 = floor(b1)*2.0f + 1.0f;
    glm::vec4 sh = -step(h, glm::vec4(0.0));

    glm::vec4 a0 = glm::vec4(b0.x, b0.z, b0.y, b0.w) + glm::vec4(s0.x, s0.z, s0.y, s0.w)*glm::vec4(sh.x, sh.x, sh.y, sh.y);//b0.xzyw + s0.xzyw*sh.xxyy ;
    glm::vec4 a1 = glm::vec4(b1.x, b1.z, b1.y, b1.w) + glm::vec4(s1.x, s1.z, s1.y, s1.w)*glm::vec4(sh.z, sh.z, sh.w, sh.w);//b1.xzyw + s1.xzyw*sh.zzww ;

    glm::vec3 p0 = glm::vec3(glm::vec2(a0.x, a0.y), h.x);//a0.xy,h.x);
    glm::vec3 p1 = glm::vec3(glm::vec2(a0.z, a0.w), h.y);//a0.zw,h.y);
    glm::vec3 p2 = glm::vec3(glm::vec2(a1.x, a1.y), h.z);//a1.xy,h.z);
    glm::vec3 p3 = glm::vec3(glm::vec2(a1.z, a1.w), h.w);//a1.zw,h.w);

    //Normalise gradients
    glm::vec4 norm = taylorInvSqrt(glm::vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    glm::vec4 m = max(0.6f - glm::vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0f);
    m = m * m;
    return 42.0 * dot( m*m, glm::vec4( dot(p0,x0), dot(p1,x1),
                                  dot(p2,x2), dot(p3,x3) ) );
}

float scale(float i, float iLow, float iHigh, float oLow, float oHigh)
{
    return ((i - iLow) / (iHigh - iLow)) * (oHigh - oLow) + oLow;
}

float noise(glm::vec3 position)
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

float mountain_noise(glm::vec3 position)
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

float mountain_range_noise(glm::vec3 position)
{

    static int loadingCounter = 0;
    while(!(loadingCounter++ % 10000))
    {
        printf("loading");
        for(int i = 0; i < loadingCounter % 9; i++)
            putchar('.');
        putchar('\n');
    }

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
    n = glm::clamp(n, 0.f, 1.f);
    return n*mountain_noise(position);
}

#endif //OBTAIN_NOISE_H
