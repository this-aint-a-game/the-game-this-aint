#version 330 core

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

//uniform sampler2D Texture;

in vec3 fragNor;
in vec3 iLightVec;
in vec3 HVec;

in float water;

in vec4 pos;

layout(location = 0) out vec4 color;

void main()
{
    vec3 norm = fragNor;

    vec3 fdx = vec3(dFdx(pos.x), dFdx(pos.y), dFdx(pos.z));
    vec3 fdy = vec3(dFdy(pos.x), dFdy(pos.y), dFdy(pos.z));
    norm = normalize(cross(fdx, fdy));

    //vec2 vTexCoord = vec2(pos.x, pos.z)/100;
    //vec4 texColor = texture(Texture, vTexCoord);

    vec3 ambient = MatAmb;

    vec3 diffuse = MatDif*(clamp(dot(norm, iLightVec), 0, 1));//*texColor.xyz;

    vec3 specular = MatSpec*pow(clamp(dot(HVec, norm), 0, 1), shine);

    vec3 phong = ambient + diffuse + specular;

    if (water == 1.0){
        color = vec4(phong, 1.0)*vec4(0.216, 0.663, 0.835, 0.25);
    } else {
        color = vec4(phong, 1.0);
    }

}
