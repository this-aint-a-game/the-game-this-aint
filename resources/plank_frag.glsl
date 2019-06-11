#version 330 core

out vec4 Outcolor;

uniform sampler2D normalTex;

in OUT_struct {
    vec3 fPos;
    vec3 fragNor;
    vec2 vTexCoord;
    vec3 butterflyPosition;
} in_struct;

void main() {
    float distance = distance(in_struct.butterflyPosition, in_struct.fPos.xyz);
    distance = pow(distance, 1.4);

    vec3 lightDirection = normalize(in_struct.butterflyPosition - in_struct.fPos);

    // get basic diffuse by dotting normal with light direction
    float diffuse = clamp(dot(vec3(in_struct.fragNor) + 0.05, lightDirection), 0, 1);
    // add distance attenuation
    diffuse *= clamp(6/distance, 0, 1);
    // increase falloff
    diffuse = clamp(diffuse*4, 0, 1);
    diffuse = pow(diffuse, 1.5);

    Outcolor = vec4(in_struct.fragNor, 1);

    vec3 normal = texture(normalTex, in_struct.vTexCoord).xyz;
    vec3 shiftNor = 2.0*normal-vec3(1.0);
    float dC = max(0, dot(lightDirection, shiftNor));

    Outcolor.rgb += dC * vec3(1,1,1);
}
