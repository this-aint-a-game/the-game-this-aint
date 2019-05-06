#version 330 core

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform float numberLights;
uniform sampler2D lighting;
uniform vec3 cameraPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec4 color;

in float water;
in vec4 pos;

vec3 calcLight(vec3 light, vec3 viewVec, vec3 fragNor)
{
    vec3 lightVec = normalize(light.xyz);
    vec3 l = normalize(lightVec - (pos).xyz);
    vec3 halfVec = normalize(l + viewVec);
    vec3 k_a = MatAmb;
    vec3 k_s = MatSpec;
    vec3 k_d = MatDif;

    vec3 diffuse = MatDif*(clamp(dot(fragNor, lightVec), 0, 1));//*texColor.xyz;
    vec3 specular = MatSpec*pow(clamp(dot(halfVec, fragNor), 0, 1), shine);

    vec3 phongColor = (k_d * diffuse) + (k_s * specular);
//    vec3 phongColor = diffuse + specular;


    return phongColor;
}

void main()
{

    vec3 fdx = vec3(dFdx(pos.x), dFdx(pos.y), dFdx(pos.z));
    vec3 fdy = vec3(dFdy(pos.x), dFdy(pos.y), dFdy(pos.z));
    vec3 norm = normalize(cross(fdx, fdy));
    vec3 viewVec = normalize(cameraPos - (V * M * vec4(normalize(norm), 1.0)).xyz);
    vec3 finalColor = MatAmb;

    highp int nl = int(numberLights);
    int i, j;
    for (i = 0; i < nl; i++) {
        for (j = 0; j < nl; j++) {
            ivec2 ij = ivec2(i, j);
            vec4 light = texelFetch(lighting, ij, 0);
            finalColor += calcLight(light.xyz, viewVec, norm);
        }
    }

    if (water == 1.0){
        color = vec4(finalColor, 1.0)*vec4(0.216, 0.663, 0.835, 0.25);
    } else {
        color = vec4(finalColor, 1.0);
    }


}
