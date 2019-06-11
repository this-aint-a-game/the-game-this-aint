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

vec3 calcLight(float distance, vec3 fragNor, vec3 viewVec)
{
    distance = pow(distance, 1.5);
    
    vec3 lightDirection = normalize(lightPos - pos.xyz);

    // get basic diffuse by dotting normal with light direction
    float diffuse = clamp(dot(vec3(fragNor), lightDirection), 0, 1);
    // add distance attenuation
    diffuse *= clamp(6/distance, 0, 0.7);
    // increase falloff
    diffuse = pow(diffuse, 0.9);

    vec3 halfVec = normalize(distance + viewVec);
    vec3 specular = MatSpec*pow((10/distance)*pow(clamp(dot(halfVec, fragNor), 0, 1), shine),2);

    return diffuse * MatDif + specular;
}

/* returns percent shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {

    float bias = 0.005;
    //1: shift the coordinates from -1, 1 to 0 ,1
    // clip space range is -1 to 1 and depth buffer is 0 to 1

    float percentage = 0;
    vec3 shifted = (LSfPos.xyz + vec3(1))*0.5;
    float shiftedBias = shifted.z - bias;
    vec2 texelSize = 1.0 / textureSize(shadowDepth, 0);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            //2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
            float Ldepth = texture(shadowDepth, shifted.xy + vec2(i, j)*texelSize).r; // reading rgb from texture

            //3: compare to the current depth (.z) of the projected depth
            if (Ldepth < shiftedBias)
            {
                percentage += 1.0;
            }
        }
    }

    // lighter shadows
    return percentage/15.f;
}

void main()
{
    float distance = distance(lightPos, pos.xyz);
    if(distance < 20)
    {
        vec3 fdx = vec3(dFdx(pos.x), dFdx(pos.y), dFdx(pos.z));
        vec3 fdy = vec3(dFdy(pos.x), dFdy(pos.y), dFdy(pos.z));
        vec3 norm = normalize(cross(fdx, fdy));
        vec3 viewVec = normalize(cameraPos - (V * M * vec4(normalize(norm), 1.0)).xyz);
        vec3 finalColor = vec3(0.f);
        finalColor += calcLight(distance, norm, viewVec);

        float shade = TestShadow(fPosLS);
        color = vec4((1.0 - shade)*finalColor, 1.0);
    }
    else
    {
        color = vec4(0,0,0,1);
    }
}
